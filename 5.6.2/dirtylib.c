/*H********************************************************************************/
/*!
    \File dirtylib.c

    \Description
        Platform independent routines for support library for network code.

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 09/15/1999 (gschaefer) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dirtysock.h"

// defined in dirtylib<platform>.c
extern NetCritT *_NetLib_pIdleCrit;

/*** Defines **********************************************************************/

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

static int32_t _NetLib_iIdleSize = 0;

// idle task list
static struct
{
    void (*pProc)(void *pRef);
    void *pRef;
} _NetLib_IdleList[32];

#if DIRTYCODE_LOGGING
//! instantiation of the platform print function
int (*_Platform_pLogPrintf)(const char *pFmt, ...) = NetPrintfCode;
#endif

/*** Private Functions ************************************************************/

/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function NetIdleReset

    \Description
        Reset idle function count.

    \Version 06/21/2006 (jbrookes)
*/
/********************************************************************************F*/
void NetIdleReset(void)
{
    _NetLib_iIdleSize = 0;
}

/*F********************************************************************************/
/*!
    \Function NetIdleAdd

    \Description
        Add a function to the idle callback list. The functions are called whenever
        NetIdleCall() is called.

    \Input *pProc   - callback function pointer
    \Input *pRef    - function specific parameter

    \Version 09/15/1999 (gschaefer)
*/
/********************************************************************************F*/
void NetIdleAdd(void (*pProc)(void *pRef), void *pRef)
{
    // make sure proc is valid
    if (pProc == NULL)
    {
        NetPrintf(("dirtylib: attempt to add an invalid idle function\n"));
        return;
    }

    // add item to list
    _NetLib_IdleList[_NetLib_iIdleSize].pProc = pProc;
    _NetLib_IdleList[_NetLib_iIdleSize].pRef = pRef;
    _NetLib_iIdleSize += 1;
}

/*F********************************************************************************/
/*!
    \Function NetIdleDel

    \Description
        Remove a function from the idle callback list.

    \Input *pProc   - callback function pointer
    \Input *pRef    - function specific parameter

    \Version 09/15/1999 (gschaefer)
*/
/********************************************************************************F*/
void NetIdleDel(void (*pProc)(void *pRef), void *pRef)
{
    int32_t iProc;

    // make sure proc is valid
    if (pProc == NULL)
    {
        NetPrintf(("dirtylib: attempt to delete an invalid idle function\n"));
        return;
    }

    // mark item as deleted
    for (iProc = 0; iProc < _NetLib_iIdleSize; ++iProc)
    {
        if ((_NetLib_IdleList[iProc].pProc == pProc) && (_NetLib_IdleList[iProc].pRef == pRef))
        {
            _NetLib_IdleList[iProc].pProc = NULL;
            _NetLib_IdleList[iProc].pRef = NULL;
            break;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function NetIdleDone

    \Description
        Make sure all idle calls have completed

    \Version 09/15/1999 (gschaefer)
*/
/********************************************************************************F*/
// void NetIdleDone(void)
// {
//     NetCritEnter(_NetLib_pIdleCrit);
//     NetCritLeave(_NetLib_pIdleCrit);
// }

/*F********************************************************************************/
/*!
    \Function NetIdleCall

    \Description
        Call all of the functions in the idle list.

    \Version 09/15/1999 (gschaefer)
*/
/********************************************************************************F*/
// void NetIdleCall(void)
// {
//     int32_t iProc;

//     // only do idle call if we have control
//     if (NetCritTry(_NetLib_pIdleCrit))
//     {
//         // walk the table calling routines
//         for (iProc = 0; iProc < _NetLib_iIdleSize; ++iProc)
//         {
//             // get pProc pointer
//             void (*pProc)(void *pRef) = _NetLib_IdleList[iProc].pProc;
//             void *pRef = _NetLib_IdleList[iProc].pRef;

//             /* if pProc is deleted, handle removal here (this
//                helps prevent corrupting table in race condition) */
//             if (pProc == NULL)
//             {
//                 // swap with final element
//                 _NetLib_IdleList[iProc].pProc = _NetLib_IdleList[_NetLib_iIdleSize-1].pProc;
//                 _NetLib_IdleList[iProc].pRef = _NetLib_IdleList[_NetLib_iIdleSize-1].pRef;
//                 _NetLib_IdleList[_NetLib_iIdleSize-1].pProc = NULL;
//                 _NetLib_IdleList[_NetLib_iIdleSize-1].pRef = NULL;

//                 // drop the item count
//                 _NetLib_iIdleSize -= 1;

//                 // restart the loop at new current element
//                 --iProc;
//                 continue;
//             }
//             // perform the idle call
//             (*pProc)(pRef);
//         }
//         // done with critical section
//         NetCritLeave(_NetLib_pIdleCrit);
//     }
// }

/*F********************************************************************************/
/*!
    \Function NetHash

    \Description
        Calculate a unique 32-bit hash based on the given input string.

    \Input  *pString    - pointer to string to calc hash of

    \Output
        int32_t         - resultant 32bit hash

    \Version 2.0 07/26/2011 (jrainy) rewrite to lower collision rate
    
    \v5.6.2 reimplementation
*/
/********************************************************************************F*/
int32_t NetHash(const char *pString)
{
    uint32_t uHash = 0;
    for (const char *pChar = pString; *pChar != '\0'; pChar++) {
        uHash = (int)*pChar ^ (int)uHash >> 0x1b ^ uHash << 5;
    }
    return (int32_t)uHash;
}

/*F*************************************************************************************************/
/*!
    \Function NetRand

    \Description
        A simple pseudo-random sequence generator. The sequence is implicitly seeded in the first
        call with the millisecond tick count at the time of the call

    \Input uLimit   - upper bound of pseudo-random number output

    \Output
        uint32_t     - pseudo-random number from [0...(uLimit - 1)]

    \Version 06/25/2009 (jbrookes)
*/
/*************************************************************************************************F*/
// uint32_t NetRand(uint32_t uLimit)
// {
//     static uint32_t _aRand = 0;
//     if (_aRand == 0)
//     {
//         _aRand = NetTick();
//     }
//     if (uLimit == 0)
//     {
//         return(0);
//     }
//     _aRand = (_aRand * 125) % 2796203;
//     return(_aRand % uLimit);
// }


/*F********************************************************************************/
/*!
    \Function NetPrintfVerboseCode

    \Description
        Display input data if iVerbosityLevel is > iCheckLevel

    \Input iVerbosityLevel  - current verbosity level
    \Input iCheckLevel      - level to check against
    \Input *pFormat         - format string

    \Version 12/20/2005 (jbrookes)
*/
/********************************************************************************F*/
#if DIRTYCODE_LOGGING
void NetPrintfVerboseCode(int32_t iVerbosityLevel, int32_t iCheckLevel, const char *pFormat, ...)
{
    va_list Args;
    char strText[1024];

    va_start(Args, pFormat);
    ds_vsnprintf(strText, sizeof(strText), pFormat, Args);
    va_end(Args);

    if (iVerbosityLevel > iCheckLevel)
    {
        NetPrintf(("%s", strText));
    }
}
#endif

/*F********************************************************************************/
/*!
    \Function NetPrintWrapCode

    \Description
        Display input data with wrapping.

    \Input *pString - pointer to packet data to display
    \Input iWrapCol - number of columns to wrap at

    \Version 09/15/2005 (jbrookes)
*/
/********************************************************************************F*/
#if DIRTYCODE_LOGGING
void NetPrintWrapCode(const char *pString, int32_t iWrapCol)
{
    const char *pTemp, *pEnd, *pEqual, *pSpace;
    char strTemp[256] = "   ";
    uint32_t bDone;
    int32_t iLen;

    // loop through whole packet
    for (bDone = FALSE; bDone == FALSE; )
    {
        // scan forward, tracking whitespace, linefeeds, and equal signs
        for (pTemp=pString, pEnd=pTemp+iWrapCol, pSpace=NULL, pEqual=NULL; (pTemp < pEnd); pTemp++)
        {
            // remember most recent whitespace
            if ((*pTemp == ' ') || (*pTemp == '\t'))
            {
                pSpace = pTemp;
            }

            // remember most recent equal sign
            if (*pTemp == '=')
            {
                pEqual = pTemp;
            }

            // if eol or eos, break here
            if ((*pTemp == '\n') || (*pTemp == '\0'))
            {
                break;
            }
        }

        // scanned an entire line?
        if (pTemp == pEnd)
        {
            // see if we have whitespace to break on
            if (pSpace != NULL)
            {
                pTemp = pSpace;
            }
            // see if we have an equals to break on
            else if (pEqual != NULL)
            {
                pTemp = pEqual;
            }
        }

        // format string for output
        iLen = pTemp - pString + 1;
        strncpy(strTemp + 3, pString, iLen);
        if (*pTemp == '\0')
        {
            strTemp[iLen+2] = '\n';
            strTemp[iLen+3] = '\0';
            bDone = TRUE;
        }
        else if ((*pTemp != '\n') && (*pTemp != '\r'))
        {
            strTemp[iLen+3] = '\n';
            strTemp[iLen+4] = '\0';
        }
        else
        {
            strTemp[iLen+3] = '\0';
        }

        // print it out
        NetPrintf(("%s", strTemp));

        // increment to next line
        pString += iLen;
    }
}
#endif // #if DIRTYCODE_LOGGING

/*F*************************************************************************************************/
/*!
    \Function    NetPrintMemCode

    \Description
        Dump memory to debug output

    \Input *pMem    - pointer to memory to dump
    \Input iSize    - size of memory block to dump
    \Input *pTitle  - pointer to title of memory block

    \Version    1.0        05/17/05 (jbrookes) First Version
*/
/*************************************************************************************************F*/
#if DIRTYCODE_LOGGING
void NetPrintMemCode(const void *pMem, int32_t iSize, const char *pTitle)
{
    static const char _hex[] = "0123456789ABCDEF";
    char strOutput[128];
    int32_t iBytes, iOutput = 2;

    memset(strOutput, ' ', sizeof(strOutput)-1);
    strOutput[sizeof(strOutput)-1] = '\0';

    NetPrintf(("dirtylib: dumping memory for object %s (%d bytes)\n", pTitle, iSize));

    for (iBytes = 0; iBytes < iSize; iBytes++, iOutput += 2)
    {
        unsigned char cByte = ((unsigned char *)pMem)[iBytes];
        strOutput[iOutput]   = _hex[cByte>>4];
        strOutput[iOutput+1] = _hex[cByte&0xf];
        strOutput[(iOutput/2)+40] = isprint(cByte) ? cByte : '.';
        if (iBytes > 0)
        {
            if (((iBytes+1) % 16) == 0)
            {
                strOutput[(iOutput/2)+40+1] = '\0';
                NetPrintf(("%s\n", strOutput));
                memset(strOutput, ' ', sizeof(strOutput)-1);
                strOutput[sizeof(strOutput)-1] = '\0';
                iOutput = 0;
            }
            else if (((iBytes+1) % 4) == 0)
            {
                iOutput++;
            }
        }
    }

    if ((iBytes % 16) != 0)
    {
        strOutput[(iOutput/2)+40+1] = '\0';
        NetPrintf(("%s\n", strOutput));
    }
}
#endif
