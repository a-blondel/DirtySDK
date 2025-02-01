/*H********************************************************************************/
/*!
    \File dirtymem.h

    \Description
        DirtySock memory allocation routines.

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 10/12/2005 (jbrookes) First Version
    \Version 11/19/2008 (mclouatre) Adding pMemGroupUserData to mem groups
*/
/********************************************************************************H*/

#ifndef _dirtymem_h
#define _dirtymem_h

/*** Include files ****************************************************************/

// included for DIRTYCODE_DEBUG definition
#include "platform.h"

/*** Defines **********************************************************************/

/*!
    All DirtySock modules have their memory identifiers defined here.
*/

// buddy modules
#define BUDDYAPI_MEMID          ('budd')
#define CLUBAPI_MEMID           ('club')
#define HLBUDAPI_MEMID          ('hbud')
#define XBFRIENDS_MEMID         ('xbfr')
#define XNFRIENDS_MEMID         ('xnfr')

// contrib modules
#define PCISP_MEMID             ('pcip')

// comm modules
#define COMMSER_MEMID           ('cser')
#define COMMSRP_MEMID           ('csrp')
#define COMMTCP_MEMID           ('ctcp')
#define COMMUDP_MEMID           ('cudp')

// dirtysock modules
#define DIRTYLSP_MEMID          ('dlsp')
#define DIRTYSESSMGR_MEMID      ('dsmg')
#define SOCKET_MEMID            ('dsoc')
#define RPC_MEMID               ('drpc')
#define NETCONN_MEMID           ('ncon')

// friends modules
#define FRIENDAPI_MEMID         ('frnd')

// game modules
#define CONNAPI_MEMID           ('conn')
#define NETGAMEDIST_MEMID       ('ngdt')
#define NETGAMEDISTSERV_MEMID   ('ngds')
#define NETGAMELINK_MEMID       ('nglk')
#define NETGAMEUTIL_MEMID       ('ngut')

// graph modules
#define DIRTYGRAPH_MEMID        ('dgph')
#define DIRTYJPG_MEMID          ('djpg')
#define DIRTYPNG_MEMID          ('dpng')

// lobby modules
#define FAVORITES_MEMID         ('favr')
#define GAMECONFIGAPI_MEMID     ('gcfg')
#define GAMEMANAGER_MEMID       ('gmgr')
#define USERSETMANAGER_MEMID    ('umgr')
#define LEAGAPI_MEMID           ('leag')
#define NEW_TOURNEY_MEMID       ('ntou')
#define ONLINE_LEAGAPI_MEMID    ('lolg')
#define LOBBYAPI_MEMID          ('lapi')
#define LOBBYAPIASYNC_MEMID     ('lasc')
#define LOBBYAPILIST_MEMID      ('llst')
#define LOBBYCHAL_MEMID         ('lchl')
#define LOBBYFINDUSER_MEMID     ('lfus')
#define LOBBYGLUE_MEMID         ('lglu')
#define BUDDYTICKERGLUE_MEMID   ('btgl')
#define LOBBYLOCALIZE_MEMID     ('lloc')
#define LOBBYLOGGER_MEMID       ('llgr')
#define LOBBYLOGIN_MEMID        ('llgn')
#define MSGLOG_MEMID            ('lmlg')
#define LOBBYRANK_MEMID         ('lrnk')
#define LOBBYREG_MEMID          ('lreg')
#define LOBBYSETTING_MEMID      ('lset')
#define LOBBYSTATBOOK_MEMID     ('lsta')
#define BLAZESTATS_MEMID        ('bzst')
#define LOBBYROOMLIST_MEMID     ('lrml')
#define TOURNEYAPI_MEMID        ('tour')
#define LOBBYAPIPLAT_MEMID      ('lapp')

// misc modules
#define LOBBYLAN_MEMID          ('llan')
#define PORTTESTER_MEMID        ('ptst')
#define TELEMETRYAPI_MEMID      ('telm')
#define TICKERAPI_MEMID         ('tick')

// proto modules
#define PROTOADVT_MEMID         ('padv')
#define PROTOARIES_MEMID        ('pari')
#define PROTOHTTP_MEMID         ('phtp')
#define HTTPMGR_MEMID           ('hmgr')
#define PROTOMANGLE_MEMID       ('pmgl')
#define PROTONAME_MEMID         ('pnam')
#define PROTOPING_MEMID         ('ppng')
#define PINGMGR_MEMID           ('lpmg')
#define PROTOSSL_MEMID          ('pssl')
#define PROTOSTREAM_MEMID       ('pstr')
#define PROTOTUNNEL_MEMID       ('ptun')
#define PROTOUDP_MEMID          ('pudp')
#define PROTOUPNP_MEMID         ('pupp')
#define PROTOFILTER_MEMID       ('pflt')

// util modules
#define DISPLIST_MEMID          ('ldsp')
#define HASHER_MEMID            ('lhsh')
#define LOBBYSORT_MEMID         ('lsor')

// qos module
#define QOSAPI_MEMID            ('dqos')

// voip module
#define VOIP_MEMID              ('voip')

// voiptunnel module
#define VOIPTUNNEL_MEMID        ('vtun')

// web modules
#define DIGOBJAPI_MEMID         ('dobj')
#define LOCKERAPI_MEMID         ('lckr')
#define NETRSRC_MEMID           ('nrsc')
#define WEBLOG_MEMID            ('wlog')
#define WEBOFFER_MEMID          ('webo')

// xml modules
#define XMLLIST_MEMID           ('xmll')

// Nintendo Wii network modules
#define NINSOC_MEMID            ('nsoc')
#define NINDWC_MEMID            ('ndwc')

// external packages
#define PLAYERSYNCSERVICE_MEMID ('plss')  // used by PlayerSyncSDK package


/*** Macros ***********************************************************************/

#if !DIRTYCODE_DEBUG
 #define DirtyMemDebugAlloc(_pMem, _iSize, _iMemModule, _iMemGroup, _pMemGroupUserData) {;}
 #define DirtyMemDebugFree(_pMem, _iSize, _iMemModule, _iMemGroup, _pMemGroupUserData) {;}
#endif

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//! enter memory group
void DirtyMemGroupEnter(int32_t iGroup, void *pMemGroupUserData);

//! leave memory group
void DirtyMemGroupLeave(void);

//! get current memory group
void DirtyMemGroupQuery(int32_t *pMemGroup, void **ppMemGroupUserData);

#if DIRTYCODE_DEBUG
//! display memory allocation info to debug output (debug only)
void DirtyMemDebugAlloc(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

//! display memory free info to debug output (debug only)
void DirtyMemDebugFree(void *pMem, int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);
#endif

/*
 Memory allocation routines - not implemented in the lib; these must be supplied by the user
*/

//! allocate memory
void *DirtyMemAlloc(int32_t iSize, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

//! free memory
void DirtyMemFree(void *pMem, int32_t iMemModule, int32_t iMemGroup, void *pMemGroupUserData);

#ifdef __cplusplus
}
#endif

#endif // _dirtymem_h

