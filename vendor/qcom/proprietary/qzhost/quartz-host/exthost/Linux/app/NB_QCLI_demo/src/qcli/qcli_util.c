/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qapi_status.h"
#include "qcli_util.h"
#include <stdio.h>

/*-------------------------------------------------------------------------
 * Preprocessor Definitions
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef struct QAPI_Status_String_Map_s
{
   qapi_Status_t  Status;
   const char *String;
} QAPI_Status_String_Map_t;


/*-------------------------------------------------------------------------
 * Variables and Constants
 *-----------------------------------------------------------------------*/

static const QAPI_Status_String_Map_t QAPI_Status_String_Map[] =
{
   {QAPI_OK,                "OK"           },
   {QAPI_ERROR,             "ERROR"        },
   {QAPI_ERR_INVALID_PARAM, "INVALID_PARAM"},
   {QAPI_ERR_NO_MEMORY,     "NO_MEMORY"    },
   {QAPI_ERR_NO_RESOURCE,   "NO_RESOURCE"  },
   {QAPI_ERR_BUSY,          "BUSY"         },
   {QAPI_ERR_NO_ENTRY,      "NO_ENTRY"     },
   {QAPI_ERR_NOT_SUPPORTED, "NOT_SUPPORTED"},
   {QAPI_ERR_TIMEOUT,       "TIMEOUT"      },
   {QAPI_ERR_BOUNDS,        "BOUNDS"       },
   {QAPI_ERR_BAD_PAYLOAD,   "BAD_PAYLOAD"  },
   {QAPI_ERR_EXISTS,        "EXISTS"       }
};

#define QCLI_STATUS_STRING_MAP_SIZE          (sizeof(QAPI_Status_String_Map) / sizeof(QAPI_Status_String_Map_t))


/*-------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Definitions
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
int32_t Memcmpi(const void *Source1, const void *Source2, uint32_t size)
{
   int32_t  Ret_Val;
   uint8_t  Byte1;
   uint8_t  Byte2;
   uint32_t Index;

   Ret_Val = 0;

   /* Simply loop through each byte pointed to by each pointer and check to see
      if they are equal. */
   for(Index = 0; (Index < size) && (!Ret_Val); Index ++)
   {
      /* Note each Byte that we are going to compare. */
      Byte1 = ((uint8_t *)Source1)[Index];
      Byte2 = ((uint8_t *)Source2)[Index];

      /* If the Byte in the first array is lower case, go ahead and make it
         upper case (for comparisons below). */
      if((Byte1 >= 'a') && (Byte1 <= 'z'))
      {
         Byte1 = Byte1 - ('a' - 'A');
      }

      /* If the Byte in the second array is lower case, go ahead and make it
         upper case (for comparisons below). */
      if((Byte2 >= 'a') && (Byte2 <= 'z'))
      {
         Byte2 = Byte2 - ('a' - 'A');
      }

      /* If the two Bytes are equal then there is nothing to do. */
      if(Byte1 != Byte2)
      {
         /* Bytes are not equal, so set the return value accordingly. */
         if(Byte1 < Byte2)
         {
            Ret_Val = -1;
         }
         else
         {
            Ret_Val = 1;
         }
      }
   }

   /* Simply return the result of the above comparison(s). */
   return(Ret_Val);
}

/**
   @brief Converts a hex character to an integer.

   @param Nibble is the hex character to be converted.
   @param Output is a pointer to where the converted number will be stored upon
          a successful return.

   @return
    - true if the nibble was converted successfully.
    - false if there was an error.
*/
qbool_t Hex_Nibble_To_Int(uint8_t Nibble, uint8_t *Output)
{
   qbool_t Ret_Val;

   /* Convert the number to lower case to simplify the check for
      characters 'a' through 'f'. */
   Nibble |= 0x20;

   if((Nibble >= '0') && (Nibble <= '9'))
   {
      *Output = Nibble - '0';
      Ret_Val = true;
   }
   else if((Nibble >= 'a') && (Nibble <= 'f'))
   {
      *Output = Nibble - 'a' + 10;
      Ret_Val = true;
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

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
qbool_t String_To_Integer(const char *String, int32_t *Output)
{
   qbool_t  Ret_Val;
   uint8_t  Temp_Val;
   uint32_t Base;
   uint32_t Number;
   uint32_t Previous_Number;
   qbool_t  Number_Is_Negative;

   Ret_Val = true;
   Number  = 0;

   /* Determine if the number is negative and consume the first character of the
      string ('-') if it is. */
   if(String[0] == '-')
   {
      Number_Is_Negative = true;
      String ++;
   }
   else
   {
      Number_Is_Negative = false;
   }

   /* Determine if the number is decimal or hexadecimal.  Note that the second
      character is converted to lower case to make the check simpler. */
   if((String[0] == '0') && ((String[1] | 0x20) == 'x'))
   {
      /* Number will be in hexadecimal.  Consume the first two characters
         ("0x"). */
      Base                = 0x10;
      String             += 2;
   }
   else
   {
      /* Number will be in decimal. */
      Base               = 10;
   }

   /* Loop until the end of the string is reached or the number is flagged as
      invalid. */
   while((String[0] != '\0') && (Ret_Val))
   {
      Ret_Val = Hex_Nibble_To_Int(String[0], &Temp_Val);

      if(Ret_Val)
      {
         /* Make sure that the value is correct for the number base. */
         if(Temp_Val < Base)
         {
            String ++;
            Previous_Number = Number;
            Number          = (Number * Base) + Temp_Val;

            /* Make sure that the number has not overflowed.  Signed overflow will
               checked at the end. */
            Ret_Val = (qbool_t)(Number >= Previous_Number);
         }
         else
         {
            Ret_Val = false;
         }
      }
   }

   /* For signed values (decimal or explicitly negative entires), make sure the
      number isn't already negative. */
   if((Ret_Val) && ((Base == 10) || (Number_Is_Negative)))
   {
      Ret_Val = (qbool_t)(((int32_t)Number) >= 0);
   }

   if(Ret_Val)
   {
      /* If the number is valid and negative, invert it now. */
      if(Number_Is_Negative)
      {
         *Output = 0 - (int32_t)Number;
      }
      else
      {
         *Output = (int32_t)Number;
      }
   }
   else
   {
      /* If the conversion failed, zero out the integer value. */
      *Output = 0;
   }

   return(Ret_Val);
}

/**
   @brief Converts a string of hex digits to an unsigned 64-bit integer.  A
          leading '0x' is optional.

   @param String is the input string to be converted.
   @param Output is a pointer to where the converted number will be stored upon
          a successful return.

   @return
    - true if the string was converted successfully.
    - false if there was an error.
*/
qbool_t Hex_String_To_ULL(char *String, uint64_t *Output)
{
   qbool_t  Ret_Val;
   uint8_t  Temp_Val;
   uint64_t Previous_Number;

   /* Strip off the leading "0x" if present. */
   if((String[0] == '0') && ((String[1] | 0x20) == 'x'))
   {
      String += 2;
   }

   Ret_Val = true;
   *Output = 0;

   /* Loop until the end of the string is reached or the number is flagged as
      invalid. */
   while((String[0] != '\0') && (Ret_Val))
   {
      Ret_Val = Hex_Nibble_To_Int(String[0], &Temp_Val);

      if(Ret_Val)
      {
         String ++;
         Previous_Number = *Output;
         *Output         = (*Output * 0x10) + Temp_Val;

         /* Make sure the value hasn't overflowed. */
         Ret_Val = (qbool_t)(*Output >= Previous_Number);
      }
   }

   /* If the conversion failed, zero out the integer value. */
   if(!Ret_Val)
   {
      *Output = 0;
   }

   return(Ret_Val);
}

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
qbool_t Hex_String_To_Array(char *String, uint32_t *OutputSize, uint8_t *Output)
{
   qbool_t  Ret_Val;
   uint8_t  Temp_Val1;
   uint8_t  Temp_Val2;
   uint32_t InputSize;

   /* Strip off the leading "0x" if present. */
   if((String[0] == '0') && ((String[1] | 0x20) == 'x'))
   {
      String += 2;
   }

   memset(Output, 0, *OutputSize);
   InputSize = 0;
   Ret_Val   = true;

   /* Loop until the end of the string is reached or the number is flagged as
      invalid. */
   while((String[0] != '\0') && (Ret_Val) && (InputSize < *OutputSize))
   {
      /* Make sure the next Nibble is also not NULL. */
      if(String[1] != '\0')
      {
         Ret_Val = Hex_Nibble_To_Int(String[0], &Temp_Val1) && Hex_Nibble_To_Int(String[1], &Temp_Val2);
         if(Ret_Val)
         {
            *Output = (Temp_Val1 << 4) | Temp_Val2;

            Output ++;
            InputSize ++;
            String += 2;
         }
      }
      else
      {
         Ret_Val = false;
      }
   }

   if(Ret_Val)
   {
      *OutputSize = InputSize;
   }

   return(Ret_Val);
}

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
qbool_t Verify_Integer_Parameter(QCLI_Parameter_t *Parameter, int32_t MinValue, int32_t MaxValue)
{
   qbool_t Ret_Val;

   Ret_Val = (qbool_t)((Parameter->Integer_Is_Valid) && (Parameter->Integer_Value >= MinValue) && (Parameter->Integer_Value <= MaxValue));

   return(Ret_Val);
}

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
qbool_t Verify_Unsigned_Integer_Parameter(QCLI_Parameter_t *Parameter, uint32_t MinValue, uint32_t MaxValue)
{
   qbool_t Ret_Val;

   Ret_Val = (qbool_t)((Parameter->Integer_Is_Valid) && (Parameter->Integer_Value >= MinValue) && (Parameter->Integer_Value <= MaxValue));

   return(Ret_Val);
}

/**
   @brief Converts an HMI status code to a string value for display.

   @param Status is the HMI status code to be converted.

   @return
      A pointer to the string representation of the status code.
*/
const char *Status_To_String(qapi_Status_t Status)
{
   const char *Ret_Val;
   int            Index;

   Ret_Val = "Unknown";

   for(Index = 0; Index < QCLI_STATUS_STRING_MAP_SIZE; Index ++)
   {
      if(QAPI_Status_String_Map[Index].Status == Status)
      {
         Ret_Val = QAPI_Status_String_Map[Index].String;
         break;
      }
   }

   return(Ret_Val);
}

/**
   @brief Displays a message indicating a function was executed successfully.

   @param QCLI_Handle   is the handle of the QCLI group that is displaying the
                        message.
   @param Function_Name is name of the function to be displayed.
*/
void Display_Function_Success(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name)
{
   if(Function_Name != NULL)
   {
      QCLI_Printf(QCLI_Handle, "%s() success.\n", Function_Name);
   }

   return;
}

/**
   @brief Displays a message indicating that a function returned an error.

   @param QCLI_Handle   is the handle of the QCLI group that is displaying the
                        message.
   @param Function_Name is name of the function to be displayed.
   @param Result        is the error code.
*/
void Display_Function_Error(QCLI_Group_Handle_t QCLI_Handle, char *Function_Name, qapi_Status_t Result)
{
   const char *Qapi_Status_String;
   uint8_t        Index;

   if(Function_Name != NULL)
   {
      Qapi_Status_String = "Unknown";

      for(Index = 0; Index < (sizeof(QAPI_Status_String_Map) / sizeof(QAPI_Status_String_Map[0])); Index ++)
      {
         if(QAPI_Status_String_Map[Index].Status == Result)
         {
            Qapi_Status_String = QAPI_Status_String_Map[Index].String;
            break;
         }
      }

      QCLI_Printf(QCLI_Handle, "%s returned %d (%s)\n", Function_Name, Result, Qapi_Status_String);
   }
}

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
void Dump_Data(QCLI_Group_Handle_t QCLI_Handle, const char *Prefix, uint16_t Length, const uint8_t *Buffer)
{
   uint16_t DataOffset;
   uint32_t LineOffset;
   uint32_t Index;
   uint8_t  LineBuffer[(3 * 16) + 1];
   uint8_t  AsciiBuffer[17];

   if((Prefix != NULL) && (Length > 0) && (Buffer != NULL))
   {
      DataOffset = 0;
      while(Length > 0)
      {
         /* Print out all the bytes for the line. */
         memset(LineBuffer, ' ', sizeof(LineBuffer));
         LineOffset = 0;
         for(Index = 0; (Index < 16) && (Length > 0); Index ++)
         {
            LineOffset += snprintf((char *)(&(LineBuffer[LineOffset])), sizeof(LineBuffer) - LineOffset, "%02X ", *Buffer);
            if(LineOffset >= sizeof(LineBuffer))
            {
               LineOffset = sizeof(LineBuffer) - 1;
            }

            /* Put the character in the AsciiBuffer if it is printable. */
            if((*Buffer >= ' ') && (*Buffer <= '~'))
            {
               AsciiBuffer[Index] = *Buffer;
            }
            else
            {
               AsciiBuffer[Index] = '.';
            }

            Length --;
            Buffer ++;
         }

         /* Handle the case if the buffer was empty before the line was
            complete. */
         LineBuffer[LineOffset] = ' ';
         LineBuffer[sizeof(LineBuffer) - 1] = '\0';

         /* Null terminate the AsciiBuffer. */
         AsciiBuffer[Index] = '\0';

         /* Display the line. */
         QCLI_Printf(QCLI_Handle, "%s%04X %s  %s\n", Prefix, DataOffset, LineBuffer, AsciiBuffer);

         DataOffset += 16;
      }
   }
}


