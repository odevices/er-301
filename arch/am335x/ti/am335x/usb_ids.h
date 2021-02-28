//*****************************************************************************
//
//
//*****************************************************************************

/* ============================================================= */
/**
 *
 *   usb_ids.h 
 *   Definitions of VIDs and PIDs used by Stellaris USB examples.
 *
 *  ============================================================
 *  Copyright (c) Texas Instruments Incorporated 2015
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/




#ifndef __USBIDS_H__
#define __USBIDS_H__

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// TI Stellaris Vendor ID.
//
//*****************************************************************************
#define USB_VID_STELLARIS       0x1cbe

//*****************************************************************************
//
// Product IDs.
//
//*****************************************************************************
#define USB_PID_MOUSE           0x0000
#define USB_PID_KEYBOARD        0x0001
#define USB_PID_SERIAL          0x0002
#define USB_PID_BULK            0x0003
#define USB_PID_SCOPE           0x0004
#define USB_PID_MSC             0x0005
#define USB_PID_AUDIO           0x0006
#define USB_PID_COMP_SERIAL     0x0007
#define USB_PID_COMP_AUDIO_HID  0x0008
#define USB_PID_COMP_HID_SER    0x0009
#define USB_PID_DFU             0x00FF

#ifndef DEPRECATED
//*****************************************************************************
//
// Deprecated definitions.
//
//*****************************************************************************
#define USB_VID_LUMINARY        USB_VID_STELLARIS
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USBIDS_H__ */
