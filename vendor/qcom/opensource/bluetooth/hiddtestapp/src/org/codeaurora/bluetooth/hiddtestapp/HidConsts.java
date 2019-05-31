/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *            notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *            notice, this list of conditions and the following disclaimer in the
 *            documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *            the names of its contributors may be used to endorse or promote
 *            products derived from this software without specific prior written
 *            permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.codeaurora.bluetooth.hiddtestapp;

public class HidConsts {

    public final static String NAME = "HID Device Testapp";

    public final static String DESCRIPTION = "";

    public final static String PROVIDER = "Codeaurora";

    /* @formatter:off */
    public final static byte[] DESCRIPTOR = {
        (byte) 0x05, (byte) 0x01,                    // USAGE_PAGE (Generic Desktop)
        (byte) 0x09, (byte) 0x02,                    // USAGE (Mouse)
        (byte) 0xa1, (byte) 0x01,                    // COLLECTION (Application)
        (byte) 0x09, (byte) 0x01,                    //   USAGE (Pointer)
        (byte) 0xa1, (byte) 0x00,                    //   COLLECTION (Physical)
        (byte) 0x85, (byte) 0x02,                    //     REPORT_ID (2)
        (byte) 0x05, (byte) 0x09,                    //     USAGE_PAGE (Button)
        (byte) 0x19, (byte) 0x01,                    //     USAGE_MINIMUM (Button 1)
        (byte) 0x29, (byte) 0x03,                    //     USAGE_MAXIMUM (Button 3)
        (byte) 0x15, (byte) 0x00,                    //     LOGICAL_MINIMUM (0)
        (byte) 0x25, (byte) 0x01,                    //     LOGICAL_MAXIMUM (1)
        (byte) 0x95, (byte) 0x03,                    //     REPORT_COUNT (3)
        (byte) 0x75, (byte) 0x01,                    //     REPORT_SIZE (1)
        (byte) 0x81, (byte) 0x02,                    //     INPUT (Data,Var,Abs)
        (byte) 0x95, (byte) 0x01,                    //     REPORT_COUNT (1)
        (byte) 0x75, (byte) 0x05,                    //     REPORT_SIZE (5)
        (byte) 0x81, (byte) 0x03,                    //     INPUT (Cnst,Var,Abs)
        (byte) 0x05, (byte) 0x01,                    //     USAGE_PAGE (Generic Desktop)
        (byte) 0x09, (byte) 0x30,                    //     USAGE (X)
        (byte) 0x09, (byte) 0x31,                    //     USAGE (Y)
        (byte) 0x15, (byte) 0x81,                    //     LOGICAL_MINIMUM (-127)
        (byte) 0x25, (byte) 0x7f,                    //     LOGICAL_MAXIMUM (127)
        (byte) 0x75, (byte) 0x08,                    //     REPORT_SIZE (8)
        (byte) 0x95, (byte) 0x02,                    //     REPORT_COUNT (2)
        (byte) 0x81, (byte) 0x06,                    //     INPUT (Data,Var,Rel)
        (byte) 0x09, (byte) 0x38,                    //     USAGE (Wheel)
        (byte) 0x15, (byte) 0x81,                    //     LOGICAL_MINIMUM (-127)
        (byte) 0x25, (byte) 0x7f,                    //     LOGICAL_MAXIMUM (127)
        (byte) 0x75, (byte) 0x08,                    //     REPORT_SIZE (8)
        (byte) 0x95, (byte) 0x01,                    //     REPORT_COUNT (1)
        (byte) 0x81, (byte) 0x06,                    //     INPUT (Data,Var,Rel)
        (byte) 0xc0,                                 //   END_COLLECTION
        (byte) 0xc0,                                 // END_COLLECTION

        // battery strength
        (byte) 0x05, (byte) 0x0c,
        (byte) 0x09, (byte) 0x01,
        (byte) 0xa1, (byte) 0x01,
        (byte) 0x85, (byte) 0x20,                    //   REPORT_ID (32)
        (byte) 0x05, (byte) 0x01,
        (byte) 0x09, (byte) 0x06,
        (byte) 0xa1, (byte) 0x02,
        (byte) 0x05, (byte) 0x06,                    // USAGE_PAGE (Generic Device Controls)
        (byte) 0x09, (byte) 0x20,                    // USAGE (Battery Strength)
        (byte) 0x15, (byte) 0x00,                    // LOGICAL_MINIMUM (0)
        (byte) 0x26, (byte) 0xff, (byte) 0x00,      // LOGICAL_MAXIMUM (100)
        (byte) 0x75, (byte) 0x08,                    // REPORT_SIZE (8)
        (byte) 0x95, (byte) 0x01,                    // REPORT_COUNT (1)
        (byte) 0x81, (byte) 0x02,                    // INPUT (Data,Var,Abs)
        (byte) 0xc0,
        (byte) 0xc0,

        (byte) 0x05, (byte) 0x01,                    // USAGE_PAGE (Generic Desktop)
        (byte) 0x09, (byte) 0x06,                    // USAGE (Keyboard)
        (byte) 0xa1, (byte) 0x01,                    // COLLECTION (Application)
        (byte) 0x85, (byte) 0x01,                    //   REPORT_ID (1)
        (byte) 0x05, (byte) 0x07,                    //   USAGE_PAGE (Keyboard)
        (byte) 0x19, (byte) 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
        (byte) 0x29, (byte) 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
        (byte) 0x15, (byte) 0x00,                    //   LOGICAL_MINIMUM (0)
        (byte) 0x25, (byte) 0x01,                    //   LOGICAL_MAXIMUM (1)
        (byte) 0x75, (byte) 0x01,                    //   REPORT_SIZE (1)
        (byte) 0x95, (byte) 0x08,                    //   REPORT_COUNT (8)
        (byte) 0x81, (byte) 0x02,                    //   INPUT (Data,Var,Abs)
        (byte) 0x05, (byte) 0x0c,                    //   USAGE_PAGE (Consumer Devices)
        (byte) 0x15, (byte) 0x00,                    //   LOGICAL_MINIMUM (0)
        (byte) 0x25, (byte) 0x01,                    //   LOGICAL_MAXIMUM (1)
        (byte) 0x95, (byte) 0x07,                    //   REPORT_COUNT (7)
        (byte) 0x75, (byte) 0x01,                    //   REPORT_SIZE (1)
        (byte) 0x09, (byte) 0xb6,                    //   USAGE (Scan Previous Track)
        (byte) 0x09, (byte) 0xb5,                    //   USAGE (Scan Next Track)
        (byte) 0x09, (byte) 0xb7,                    //   USAGE (Stop)
        (byte) 0x09, (byte) 0xcd,                    //   USAGE (Play/Pause)
        (byte) 0x09, (byte) 0xe2,                    //   USAGE (Mute)
        (byte) 0x09, (byte) 0xe9,                    //   USAGE (Volume Up)
        (byte) 0x09, (byte) 0xea,                    //   USAGE (Volume Down)
        (byte) 0x81, (byte) 0x02,                    //   INPUT (Data,Var,Abs)
        (byte) 0x95, (byte) 0x01,                    //   REPORT_COUNT (1)
        (byte) 0x75, (byte) 0x01,                    //   REPORT_SIZE (1)
        (byte) 0x81, (byte) 0x03,                    //   INPUT (Constant,Var,Abs)
        (byte) 0x05, (byte) 0x07,                    //   USAGE_PAGE (Keyboard)
        (byte) 0x95, (byte) 0x05,                    //   REPORT_COUNT (5)
        (byte) 0x75, (byte) 0x01,                    //   REPORT_SIZE (1)
        (byte) 0x85, (byte) 0x01,                    //   REPORT_ID (1)
        (byte) 0x05, (byte) 0x08,                    //   USAGE_PAGE (LEDs)
        (byte) 0x19, (byte) 0x01,                    //   USAGE_MINIMUM (Num Lock)
        (byte) 0x29, (byte) 0x05,                    //   USAGE_MAXIMUM (Kana)
        (byte) 0x91, (byte) 0x02,                    //   OUTPUT (Data,Var,Abs)
        (byte) 0x95, (byte) 0x01,                    //   REPORT_COUNT (1)
        (byte) 0x75, (byte) 0x03,                    //   REPORT_SIZE (3)
        (byte) 0x91, (byte) 0x03,                    //   OUTPUT (Cnst,Var,Abs)
        (byte) 0x95, (byte) 0x06,                    //   REPORT_COUNT (6)
        (byte) 0x75, (byte) 0x08,                    //   REPORT_SIZE (8)
        (byte) 0x15, (byte) 0x00,                    //   LOGICAL_MINIMUM (0)
        (byte) 0x25, (byte) 0x65,                    //   LOGICAL_MAXIMUM (101)
        (byte) 0x05, (byte) 0x07,                    //   USAGE_PAGE (Keyboard)
        (byte) 0x19, (byte) 0x00,                  //   USAGE_MINIMUM (Reserved (no event indicated))
        (byte) 0x29, (byte) 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
        (byte) 0x81, (byte) 0x00,                    //   INPUT (Data,Ary,Abs)
        (byte) 0xc0                                  // END_COLLECTION
    };
    /* @formatter:on */

    public final static byte KEYBOARD_INPUT_REPORT_ID = 1;

    public final static byte KEYBOARD_OUTPUT_REPORT_ID = 1;

    public final static byte MOUSE_REPORT_ID = 2;

    public final static byte BOOT_KEYBOARD_REPORT_ID = 1;

    public final static byte BOOT_MOUSE_REPORT_ID = 2;

    public final static byte BATTERY_REPORT_ID = 32;

    public final static byte KEYBOARD_LED_NUM_LOCK = 0x01;

    public final static byte KEYBOARD_LED_CAPS_LOCK = 0x02;

    public final static byte KEYBOARD_LED_SCROLL_LOCK = 0x04;
}
