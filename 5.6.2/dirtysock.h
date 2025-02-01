/*H*************************************************************************************************/
/*!

    \File    dirtysock.h

    \Description
        Platform independent interface to network layers. Based on
        BSD sockets, but with performance modifications. Allows truly
        portable modules to be written and moved to different platforms
        needing only different support wrappers (no change to actual
        network modes).

    \Notes
        None.

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2001-2002.  ALL RIGHTS RESERVED.

    \Version    1.0        08/01/01 (GWS) First Version

*/
/*************************************************************************************************H*/

#ifndef _dirtysock_h
#define _dirtysock_h

/*!
\Module DirtySock
*/
//@{

/*** Include files *********************************************************************/

#ifndef DIRTYSOCK
#define DIRTYSOCK (TRUE)
#include "dirtydefs.h"
#include "dirtynet.h"
#include "dirtylib.h"
#endif

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

//@}

//
// DirtySock SDK documentation
//

/*!

    \Mainpage DirtySock

    \Section intro Introduction

        DirtySock is a platform independent API providing access to common network library
        functions. It provides a Sockets interface similar in many respects to BSD/Sockets
        and Winsock but with much of the weirdness of those APIs fixed (since it does not
        have to be completely backwards compatible with existing code). To support the upper
        layer protocols, the Dirtysock library also includes some basic systems functions
        like memory management, critical sections, idle functions, and diagnostic output.

        Running on top of Dirtysock are a number of different protocols.  These include a
        reliable UDP protocol used to pass game controller packets, an Aries protocol implementation
        for talking to the lobby server (a simple TCP length prefixed packet format), an HTTP
        protocol implementation and an "advertising" module for broadcasting announcements
        over a LAN.  For more on protocols, follow the Comm and Proto links below.

        Also included in the DirtySock SDK are crypt routines used by the secure protocols,
        a lobby client module for interacting with the lobby server, a pogo client module
        for interacting with pogo.com, and an XML module for dealing with an XML server.

    \Section organization Organization

    \Subsection org1 Module Organization

    \if platform_pc

    DirtySock on the PC can be broken down into eleven basic groupings:

        - \link Buddy       Buddy       \endlink - Buddy Client API
        - \link Comm        Comm        \endlink - Low-level communications protocols
        - \link Crypt       Crypt       \endlink - Encryption/decryption routines
        - \link DirtySock   DirtySock   \endlink - Platform independent sockets layer
        - \link NetConn     NetConn     \endlink - Network startup routines
        - \link NetGame     NetGame     \endlink - Peer game networking routines
        - \link Proto       Proto       \endlink - Mid-level communications protocols
    \endif

    \Section history Revision History


    \Version    1.8.1       08/09/2003  TourneyApi fixes, ProtoMangle bugfix, LobbyApiConnect name resolve, fixed two rare PS2 GameLink bugs
    \Version    1.7.0       07/03/2003  Bugfixes to LobbyApi, NetConn, ProtoHttp, TourneyApi
    \Version    1.6.0       06/06/2003  Updates to BuddyApi, LobbyApi, TourneyApi; added Crypt modules (PS2)
    \Version    1.5.0       05/02/2003  Added NetConnMAC (PS2), ProtoMangle, updated CommSRP and CommUDP for ProtoMangle support
    \Version    1.4.0       03/28/2003  Added LobbyUtf8, NetConn, ProtoName (PS2), CommTAPI (PC)
    \Version    1.3.0       02/28/2003  Added TourneyApi & sample app; bugfixes
    \Version    1.2.0       01/21/2003  Added BuddyApi and CommSRP
    \Version    1.1.0       12/23/2002  LobbyApi added to distribution
    \Version    1.0.0       11/27/2002  First official release
    \Version    0.1.0       11/08/2002  Pre-release distribution (documentation only)

*/

#endif // _dirtysock_h

