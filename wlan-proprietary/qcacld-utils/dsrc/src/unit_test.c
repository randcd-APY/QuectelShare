/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// unit tests for the dsrc_util library

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include "dsrc_util.h"

// print pass only if the result was a pass
#define PRINT_PASS_RESULT(x) do { if (x) printf("%s: passed\n",__FUNCTION__);}while(0)

int test_ext_field_macro(void);
int test_psid_macro(void);
int test_wsmp_field_start_macro(void);
int test_decode_wsmp_packet(void);
int test_decode_eth_packet(void);

int main(void)
{
    int result;

    result = test_ext_field_macro();
    result = test_psid_macro();
    result = test_wsmp_field_start_macro();
    result = test_decode_wsmp_packet();
    result = test_decode_eth_packet();

    return result;
}

int test_decode_eth_packet(void)
{


    int result = 0;

    // wsmp packet with no extension fields
    uint8_t packet[] =
    {
        // ethernet header
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // destination address
        0x00, 0x03, 0x7f, 0x2a, 0x00, 0x11, // src MAC address
        0x88, 0xdc,                         // ethertype (up to here is the eth header)
        // LLC SNAP header
        0xaa, 0xaa, 0x03,0x00, 0x00,0x00, 0x88, 0xdc,  // 0x88DC is the WSMP packet type
        // WSMP header
        0x02,                               // version
        0xef, 0xff, 0xff, 0xff,             // PSID
        0x80,                               // 0x80 is wave element ID for data (no extension field)
        0x00, 0x10,                         // 16 bytes of data
        // payload
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };


    do {
        uint8_t *payload, rate, chan, pwr;
        uint16_t payload_len;
        uint8_t *wsmp_pkt;

        // packet should be after the eth header + LLC SNAP header
        wsmp_pkt = packet + sizeof(struct eth_hdr) + sizeof(struct llc_snap_hdr);

        dsrc_decode_wsmp_packet(wsmp_pkt, &payload, &payload_len, &rate, &chan, &pwr);

        if (payload_len != 0x10){
            printf("%s(%d): payload length verification failed, length=0x%x (expected 0x10)\n",__FUNCTION__,__LINE__, payload_len);
            break;
        }

        result = 1;
    }
    while (0);

    PRINT_PASS_RESULT(result);

    return result;

}


int test_decode_wsmp_packet(void)
{

    int result = 0;

    // a wsmp packet for testing
    uint8_t packet[] =
    {
        // WSMP header
        0x02,                               // version
        0xef, 0xff, 0xff, 0xff,             // PSID
        0x80,                               // 0x80 is wave element ID for data (no extension field)
        0x00, 0x10,                         // 16 bytes of data
        // payload
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };


    do {
        uint8_t *payload, rate, chan, pwr;
        uint16_t payload_len;

        dsrc_decode_wsmp_packet(packet, &payload, &payload_len, &rate, &chan, &pwr);

        if (payload_len != 0x10){
            printf("%s(%d): payload length verification failed, length=0x%x (expected 0x10)\n",__FUNCTION__,__LINE__, payload_len);
            break;
        }

        result = 1;
    }
    while (0);

    PRINT_PASS_RESULT(result);

    return result;
}

int test_wsmp_field_start_macro(void)
{

    uint8_t buf[100], *pkt, *ext_field;
    struct wsmp_hdr *hdr;
    int result=1;

    pkt = buf;

    // fill the buffer with the fields that occur before the extension field
    // version and PSID
    hdr = (struct wsmp_hdr*)pkt;


    hdr->version = 1;
    // will cause a 1 byte PSID
    hdr->psid[0] = 0x10;

    ext_field = WSMP_EXT_FIELD_START(pkt);

    // 1 byte PSID should cause a 2-byte offset to the extension field.
    if (pkt + 2 != ext_field) {
        printf("%s: failed, offset to extension calculated to be 0x%p, should have been 0x%p\n",__FUNCTION__,ext_field, pkt+2);
        result = 0;
    }


    // now get an offset for a 2-byte PSID
    hdr->psid[0] = 0x85;
    ext_field = WSMP_EXT_FIELD_START(pkt);


    // 1 byte PSID should cause a 2-byte offset to the extension field.
    if (pkt + 3 != ext_field) {
        printf("%s: failed, offset to extension calculated to be 0x%p, should have been 0x%p\n",__FUNCTION__,ext_field, pkt+3);
        result = 0;
    }


    PRINT_PASS_RESULT(result);

    return result;
}

int test_psid_macro(void)
{

    size_t size;
    int result = 1;

    do {

        // 1 byte
        size = WSMP_PSID_FIELD_SIZE(0x15);

        if (size != 1){
            printf("%s: failed, size %zu != 1\n",__FUNCTION__,size);
            break;
        }

        size = WSMP_PSID_FIELD_SIZE(0x85);

        if (size != 2){
            printf("%s: failed, size %zu != 2\n",__FUNCTION__,size);
            break;
        }

        // 3 byte field
        size = WSMP_PSID_FIELD_SIZE(0xcf);

        if (size != 3){
            printf("%s: failed, size %zu != 3\n",__FUNCTION__,size);
            break;
        }

        // 4 byte field
        size = WSMP_PSID_FIELD_SIZE(0xE5);

        if (size != 4){
            printf("%s: failed, size %zu != 4\n",__FUNCTION__,size);
            break;
        }

        result = 1;

    }while (0);

    PRINT_PASS_RESULT(result);
    return result;

}


int test_ext_field_macro(void)
{
    void *ptr;
    size_t size;

    struct wsmp_ext_field *field;
    size_t field_size;
    int result = 1;

    // first test for a 1 byte payload
    size = offsetof(struct wsmp_ext_field,data) + sizeof(uint8_t);

    ptr = malloc(size);

    if (ptr == NULL){
        printf("%s: malloc failed",__FUNCTION__);
        return 0;
    }

    // create an extension field
    field = ptr;

    field->wave_element_id = WAVE_ELEM_ID_CHAN;
    field->length = 1;
    field->data.channel = 172;

    field_size = WSMP_EXT_FIELD_SIZE(field);

    // this should be a 3-byte field
    if (field_size != size){
        printf("Error, test %s:  WSMP_FIELD_SIZE calculated wrong, should be 3, was %zu\n",__FUNCTION__,field_size);
        result = 0;
    }


    // now test for a bigger field

    // int payload, so 6 bytes
    size = offsetof(struct wsmp_ext_field,data) + sizeof(int);
    ptr = realloc(ptr,size);


    if (ptr == NULL){
        printf("%s: malloc failed",__FUNCTION__);
        return 0;
    }


    field->wave_element_id = WAVE_ELEM_ID_CHAN;
    field->length = sizeof(int);


    field_size = WSMP_EXT_FIELD_SIZE(field);

    free(ptr);


    if (field_size != size){
        printf("Error, test %s:  WSMP_FIELD_SIZE calculated wrong, should be 6, was %zu\n",__FUNCTION__,field_size);

        result = 0;
    }

    PRINT_PASS_RESULT(result);

    return result;
}
