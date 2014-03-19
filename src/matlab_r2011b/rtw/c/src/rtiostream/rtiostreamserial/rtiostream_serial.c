/*
 * Copyright 1994-2010 The MathWorks, Inc.
 *
 * File: rtiostream_serial.c     $Revision: 1.1.10.3 $
 *
 * Abstract:
 *  Serial communications channel for a Windows PC.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "rtiostream.h"
#include "tmwtypes.h"

#ifdef _WIN32
/* WINDOWS */
# include <windows.h>
# include <assert.h>

# ifdef __LCC__
#   include <errno.h>
#   ifndef MAXDWORD
#      define MAXDWORD 0xFFFFFFFF
#   endif
# endif

#else
#error "This file is only supported on windows machines"
#endif

#ifdef USE_MEXPRINTF
#include "mex.h"
#define printf mexPrintf
#define SERVER_PORT_PRINTF(FORMAT, ARG1) mexPrintf(FORMAT, ARG1)
#else
/* If stdout is redirected to file, it is essential that the port number is 
 * available immediately in the output file. With LCC, printf does not flush 
 * correctly to the redirected output file - use fprintf & fflush instead. */
#define SERVER_PORT_PRINTF(FORMAT, ARG1) fprintf(stdout, FORMAT, ARG1); \
                                         fflush(stdout)
#endif
                                         
/***************** DEFINES ****************************************************/

#define N_SERIAL_PORTS  4       /* Number of com ports allowed to open */
#define MAX_COM_LEN 20          /* length of com port name */
#define TMP_BUF_SIZ (40)
#define BAUD_UNINITIALIZED 0

/***************** TYPEDEFS **************************************************/

typedef struct SerialCommsData_tag {
    char        port[ MAX_COM_LEN];
    HANDLE      serialHandle;
    uint32_T    baud;
} SerialCommsData;

/**************** LOCAL DATA *************************************************/

static SerialCommsData SerialData[N_SERIAL_PORTS];
static bool dataInitialised = false;

static DCB  	     dcb;
static COMMTIMEOUTS cto_timeout   = { MAXDWORD, MAXDWORD, 10000, 0, 0 };
static COMMTIMEOUTS cto_blocking  = { 0, 0, 0, 0, 0 };

/************** LOCAL FUNCTION PROTOTYPES ************************************/

static int serialDataSet(
    SerialCommsData *sd,
    const void *src,
    const size_t size,
    size_t *sizeSent);

static int serialDataGet(
    SerialCommsData *sd,
    char          *dst,
    const size_t   size,
    size_t        *sizeRecvd);

static int serialDataPending(
    SerialCommsData *sd,
    int    *outPending);

static int processArgs(
    const int      argc,
    void         * argv[],
    SerialCommsData *sd);

static void initSerialCommsDataStructure(
    void);

/*************** LOCAL FUNCTIONS **********************************************/

/* Function: initSerialCommsDataStructure ===========================================================
 * Abstract:
 *  Initializes the data structures to zero.
 */
static void initSerialCommsDataStructure(void)
{
    if (!dataInitialised) {
        memset(&SerialData, 0, sizeof( SerialData ) );
        dataInitialised = true;
    }
}

/* Function: initDCB ===========================================================
 * Abstract:
 *  Initializes the control settings for a win32 serial communications device.
 */
static void initDCB(uint32_T baud)
{
    dcb.DCBlength       = sizeof(dcb);                   
        
    /* ---------- Serial Port Config ------- */
    dcb.BaudRate        = baud;
    dcb.Parity          = NOPARITY;
    dcb.fParity         = 0;
    dcb.StopBits        = ONESTOPBIT;
    dcb.ByteSize        = 8;
    dcb.fOutxCtsFlow    = 0;
    dcb.fOutxDsrFlow    = 0;
    dcb.fDtrControl     = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fRtsControl     = RTS_CONTROL_DISABLE;
    dcb.fOutX           = 0;
    dcb.fInX            = 0;
        
    /* ---------- Misc Parameters ---------- */
    dcb.fErrorChar      = 0;
    dcb.fBinary         = 1;
    dcb.fNull           = 0;
    dcb.fAbortOnError   = 0;
    dcb.wReserved       = 0;
    dcb.XonLim          = 2;
    dcb.XoffLim         = 4;
    dcb.XonChar         = 0x13;
    dcb.XoffChar        = 0x19;
    dcb.EvtChar         = 0;

} /* end initDCB */


/* Function: serialDataPending =================================================
 * Abstract:
 *  Returns true, via the 'pending' arg, if data is pending on the comm line.
 *  Returns false otherwise.
 *
 *  RTIOSTREAM_NO_ERROR is returned on success, RTIOSTREAM_ERROR on failure.
 */
static int serialDataPending(
    SerialCommsData *sd,
    int    *outPending)
{
    int retVal = RTIOSTREAM_NO_ERROR;
    struct _COMSTAT status;
    unsigned long   etat;

    assert(sd->serialHandle != NULL);

    /* Find out how much data is available. */
    if (!ClearCommError(sd->serialHandle, &etat, &status)) {
	*outPending = 0;
        retVal = RTIOSTREAM_ERROR;
    } else {
	*outPending = status.cbInQue;
    }

    return retVal;

} /* end serialDataPending */ 


/* Function: serialDataGet =====================================================
 * Abstract:
 *  Attempts to gets the specified number of bytes from the specified serial.
 *  The number of bytes read is returned via the 'sizeRecvd' parameter.
 *  RTIOSTREAM_NO_ERROR is returned on success, RTIOSTREAM_ERROR is returned on
 *  failure.
 *
 * NOTES:
 *  o it is not an error for 'sizeRecvd' to be returned as 0
 *  o this function does NOT block if no data is available
 */
static int serialDataGet(
    SerialCommsData *sd,
    char          *dst,
    const size_t   size,
    size_t        *sizeRecvd)
{
    static const char *fnName = "serialDataGet:";
    int nRead;
    int retVal = RTIOSTREAM_NO_ERROR;
    int avail;

    *sizeRecvd = 0;     /* init for safety */

    assert( sd->serialHandle != NULL);

    retVal = serialDataPending( sd, &avail);

    if (retVal == RTIOSTREAM_ERROR || avail == 0) {
        if (retVal == RTIOSTREAM_ERROR) {
            printf( "%s Pending returned RTIOSTREAM_ERROR\n", fnName);
        }
        return retVal;
    }

    SetCommTimeouts( sd->serialHandle, &cto_timeout );

    if (!ReadFile( sd->serialHandle, dst, (DWORD) size, &nRead, NULL)) {
        printf( "%s ReadFile returned ERROR\n", fnName);
        retVal = RTIOSTREAM_ERROR;
        return retVal;
    }

    *sizeRecvd = nRead;

    return retVal;
} /* end serialDataGet */ 


/* Function: serialDataSet =====================================================
 * Abstract:
 *  Utility function to send data via the specified serial
 */
static int serialDataSet(
    SerialCommsData *sd,
    const void *src,
    const size_t size,
    size_t *sizeSent)
{
    static const char *fnName = "serialDataSet:";
    int retVal = RTIOSTREAM_NO_ERROR;
    
    if (!WriteFile( sd->serialHandle, src, (DWORD) size, (LPDWORD) sizeSent, NULL)){
        printf( "%s WriteFile returned ERROR\n", fnName);
        retVal = RTIOSTREAM_ERROR;
    }

    return retVal;
}

/* Function: serialDataFlush =================================================
 * Abstract:
 *  Utility function to flush a port
 */
static int serialDataFlush( 
    SerialCommsData *sd)
{
    char tmpBuf[TMP_BUF_SIZ];
    int numRecvd;
    int pending = 0;
    int error;
    static const char *fnName = "serialDataFlush:";

    do {
        error = serialDataPending( sd, &pending);

        if ( (pending > 0) && (error==RTIOSTREAM_NO_ERROR) ) {
            error = serialDataGet( sd, tmpBuf, sizeof( tmpBuf), (size_t *) (&numRecvd));
        }
    }  while (  (pending > 0) && (error==RTIOSTREAM_NO_ERROR) && numRecvd > 0);
    if (error == RTIOSTREAM_ERROR) {
        printf( "%s Flushing returned RTIOSTREAM_ERROR\n", fnName);
    }
    return error;
}

/* Function: processArgs ====================================================
 * Abstract:
 *  Process the arguments specified by the user when opening the rtIOStream.
 *      
 *  If any unrecognized options are encountered, ignore them.
 *
 * Returns zero if successful or RTIOSTREAM_ERROR if 
 * an error occurred.
 *
 *  o IMPORTANT!!!
 *    As the arguments are processed, their strings should be set to NULL in
 *    the argv array. 
 */
static int processArgs(
    const int      argc,
    void         * argv[],
    SerialCommsData *sd)
{
    int        retVal    = RTIOSTREAM_NO_ERROR;
    int        count           = 0;
    
    while(count < argc) {
        const char *option = (char *)argv[count];
        count++;
        
        if (option != NULL) {
            
            if ((strcmp(option, "-port") == 0) && (count != argc)) {
                const char *portStr = (char *)argv[count];
                
                count++;     
                
                if ( portStr == NULL || (strlen (portStr) > MAX_COM_LEN - 1)) {
                    retVal = RTIOSTREAM_ERROR;
                    break;
                } else {
                    strcpy( sd->port, portStr);
                    argv[count-2] = NULL;
                    argv[count-1] = NULL;
                }           
                
            } else if ((strcmp(option, "-baud") == 0) && (count != argc)) {
                int itemsConverted;
                const char *baudStr = (char *)argv[count];
                
                count++;

                itemsConverted = sscanf(baudStr,"%u", &sd->baud);

                if ( itemsConverted != 1) {
                    retVal = RTIOSTREAM_ERROR;
                    break;
                } else {
                    argv[count-2] = NULL;
                    argv[count-1] = NULL;
                }

            } else {
                /* do nothing */
            }
        }
    }
    return retVal;
}

/***************** VISIBLE FUNCTIONS ******************************************/

/* Function: rtIOStreamOpen =================================================
 * Abstract:
 *  Open the connection with the target.
 */
int rtIOStreamOpen(int argc, void * argv[])
{
    unsigned int        comNum     = 0;
    int result = RTIOSTREAM_NO_ERROR;
    int streamID;
    HANDLE serialHandle;
    int closeFile = false;
    SerialCommsData *sd;

    initSerialCommsDataStructure();

    for (streamID = 0; streamID < N_SERIAL_PORTS; streamID++) {
        if (SerialData[streamID].port[0] == 0) {
            break;
        }
    }
    if (streamID == N_SERIAL_PORTS) {
        printf( "All %d elements of the SerialCommsData structure are already in use.",  N_SERIAL_PORTS );
        return RTIOSTREAM_ERROR;
    }

    sd = &SerialData[streamID];

    sd->baud = BAUD_UNINITIALIZED;

    processArgs(argc, argv, sd);

    if (sd->baud == BAUD_UNINITIALIZED) {
        printf( "A valid bit rate must be specified via the -baud option.");
        sd->port[0] = 0;
        return RTIOSTREAM_ERROR;
    }

    initDCB( sd->baud );

    serialHandle = (void *) CreateFile( sd->port, 
                                        GENERIC_READ | GENERIC_WRITE,
                                        0, NULL, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, NULL);

    if (serialHandle == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError( );
        printf( "CreateFile failed, error %d or 0x%08x. One possible "
                "cause is that COM ports COM10 or greater must be "
                "identified by a fully qualified name, e.g. "
                "\\\\.\\COM10.\n", err, err);
        sd->port[0] = 0;
        streamID = RTIOSTREAM_ERROR;
        goto EXIT_POINT;
    }
    sd->serialHandle = serialHandle;

    if (!SetCommTimeouts(serialHandle, &cto_blocking)) {
        printf( "SetCommTimeouts failed\n");
        streamID = RTIOSTREAM_ERROR;
        closeFile = true;
        goto EXIT_POINT;
    }

    if (!SetCommState(serialHandle, &dcb)) {
        printf( "SetCommState failed\n");
        streamID = RTIOSTREAM_ERROR;
        closeFile = true;
	goto EXIT_POINT;
    }

    result = serialDataFlush( sd);      /* clear out old data on the port */
    if (result == RTIOSTREAM_ERROR) {
        printf( "serialDataFlush failed\n");
        streamID = RTIOSTREAM_ERROR;
        closeFile = true;
	goto EXIT_POINT;
    }

  EXIT_POINT:
    if (closeFile) {
        CloseHandle(serialHandle);
        sd->port[0] = 0;
    }
    return streamID;
}



/* Function: rtIOStreamSend =====================================================
 * Abstract:
 *  Sends the specified number of bytes on the comm line. Returns the number of
 *  bytes sent (if successful) or a negative value if an error occurred. As long
 *  as an error does not occur, this function is guaranteed to set the requested
 *  number of bytes; the function blocks if the TCP/IP send buffer doesn't have
 *  room for all of the data to be sent
 */
int rtIOStreamSend(
    int streamID,
    const void *src,
    size_t size,
    size_t *sizeSent)
{
    int retVal;
    SerialCommsData *sd;

    sd = &SerialData[streamID];

    retVal = serialDataSet( sd, src, size, sizeSent);

    return retVal;
}


/* Function: rtIOStreamRecv ================================================
 * Abstract: receive data
 *
 */
int rtIOStreamRecv(
    int      streamID,
    void   * dst, 
    size_t   size,
    size_t * sizeRecvd) 
{
    int retVal = RTIOSTREAM_NO_ERROR;
    SerialCommsData *sd;

    sd = &SerialData[streamID];

    retVal = serialDataGet( sd, (char *)dst, size, sizeRecvd);

    return retVal;
}


/* Function: rtIOStreamClose ================================================
 * Abstract: close the connection.
 *
 */
int rtIOStreamClose(int streamID)
{
    int retVal = RTIOSTREAM_NO_ERROR;
    SerialCommsData *sd;
    sd = &SerialData[streamID];
    
    retVal = serialDataFlush( sd);

    CloseHandle(sd->serialHandle);
    sd->serialHandle = 0;

    sd->port[0] = 0;

    return retVal;
}


