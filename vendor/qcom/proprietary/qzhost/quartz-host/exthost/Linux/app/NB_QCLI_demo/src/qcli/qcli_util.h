/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QCLI_UTIL_H__ // [
#define __QCLI_UTIL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "string.h"
#include "qapi_types.h"
#include "qapi_status.h"
#include "qcli_api.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function does a case-insensitive comparison of two buffers.

   @param Source1 is the first string to be compared.
   @param Source2 is the second string to be compared.

   @return
    - 0  if the two strings are equivalent up to the specified size.
    - -1 if Source1 is "less than" Source2.
    - 1  if Source1 is "greater than" Source2.
*/
int32_t Memcmpi(const void *Source1, const void *Source2, uint32_t size);

/**
   @brief Converts a hex character to an integer.

   @param Nibble is the hex character to be converted.
   @param Output is a pointer to where the converted number will be stored upon
          a successful return.

   @return
    - true if the nibble was converted successfully.
    - false if there was an error.
*/
qbool_t Hex_Nibble_To_Int(uint8_t Nibble, uint8_t *Output);

/**
   @brief This function attempts to convert a string to an integer.

   It supports strings represented as either a decimal or hexadecimal.  If
   hexadecimal, it is expected for the number to be preceeded by "0x".

   @param String is the NULL terminated string to be converted.
   @param Output is a pointer to where the integer output will be
          stored upon successful return.

   @return
    - true if the ocnversion was successful.
    - false if the conversion was not succcessful.
*/
qbool_t String_To_Integer(const char *String, int32_t *Output);

/**
   @brief Converts a string of hex digits to an Unsigned 64-bit integer. A
          leading '0x' is optional.

   @param String is the input string to be converted.
   @param Output is a pointer to where the converted number will be stored upon
          a successful return.

   @return
    - true if the string was converted successfully.
    - false if there was an error.
*/
qbool_t Hex_String_To_ULL(char *String, uint64_t *Output);

/**
   @brief Verifies if a given command line parameter is a valid integer in the
          specified range.

   @param Parameter is the command line parameter to verify.
   @param MinValue  is the minimum acceptable value for the parameter.
   @param MaxValue  is the maximum acceptable value for the parameter.

   @return
    - true  if the parameter is valid.
    - false if the parameter is not valid.
*/
qbool_t Verify_Integer_Parameter(QCLI_Parameter_t *Parameter, int32_t MinValue, int32_t MaxValue);

/**
   @brief Parses a string as an arbitrary length hex value.  The output will
          be an array of the nibbles in Big Endian format.

   @param String     is the input string to be converted. A preceeding 0x is
                     optional but the string must contain an even number of
                     nibbles.
   @param OutputSize is the size of the output buffer.  Upon successful return,
                     this value will be set to the number of bytes read.
   @param Output     is the buffer where the converted string will be stored
                     upon a successful return.

   @return
    - true if the string was converted successfully.
    - false if there was an error.
*/
qbool_t Hex_String_To_Array(char *String, uint32_t *OutputSize, uint8_t *Output);

/**
   @brief Verifies if a given command line parameter is a valid unsigned integer
          in the specified range.

   This function is similiar to Verfity_Integer_Parameter() but is meant to be
   used when verifying parameters that will be treated as unsigned 32-bit
   integers.

   @param Parameter is the command line parameter to verify.
   @param MinValue  is the minimum acceptable value for the parameter.
   @param MaxValue  is the maximum acceptable value for the parameter.

   @return
    - true  if the parameter is valid.
    - false if the parameter is not valid.
*/
qbool_t Verify_Unsigned_Integer_Parameter(QCLI_Parameter_t *Parameter, uint32_t MinValue, uint32_t MaxValue);

/**
   @brief Converts an HMI status code to a string value for display.

   @param Status is the HMI status code to be converted.

   @return
      A pointer to the string representation of the status code.
*/
const char *Status_To_String(qapi_Status_t Status);

/**
   @brief Displays a message indicating a function was executed successfully.

   @param QCLI_Handle   is the handle of the QCLI group that is displaying the
                        message.
   @param Function_Name is name of the function to be displayed.
*/
void Display_Function_Success(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name);

/**
   @brief Displays a message indicating that a function returned an error.

   @param QCLI_Handle   is the handle of the QCLI group that is displaying the
                        message.
   @param Function_Name is name of the function to be displayed.
   @param Result        is the error code.
*/
void Display_Function_Error(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name, qapi_Status_t Result);

/**
   @brief Outputs a buffer containing raw binary data (i.e., not characters) to
          the console.

   @param QCLI_Handle is the handle of the QCLI group that is displaying the
                      message.
   @param Prefix      is the string prefixed to each line printed. This is
                      usually the leading whitespace for the line.
   @param Length      is the length of the data to be displayed.
   @param Buffer      is a pointer to the buffer containing the data to be
                      displayed.
*/
void Dump_Data(QCLI_Group_Handle_t QCLI_Handle, const char *Prefix, uint16_t Length, const uint8_t *Buffer);

#endif // ] #ifndef __QCLI_UTIL_H__

