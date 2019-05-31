/*=========================================================================
                       NTAG FTM C File
Description
   This file contains the definitions of the functions
   used to communicate with NTAG Chip.

Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#define NTAG_FTM_DEBUG
#include "ftm_ntag.h"

/* Global variables */
int fdNtag = 0;

/*=========================================================================
FUNCTION   ftm_ntag_close

DESCRIPTION
  Close the kernel driver for the NTAG Chip

PARAMETERS
  None

RETURN VALUE
  int
===========================================================================*/
int ftm_ntag_close( void )
{
    fdNtag = close( fdNtag );                         // close the file descriptor
    LOG_NTAG_MESSAGE( "%s : Exit with fdNtag = %d\n", __func__, fdNtag );
    return fdNtag;                                   // return the result
}

/*=========================================================================
FUNCTION   ftm_ntag_open

DESCRIPTION
  Open the kernel driver for the NTAG Chip

PARAMETERS
  None

RETURN VALUE
  int
===========================================================================*/
int ftm_ntag_open( void )
{
    fdNtag = open( "/dev/nq-ntag", O_RDWR);           // try to open /dev/nq-ntag
    if( fdNtag < 0 )                             // successful?
    {
        LOG_NTAG_ERROR( "\n\t %s: either I2C issue or HW not present = %d\n",
                        __func__, fdNtag );
    }
    return fdNtag;                                   // return the result
}

/*=========================================================================
FUNCTION   PrintNtagData

DESCRIPTION
  Print bytes from an array

PARAMETERS
  char *buf - Byte array to print
  uint8_t  len - Length of the array
RETURN VALUE
  void
===========================================================================*/
void PrintNtagData( char *buf, uint8_t len)
{
#ifdef NTAG_FTM_DEBUG
    int idx;

    LOG_NTAG_MESSAGE( "\n %s: Length: %d bytes \n", __func__, len );    // print the number of bytes
    for( idx = 0; idx < len; idx++ )                            // print every byte
    {
        LOG_NTAG_MESSAGE( "%02x ", buf[idx] );
    }
    LOG_NTAG_MESSAGE( "\n" );
#else
    UNUSED_PARAMETER( buf );
    UNUSED_PARAMETER( len );
#endif
}

/*=========================================================================
FUNCTION   ftm_ntag_read

DESCRIPTION
  Reads a message from the chip

PARAMETERS
  int      len - the length of the buffer

RETURN VALUE
  int      ret - Number of bytes read
===========================================================================*/
int ftm_ntag_read( char* buf, int len )
{
    int ret = NTAG_ERROR;

    ret = read( fdNtag, &buf[0], len );                 // try to read
    return ret;
}
/*=========================================================================
FUNCTION   ftm_ntag_write

DESCRIPTION
  Writes a message to the chip

PARAMETERS
  int      len - the length of the buffer

RETURN VALUE
  int      ret - Number of bytes written
===========================================================================*/
int ftm_ntag_write( char* buf, int len )
{
   int ret = NTAG_ERROR;

   ret = write( fdNtag, buf, len );                 // try to read
   return ret;
}
/*===========================================================================
FUNCTION
    ntag usage

DESCRIPTION
    Print usage information for test

PARAMETERS

RETURN VALUE
    void
===========================================================================*/
void ntag_usage()
{
    LOG_NTAG_MESSAGE("\nUsage:");
    LOG_NTAG_MESSAGE("[-r] [-n] [-h] \n");
    LOG_NTAG_MESSAGE(" -r ..for NTAG I2C Read/Write test \n");
    LOG_NTAG_MESSAGE(" -n ..for NTAG FD Test \n");
    LOG_NTAG_MESSAGE(" -h HELP\n");
}
/*==========================================================================
FUNCTION
    ntagi2creadwrite test

DESCRIPTION
    Send  commands to NTAG for reading/writing data

PARAMETERS

RETURN VALUE
    0 if read/write is successful
===========================================================================*/
int ntagi2creadwritetest()
{
    char  *readdata,*tmpdata,*testdata;
    const char testwritedata[BUFFER_SIZE] = {0x83,0x8F,0x13,0x77,0xC1,0x01,0x00,
                                             0x00,0x03,0x70,0x54,0x02,0x65,0x6E,
                                             0x4E,0x00};
    int ret = NTAG_ERROR;

    ret = ioctl( fdNtag, NTAG_SET_OFFSET, 0 );
    if( ret != 0 )
    {
        ret = NTAG_IOCTL_FAIL;
        LOG_NTAG_ERROR( "\n\t %s: IOCTL fail, ret = %d\n", __func__, ret );
        return ret;
    }
    readdata = (char*) (calloc( BUFFER_SIZE,sizeof(char) ));
    if( readdata == NULL )
    {
        ret = NTAG_NULL_POINTER;
        LOG_NTAG_ERROR( "\n\t %s: Mem alloc fail, ret = %d\n", __func__, ret );
        return ret;
    }
    tmpdata = (char*) (calloc( BUFFER_SIZE,sizeof(char) ));
    if( tmpdata == NULL )
    {
        ret = NTAG_NULL_POINTER;
        LOG_NTAG_ERROR( "\n\t %s: Mem alloc fail, ret = %d\n", __func__, ret );
        free(readdata);
        return ret;
    }
    ret = ftm_ntag_read( readdata, BUFFER_SIZE );
    //Reading from user unprotected memory block 0
    LOG_NTAG_MESSAGE( "\n %s: Read from User Memory Offset 0 \n", __func__ );
    if( ret != BUFFER_SIZE )
    {
        ret = NTAG_INVALID_LENGTH;
        LOG_NTAG_ERROR( "\n\t %s: Read fail, ret = %d\n", __func__, ret );
        free( tmpdata );
        free( readdata );
        return ret;
    }
    PrintNtagData( readdata, BUFFER_SIZE );
    //Setting offset to user unprotected memory block 1
    ret = ioctl( fdNtag, NTAG_SET_OFFSET, 1 );
    if( ret != 0 )
    {
        ret = NTAG_IOCTL_FAIL;
        LOG_NTAG_ERROR( "\n\t %s: IOCTL fail, ret = %d\n", __func__, ret );
        free( tmpdata );
        free( readdata );
        return ret;
    }
    //Writing Zeros to user unprotected memory block 1
    LOG_NTAG_MESSAGE( "\n %s: Write Zeros to User Memory Offset 1 \n",
                      __func__ );
    ret = ftm_ntag_write( tmpdata, BUFFER_SIZE );
    sleep( WRITEDELAY );
    if( ret != BUFFER_SIZE )
    {
       ret = NTAG_INVALID_LENGTH;
       LOG_NTAG_ERROR( "\n\t %s: Write fail, ret = %d\n", __func__, ret );
       free( tmpdata );
       free( readdata );
       return ret;
    }
    LOG_NTAG_MESSAGE( "\n %s: Read from User Memory Offset 1 \n",
                      __func__ );
    //Reading from user unprotected memory block 1
    ret = ftm_ntag_read( readdata, BUFFER_SIZE );
    if( ret != BUFFER_SIZE )
    {
       ret = NTAG_INVALID_LENGTH;
       LOG_NTAG_ERROR( "\n\t %s: Read fail, ret = %d\n", __func__, ret );
       free( tmpdata );
       free( readdata );
       return ret;
    }
    PrintNtagData( tmpdata, BUFFER_SIZE );
    free( tmpdata );
    testdata = (char*) (calloc( BUFFER_SIZE,sizeof(char) ));
    if( testdata == NULL )
    {
       LOG_NTAG_ERROR( "\n\t %s: Mem alloc fail, ret = %d\n", __func__, ret );
       free( readdata );
       return ret;
    }
    memcpy( testdata, &testwritedata, BUFFER_SIZE );
    //Writing Test Data to user unprotected memory block 1
    LOG_NTAG_MESSAGE( "\n %s: Write test data to User Memory Offset 1 \n",
                      __func__ );
    ret = ftm_ntag_write( testdata , BUFFER_SIZE );
    if( ret != BUFFER_SIZE )
    {
       ret = NTAG_INVALID_LENGTH;
       LOG_NTAG_ERROR( "\n\t %s: Write fail, ret = %d\n", __func__, ret );
       free( readdata );
       free( testdata );
       return ret;
    }
    sleep( WRITEDELAY );
    LOG_NTAG_MESSAGE( "\n %s: Read test data from User Memory Offset 1 \n",
                      __func__ );
    ret = ftm_ntag_read( readdata, BUFFER_SIZE );
    //Reading Test Data from user unprotected memory block 1
    if( ret != BUFFER_SIZE )
    {
      ret = NTAG_INVALID_LENGTH;
      LOG_NTAG_ERROR( "\n\t %s: Read fail, ret = %d\n", __func__, ret );
      free( readdata );
      free( testdata );
      return ret;
     }
     PrintNtagData( readdata, BUFFER_SIZE );
     free( readdata );
     free( testdata );
     return NTAG_SUCCESS;
}
/*============================================================================
FUNCTION
    ntagfd test

DESCRIPTION
    enable fd interrupt and wait till interrupt is received

PARAMETERS

RETURN VALUE
   0 if success
=============================================================================*/
int ntagfdtest()
{
    int ret = NTAG_ERROR;

    ret = ioctl( fdNtag, NTAG_FD_STATE, 1 );
    return ret;
}
/*=========================================================================
FUNCTION   ftm_ntag_dispatch_test

DESCRIPTION
  called by main() in ftm_main.c to start the nfc test routine

PARAMETERS
  int argc - argument count
  char **argv - argument vector

RETURN VALUE
  0 if success
===========================================================================*/
int ftm_ntag_dispatch_test( int argc, char **argv )
{
    int ret = 0;
    int testtype = 0;

    if( !fdNtag )                                 // Already initialized?
    {
        ret = ftm_ntag_open( );                   // open the kernel driver
        if( ret < 0 )                             // successful?
        {
            LOG_NTAG_ERROR( "\n\t %s: nq_ntag_open() failed = %d\n",
                            __func__, ret );
            return ret;
        }
    }
    testtype = getopt( argc, argv, "rnh" );
    switch (testtype) {
        case 'r':
                LOG_NTAG_MESSAGE("NTAG I2C Read/Write test \n");
                ret = ntagi2creadwritetest();
                if (ret == 0 )
                {
                    LOG_NTAG_MESSAGE("Read/Write Completed Successfully \n");
                }
                else {
                    LOG_NTAG_MESSAGE("NTAG I2C Read/Write Failed \n");
                }
        break;
        case 'n':
                LOG_NTAG_MESSAGE("NTAG FD Test \n");
                ret = ntagfdtest();
                if (ret == 0 )
                {
                    LOG_NTAG_MESSAGE("Interrupt received successfully \n");
                }
                else {
                    LOG_NTAG_MESSAGE("Failed to receive Interrupt \n");
                }
        break;
        case 'h':
                ntag_usage();
        break;
        default:
                ntag_usage();
                LOG_NTAG_MESSAGE("NTAG I2C Read/Write default test \n");
                ret = ntagi2creadwritetest();
                if (ret == 0 )
                {
                    LOG_NTAG_MESSAGE("Read/Write Completed Successfully \n");
                }
                else {
                    LOG_NTAG_MESSAGE("NTAG I2C Read/Write test Failed \n");
                }
        break;
    }
     return ret;
}
