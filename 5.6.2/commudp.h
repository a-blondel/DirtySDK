#ifndef _commudp_h
#define _commudp_h

// basic reference returned/used by all routines
typedef struct CommUDPRef CommUDPRef;


// construct the class
CommUDPRef *CommUDPConstruct(int32_t maxwid, int32_t maxinp, int32_t maxout);

// destruct the class
void CommUDPDestroy(CommUDPRef *what);

// resolve an address
int32_t CommUDPResolve(CommUDPRef *what, const char *addr, char *buf, int32_t len, char iDiv);

// resolve an address
void CommUDPUnresolve(CommUDPRef *what);

// listen for a connection
int32_t CommUDPListen(CommUDPRef *what, const char *addr);

// stop listening
int32_t CommUDPUnlisten(CommUDPRef *what);

// initiate a connection to a peer
int32_t CommUDPConnect(CommUDPRef *what, const char *addr);

// terminate a connection
int32_t CommUDPUnconnect(CommUDPRef *what);

// set event callback hook
void CommUDPCallback(CommUDPRef *what, void (*callback)(void *ref, int32_t event));

// return current stream status
int32_t CommUDPStatus(CommUDPRef *what);

// control connection behavior
int32_t CommUDPControl(CommUDPRef *pRef, int32_t iControl, int32_t iValue, void *pValue);

// return current clock tick
uint32_t CommUDPTick(CommUDPRef *what);

// send a packet
int32_t CommUDPSend(CommUDPRef *what, const void *buffer, int32_t length, uint32_t flags);

// peek at waiting packet
int32_t CommUDPPeek(CommUDPRef *what, void *target, int32_t length, uint32_t *when);

// receive a packet from the buffer
int32_t CommUDPRecv(CommUDPRef *what, void *target, int32_t length, uint32_t *when);

#endif // _commudp_h
