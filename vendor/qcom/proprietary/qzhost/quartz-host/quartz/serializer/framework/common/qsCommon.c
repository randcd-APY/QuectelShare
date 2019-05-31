/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qsCommon.h"
#include "qsOSAL.h"
#include "qsPack.h"

#define QS_POINTER_HEADER_FLAG_NULL_PTR         (0x01)

/**
  Initialize packed buffer.

  @param Buffer Pointer to the buffer to initialize
  @param Value  Pointer to value to be packed.

*/
void InitializePackedBuffer(PackedBuffer_t *Buffer, uint8_t *Start, uint32_t Length)
{
    if(Buffer != NULL)
    {
        Buffer->CurrentPos = Start;
        Buffer->Start      = Start;
        Buffer->Length     = Length;
        Buffer->Remaining  = Length;
    }
}

/**
  Allocates a buffer and adds it to the provided linked list.

  @param BufferList The buffer list to add the entry to.
  @param Size       Required size of the buffer.

  @return The newly allocated buffer or NULL if there was an error.
  */
void *AllocateBufferListEntry(BufferListEntry_t **BufferList, uint16_t Size)
{
    BufferListEntry_t *NewEntry;
    BufferListEntry_t *CurrentEntry;
    void *RetVal;

    if(BufferList != NULL)
    {
        NewEntry = QSOSAL_AllocateMemory(Size + sizeof(BufferListEntry_t));
        if(NewEntry != NULL)
        {
            NewEntry->Next = NULL;

            if(*BufferList == NULL)
            {
                *BufferList = NewEntry;
            }
            else
            {
                CurrentEntry = *BufferList;
                while(CurrentEntry->Next != NULL)
                {
                    CurrentEntry = CurrentEntry->Next;
                }

                CurrentEntry->Next = NewEntry;
            }

            RetVal = (void *)(NewEntry + 1);

            QSOSAL_MemInitialize(RetVal, 0, Size);
        }
        else
        {
            RetVal = NULL;
        }
    }
    else
    {
        RetVal = NULL;
    }

    return(RetVal);
}

/**
  Frees a buffer list.

  @param BufferList Buffer list to free.
  */
void FreeBufferList(BufferListEntry_t **BufferList)
{
    BufferListEntry_t *CurrentEntry;
    BufferListEntry_t *NextEntry;

    if(BufferList)
    {
        CurrentEntry = *BufferList;
        while(CurrentEntry != NULL)
        {
            NextEntry = CurrentEntry->Next;
            QSOSAL_FreeMemory(CurrentEntry);
            CurrentEntry = NextEntry;
        }
    }
}

/**
  Write a packed value (int).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_int(PackedBuffer_t *Buffer, int *Value)
{
    int32_t     SexedValue;
    SerStatus_t ret_val;

    if((Buffer) && (Buffer->Remaining >= sizeof(int32_t)) && (Buffer->CurrentPos != NULL))
    {
        if (Value)
        {
            SexedValue            = (int32_t)*Value;
            Buffer->CurrentPos[0] = (uint8_t)((SexedValue) & 0xFF);
            Buffer->CurrentPos[1] = (uint8_t)(((SexedValue) >> 8) & 0xFF);
            Buffer->CurrentPos[2] = (uint8_t)(((SexedValue) >> 16) & 0xFF);
            Buffer->CurrentPos[3] = (uint8_t)(((SexedValue) >> 24) & 0xFF);

            Buffer->CurrentPos += sizeof(int32_t);
            Buffer->Remaining  -= sizeof(int32_t);
            ret_val             = ssSuccess;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Write a packed value (8 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_8(PackedBuffer_t *Buffer, uint8_t *Value)
{
    SerStatus_t ret_val = ssInvalidParameter;

    if((Buffer) && (Buffer->Remaining >= sizeof(uint8_t)) && (Buffer->CurrentPos != NULL))
    {
        if (Value)
        {
            Buffer->CurrentPos[0] = *Value;

            Buffer->CurrentPos += sizeof(uint8_t);
            Buffer->Remaining  -= sizeof(uint8_t);
            ret_val             = ssSuccess;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;


    return(ret_val);
}

/**
  Write a packed value (16 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_16(PackedBuffer_t *Buffer, uint16_t *Value)
{
    SerStatus_t ret_val = ssInvalidParameter;

    if((Buffer) && (Buffer->Remaining >= sizeof(uint16_t)) && (Buffer->CurrentPos != NULL))
    {
        if (Value)
        {
            Buffer->CurrentPos[0] = (uint8_t)(*Value & 0xFF);
            Buffer->CurrentPos[1] = (uint8_t)((*Value >> 8) & 0xFF);

            Buffer->CurrentPos += sizeof(uint16_t);
            Buffer->Remaining  -= sizeof(uint16_t);
            ret_val             = ssSuccess;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Write a packed value (32 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_32(PackedBuffer_t *Buffer, uint32_t *Value)
{
    SerStatus_t ret_val = ssInvalidParameter;

    if((Buffer) && (Buffer->Remaining >= sizeof(uint32_t)) && (Buffer->CurrentPos != NULL))
    {
        if (Value)
        {
            Buffer->CurrentPos[0] = (uint8_t)((*Value) & 0xFF);
            Buffer->CurrentPos[1] = (uint8_t)(((*Value) >> 8) & 0xFF);
            Buffer->CurrentPos[2] = (uint8_t)(((*Value) >> 16) & 0xFF);
            Buffer->CurrentPos[3] = (uint8_t)(((*Value) >> 24) & 0xFF);

            Buffer->CurrentPos += sizeof(uint32_t);
            Buffer->Remaining  -= sizeof(uint32_t);
            ret_val             = ssSuccess;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Write a packed value (64 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_64(PackedBuffer_t *Buffer, uint64_t *Value)
{
    SerStatus_t ret_val = ssInvalidParameter;

    if((Buffer) && (Buffer->Remaining >= sizeof(uint64_t)) && (Buffer->CurrentPos != NULL))
    {
        if (Value)
        {
            Buffer->CurrentPos[0] = (uint8_t)((*Value) & 0xFF);
            Buffer->CurrentPos[1] = (uint8_t)(((*Value) >> 8) & 0xFF);
            Buffer->CurrentPos[2] = (uint8_t)(((*Value) >> 16) & 0xFF);
            Buffer->CurrentPos[3] = (uint8_t)(((*Value) >> 24) & 0xFF);
            Buffer->CurrentPos[4] = (uint8_t)(((*Value) >> 32) & 0xFF);
            Buffer->CurrentPos[5] = (uint8_t)(((*Value) >> 40) & 0xFF);
            Buffer->CurrentPos[6] = (uint8_t)(((*Value) >> 48) & 0xFF);
            Buffer->CurrentPos[7] = (uint8_t)(((*Value) >> 56) & 0xFF);

            Buffer->CurrentPos += sizeof(uint64_t);
            Buffer->Remaining  -= sizeof(uint64_t);
            ret_val             = ssSuccess;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Write an array value.

  @param Buffer      Pointer to the buffer to pack the value
  @param Value       Pointer to value to be packed.
  @param ValueLength Length of the value.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_Array(PackedBuffer_t *Buffer, void *Value, uint32_t ElementSize, uint32_t ElementCount)
{
    SerStatus_t ret_val;
    uint32_t ValueLength;

    if((Buffer) && (Buffer->CurrentPos != NULL))
    {
        if(Value)
        {
            ValueLength = ElementSize * ElementCount;

            if (Buffer->Remaining >= ValueLength)
            {
                /* Copy the buffer. */
                QSOSAL_MemCopy(Buffer->CurrentPos, Value, ValueLength);

                Buffer->CurrentPos += ValueLength;
                Buffer->Remaining  -= ValueLength;
                ret_val = ssSuccess;
            }
            else
                ret_val = ssInvalidLength;
        }
        else
            ret_val = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Writes a pointer header value. Accepts a NULL pointer.

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to the value whose header to write.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_PointerHeader(PackedBuffer_t *Buffer, void *Value)
{
    SerStatus_t ret_val;
    uint8_t   header  = 0;

    if((Buffer) && (Buffer->Remaining >= sizeof(uint8_t)) && (Buffer->CurrentPos != NULL))
    {
        if(Value == NULL)
            header |= QS_POINTER_HEADER_FLAG_NULL_PTR;

        Buffer->CurrentPos[0] = header;

        Buffer->CurrentPos += sizeof(uint8_t);
        Buffer->Remaining  -= sizeof(uint8_t);
        ret_val             = ssSuccess;
    }
    else
        ret_val = ssInvalidParameter;

    return(ret_val);
}

/**
  Read a packed value (int).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_int(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, int *Value)
{
    int32_t     UnPackedValue;
    SerStatus_t Status = ssInvalidParameter;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint32_t)) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        UnPackedValue            = ((int32_t)((((uint32_t)(InputBuffer->CurrentPos[3])) << 24) | (((uint32_t)(InputBuffer->CurrentPos[2])) << 16) | (((uint32_t)(InputBuffer->CurrentPos[1])) << 8) | ((uint32_t)(InputBuffer->CurrentPos[0]))));
        *Value                   = (int)UnPackedValue;

        InputBuffer->CurrentPos += sizeof(int32_t);
        InputBuffer->Remaining  -= sizeof(int32_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Read a packed value (8 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_8(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint8_t *Value)
{
    SerStatus_t Status = ssInvalidParameter;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint8_t)) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        *Value                   = (uint8_t)(InputBuffer->CurrentPos[0]);

        InputBuffer->CurrentPos += sizeof(uint8_t);
        InputBuffer->Remaining  -= sizeof(uint8_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Read a packed value (16 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_16(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint16_t *Value)
{
    SerStatus_t Status = ssInvalidParameter;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint16_t)) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        *Value                   = (uint16_t)((((uint16_t)(InputBuffer->CurrentPos[1])) << 8) | ((uint16_t)(InputBuffer->CurrentPos[0])));

        InputBuffer->CurrentPos += sizeof(uint16_t);
        InputBuffer->Remaining  -= sizeof(uint16_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Read a packed value (32 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_32(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint32_t *Value)
{
    SerStatus_t Status = ssInvalidParameter;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint32_t)) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        *Value                   = ((uint32_t)((((uint32_t)(InputBuffer->CurrentPos[3])) << 24) | (((uint32_t)(InputBuffer->CurrentPos[2])) << 16) | (((uint32_t)(InputBuffer->CurrentPos[1])) << 8) | ((uint32_t)(InputBuffer->CurrentPos[0]))));

        InputBuffer->CurrentPos += sizeof(uint32_t);
        InputBuffer->Remaining  -= sizeof(uint32_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Read a packed value (64 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_64(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint64_t *Value)
{
    SerStatus_t Status = ssInvalidParameter;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint64_t)) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        *Value                   = ((uint64_t)((((uint64_t)(InputBuffer->CurrentPos[7])) << 56) | (((uint64_t)(InputBuffer->CurrentPos[6])) << 48) |
                    (((uint64_t)(InputBuffer->CurrentPos[5])) << 40) | (((uint64_t)(InputBuffer->CurrentPos[4])) << 32) |
                    (((uint64_t)(InputBuffer->CurrentPos[3])) << 24) | (((uint64_t)(InputBuffer->CurrentPos[2])) << 16) |
                    (((uint64_t)(InputBuffer->CurrentPos[1])) << 8 ) | (((uint64_t)(InputBuffer->CurrentPos[0])))));

        InputBuffer->CurrentPos += sizeof(uint64_t);
        InputBuffer->Remaining  -= sizeof(uint64_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Read an array value.

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.
  @param ValueLength Length of the array.

  @return the status result of the read.
  */
SerStatus_t PackedRead_Array(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, void *Value, uint32_t ElementSize, uint32_t ElementCount)
{
    SerStatus_t Status = ssInvalidParameter;
    uint32_t ValueLength;

    if((InputBuffer) && (InputBuffer->CurrentPos != NULL) && (Value))
    {
        ValueLength = ElementSize * ElementCount;

        if (InputBuffer->Remaining >= ValueLength)
        {
            /* Copy the buffer. */
            QSOSAL_MemCopy(Value, InputBuffer->CurrentPos, ValueLength);

            InputBuffer->CurrentPos += ValueLength;
            InputBuffer->Remaining  -= ValueLength;
            Status = ssSuccess;
        }
    }

    return(Status);
}

/**
  Read a pointer header value and checks if the pointer is valid.

  @param InputBuffer  Pointer to the packed buffer to be
  allocated.
  @param BufferList   List of Buffers.
  @param PointerValid Output pointer to the pointer header
  validity, TRUE if non-NULL and FALSE
  if NULL.

  @return the status result of the read.
  */
SerStatus_t PackedRead_PointerHeader(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, Boolean_t *PointerValid)
{
    SerStatus_t Status = ssInvalidParameter;
    uint8_t     Value;

    if((InputBuffer) && (InputBuffer->Remaining >= sizeof(uint8_t)) && (InputBuffer->CurrentPos != NULL) && (PointerValid))
    {
        Value                    = (uint8_t)(InputBuffer->CurrentPos[0]);

        if(Value & QS_POINTER_HEADER_FLAG_NULL_PTR)
            *PointerValid = FALSE;
        else
            *PointerValid = TRUE;

        InputBuffer->CurrentPos += sizeof(uint8_t);
        InputBuffer->Remaining  -= sizeof(uint8_t);
        Status                   = ssSuccess;
    }

    return(Status);
}

/**
  Calculates the packed size of an integer value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_int(int *Value)
{
    uint32_t qsResult;

    if(Value == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = 4;
    }

    return(qsResult);
}

/**
  Calculates the packed size of an 8 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_8(uint8_t *Value)
{
    uint32_t qsResult;

    if(Value == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = 1;
    }

    return(qsResult);
}

/**
  Calculates the packed size of a 16 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_16(uint16_t *Value)
{
    uint32_t qsResult;

    if(Value == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = 2;
    }

    return(qsResult);
}

/**
  Calculates the packed size of a 32 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_32(uint32_t *Value)
{
    uint32_t qsResult;

    if(Value == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = 4;
    }

    return(qsResult);
}

/**
  Calculates the packed size of a 64 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_64(uint64_t *Value)
{
    uint32_t qsResult;

    if(Value == NULL)
    {
        qsResult = 0;
    }
    else
    {
        qsResult = 8;
    }

    return(qsResult);
}

