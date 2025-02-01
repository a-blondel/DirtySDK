/*H*************************************************************************************************/
/*!

    \File    commudp.c

    \Description
        This is a reliable UDP transport class optimized for use in a
        controller passing game applications. When the actual data
        bandwidth is low (as is typical with controller passing), it
        sends redundant data in order to quickly recover from any lost
        packets. Overhead is low adding only 8 bytes per packet in
        addition to UDP/IP overhead. This module support mutual
        connects in order to allow connections through firewalls.

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 1999-2003.  ALL RIGHTS RESERVED.

    \Version    0.1        02/09/99 (GWS) First Version
    \Version    0.2        02/14/99 (GWS) Revised and enhanced
    \Version    0.5        02/14/99 (GWS) Alpha release
    \Version    1.0        07/30/99 (GWS) Final release
    \Version    2.0        10/27/99 (GWS) Revised to use winsock 1.1/2.0
    \Version    2.1        12/04/99 (GWS) Removed winsock 1.1 support
    \Version    2.2        01/12/00 (GWS) Fixed receive tick bug
    \Version    2.3        06/12/00 (GWS) Added fastack for low-latency nets
    \Version    2.4        12/04/00 (GWS) Added firewall penetrator
    \Version    3.0        12/04/00 (GWS) Reported to dirtysock
    \Version    3.1        11/20/02 (JLB) Added Send() flags parameter
    \Version    3.2        02/18/03 (JLB) Fixes for multiple connection support
    \Version    3.3        05/06/03 (GWS) Allowed poke to come from any IP
    \Version    3.4        09/02/03 (JLB) Added unreliable packet type
    \Version    4.0        09/12/03 (JLB) Per-send optional unreliable transport
    \Version    5.0        07/07/09 (jrainy) Putting meta-data bits over the high bits of the sequence number
*/
/*************************************************************************************************H*/


/*** Include files *********************************************************************/

#include <stdio.h>
#include <string.h>

#include "dirtysock.h"
#include "dirtymem.h"
#include "commall.h"
#include "commudp.h"

/*** Defines ***************************************************************************/

#undef COMM_PRINT
#define COMM_PRINT      (0)
#define ESC_CAUSES_LOSS (DIRTYCODE_DEBUG && DIRTYCODE_PLATFORM == DIRTYCODE_PC && 0)

#if ESC_CAUSES_LOSS
#include <windows.h>
#endif

#define BUSY_KEEPALIVE  (100)
#define IDLE_KEEPALIVE  (2500)
#define PENETRATE_RATE  (1000)
#define UNACK_LIMIT     (2048)

//! define protocol packet types
enum {
    RAW_PACKET_INIT = 1,        // initiate a connection
    RAW_PACKET_CONN,            // confirm a connection
    RAW_PACKET_DISC,            // terminate a connection
    RAW_PACKET_NAK,             // force resend of lost data
    RAW_PACKET_POKE,            // try and poke through firewall

    RAW_PACKET_UNREL = 128,     // unreliable packet send (must be power of two)
                                // 128-255 reserved for unreliable packet sequence
    RAW_PACKET_DATA = 256,      // initial data packet sequence number (must be power of two)

    /*  Width of the sequence window, can be anything provided 
        RAW_PACKET_DATA + RAW_PACKET_DATA_WINDOW
        doesn't overlap the meta-data bits.
    */  
    RAW_PACKET_DATA_WINDOW = (1 << 24) - 256
};

#define RAW_METATYPE1_SIZE  (8)
//! max additional space needed by a commudp meta type
#define COMMUDP_MAX_METALEN (8)

#define SEQ_MASK (0x00ffffff)
#define SEQ_MULTI_SHIFT (28)
#define SEQ_META_SHIFT  (28 - 4)
#define SEQ_MULTI_INC (1 << SEQ_MULTI_SHIFT)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

//! raw protocol packet format
typedef struct
{
    //! packet header which is not sent/received (this data is used internally)
    struct {
        int32_t len;                        //!< variable data len (-1=none) (used int32_t for alignment)
        uint32_t when;                      //!< tick when a packet was received
        uint32_t meta;                      //!< four-bit metadata field extracted from seq field
    } head;
    //! packet body which is sent/received
    struct {
        uint32_t seq;                       //!< packet type or sequence number
        uint32_t ack;                       //!< acknowledgement of last packet
        uint8_t  data[SOCKET_MAXUDPRECV-8]; //!< user data
    } body;
} RawUDPPacketT;

//! raw protocol packet header (no data) -- used for stack local formatting of packets without data
typedef struct
{
    //! packet header which is not sent/received (this data is used internally)
    struct {
        int32_t len;            //!< variable data len (-1=none) (used int32_t for alignment)
        uint32_t when;          //!< tick when a packet was received
        uint32_t meta;          //!< four-bit metadata field extracted from seq field
    } head;
    //! packet body which is sent/received
    struct {
        uint32_t seq;           //!< packet type or seqeunce number
        uint32_t ack;           //!< acknowledgement of last packet
        uint32_t cid;           //!< client id (only included in INIT and CONN packets)
        uint32_t data[16];      //!< space for possible metadata included in control packets
    } body;
} RawUDPPacketHeadT;

//! private module storage
struct CommUDPRef
{
    //! common header is first
    CommRef common;

    //! max amount of unacknowledged data that can be sent in one go (default 2k)
    uint32_t unacklimit; 

    //! max amount of data that can be sent redundantly (default 64)
    uint32_t redundantlimit;

    //! linked list of all instances
    CommUDPRef *link;
    //! comm socket
    SocketT *socket;
    //! peer address
    struct sockaddr peeraddr;

    //! port state
    enum {
        DEAD,       //!< dead
        IDLE,       //!< idle
        CONN,       //!< conn
        LIST,       //!< list
        OPEN,       //!< open
        CLOSE       //!< close
    } state;

    //! identifier to keep from getting spoofed
    uint32_t connident;
    
    //! type of metachunk to include in stream (zero=none)
    uint32_t metatype;
    
    //! unique client identifier (used for game server identification)
    uint32_t clientident;
    //! remote client identifier
    uint32_t rclientident;

    //! width of receive records (same as width of send)
    int32_t rcvwid;
    //! length of receive buffer (multiple of rcvwid)
    int32_t rcvlen;
    //! fifo input offset
    int32_t rcvinp;
    //! fifo output offset
    int32_t rcvout;
    //! pointer to buffer storage
    char *rcvbuf;
    //! next packet expected (sequence number)
    uint32_t rcvseq;
    //! next unreliable packet expected
    uint32_t urcvseq;
    //! last packet we acknowledged
    uint32_t rcvack;
    //! number of unacknowledged received bytes
    int32_t rcvuna;

    //! width of send record (same as width of receive)
    int32_t sndwid;
    //! length of send buffer (multiple of sndwid)
    int32_t sndlen;
    //! fifo input offset
    int32_t sndinp;
    //! fifo output offset
    int32_t sndout;
    //! current output point within fifo
    int32_t sndnxt;
    //! pointer to buffer storage
    char *sndbuf;
    //! next packet to send (sequence number)
    uint32_t sndseq;
    //! unreliable packet sequence number
    uint32_t usndseq;
    //! last send result
    uint32_t snderr;

    //! tick at which last packet was sent
    uint32_t sendtick;
    //! tick at which last packet was received
    uint32_t recvtick;
    //! tick at which last idle callback made
    uint32_t idletick;

    //! control access during callbacks
    volatile int32_t callback;
    //! indicate there is an event pending
    uint32_t gotevent;
    //! callback routine pointer
    void (*callproc)(void *ref, int32_t event);
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

// Private variables

//! linked list of port objects
static CommUDPRef   *g_link = NULL;

//! semaphore to synchronize thread access
static NetCritT     g_crit;

//! missed event marker
static int32_t      g_missed;

//! variable indicates call to _CommUDPEvent() in progress
static int32_t      g_inevent;


/*F*************************************************************************************************/
/*!
    \Function    _CommUDPSetConnID

    \Description
        Sets connident to the 32bit hash of the specified connection identifier string, if any.

    \Input *ref         - reference pointer
    \Input *pStrConn    - pointer to user-specified connection string

    \Version 06/16/04 (JLB)

    \v5.6.2 reimplementation
*/
/*************************************************************************************************F*/
static void _CommUDPSetConnID(CommUDPRef *ref, const char *pStrConn)
{
    char *pConnID = strchr(pStrConn, '#');
    if (pConnID != NULL)
    {
        ref->connident = NetHash(pConnID+1);
    }
}