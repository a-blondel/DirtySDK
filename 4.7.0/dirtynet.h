/*H*************************************************************************************************/
/*!

    \File    dirtynet.h

    \Description
        Platform independent interface to network layers. Based on BSD sockets, but with
        performance modifications. Allows truly portable modules to be written and moved
        to different platforms needing only different support wrappers (no change to actual
        network modes).

    \Notes

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2001-2002.  ALL RIGHTS RESERVED.

    \Version    0.5        08/01/01 (GWS) First Version
    \Version    1.1        01/02/02 (GWS) Final Release

*/
/*************************************************************************************************H*/


#ifndef _dirtynet_h
#define _dirtynet_h

/*!
\Module DirtySock
*/
//@{

/*** Include files *********************************************************************/

/*** Defines ***************************************************************************/

// Constants

#if DIRTYCODE_PLATFORM == DIRTYCODE_PS3
#include <sdk_version.h> // included so we know where to find network headers
#if CELL_SDK_VERSION >= 0x080000
 #include <sys/socket.h>
 #include <netinet/in.h>         /* struct sockaddr_in */
 #include <arpa/inet.h>          /* struct in_addr */
#else
 #include <network/sys/socket.h>
 #include <network/netinet/in.h>         /* struct sockaddr_in */
 #include <network/arpa/inet.h>          /* struct in_addr */
#endif
#elif defined(DIRTY_HAVE_SOCKET_HEADER)
#include <sys/socket.h>
#include <netinet/in.h>         /* struct sockaddr_in */
#include <arpa/inet.h>          /* struct in_addr */
#else
#define AF_INET 2               //!< internet (standard numbering)

#define SOCK_STREAM 1           //!< stream protocol (TCP for AF_INET)
#define SOCK_DGRAM 2            //!< datagram protocol (UDP for AF_INET)
#define SOCK_RAW 3              //!< raw protocol

#define IPPROTO_IP     0
#define IPPROTO_ICMP   1
#define IPPROTO_IPV4   4
#define IPPROTO_TCP    6
#define IPPROTO_UDP   17
#define IPPROTO_RAW  255

#ifndef INADDR_ANY
#define INADDR_ANY          0x00000000
#endif
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK     0x7f000001
#endif

#endif /* !DIRTY_HAVE_SOCKET_HEADER */

#define SOCK_NORECV 1           //!< caller does not want to receive more data
#define SOCK_NOSEND 2           //!< caller does not want to send more data

#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST    0xffffffff
#endif

#define CALLB_NONE 0            //!< no callback
#define CALLB_SEND 1            //!< call when we can send
#define CALLB_RECV 2            //!< call when we can receive

#define SOCKLOOK_LOCALADDR      "*"

#define SOCKLOOK_FLAGS_ALLOWXDNS    (1)

// define if a given platform has async read support implemented
#ifndef SOCKET_ASYNCRECVTHREAD
 #if DIRTYCODE_PLATFORM == DIRTYCODE_PC
   #define SOCKET_ASYNCRECVTHREAD 1
 #elif DIRTYCODE_PLATFORM == DIRTYCODE_PS3
   #define SOCKET_ASYNCRECVTHREAD 1
 #elif DIRTYCODE_PLATFORM == DIRTYCODE_REVOLUTION
   #define SOCKET_ASYNCRECVTHREAD 1
 #elif DIRTYCODE_PLATFORM == DIRTYCODE_XENON
   #define SOCKET_ASYNCRECVTHREAD 1
 #elif (DIRTYCODE_PLATFORM == DIRTYCODE_APPLEIOS) || (DIRTYCODE_PLATFORM == DIRTYCODE_LINUX) || (DIRTYCODE_PLATFORM == DIRTYCODE_PLAYBOOK)
   #define SOCKET_ASYNCRECVTHREAD 1
 #else
   #define SOCKET_ASYNCRECVTHREAD 0
 #endif
#endif

//! maximum udp packet size we can receive (constrained by Xbox 360 max VDP packet size)
#define SOCKET_MAXUDPRECV       (1264)

//! maximum number of virtual ports that can be specified
#define SOCKET_MAXVIRTUALPORTS  (32)

// Errors

#define SOCKERR_NONE 0          //!< no error
#define SOCKERR_CLOSED -1       //!< the socket is closed
#define SOCKERR_NOTCONN -2      //!< the socket is not connected
#define SOCKERR_BLOCKED -3      //!< operation would result in blocking
#define SOCKERR_ADDRESS -4      //!< the address is invalid
#define SOCKERR_UNREACH -5      //!< network cannot be accessed by this host
#define SOCKERR_REFUSED -6      //!< connection refused by the recipient
#define SOCKERR_OTHER   -7      //!< unclassified error
#define SOCKERR_NOMEM   -8      //!< out of memory
#define SOCKERR_NORSRC  -9      //!< out of resources
#define SOCKERR_UNSUPPORT -10   //!< unsupported operation
#define SOCKERR_INVALID -11     //!< resource or operation is invalid
#define SOCKERR_ADDRINUSE -12   //!< address already in use
#define SOCKERR_CONNRESET -13   //!< connection has been reset

/*** Macros ****************************************************************************/

//! init a sockaddr to zero and set its family type
#define SockaddrInit(addr,fam)      { (addr)->sa_family = (fam); *((uint16_t *)&(addr)->sa_data[0]) = 0; *((uint32_t *)&(addr)->sa_data[2]) = 0; *((uint32_t *)&(addr)->sa_data[6]) = 0; *((uint32_t *)&(addr)->sa_data[10]) = 0; }

//! get the port in host format from sockaddr
#define SockaddrInGetPort(addr)     ((((unsigned char)(addr)->sa_data[0])<<8)|(((unsigned char)(addr)->sa_data[1])<<0))

//! set the port in host format in a sockaddr
#define SockaddrInSetPort(addr,val) { (addr)->sa_data[0] = (unsigned char)((val)>>8); (addr)->sa_data[1] = (unsigned char)(val); }

//! get the address in host format from sockaddr
#define SockaddrInGetAddr(addr)     (((((((unsigned char)((addr)->sa_data[2])<<8)|(unsigned char)((addr)->sa_data[3]))<<8)|(unsigned char)((addr)->sa_data[4]))<<8)|(unsigned char)((addr)->sa_data[5]))

//! set the address in host format in a sockaddr
#define SockaddrInSetAddr(addr,val) { uint32_t val2 = (val); (addr)->sa_data[5] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[4] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[3] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[2] = (unsigned char)val2; }

//! get the misc field in host format from sockaddr
#define SockaddrInGetMisc(addr)     (((((((unsigned char)((addr)->sa_data[6])<<8)|(unsigned char)((addr)->sa_data[7]))<<8)|(unsigned char)((addr)->sa_data[8]))<<8)|(unsigned char)((addr)->sa_data[9]))

//! set the misc field in host format in a sockaddr
#define SockaddrInSetMisc(addr,val) { uint32_t val2 = (val); (addr)->sa_data[9] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[8] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[7] = (unsigned char)val2; val2 >>= 8; (addr)->sa_data[6] = (unsigned char)val2; }

//! detect loopback address (family independent)
#define SockaddrIsLoopback(addr)   (( ((addr)->sa_family == AF_INET) && ((addr)->sa_data[0] == 127) && ((addr)->sa_data[1] == 0) && ((addr)->sa_data[2] == 0) && ((addr)->sa_data[3] == 1) ))


/*** Type Definitions ******************************************************************/


// basic socket type is a pointer
typedef struct SocketT SocketT;

//! a host lookup structure -- uses a callback
//! system to determine when lookup has finished
typedef struct HostentT
{
    int32_t done;                       //!< public: indicates when lookup is complete
    uint32_t addr;                      //!< public: resolved host address

    int32_t (*Done)(struct HostentT *); //!< public: callback to indicate completion status
    void (*Free)(struct HostentT *);    //!< public: callback to release Hostent structure

    char name[64];                      //!< private
    int32_t sema;                       //!< private
    int32_t thread;                     //!< private
    void* pData;                        //!< private
    uint32_t timeout;                   //!< private
} HostentT;

//! address mapping structure (used on Xbox platforms)
typedef struct SocketAddrMapT
{
    uint32_t uAddr;                 //!< address to match (192.168.0.0)
    uint32_t uMask;                 //!< subnet mask (255.255.0.0)
    uint32_t uRemap;                //!< address to remap to (1.2.3.4)
    uint32_t uServiceId;            //!< serviceId of associated service
    uint16_t uSrcPort;              //!< src port to map from (zero=wildcard)
    uint16_t uDstPort;              //!< dst port to map to (zero=use source port)
} SocketAddrMapT;

//! dns name mapping structure (used on Xbox platforms)
typedef struct SocketNameMapT
{
    char strDnsName[64];    //!< DNS name.  if name starts with a '.', wc match is performed
    char strRemap[32];      //!< \verbatim remap name, or '\0' if remapping to an address \endverbatim
    uint32_t uRemap;        //!< remap addr, or zero if remapping to a name
    uint32_t uServiceId;    //!< serviceId of associated service
    uint16_t uSrcPort;      //!< src port to map from (zero=wildcard)
    uint16_t uDstPort;      //!< dst port to map to (zero=use source port)
} SocketNameMapT;


#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_) && (DIRTYCODE_PLATFORM != DIRTYCODE_PS3)

// prevent clash with winsock.h/winsockx.h on PC/Xbox
#define _WINSOCKAPI_
#define _WINSOCK2API_

//! standard bsd sockaddr definition
struct sockaddr
{
    uint16_t sa_family;
    unsigned char sa_data[14];
};

//! included for bsd/windock compatibility
//! use SockaddrIn macros with sockaddr instead
struct in_addr
{
    uint32_t s_addr;
};

//! included for bsd/windock compatibility
//! use SockaddrIn macros with sockaddr instead
struct sockaddr_in
{
    uint16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    uint32_t sin_misc;
    char sin_zero[4];
};
#endif // !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_)

//! global socket send callback
typedef int32_t (SocketSendCallbackT)(SocketT *pSocket, int32_t iType, const uint8_t *pData, int32_t iDataSize, const struct sockaddr *pTo, void *pCallref);

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
    platform independent functions, defined in dirtynet.c
*/

// compare two sockaddr structures
int32_t SockaddrCompare(struct sockaddr *addr1, struct sockaddr *addr2);

// set the address in text format in a sockaddr
int32_t SockaddrInSetAddrText(struct sockaddr *addr, const char *str);

// get the address in text format from sockaddr
char *SockaddrInGetAddrText(struct sockaddr *addr, char *str, int32_t len);

// convert int16_t from host to network byte order
uint16_t SocketHtons(uint16_t addr);

// convert int32_t from host to network byte order
uint32_t SocketHtonl(uint32_t addr);

// convert int16_t from network to host byte order
uint16_t SocketNtohs(uint16_t addr);

// convert int32_t from network to host byte order.
uint32_t SocketNtohl(uint32_t addr);

// convert 32-bit internet address into textual form
char *SocketInAddrGetText(uint32_t addr, char *str, int32_t len);

// convert textual internet address into 32-bit integer form
int32_t SocketInTextGetAddr(const char *addrtext);

// parse address:port combination
int32_t SockaddrInParse(struct sockaddr *addr, const char *parse);

// parse address:port combination into separate components
int32_t SockaddrInParse2(uint32_t *pAddr, int32_t *pPort, int32_t *pPort2, const char *parse);

// create new instance of socket interface module
int32_t SocketCreate(int32_t iThreadPrio);

// release resources and destroy module.
int32_t SocketDestroy(uint32_t uShutdownFlags);

// create a new transfer endpoint
SocketT *SocketOpen(int32_t af, int32_t type, int32_t protocol);

// perform partial/complete shutdown of socket
int32_t SocketShutdown(SocketT *pSocket, int32_t how);

// close a socket
int32_t SocketClose(SocketT *pSocket);

// import a socket - may be SocketT pointer or sony socket ref
SocketT *SocketImport(intptr_t uSockRef);

// release an imported socket
void SocketRelease(SocketT *pSocket);

// bind a local address/port to a socket
int32_t SocketBind(SocketT *pSocket, const struct sockaddr *name, int32_t namelen);

// return information about an existing socket.
int32_t SocketInfo(SocketT *pSocket, int32_t iInfo, int32_t iData, void *pBuf, int32_t iLen);

// send a control message to the dirtysock layer
int32_t SocketControl(SocketT *pSocket, int32_t option, int32_t data1, void *data2, void *data3);

// start listening for an incoming connection on the socket
int32_t SocketListen(SocketT *pSocket, int32_t backlog);

// attempt to accept an incoming connection from a
SocketT *SocketAccept(SocketT *pSocket, struct sockaddr *addr, int32_t *addrlen);

// initiate a connection attempt to a remote host
int32_t SocketConnect(SocketT *pSocket, struct sockaddr *name, int32_t namelen);

// send data to a remote host
int32_t SocketSendto(SocketT *pSocket, const char *buf, int32_t len, int32_t flags, const struct sockaddr *to, int32_t tolen);

// same as SocketSendto() with "to" set to NULL
#define SocketSend(_pSocket, _pBuf, iLen, iFlags)   SocketSendto(_pSocket, _pBuf, iLen, iFlags, NULL, 0)

// receive data from a remote host
int32_t SocketRecvfrom(SocketT *pSocket, char *buf, int32_t len, int32_t flags, struct sockaddr *from, int32_t *fromlen);

// same as SocketRecvfrom() with "from" set to NULL.
#define SocketRecv(_pSocket, pBuf, iLen, iFlags)   SocketRecvfrom(_pSocket, pBuf, iLen, iFlags, NULL, 0)

// register a callback routine for notification of socket events
int32_t SocketCallback(SocketT *pSocket, int32_t flags, int32_t timeout, void *ref, int32_t (*proc)(SocketT *pSocket, int32_t flags, void *ref));

// return the host address that would be used in order to communicate with the given destination address.
int32_t SocketHost(struct sockaddr *host, int32_t hostlen, const struct sockaddr *dest, int32_t destlen);

// translate a numeric error number into its textual equivilent.
const char *SocketError(int32_t iErrno);

// lookup a host by name and return the corresponding Internet address
HostentT *SocketLookup(const char *text, int32_t timeout);

// lookup a host by name and return the corresponding Internet address, with flags
#if DIRTYCODE_PLATFORM == DIRTYCODE_XENON
HostentT *SocketLookup2(const char *text, int32_t timeout, uint32_t flags);
#else
#define SocketLookup2(_text, _timeout, _flags) SocketLookup(_text, _timeout)
#endif

// return "external" local address
uint32_t SocketGetLocalAddr(void);

#if DIRTYCODE_DEBUG
// for internal use only
uint32_t SocketSimulatePacketLoss(uint32_t uPacketLossParam);
#endif

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtynet_h


