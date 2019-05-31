/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_app.h"

char* CodDecodeDeviceClass(CsrUint32 cod)
{
    CsrUint32 majorDeviceClass = (cod & 0x00001F00) >> 8;
    CsrUint32 minorDeviceClass = (cod & 0x000000FC) >> 2;

    switch (majorDeviceClass) /* Switch on Major Device Class */
    {
        case 1: /* Computer */
            switch (minorDeviceClass)
            {
                case 0:
                    return "Uncategorized Computer";
                case 1:
                    return "Desktop Workstation";
                case 2:
                    return "Server-class Computer";
                case 3:
                    return "Laptop";
                case 4:
                    return "Handheld PC/PDA";
                case 5:
                    return "Palm sized PC/PDA";
                case 6:
                    return "Wearable Computer";
                default:
                    return "Computer";
            }
        case 2: /* Phone */
            switch (minorDeviceClass)
            {
                case 0:
                    return "Uncategorized Phone";
                case 1:
                    return "Cellular Phone";
                case 2:
                    return "Cordless Phone";
                case 3:
                    return "Smart Phone";
                case 4:
                    return "Wired Modem or Voice Gateway";
                case 5:
                    return "Common ISDN Gateway";
                default:
                    return "Phone";
            }
        case 3: /* LAN/Network Access Point */
            return "LAN/Network Access Point";
        case 4: /* Audio/Video */
            switch (minorDeviceClass)
            {
                case 0:
                    return "Uncategorized Audio/Video Device";
                case 1:
                    return "Wearable Headset";
                case 2:
                    return "Handsfree Device";
                case 4:
                    return "Microphone";
                case 5:
                    return "Loudspeaker";
                case 6:
                    return "Headphones";
                case 7:
                    return "Portable Audio Device";
                case 8:
                    return "Car Audio Device";
                case 9:
                    return "Set-top Box";
                case 10:
                    return "HiFi Audio Device";
                case 11:
                    return "Video Cassette Recorder";
                case 12:
                    return "Video Camera";
                case 13:
                    return "Camcorder";
                case 14:
                    return "Video Monitor";
                case 15:
                    return "Video Display and Loudspeaker";
                case 16:
                    return "Video Conferencing";
                case 18:
                    return "Gaming/Toy";
                default:
                    return "Audio/Video Device";
            }
        case 5: /* Peripheral */
            switch (minorDeviceClass & 0x0000000F)
            {
                case 1:
                    return "Joystick";
                case 2:
                    return "Gamepad";
                case 3:
                    return "Remote Control";
                case 4:
                    return "Sensing Device";
                case 5:
                    return "Digitizer Tablet";
                case 6:
                    return "Card Reader";
                case 0: /* Uncategorized - take a look at top 2 bits for additional clues */
                default:
                    switch (minorDeviceClass >> 4)
                    {
                        case 1:
                            return "Keyboard";
                        case 2:
                            return "Pointing Device";
                        case 3:
                            return "Keyboard and Pointing Device";
                        default:
                            return "Peripheral";
                    }
            }
        case 6: /* Imaging */
            switch (minorDeviceClass >> 2)
            {
                case 0:
                    return "Uncategorized Imaging Device";
                case 1:
                    return "Display";
                case 2:
                    return "Camera";
                case 3:
                    return "Display and Camera";
                case 4:
                    return "Scanner";
                case 5:
                    return "Display and Scanner";
                case 6:
                    return "Camera and Scanner";
                case 7:
                    return "Display, Camera and Scanner";
                case 8:
                    return "Printer";
                case 9:
                    return "Display and Printer";
                case 10:
                    return "Camera and Printer";
                case 11:
                    return "Display, Camera and Printer";
                case 12:
                    return "Scanner and Printer";
                case 13:
                    return "Display, Scanner and Printer";
                case 14:
                    return "Camera, Scanner and Printer";
                case 15:
                    return "Display, Camera, Scanner and Printer";
                default:
                    return "Imaging Device";
            }
        case 7: /* Wearable */
            switch (minorDeviceClass)
            {
                case 1:
                    return "Wrist Watch";
                case 2:
                    return "Pager";
                case 3:
                    return "Jacket";
                case 4:
                    return "Helmet";
                case 5:
                    return "Glasses";
                default:
                    return "Wearable Device";
            }
        case 8: /* Toy */
            switch (minorDeviceClass)
            {
                case 1:
                    return "Robot";
                case 2:
                    return "Vehicle";
                case 3:
                    return "Action Figure";
                case 4:
                    return "Controller";
                case 5:
                    return "Game";
                default:
                    return "Toy";
            }
        case 9: /* Health */
            switch (minorDeviceClass)
            {
                case 0:
                    return "Undefined";
                case 1:
                    return "Blood Pressure Monitor";
                case 2:
                    return "Thermometer";
                case 3:
                    return "Weighing Scale";
                case 4:
                    return "Glucose Meter";
                case 5:
                    return "Pulse Oximeter";
                case 6:
                    return "Heart/Rate Monitor";
                case 7:
                    return "Health Data Display";
                default:
                    return "Health Minor Class - Reserved minor Device class";
            }
        case 31: /* Uncategorized */
            return "Uncategorized Device";
        default:
            return "Invalid, Reserved or Unspecified Device Class";
    }
}

