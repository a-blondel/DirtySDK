/*H*************************************************************************************************/
/*!

    \File    dirtylib.h

    \Description
        Provide basic library functions for use by network layer code.
        This is needed because the network code is platform/project
        independent and needs to rely on a certain set of basic
        functions.

    \Notes
        None.

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2001-2002.  ALL RIGHTS RESERVED.

    \Version    0.5        08/01/01 (GWS) First Version
    \Version    1.0        12/31/01 (GWS) Redesigned for Tiburon environment
*/
/*************************************************************************************************H*/


#ifndef _dirtylib_h
#define _dirtylib_h

/*!
    \Moduledef DirtySock DirtySock

    \Description
        The Dirtysock module provides a portable sockets implementation.

        <b>Overview</b>

            Dirtysock is a platform independent API providing access to common network library
            functions. It it similar in many respects to BSD/Sockets and Winsock, but targeted
            specifically for a real-time game networking environment.  It will be helpful to have
            an understanding of sockets before reading this documentation.

            Unlike BSD and Windows sockets, only the AF_INET address family is supported, and blocking
            read and write calls are not supported.  Instead, the Dirtysock API supports two
            extensions not present in a normal socket environment that allow user-defined callbacks
            and idle functions.  The SocketCallback() function allows you to register a function
            that gets called when an incoming packet arrives on a particular socket. This can be
            used to process incoming packets in essentially real-time. If the \em idle parameter
            of SocketCallback() is nonzero, the registered function will be called approximately
            every \em idle milliseconds. This can be used to deal with resends and things of
            that nature.  Callback and idle functions are called by a private Dirtysock thread.

        <b>Module Dependency Graph</b>

            <img alt="" src="dirtysock.png">

        <b>Initializing DirtySock</b>

            \code

            #include "dirtydefs.h"
            #include "dirtysock.h"

            // initialize DirtySock
            SocketCreate();

            \endcode

        <b>BSD Socket Function Equivalency</b>

            The following is a table of BSD socket functions with their Dirtysock equivalents:

            <TABLE>
            <TR><TD>BSD function</TD>   <TD>Dirtysock function</TD>      <TD>Notes</TD>
            <TR><TD>accept</TD>         <TD>SocketAccept()</TD>          <TD>-</TD>
            <TR><TD>bind</TD>           <TD>SocketBind()</TD>            <TD>-</TD>
            <TR><TD>closesocket</TD>    <TD>SocketClose()</TD>           <TD>-</TD>
            <TR><TD>connect</TD>        <TD>SocketConnect()</TD>         <TD>-</TD>
            <TR><TD>getpeername</TD>    <TD>SocketInfo()</TD>            <TD>Call with \em info = 'peer'</TD>
            <TR><TD>getsockname</TD>    <TD>SocketInfo()</TD>            <TD>Call with \em info = 'bind'</TD>
            <TR><TD>getsockopt</TD>     <TD>No equivalent</TD>           <TD>-</TD>
            <TR><TD>htonl</TD>          <TD>SocketHtonl()</TD>           <TD>-</TD>
            <TR><TD>htons</TD>          <TD>SocketHtons()</TD>           <TD>-</TD>
            <TR><TD>inet_addr</TD>      <TD>SocketInTextGetAddr</TD>     <TD>-</TD>
            <TR><TD>inet_ntoa</TD>      <TD>SocketInAddrGetText</TD>     <TD>-</TD>
            <TR><TD>ioctlsocket</TD>    <TD>No equivalent</TD>           <TD>-</TD>
            <TR><TD>listen</TD>         <TD>SocketListen()</TD>          <TD>-</TD>
            <TR><TD>ntohl</TD>          <TD>SocketNtohl()</TD>           <TD>-</TD>
            <TR><TD>ntohs</TD>          <TD>SocketNtohs()</TD>           <TD>-</TD>
            <TR><TD>recv</TD>           <TD>SocketRecv()</TD>            <TD>-</TD>
            <TR><TD>recvfrom</TD>       <TD>SocketRecvfrom()</TD>        <TD>-</TD>
            <TR><TD>select</TD>         <TD>No direct equivalent</TD>    <TD>Calling SocketInfo() with \em info = 'stat' returns connection status</TD>
            <TR><TD>send</TD>           <TD>SocketSend()</TD>            <TD>-</TD>
            <TR><TD>sendto</TD>         <TD>SocketSendto()</TD>          <TD>-</TD>
            <TR><TD>setsockopt</TD>     <TD>No equivalent</TD>           <TD>-</TD>
            <TR><TD>shutdown</TD>       <TD>SocketShutdown()</TD>        <TD>-</TD>
            <TR><TD>socket</TD>         <TD>SocketOpen()</TD>            <TD>-</TD>
            </TABLE>

        <b>Examples</b>

            <em>Create a new stream socket connection</em>

            \code

            struct sockaddr peeraddr;

            pSock = SocketOpen(AF_INET, SOCK_STREAM, 0);
            if (pSock != NULL)
            {
                // init sockaddr structure
                SockaddrInit(&peeraddr, AF_INET);
                SockaddrInSetAddr(&peeraddr, iAddr);
                SockaddrInSetPort(&peeraddr, iPort);

                // start the connect
                SocketConnect(pSock, (struct sockaddr *)&peeraddr, sizeof(peeraddr));
            }

            \endcode

            <em>Check for connection ready</em>

            \code
            // note: as connection resolution is asynchronous, SocketInfo() is usually
            // called repeatedly until either the connection is ready or a given timeout occurs
            if (SocketInfo(pSock, 'stat', NULL, 0) > 0)
            {
                NetPrint("Connection ready\n");
            }

            \endcode

            <em>Send data on a stream socket</em>

            \code

            char data[] = "Data to send to remote host";
            SocketSend(pSock, data, sizeof(data), 0);

            \endcode

            <em>Receive data from a datagram socket</em>

            \code

            char data[256];
            struct sockaddr from;
            int32_t len, fromlen;

            if ((len = SocketRecvfrom(pSock, data, sizeof(data), 0, &from, &fromlen)) > 0)
            {
                NetPrint("Received %d bytes from socket\n",len);
            }

            \endcode
*/
//@{

/*** Include files *********************************************************************/

// include for platform identification and definitions
#include "dirtydefs.h"

/*** Defines ***************************************************************************/

// define platform-specific options
#if DIRTYCODE_PLATFORM == DIRTYCODE_PS3
 #define CRIT_SECT_LEN (24)
#elif DIRTYCODE_PLATFORM == DIRTYCODE_PC
 #if defined(_WIN64)
  #define CRIT_SECT_LEN (56)
 #else
  #define CRIT_SECT_LEN (36)
 #endif
#elif DIRTYCODE_PLATFORM == DIRTYCODE_REVOLUTION
 #define CRIT_SECT_LEN (36)
#elif DIRTYCODE_PLATFORM == DIRTYCODE_XENON
 #define CRIT_SECT_LEN (40)
#elif (DIRTYCODE_PLATFORM == DIRTYCODE_ANDROID) || (DIRTYCODE_PLATFORM == DIRTYCODE_APPLEIOS) || (DIRTYCODE_PLATFORM == DIRTYCODE_LINUX) || (DIRTYCODE_PLATFORM == DIRTYCODE_PLAYBOOK)
 #define CRIT_SECT_LEN (72)
#else
 #error critical section length must be defined!
#endif

// if not already defined, define NULL
#ifndef NULL
 #ifdef __cplusplus
  #define NULL 0
 #else
  #define NULL ((void *)0)
 #endif
#endif

#ifndef DIRTYCODE_LOGGING
 #if DIRTYCODE_DEBUG
  //in debug mode logging is defaulted to on
  #define DIRTYCODE_LOGGING (0)
 #else
  //if its not specified then turn it off
  #define DIRTYCODE_LOGGING (0)
 #endif
#endif

// debug printing routines
#if DIRTYCODE_LOGGING
 #define NetPrintf(_x) NetPrintfCode _x
 #define NetPrintfVerbose(_x) NetPrintfVerboseCode _x
 #define NetPrintMem(_pMem, _iSize, _pTitle) NetPrintMemCode(_pMem, _iSize, _pTitle)
 #define NetPrintWrap(_pString, _iWrapCol) NetPrintWrapCode(_pString, _iWrapCol)
#else
 #define NetPrintf(_x) { }
 #define NetPrintfVerbose(_x) { }
 #define NetPrintMem(_pMem, _iSize, _pTitle) { }
 #define NetPrintWrap(_pString, _iWrapCol) { }
#endif

// global module shutdown flags
#define NET_SHUTDOWN_NETACTIVE    (1)   //!< leave network active in preparation for launching to account management (Xbox 360 only)
#define NET_SHUTDOWN_THREADSTARVE (2)   //!< special shutdown mode for PS3 that starves threads, allowing for quick exit to XMB

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

//! critical section definition
typedef struct NetCritT
{
    int32_t data[(CRIT_SECT_LEN+3)/4];  // force int32_t alignment
} NetCritT;

/*** Variables *************************************************************************/

/*** Functions *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
 Portable routines implemented in dirtynet.c
*/

// reset net idle list
void NetIdleReset(void);

// remove a function to the idle callback list.
void NetIdleAdd(void (*proc)(void *ref), void *ref);

// call all the functions in the idle list.
void NetIdleDel(void (*proc)(void *ref), void *ref);

// make sure all idle calls have completed
void NetIdleDone(void);

// add a function to the idle callback list
void NetIdleCall(void);

// print memory as hex (do not call directly; use NetPrintMem() wrapper)
void NetPrintMemCode(const void *pMem, int32_t iSize, const char *pTitle);

// print input buffer with wrapping (do not call directly; use NetPrintWrap() wrapper)
void NetPrintWrapCode(const char *pData, int32_t iWrapCol);

/*
 Platform-specific routines implemented in dirtynet<platform>.c
*/

// initialize the network library functions.
void NetLibCreate(int32_t iThreadPrio);

// shutdown the network library -- DEPRECATE in V9
void NetLibDestroy(void);

// shutdown the network library.
void NetLibDestroy2(uint32_t uShutdownFlags);

// return an increasing tick count with millisecond scale
uint32_t NetTick(void);

// return signed difference between new tick count and old tick count (new - old)
#define NetTickDiff(_uNewTime, _uOldTime) ((signed)((_uNewTime) - (_uOldTime)))

// return 32-bit hash from given input string
int32_t NetHash(const char *pString);

// A simple psuedo-random sequence generator
uint32_t NetRand(uint32_t uLimit);

// diagnostic output routine (do not call directly, use NetPrintf() wrapper
int32_t NetPrintfCode(const char *fmt, ...);

// diagnostic output routine (do not call directly, use NetPrintf() wrapper
void NetPrintfVerboseCode(int32_t iVerbosityLevel, int32_t iCheckLevel, const char *pFormat, ...);

// hook into debug output
#if DIRTYCODE_LOGGING
void NetPrintfHook(int32_t (*pPrintfDebugHook)(void *pParm, const char *pText), void *pParm);
#endif

// initialize a critical section for use -- includes name for verbose debugging on some platforms
void NetCritInit(NetCritT *pCrit, const char *pCritName);

// release resources and destroy critical section
void NetCritKill(NetCritT *pCrit);

// attempt to gain access to critical section
int32_t NetCritTry(NetCritT *pCrit);

// enter a critical section, blocking if needed
void NetCritEnter(NetCritT *pCrit);

// leave a critical section
void NetCritLeave(NetCritT *pCrit);

#ifdef __cplusplus
}
#endif

//@}

#endif // _dirtylib_h

