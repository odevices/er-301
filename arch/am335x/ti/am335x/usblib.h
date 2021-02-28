/* ============================================================= */
/**
 * usblib.h - Main header file for the USB Library.
 *  ============================================================
 *  Copyright (c) Texas Instruments Incorporated 2018 - 2019
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

#ifndef PDK__USBLIB_H
#define PDK__USBLIB_H

/******************************************************************************
 *
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *
 *****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <ti/am335x/usb_dev_object.h>

/******************************************************************************
 *
 * This is the maximum number of endpoints supported by the usblib.
 *
 *****************************************************************************/
#define USBLIB_NUM_EP 16 /* Number of supported endpoints. */

/******************************************************************************
 *
 * The following macro allows compiler-independent syntax to be used to
 * define packed structures.  A typical structure definition using these
 * macros will look similar to the following example:
 *
 *   #ifdef ewarm
 *   #pragma pack(1)
 *   #endif
 *
 *   typedef struct _PackedStructName
 *   {
 *      uint32_t ulFirstField;
 *      char cCharMember;
 *      uint16_t usShort;
 *   }
 *   USBLIB_PACKED tPackedStructName;
 *
 *   #ifdef ewarm
 *   #pragma pack()
 *   #endif
 *
 * The conditional blocks related to ewarm include the #pragma pack() lines
 * only if the IAR Embedded Workbench compiler is being used.  Unfortunately,
 * it is not possible to emit a #pragma from within a macro definition so this
 * must be done explicitly.
 *
 ******************************************************************************/
#if defined(ccs) ||             \
    defined(codered) ||         \
    defined(gcc) ||             \
    defined(__aarch64__) ||     \
    defined(rvmdk) ||           \
    defined(__ARMCC_VERSION) || \
    defined(sourcerygxx)
#define USBLIB_PACKED __attribute__((packed))
#elif defined(ewarm) || defined(__IAR_SYSTEMS_ICC__)
#define USBLIB_PACKED
#elif defined(__TMS470__)
#if defined(__TI_TMS470_V7__) || (__TI_COMPILER_VERSION__ >= 5000000)
#define USBLIB_PACKED __attribute__((packed))
#else
#warn The packed attribute is not supported by this version of the TI ARM CGT.Please upgrade to version 5.x or use an alternate toolchain instead.
#define USBLIB_PACKED
#endif
#elif defined(_TMS320C6X)
#define USBLIB_PACKED __attribute__((packed))
#else
#error Unrecognized COMPILER!
#endif

/******************************************************************************
 *
 * Assorted language IDs from the document "USB_LANGIDs.pdf" provided by the
 * USB Implementers' Forum (Version 1.0).
 *
 ******************************************************************************/
#define USB_LANG_CHINESE_PRC (0x0804)    /* Chinese (PRC) */
#define USB_LANG_CHINESE_TAIWAN (0x0404) /* Chinese (Taiwan) */
#define USB_LANG_EN_US (0x0409)          /* English (United States) */
#define USB_LANG_EN_UK (0x0809)          /* English (United Kingdom) */
#define USB_LANG_EN_AUS (0x0C09)         /* English (Australia) */
#define USB_LANG_EN_CA (0x1009)          /* English (Canada) */
#define USB_LANG_EN_NZ (0x1409)          /* English (New Zealand) */
#define USB_LANG_FRENCH (0x040C)         /* French (Standard) */
#define USB_LANG_GERMAN (0x0407)         /* German (Standard) */
#define USB_LANG_HINDI (0x0439)          /* Hindi */
#define USB_LANG_ITALIAN (0x0410)        /* Italian (Standard) */
#define USB_LANG_JAPANESE (0x0411)       /* Japanese */
#define USB_LANG_KOREAN (0x0412)         /* Korean */
#define USB_LANG_ES_TRAD (0x040A)        /* Spanish (Traditional) */
#define USB_LANG_ES_MODERN (0x0C0A)      /* Spanish (Modern) */
#define USB_LANG_SWAHILI (0x0441)        /* Swahili (Kenya) */
#define USB_LANG_URDU_IN (0x0820)        /* Urdu (India) */
#define USB_LANG_URDU_PK (0x0420)        /* Urdu (Pakistan) */
/******************************************************************************
 *
 *! \addtogroup usbchap9_src
 *! @{
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Note:
 *
 * Structure definitions which are derived directly from the USB specification
 * use field names from the specification.  Since a somewhat different version
 * of Hungarian prefix notation is used from the Stellaris standard, beware of
 * making assumptions about field sizes based on the field prefix when using
 * these structures.  Of particular note is the difference in the meaning of
 * the 'i' prefix.  In USB structures, this indicates a single byte index
 * whereas in Stellaris code, this is a 32 bit integer.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * All structures defined in this section of the header require byte packing of
 * fields.  This is usually accomplished using the USBLIB_PACKED macro but, for IAR
 * Embedded Workbench, this requires a pragma.
 *
 ******************************************************************************/
#if defined(ewarm) || defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif

    /******************************************************************************
 *
 * Definitions related to standard USB device requests (sections 9.3 & 9.4)
 *
 ******************************************************************************/

    /******************************************************************************
 *
 *! The standard USB request header as defined in section 9.3 of the USB 2.0
 *! specification.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! Determines the type and direction of the request.
     */
        uint8_t bmRequestType;

        /*
     *! Identifies the specific request being made.
     */
        uint8_t bRequest;

        /*
     *! Word-sized field that varies according to the request.
     */
        uint16_t wValue;

        /*
     *! Word-sized field that varies according to the request; typically used
     *! to pass an index or offset.
     */
        uint16_t wIndex;

        /*
     *! The number of bytes to transfer if there is a data stage to the
     *! request.
     */
        uint16_t wLength;

    } USBLIB_PACKED tUSBRequest;

/******************************************************************************
 *
 * The following defines are used with the bmRequestType member of tUSBRequest.
 *
 * Request types have 3 bit fields:
 * 4:0 - Is the recipient type.
 * 6:5 - Is the request type.
 * 7 - Is the direction of the request.
 *
 ******************************************************************************/
#define USB_RTYPE_DIR_IN 0x80
#define USB_RTYPE_DIR_OUT 0x00

#define USB_RTYPE_TYPE_M 0x60
#define USB_RTYPE_VENDOR 0x40
#define USB_RTYPE_CLASS 0x20
#define USB_RTYPE_STANDARD 0x00

#define USB_RTYPE_RECIPIENT_M 0x1f
#define USB_RTYPE_OTHER 0x03
#define USB_RTYPE_ENDPOINT 0x02
#define USB_RTYPE_INTERFACE 0x01
#define USB_RTYPE_DEVICE 0x00

/******************************************************************************
 *
 * Standard USB requests IDs used in the bRequest field of tUSBRequest.
 *
 ******************************************************************************/
#define USBREQ_GET_STATUS 0x00
#define USBREQ_CLEAR_FEATURE 0x01
#define USBREQ_SET_FEATURE 0x03
#define USBREQ_SET_ADDRESS 0x05
#define USBREQ_GET_DESCRIPTOR 0x06
#define USBREQ_SET_DESCRIPTOR 0x07
#define USBREQ_GET_CONFIG 0x08
#define USBREQ_SET_CONFIG 0x09
#define USBREQ_GET_INTERFACE 0x0a
#define USBREQ_SET_INTERFACE 0x0b
#define USBREQ_SYNC_FRAME 0x0c

/******************************************************************************
 *
 * Data returned from a USBREQ_GET_STATUS request to a device.
 *
 ******************************************************************************/
#define USB_STATUS_SELF_PWR 0x0001    /* Currently self powered. */
#define USB_STATUS_BUS_PWR 0x0000     /* Currently bus-powered. */
#define USB_STATUS_PWR_M 0x0001       /* Mask for power mode. */
#define USB_STATUS_REMOTE_WAKE 0x0002 /* Remote wake-up is currently enabled.*/

/******************************************************************************
 *
 * Feature Selectors (tUSBRequest.wValue) passed on USBREQ_CLEAR_FEATURE and
 * USBREQ_SET_FEATURE.
 *
 ******************************************************************************/
#define USB_FEATURE_EP_HALT 0x0000     /* Endpoint halt feature. */
#define USB_FEATURE_REMOTE_WAKE 0x0001 /* Remote wake feature, device only. */
#define USB_FEATURE_TEST_MODE 0x0002   /* Test mode */

/******************************************************************************
 *
 * Endpoint Selectors (tUSBRequest.wIndex) passed on USBREQ_CLEAR_FEATURE,
 * USBREQ_SET_FEATURE and USBREQ_GET_STATUS.
 *
 ******************************************************************************/
#define USB_REQ_EP_NUM_M 0x007F
#define USB_REQ_EP_DIR_M 0x0080
#define USB_REQ_EP_DIR_IN 0x0080
#define USB_REQ_EP_DIR_OUT 0x0000

/******************************************************************************
 *
 * Standard USB descriptor types.  These values are passed in the upper bytes
 * of tUSBRequest.wValue on USBREQ_GET_DESCRIPTOR and also appear in the
 * bDescriptorType field of standard USB descriptors.
 *
 ******************************************************************************/
#define USB_DTYPE_DEVICE 1
#define USB_DTYPE_CONFIGURATION 2
#define USB_DTYPE_STRING 3
#define USB_DTYPE_INTERFACE 4
#define USB_DTYPE_ENDPOINT 5
#define USB_DTYPE_DEVICE_QUAL 6
#define USB_DTYPE_OSPEED_CONF 7
#define USB_DTYPE_INTERFACE_PWR 8
#define USB_DTYPE_OTG 9
#define USB_DTYPE_INTERFACE_ASC 11
#define USB_DTYPE_CS_INTERFACE 36

    /******************************************************************************
 *
 * Definitions related to USB descriptors (sections 9.5 & 9.6)
 *
 ******************************************************************************/

    /******************************************************************************
 *
 *! This structure describes a generic descriptor header.  These fields are to
 *! be found at the beginning of all valid USB descriptors.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor (including this length byte) expressed
     *! in bytes.
     */
        uint8_t bLength;

        /*
     *! The type identifier of the descriptor whose information follows.  For
     *! standard descriptors, this field could contain, for example,
     *! USB_DTYPE_DEVICE to identify a device descriptor or USB_DTYPE_ENDPOINT
     *! to identify an endpoint descriptor.
     */
        uint8_t bDescriptorType;
    } USBLIB_PACKED tDescriptorHeader;

    /******************************************************************************
 *
 *! This structure describes the USB device descriptor as defined in USB
 *! 2.0 specification section 9.6.1.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  All device descriptors are
     *! 18 bytes long.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For a device descriptor, this will be
     *! USB_DTYPE_DEVICE (1).
     */
        uint8_t bDescriptorType;

        /*
     *! The USB Specification Release Number in BCD format.  For USB 2.0, this
     *! will be 0x0200.
     */
        uint16_t bcdUSB;

        /*
     *! The device class code.
     */
        uint8_t bDeviceClass;

        /*
     *! The device subclass code.  This value qualifies the value found in the
     *! bDeviceClass field.
     */
        uint8_t bDeviceSubClass;

        /*
     *! The device protocol code.  This value is qualified by the values of
     *! bDeviceClass and bDeviceSubClass.
     */
        uint8_t bDeviceProtocol;

        /*
     *! The maximum packet size for endpoint zero.  Valid values are 8, 16, 32
     *! and 64.
     */
        uint8_t bMaxPacketSize0;

        /*
     *! The device Vendor ID (VID) as assigned by the USB-IF.
     */
        uint16_t idVendor;

        /*
     *! The device Product ID (PID) as assigned by the manufacturer.
     */
        uint16_t idProduct;

        /*
     *! The device release number in BCD format.
     */
        uint16_t bcdDevice;

        /*
     *! The index of a string descriptor describing the manufacturer.
     */
        uint8_t iManufacturer;

        /*
     *! The index of a string descriptor describing the product.
     */
        uint8_t iProduct;

        /*
     *! The index of a string descriptor describing the device's serial
     *! number.
     */
        uint8_t iSerialNumber;

        /*
     *! The number of possible configurations offered by the device.  This
     *! field indicates the number of distinct configuration descriptors that
     *! the device offers.
     */
        uint8_t bNumConfigurations;
    } USBLIB_PACKED tDeviceDescriptor;

/******************************************************************************
 *
 * USB Device Class codes used in the tDeviceDescriptor.bDeviceClass field.
 * Definitions for the bDeviceSubClass and bDeviceProtocol fields are device
 * specific and can be found in the appropriate device class header files.
 *
 ******************************************************************************/
#define USB_CLASS_DEVICE 0x00
#define USB_CLASS_AUDIO 0x01
#define USB_CLASS_CDC 0x02
#define USB_CLASS_HID 0x03
#define USB_CLASS_PHYSICAL 0x05
#define USB_CLASS_IMAGE 0x06
#define USB_CLASS_PRINTER 0x07
#define USB_CLASS_MASS_STORAGE 0x08
#define USB_CLASS_HUB 0x09
#define USB_CLASS_CDC_DATA 0x0a
#define USB_CLASS_SMART_CARD 0x0b
#define USB_CLASS_SECURITY 0x0d
#define USB_CLASS_VIDEO 0x0e
#define USB_CLASS_HEALTHCARE 0x0f
#define USB_CLASS_DIAG_DEVICE 0xdc
#define USB_CLASS_WIRELESS 0xe0
#define USB_CLASS_MISC 0xef
#define USB_CLASS_APP_SPECIFIC 0xfe
#define USB_CLASS_VEND_SPECIFIC 0xff
#define USB_CLASS_EVENTS (0xffffffffU)

/******************************************************************************
 *
 * Generic values for undefined subclass and protocol.
 *
 ******************************************************************************/
#define USB_SUBCLASS_UNDEFINED 0x00
#define USB_PROTOCOL_UNDEFINED 0x00

/******************************************************************************
 *
 * The following are the miscellaneous subclass values.
 *
 ******************************************************************************/
#define USB_MISC_SUBCLASS_SYNC 0x01
#define USB_MISC_SUBCLASS_COMMON 0x02

/******************************************************************************
 *
 * These following are miscellaneous protocol values.
 *
 ******************************************************************************/
#define USB_MISC_PROTOCOL_IAD 0x01

    /******************************************************************************
 *
 *! This structure describes the USB device qualifier descriptor as defined in
 *! the USB 2.0 specification, section 9.6.2.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  All device qualifier
     *! descriptors are 10 bytes long.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For a device descriptor, this will be
     *! USB_DTYPE_DEVICE_QUAL (6).
     */
        uint8_t bDescriptorType;

        /*
     *! The USB Specification Release Number in BCD format.  For USB 2.0, this
     *! will be 0x0200.
     */
        uint16_t bcdUSB;

        /*
     *! The device class code.
     */
        uint8_t bDeviceClass;

        /*
     *! The device subclass code.  This value qualifies the value found in the
     *! bDeviceClass field.
     */
        uint8_t bDeviceSubClass;

        /*
     *! The device protocol code.  This value is qualified by the values of
     *! bDeviceClass and bDeviceSubClass.
     */
        uint8_t bDeviceProtocol;

        /*
     *! The maximum packet size for endpoint zero when operating at a speed
     *! other than high speed.
     */
        uint8_t bMaxPacketSize0;

        /*
     *! The number of other-speed configurations supported.
     */
        uint8_t bNumConfigurations;

        /*
     *! Reserved for future use.  Must be set to zero.
     */
        uint8_t bReserved;
    } USBLIB_PACKED tDeviceQualifierDescriptor;

    /******************************************************************************
 *
 *! This structure describes the USB configuration descriptor as defined in
 *! USB 2.0 specification section 9.6.3.  This structure also applies to the
 *! USB other speed configuration descriptor defined in section 9.6.4.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  All configuration descriptors
     *! are 9 bytes long.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For a configuration descriptor, this will
     *! be USB_DTYPE_CONFIGURATION (2).
     */
        uint8_t bDescriptorType;

        /*
     *! The total length of data returned for this configuration.  This
     *! includes the combined length of all descriptors (configuration,
     *! interface, endpoint and class- or vendor-specific) returned for this
     *! configuration.
     */
        uint16_t wTotalLength;

        /*
     *! The number of interface supported by this configuration.
     */
        uint8_t bNumInterfaces;

        /*
     *! The value used as an argument to the SetConfiguration standard request
     *! to select this configuration.
     */
        uint8_t bConfigurationValue;

        /*
     *! The index of a string descriptor describing this configuration.
     */
        uint8_t iConfiguration;

        /*
     *! Attributes of this configuration.
     */
        uint8_t bmAttributes;

        /*
     *! The maximum power consumption of the USB device from the bus in this
     *! configuration when the device is fully operational.  This is expressed
     *! in units of 2mA so, for example, 100 represents 200mA.
     */
        uint8_t bMaxPower;
    } USBLIB_PACKED tConfigDescriptor;

/******************************************************************************
 *
 * Flags used in constructing the value assigned to the field
 * tConfigDescriptor.bmAttributes.  Note that bit 7 is reserved and must be set
 * to 1.
 *
 ******************************************************************************/
#define USB_CONF_ATTR_PWR_M 0xC0

#define USB_CONF_ATTR_SELF_PWR 0xC0
#define USB_CONF_ATTR_BUS_PWR 0x80
#define USB_CONF_ATTR_RWAKE 0xA0

    /******************************************************************************
 *
 *! This structure describes the USB interface descriptor as defined in USB
 *! 2.0 specification section 9.6.5.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  All interface descriptors
     *! are 9 bytes long.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For an interface descriptor, this will
     *! be USB_DTYPE_INTERFACE (4).
     */
        uint8_t bDescriptorType;

        /*
     *! The number of this interface.  This is a zero based index into the
     *! array of concurrent interfaces supported by this configuration.
     */
        uint8_t bInterfaceNumber;

        /*
     *! The value used to select this alternate setting for the interface
     *! defined in bInterfaceNumber.
     */
        uint8_t bAlternateSetting;

        /*
     *! The number of endpoints used by this interface (excluding endpoint
     *! zero).
     */
        uint8_t bNumEndpoints;

        /*
     *! The interface class code as assigned by the USB-IF.
     */
        uint8_t bInterfaceClass;

        /*
     *! The interface subclass code as assigned by the USB-IF.
     */
        uint8_t bInterfaceSubClass;

        /*
     *! The interface protocol code as assigned by the USB-IF.
     */
        uint8_t bInterfaceProtocol;

        /*
     *! The index of a string descriptor describing this interface.
     */
        uint8_t iInterface;
    } USBLIB_PACKED tInterfaceDescriptor;

    /******************************************************************************
 *
 *! This structure describes the USB endpoint descriptor as defined in USB
 *! 2.0 specification section 9.6.6.
 *
******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  All endpoint descriptors
     *! are 7 bytes long.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For an endpoint descriptor, this will
     *! be USB_DTYPE_ENDPOINT (5).
     */
        uint8_t bDescriptorType;

        /*
     *! The address of the endpoint.  This field contains the endpoint number
     *! ORed with flag USB_EP_DESC_OUT or USB_EP_DESC_IN to indicate the
     *! endpoint direction.
     */
        uint8_t bEndpointAddress;

        /*
     *! The endpoint transfer type, USB_EP_ATTR_CONTROL, USB_EP_ATTR_ISOC,
     *! USB_EP_ATTR_BULK or USB_EP_ATTR_INT and, if isochronous, additional
     *! flags indicating usage type and synchronization method.
     */
        uint8_t bmAttributes;

        /*
     *! The maximum packet size this endpoint is capable of sending or
     *! receiving when this configuration is selected.  For high speed
     *! isochronous or interrupt endpoints, bits 11 and 12 are used to
     *! pass additional information.
     */
        uint16_t wMaxPacketSize;

        /*
     *! The polling interval for data transfers expressed in frames or
     *! micro frames depending upon the operating speed.
     */
        uint8_t bInterval;
    } USBLIB_PACKED tEndpointDescriptor;

/******************************************************************************
 *
 * Flags used in constructing the value assigned to the field
 * tEndpointDescriptor.bEndpointAddress.
 *
 ******************************************************************************/
#define USB_EP_DESC_OUT 0x00
#define USB_EP_DESC_IN 0x80
#define USB_EP_DESC_NUM_M 0x0f

/******************************************************************************
 *
 * Mask used to extract the maximum packet size (in bytes) from the
 * wMaxPacketSize field of the endpoint descriptor.
 *
 ******************************************************************************/
#define USB_EP_MAX_PACKET_COUNT_M 0x07FF

/******************************************************************************
 *
 * Endpoint attributes used in tEndpointDescriptor.bmAttributes.
 *
 ******************************************************************************/
#define USB_EP_ATTR_CONTROL 0x00
#define USB_EP_ATTR_ISOC 0x01
#define USB_EP_ATTR_BULK 0x02
#define USB_EP_ATTR_INT 0x03
#define USB_EP_ATTR_TYPE_M 0x03

#define USB_EP_ATTR_ISOC_M 0x0c
#define USB_EP_ATTR_ISOC_NOSYNC 0x00
#define USB_EP_ATTR_ISOC_ASYNC 0x04
#define USB_EP_ATTR_ISOC_ADAPT 0x08
#define USB_EP_ATTR_ISOC_SYNC 0x0c
#define USB_EP_ATTR_USAGE_M 0x30
#define USB_EP_ATTR_USAGE_DATA 0x00
#define USB_EP_ATTR_USAGE_FEEDBACK 0x10
#define USB_EP_ATTR_USAGE_IMPFEEDBACK 0x20

    /******************************************************************************
 *
 *! This structure describes the USB string descriptor for index 0 as defined
 *! in USB 2.0 specification section 9.6.7.  Note that the number of language
 *! IDs is variable and can be determined by examining bLength.  The number of
 *! language IDs present in the descriptor is given by ((bLength - 2) / 2).
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  This value will vary
     *! depending upon the number of language codes provided in the descriptor.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For a string descriptor, this will be
     *! USB_DTYPE_STRING (3).
     */
        uint8_t bDescriptorType;

        /*
     *! The language code (LANGID) for the first supported language.  Note that
     *! this descriptor may support multiple languages, in which case, the
     *! number of elements in the wLANGID array will increase and bLength will
     *! be updated accordingly.
     */
        uint16_t wLANGID[1];
    } USBLIB_PACKED tString0Descriptor;

    /******************************************************************************
 *
 *! This structure describes the USB string descriptor for all string indexes
 *! other than 0 as defined in USB 2.0 specification section 9.6.7.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The length of this descriptor in bytes.  This value will be 2 greater
     *! than the number of bytes comprising the UNICODE string that the
     *! descriptor contains.
     */
        uint8_t bLength;

        /*
     *! The type of the descriptor.  For a string descriptor, this will be
     *! USB_DTYPE_STRING (3).
     */
        uint8_t bDescriptorType;

        /*
     *! The first byte of the UNICODE string.  This string is not NULL
     *! terminated.  Its length (in bytes) can be computed by subtracting 2
     *! from the value in the bLength field.
     */
        uint8_t bString;
    } USBLIB_PACKED tStringDescriptor;

/******************************************************************************
 *
 *! Write a 2 byte uint16_t value to a USB descriptor block.
 *!
 *! \param usValue is the two byte uint16_t that is to be written to
 *! the descriptor.
 *!
 *! This helper macro is used in descriptor definitions to write two-byte
 *! values.  Since the configuration descriptor contains all interface and
 *! endpoint descriptors in a contiguous block of memory, these descriptors are
 *! typically defined using an array of bytes rather than as packed structures.
 *!
 *! \return Not a function.
 *
 ******************************************************************************/
#define USBShort(usValue) ((usValue)&0xff), ((usValue) >> 8)

/******************************************************************************
 *
 *! Write a 3 byte unsigned long value to a USB descriptor block.
 *!
 *! \param ulValue is the three byte unsigned value that is to be written to the
 *! descriptor.
 *!
 *! This helper macro is used in descriptor definitions to write three-byte
 *! values.  Since the configuration descriptor contains all interface and
 *! endpoint descriptors in a contiguous block of memory, these descriptors are
 *! typically defined using an array of bytes rather than as packed structures.
 *!
 *! \return Not a function.
 *
 ******************************************************************************/
#define USB3Byte(ulValue) ((ulValue)&0xff),          \
                          (((ulValue) >> 8) & 0xff), \
                          (((ulValue) >> 16) & 0xff)

/******************************************************************************
 *
 *! Write a 4 byte unsigned long value to a USB descriptor block.
 *!
 *! \param ulValue is the four byte unsigned long that is to be written to the
 *! descriptor.
 *!
 *! This helper macro is used in descriptor definitions to write four-byte
 *! values.  Since the configuration descriptor contains all interface and
 *! endpoint descriptors in a contiguous block of memory, these descriptors are
 *! typically defined using an array of bytes rather than as packed structures.
 *!
 *! \return Not a function.
 *
 ******************************************************************************/
#define USBLong(ulValue) (ulValue & 0xff),         \
                         ((ulValue >> 8) & 0xff),  \
                         ((ulValue >> 16) & 0xff), \
                         ((ulValue >> 24) & 0xff)

/******************************************************************************
 *
 *! Traverse to the next USB descriptor in a block.
 *!
 *! \param ptr points to the first byte of a descriptor in a block of
 *! USB descriptors.
 *!
 *! This macro aids in traversing lists of descriptors by returning a pointer
 *! to the next descriptor in the list given a pointer to the current one.
 *!
 *! \return Returns a pointer to the next descriptor in the block following
 *! \e ptr.
 *!
 ******************************************************************************/
#define NEXT_USB_DESCRIPTOR(ptr)               \
    (tDescriptorHeader *)(((uint8_t *)(ptr)) + \
                          *((uint8_t *)(ptr)))

/******************************************************************************
 *
 * Return to default packing when using the IAR Embedded Workbench compiler.
 *
 ******************************************************************************/
#if defined(ewarm) || defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

    /******************************************************************************
 *
 * Close the usbchap9_src Doxygen group.
 *! @}
 *
 ******************************************************************************/

    /******************************************************************************
 *
 *! \addtogroup device_api
 *! @{
 *
 ******************************************************************************/

    /******************************************************************************
 *
 * Function prototype for any standard USB request.
 *
 ******************************************************************************/
    typedef void (*tStdRequest)(struct usbGadgetObj *pGadgetObject,
                                usbEndpt0event_t endptEvent,
                                usbSetupPkt_t *setup);

    /******************************************************************************
 *
 * Data callback for receiving data from an endpoint.
 *
 ******************************************************************************/
    typedef void (*tInfoCallback)(void *pvInstance, uint32_t ulInfo,
                                  uint32_t ulIndex);

    /******************************************************************************
 *
 * Callback made to indicate that an interface alternate setting change has
 * occurred.
 *
 ******************************************************************************/
    typedef void (*tInterfaceCallback)(void *pvInstance,
                                       uint8_t ucInterfaceNum,
                                       uint8_t ucAlternateSetting);

    /******************************************************************************
 *
 * Generic interrupt handler callbacks.
 *
 ******************************************************************************/
    typedef void (*tUSBIntHandler)(void *pvInstance);

    /******************************************************************************
 *
 * Interrupt handler callbacks that have status information.
 *
 ******************************************************************************/
    typedef void (*tUSBEPIntHandler)(struct usbGadgetObj *pUsbGadgetObj,
                                     usbGenericEvent_t endptEvent,
                                     uint32_t epNo,
                                     uint32_t length);

    /******************************************************************************
 *
 * Generic handler callbacks that are used when the callers needs to call into
 * an instance of class.
 *
 ******************************************************************************/
    typedef void (*tUSBDeviceHandler)(void *pvInstance,
                                      uint32_t ulRequest,
                                      void *pvRequestData);

    /******************************************************************************
 *
 *! USB event handler functions used during enumeration and operation of the
 *! device stack.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! This callback is made whenever the USB host requests a non-standard
     *! descriptor from the device.
     */
        tStdRequest pfnGetDescriptor;

        /*
     *! This callback is made whenever the USB host makes a non-standard
     *! request.
     */
        tStdRequest pfnRequestHandler;

        /*
     *! This callback is made in response to a SetInterface request from the
     *! host.
     */
        tInterfaceCallback pfnInterfaceChange;

        /*
     *! This callback is made in response to a SetConfiguration request from
     *! the host.
     */
        tInfoCallback pfnConfigChange;

        /*
     *! This callback is made when data has been received following to a call
     *! to USBDCDRequestDataEP0.
     */
        tInfoCallback pfnDataReceived;

        /*
     *! This callback is made when data has been transmitted following a call
     *! to USBDCDSendDataEP0.
     */
        tInfoCallback pfnDataSent;

        /*
     *! This callback is made when a USB reset is detected.
     */
        tUSBIntHandler pfnResetHandler;

        /*
     *! This callback is made when the bus has been inactive long enough to
     *! trigger a suspend condition.
     */
        tUSBIntHandler pfnSuspendHandler;

        /*
     *! This is called when resume signaling is detected.
     */
        tUSBIntHandler pfnResumeHandler;

        /*
     *! This callback is made when the device is disconnected from the USB bus.
     */
        tUSBIntHandler pfnDisconnectHandler;

        /*
     *! This callback is made to inform the device of activity on all endpoints
     *! other than endpoint zero.
     */
        tUSBEPIntHandler pfnEndpointHandler;

        /*
     *! This generic handler is provided to allow requests based on
     *! a given instance to be passed into a device.  This is commonly used
     *! by a top level composite device that is using multiple instances of
     *! a class.
     */
        tUSBDeviceHandler pfnDeviceHandler;
    } tCustomHandlers;

    /******************************************************************************
 *
 *! This structure defines how a given endpoint's FIFO is configured in
 *! relation to the maximum packet size for the endpoint as specified in the
 *! endpoint descriptor.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The multiplier to apply to an endpoint's maximum packet size when
     *! configuring the FIFO for that endpoint.  For example, setting this
     *! value to 2 will result in a 128 byte FIFO being configured if
     *! bDoubleBuffer is false and the associated endpoint is set to use a 64
     *! byte maximum packet size.
     */
        uint8_t cMultiplier;

        /*
     *! This field indicates whether to configure an endpoint's FIFO to be
     *! double- or single-buffered.  If true, a double-buffered FIFO is
     *! created and the amount of required FIFO storage is multiplied by two.
     */
        uint32_t bDoubleBuffer;

        /*
     *! This field defines endpoint mode flags which cannot be deduced from
     *! the configuration descriptor, namely any in the set USB_EP_AUTO_xxx or
     *! USB_EP_DMA_MODE_x.  USBDCDConfig adds these flags to the endpoint
     *! mode and direction determined from the config descriptor before it
     *! configures the endpoint using a call to USBDevEndpointConfigSet().
     */
        uint16_t usEPFlags;
    } tFIFOEntry;

    /******************************************************************************
 *
 *! This structure defines endpoint and FIFO configuration information for
 *! all endpoints that the device wishes to use.  This information cannot be
 *! determined by examining the USB configuration descriptor and is
 *! provided to USBDCDConfig by the application to allow the USB controller
 *! endpoints to be correctly configured.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! An array containing one FIFO entry for each of the IN endpoints.
     *! Note that endpoint 0 is configured and managed by the USB device stack
     *! so is excluded from this array.  The index 0 entry of the array
     *! corresponds to endpoint 1, index 1 to endpoint 2, etc.
     */
        tFIFOEntry sIn[USBLIB_NUM_EP - 1];

        /*
     *! An array containing one FIFO entry for each of the OUT endpoints.
     *! Note that endpoint 0 is configured and managed by the USB device stack
     *! so is excluded from this array.  The index 0 entry of the array
     *! corresponds to endpoint 1, index 1 to endpoint 2, etc.
     */
        tFIFOEntry sOut[USBLIB_NUM_EP - 1];
    } tFIFOConfig;

    /******************************************************************************
 *
 *! This structure defines a contiguous block of data which contains a group
 *! of descriptors that form part of a configuration descriptor for a device.
 *! It is assumed that a config section contains only whole descriptors.  It is
 *! not valid to split a single descriptor across multiple sections.
 *!
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The number of bytes of descriptor data pointed to by pucData.
     */
        uint8_t ucSize;

        /*
     *! A pointer to a block of data containing an integral number of
     *! USB descriptors which form part of a larger configuration descriptor.
     */
        const uint8_t *pucData;
    } tConfigSection;

    /******************************************************************************
 *
 *! This is the top level structure defining a USB device configuration
 *! descriptor.  A configuration descriptor contains a collection of device-
 *! specific descriptors in addition to the basic config, interface and
 *! endpoint descriptors.  To allow flexibility in constructing the
 *! configuration, the descriptor is described in terms of a list of data
 *! blocks.  The first block must contain the configuration descriptor itself
 *! and the following blocks are appended to this in order to produce the
 *! full descriptor sent to the host in response to a GetDescriptor request
 *! for the configuration descriptor.
 *!
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The number of sections comprising the full descriptor for this
     *! configuration.
     */
        uint8_t ucNumSections;

        /*
     *! A pointer to an array of ucNumSections section pointers which must
     *! be concatenated to form the configuration descriptor.
     */
        const tConfigSection *const *psSections;
    } tConfigHeader;

    /******************************************************************************
 *
 *! This structure is passed to the USB library on a call to USBDCDInit and
 *! provides the library with information about the device that the
 *! application is implementing.  It contains functions pointers for the
 *! various USB event handlers and pointers to each of the standard device
 *! descriptors.
 *
 ******************************************************************************/
    typedef struct DeviceInfo
    {
        /*
     *! A pointer to a structure containing pointers to event handler functions
     *! provided by the client to support the operation of this device.
     */
        tCustomHandlers sCallbacks;

        /*
     *! A pointer to the device descriptor for this device.
     */
        const uint8_t *pDeviceDescriptor;

        /*
     *! A pointer to an array of configuration descriptor pointers.  Each entry
     *! in the array corresponds to one configuration that the device may be set
     *! to use by the USB host.  The number of entries in the array must
     *! match the bNumConfigurations value in the device descriptor
     *! array, pDeviceDescriptor.
     */
        usbConfigHeader_t **ppConfigDescriptors;

        /*
     *! A pointer to the string descriptor array for this device.  This array
     *! must be arranged as follows:
     *!
     *!   - [0]   - Standard descriptor containing supported language codes.
     *!   - [1]   - String 1 for the first language listed in descriptor 0.
     *!   - [2]   - String 2 for the first language listed in descriptor 0.
     *!   - ...
     *!   - [n]   - String n for the first language listed in descriptor 0.
     *!   - [n+1] - String 1 for the second language listed in descriptor 0.
     *!   - ...
     *!   - [2n]  - String n for the second language listed in descriptor 0.
     *!   - [2n+1]- String 1 for the third language listed in descriptor 0.
     *!   - ...
     *!   - [3n]  - String n for the third language listed in descriptor 0.
     *!
     *! and so on.
     */
        uint8_t **ppStringDescriptors;

        /*
     *! The total number of descriptors provided in the ppStringDescriptors
     *! array.
     */
        uint32_t ulNumStringDescriptors;

        /*
     *! This value will be passed back to all call back functions so that
     *! they have access to individual instance data based on the this pointer.
     */
        void *pvInstance;

        /* pointer to gadget Obj that contains DCD data. This field would eventually
       replace pvInstance */
        void *pGadgetObj;
    } tDeviceInfo;

/******************************************************************************
 *
 * Close the Doxygen group.
 *! @}
 *
 ******************************************************************************/

/******************************************************************************
 *
 *! \addtogroup general_usblib_api
 *! @{
 *
 ******************************************************************************/

/******************************************************************************
 *
 * USB descriptor parsing functions found in usbdesc.c
 *
 ******************************************************************************/

/******************************************************************************
 *
 *! The USB_DESC_ANY label is used as a wild card in several of the descriptor
 *! parsing APIs to determine whether or not particular search criteria should
 *! be ignored.
 *
 ******************************************************************************/
#define USB_DESC_ANY (0xFFFFFFFFU)

    extern uint32_t USBDescGetNum(tDescriptorHeader *psDesc,
                                  uint32_t ulSize, uint32_t ulType);
    extern tDescriptorHeader *USBDescGet(tDescriptorHeader *psDesc,
                                         uint32_t ulSize,
                                         uint32_t ulType,
                                         uint32_t ulIndex);
    extern uint32_t
    USBDescGetNumAlternateInterfaces(tConfigDescriptor *psConfig,
                                     uint8_t ucInterfaceNumber);
    extern tInterfaceDescriptor *USBDescGetInterface(tConfigDescriptor *psConfig,
                                                     uint32_t ulIndex,
                                                     uint32_t ulAltCfg);
    extern tEndpointDescriptor *
    USBDescGetInterfaceEndpoint(tInterfaceDescriptor *psInterface,
                                uint32_t ulIndex,
                                uint32_t ulSize);

    /******************************************************************************
 *
 *! The operating mode required by the USB library client.  This type is used
 *! by applications which wish to be able to switch between host and device
 *! modes by calling the USBStackModeSet() API.
 *
 ******************************************************************************/
    typedef enum
    {
        /*
     *! The application wishes to operate as a USB device.
     */
        USB_MODE_DEVICE = 0,

        /*
     *! The application wishes to operate as a USB host.
     */
        USB_MODE_HOST,

        /*
     *! The application wishes to operate as both a host and device using
     *! On-The-Go protocols to negotiate.
     */
        USB_MODE_OTG,

        /*
     *! A marker indicating that no USB mode has yet been set by the
     *! application.
     */
        USB_MODE_NONE
    } tUSBMode;

    /******************************************************************************
 *
 * A pointer to a USB mode callback function.  This function is called by the
 * USB library to indicate to the application which operating mode it should
 * use, host or device.
 *
 ******************************************************************************/
    typedef void (*tUSBModeCallback)(uint32_t ulIndex, tUSBMode eMode);

    /******************************************************************************
 *
 * Mode selection and dual mode interrupt steering functions.
 *
 ******************************************************************************/
    extern void USBStackModeSet(uint32_t ulIndex, tUSBMode eUSBMode,
                                tUSBModeCallback pfnCallback);
    extern void USBDualModeInit(uint32_t ulIndex);
    extern void USBDualModeTerm(uint32_t ulIndex);
    extern void USBOTGMain(uint32_t ulMsTicks);
    extern void USBOTGPollRate(uint32_t ulIndex, uint32_t ulPollRate);
    extern void USBOTGModeInit(uint32_t ulIndex, uint32_t ulPollRate,
                               void *pHostData, uint32_t ulHostDataSize);
    extern void USBOTGModeTerm(uint32_t ulIndex);
    extern void USB0OTGModeIntHandler(void);
    extern void USB0DualModeIntHandler(void);

    /******************************************************************************
 *
 *! USB callback function.
 *!
 *! \param pvCBData is the callback pointer associated with the instance
 *!  generating the callback.  This is a value provided by the client during
 *! initialization of the instance making the callback.
 *! \param ulEvent is the identifier of the asynchronous event which is being
 *! notified to the client.
 *! \param ulMsgParam is an event-specific parameter.
 *! \param pvMsgData is an event-specific data pointer.
 *!
 *! A function pointer provided to the USB layer by the application
 *! which will be called to notify it of all asynchronous events relating to
 *! data transmission or reception.  This callback is used by device class
 *! drivers and host pipe functions.
 *!
 *! \return Returns an event-dependent value.
 *
 ******************************************************************************/
    typedef uint32_t (*tUSBCallback)(struct usbGadgetObj *pGadgetObject,
                                     void *pvCBData, uint32_t ulEvent,
                                     uint32_t ulMsgParam,
                                     void *pvMsgData);

/******************************************************************************
 *
 * Base identifiers for groups of USB events.  These are used by both the
 * device class drivers and host layer.
 *
 * USB_CLASS_EVENT_BASE is the lowest identifier that should be used for
 * a class-specific event.  Individual event bases are defined for each
 * of the supported device class drivers.  Events with IDs between
 * USB_EVENT_BASE and USB_CLASS_EVENT_BASE are reserved for stack use.
 *
 ******************************************************************************/
#define USB_EVENT_BASE 0x0000
#define USB_CLASS_EVENT_BASE 0x8000

/* Maximum number of USB controller instances available on the SoC.
 *
 * This macro is used to define different data structure instances used
 * by the driver based on the maximum USB cores available.
 */
#if defined(SOC_OMAPL137) || defined(SOC_OMAPL138)
#define USB_NUM_INSTANCE (1)
#else
#define USB_NUM_INSTANCE (2)
#endif

/*
 * Event base identifiers for the various device classes supported in host
 * and device modes.
 * The first 0x800 values of a range are reserved for the device specific
 * messages and the second 0x800 values of a range are used for the host
 * specific messages for a given class.
 *
 ******************************************************************************/
#define USBD_CDC_EVENT_BASE (USB_CLASS_EVENT_BASE + 0)
#define USBD_HID_EVENT_BASE (USB_CLASS_EVENT_BASE + 0x1000)
#define USBD_HID_KEYB_EVENT_BASE (USBD_HID_EVENT_BASE + 0x100)
#define USBD_BULK_EVENT_BASE (USB_CLASS_EVENT_BASE + 0x2000)
#define USBD_MSC_EVENT_BASE (USB_CLASS_EVENT_BASE + 0x3000)
#define USBD_AUDIO_EVENT_BASE (USB_CLASS_EVENT_BASE + 0x4000)

#define USBH_CDC_EVENT_BASE (USBD_CDC_EVENT_BASE + 0x800)
#define USBH_HID_EVENT_BASE (USBD_HID_EVENT_BASE + 0x800)
#define USBH_BULK_EVENT_BASE (USBD_BULK_EVENT_BASE + 0x800)
#define USBH_MSC_EVENT_BASE (USBD_MSC_EVENT_BASE + 0x800)
#define USBH_AUDIO_EVENT_BASE (USBD_AUDIO_EVENT_BASE + 0x800)

/******************************************************************************
 *
 * General events supported by device classes and host pipes.
 *
 ******************************************************************************/

/*
 *! The device is now attached to a USB host and ready to begin sending
 *! and receiving data (used by device classes only).
 */
#define USB_EVENT_CONNECTED (USB_EVENT_BASE + 0)

/*
 *! The device has been disconnected from the USB host (used by device classes
 *! only).
 *!
 *! Note: Due to a hardware erratum in revision A of LM3S3748, this
 *! event is not posted to self-powered USB devices when they are disconnected
 *! from the USB host.
 */
#define USB_EVENT_DISCONNECTED (USB_EVENT_BASE + 1)

/*
 *! Data has been received and is in the buffer provided.
 */
#define USB_EVENT_RX_AVAILABLE (USB_EVENT_BASE + 2)

/*
 *! This event is sent by a lower layer to inquire about the amount of
 *! unprocessed data buffered in the layers above.  It is used in cases
 *! where a low level driver needs to ensure that all preceding data has
 *! been processed prior to performing some action or making some notification.
 *! Clients receiving this event should return the number of bytes of data
 *! that are unprocessed or 0 if no outstanding data remains.
 */
#define USB_EVENT_DATA_REMAINING (USB_EVENT_BASE + 3)

/*
 *! This event is sent by a lower layer supporting DMA to request a buffer in
 *! which the next received packet may be stored.  The \e ulMsgValue parameter
 *! indicates the maximum size of packet that can be received in this channel
 *! and \e pvMsgData points to storage which should be written with the
 *! returned buffer pointer.  The return value from the callback should be the
 *! size of the buffer allocated (which may be less than the maximum size
 *! passed in \e ulMsgValue if the client knows that fewer bytes are expected
 *! to be received) or 0 if no buffer is being returned.
 */
#define USB_EVENT_REQUEST_BUFFER (USB_EVENT_BASE + 4)

/*
 *! Data has been sent and acknowledged.  If this event is received via the
 *! USB buffer callback, the \e ulMsgValue parameter indicates the number of
 *! bytes from the transmit buffer that have been successfully transmitted
 *! and acknowledged.
 */
#define USB_EVENT_TX_COMPLETE (USB_EVENT_BASE + 5)

/*
 *! An error has been reported on the channel or pipe.  The \e ulMsgValue
 *! parameter indicates the source(s) of the error and is the logical OR
 *! combination of "USBERR_" flags defined below.
 */
#define USB_EVENT_ERROR (USB_EVENT_BASE + 6)

/*
 *! The bus has entered suspend state.
 */
#define USB_EVENT_SUSPEND (USB_EVENT_BASE + 7)

/*
 *! The bus has left suspend state.
 */
#define USB_EVENT_RESUME (USB_EVENT_BASE + 8)

/*
 *! A scheduler event has occurred.
 */
#define USB_EVENT_SCHEDULER (USB_EVENT_BASE + 9)
    /*
 *! A device or host has detected a stall condition.
 */
#define USB_EVENT_STALL (USB_EVENT_BASE + 10)

/*
 *! The host detected a power fault condition.
 */
#define USB_EVENT_POWER_FAULT (USB_EVENT_BASE + 11)

/*
 *! The controller has detected a A-Side cable and needs power applied  This is
 *! only generated on OTG parts if automatic power control is disabled.
 */
#define USB_EVENT_POWER_ENABLE (USB_EVENT_BASE + 12)

/*
 *! The controller needs power removed,  This is only generated on OTG parts
 *! if automatic power control is disabled.
 */
#define USB_EVENT_POWER_DISABLE (USB_EVENT_BASE + 13)

/*
 *! Used with pfnDeviceHandler handler function is classes to indicate changes
 *! in the interface number by a class outside the class being accessed.
 *! Typically this is when composite device class is in use.
 *!
 *! The \e pvInstance value should point to an instance of the device being
 *! accessed.
 *!
 *! The \e ulRequest should be USB_EVENT_COMP_IFACE_CHANGE.
 *!
 *! The \e pvRequestData should point to a two byte array where the first value
 *! is the old interface number and the second is the new interface number.
 */
#define USB_EVENT_COMP_IFACE_CHANGE (USB_EVENT_BASE + 14)

/*
 *! Used with pfnDeviceHandler handler function is classes to indicate changes
 *! in endpoint number by a class outside the class being accessed.
 *! Typically this is when composite device class is in use.
 *!
 *! The \e pvInstance value should point to an instance of the device being
 *! accessed.
 *!
 *! The \e ulRequest should be USB_EVENT_COMP_EP_CHANGE.
 *!
 *! The \e pvRequestData should point to a two byte array where the first value
 *! is the old endpoint number and the second is the new endpoint number.  The
 *! endpoint numbers should be exactly as USB specification defines them and
 *! bit 7 set indicates an IN endpoint and bit 7 clear indicates an OUT
 *! endpoint.
 */
#define USB_EVENT_COMP_EP_CHANGE (USB_EVENT_BASE + 15)

/*
 *! Used with pfnDeviceHandler handler function is classes to indicate changes
 *! in string index number by a class outside the class being accessed.
 *! Typically this is when composite device class is in use.
 *!
 *! The \e pvInstance value should point to an instance of the device being
 *! accessed.
 *!
 *! The \e ulRequest should be USB_EVENT_COMP_STR_CHANGE.
 *!
 *! The \e pvRequestData should point to a two byte array where the first value
 *! is the old string index and the second is the new string index.
 */
#define USB_EVENT_COMP_STR_CHANGE (USB_EVENT_BASE + 16)

/*
 *! Used with pfnDeviceHandler handler function is classes to allow the device
 *! class to make final adjustments to the configuration descriptor.
 *! This is only used when a device class is used in a composite device class
 *! is in use.
 *!
 *! The \e pvInstance value should point to an instance of the device being
 *! accessed.
 *!
 *! The \e ulRequest should be USB_EVENT_COMP_CONFIG.
 *!
 *! The \e pvRequestData should point to the beginning of the configuration
 *! descriptor for the device instance.
 */
#define USB_EVENT_COMP_CONFIG (USB_EVENT_BASE + 17)
#define USB_EVENT_BABBLE_ERROR (USB_EVENT_BASE + 18)

/******************************************************************************
 *
 * Error sources reported via USB_EVENT_ERROR.
 *
 ******************************************************************************/

/*
 *! The host received an invalid PID in a transaction.
 */
#define USBERR_HOST_IN_PID_ERROR 0x01000000

/*
 *! The host did not receive a response from a device.
 */
#define USBERR_HOST_IN_NOT_COMP 0x00100000

/*
 *! The host received a stall on an IN endpoint.
 */
#define USBERR_HOST_IN_STALL 0x00400000

/*
 *! The host detected a CRC or bit-stuffing error (isochronous mode).
 */
#define USBERR_HOST_IN_DATA_ERROR 0x00080000

/*
 *! The host received NAK on an IN endpoint for longer than the specified
 *! timeout period (interrupt, bulk and control modes).
 */
#define USBERR_HOST_IN_NAK_TO 0x00080000

/*
 *! The host failed to communicate with a device via an IN endpoint.
 */
#define USBERR_HOST_IN_ERROR 0x00040000

/*
 *! The host receive FIFO is full.
 */
#define USBERR_HOST_IN_FIFO_FULL 0x00020000 /* RX FIFO full */
/*
 *! The host received NAK on an OUT endpoint for longer than the specified
 *! timeout period (bulk, interrupt and control modes).
 */
#define USBERR_HOST_OUT_NAK_TO 0x00000080

/*
 *! The host did not receive a response from a device (isochronous mode).
 */
#define USBERR_HOST_OUT_NOT_COMP 0x00000080

/*
 *! The host received a stall on an OUT endpoint.
 */
#define USBERR_HOST_OUT_STALL 0x00000020

/*
 *! The host failed to communicate with a device via an OUT endpoint.
 */
#define USBERR_HOST_OUT_ERROR 0x00000004

/*
 *! The host received NAK on endpoint 0 for longer than the configured
 *! timeout.
 */
#define USBERR_HOST_EP0_NAK_TO 0x00000080

/*
 *! The host failed to communicate with a device via an endpoint zero.
 */
#define USBERR_HOST_EP0_ERROR 0x00000010

/*
 *! The device detected a CRC error in received data.
 */
#define USBERR_DEV_RX_DATA_ERROR 0x00080000

/*
 *! The device was unable to receive a packet from the host since the receive
 *! FIFO is full.
 */
#define USBERR_DEV_RX_OVERRUN 0x00040000

/*
 *! The device receive FIFO is full.
 */
#define USBERR_DEV_RX_FIFO_FULL 0x00020000 /* RX FIFO full */

    /******************************************************************************
 *
 * Close the general_usblib_api Doxygen group.
 *! @}
 *
 ******************************************************************************/

    /******************************************************************************
 *
 *! \addtogroup usblib_buffer_api
 *! @{
 *
 ******************************************************************************/

    /******************************************************************************
 *
 *! A function pointer type which describes either a class driver packet read
 *! or packet write function (both have the same prototype) to the USB
 *! buffer object.
 *
 ******************************************************************************/
    typedef uint32_t (*tUSBPacketTransfer)(struct usbGadgetObj *pUsbGadgetObj,
                                           void *pvHandle,
                                           uint8_t *pcData,
                                           uint32_t ulLength,
                                           uint32_t bLast);

    /******************************************************************************
 *
 *! A function pointer type which describes either a class driver transmit
 *! or receive packet available function (both have the same prototype) to the
 *! USB buffer object.
 *
 ******************************************************************************/
    typedef uint32_t (*tUSBPacketAvailable)(void *pvHandle);

/******************************************************************************
 *
 *! The number of bytes of workspace that each USB buffer object requires.
 *! This workspace memory is provided to the buffer on USBBufferInit() in
 *! the \e pvWorkspace field of the \e tUSBBuffer structure.
 *
 ******************************************************************************/
#define USB_BUFFER_WORKSPACE_SIZE 16

    /******************************************************************************
 *
 *! The structure used by the application to initialize a buffer object that
 *! will provide buffered access to either a transmit or receive channel.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! This field sets the mode of the buffer.  If true, the buffer
     *! operates as a transmit buffer and supports calls to USBBufferWrite
     *! by the client.  If false, the buffer operates as a receive buffer
     *! and supports calls to USBBufferRead.
     */
        uint32_t bTransmitBuffer;

        /*
     *! A pointer to the callback function which will be called to notify
     *! the application of all asynchronous events related to the operation
     *! of the buffer.
     */
        tUSBCallback pfnCallback;

        /*
     *! A pointer that the buffer will pass back to the client in the
     *! first parameter of all callbacks related to this instance.
     */
        void *pvCBData;

        /*
     *! The function which should be called to transmit a packet of data
     *! in transmit mode or receive a packet in receive mode.
     */
        tUSBPacketTransfer pfnTransfer;

        /*
     *! The function which should be called to determine if the endpoint is
     *! ready to accept a new packet for transmission in transmit mode or
     *! to determine the size of the buffer required to read a packet in
     *! receive mode.
     */
        tUSBPacketAvailable pfnAvailable;

        /*
     *! The handle to pass to the low level function pointers
     *! provided in the pfnTransfer and pfnAvailable members.  For USB device
     *! use, this is the psDevice parameter required by the relevant device
     *! class driver APIs.  For USB host use, this is the pipe identifier
     *! returned by USBHCDPipeAlloc.
     */
        void *pvHandle;

        /*
     *! A pointer to memory to be used as the ring buffer for this
     *! instance.
     */
        uint8_t *pcBuffer;

        /*
     *! The size, in bytes, of the buffer pointed to by pcBuffer.
     */
        uint32_t ulBufferSize;

        /*
     *! A pointer to USB_BUFFER_WORKSPACE_SIZE bytes of RAM that the buffer
     *! object can use for workspace.
     */
        void *pvWorkspace;
    } tUSBBuffer;

    /******************************************************************************
 *
 *! The structure used for encapsulating all the items associated with a
 *! ring buffer.
 *
 ******************************************************************************/
    typedef struct
    {
        /*
     *! The ring buffer size.
     */
        uint32_t ulSize;

        /*
     *! The ring buffer write index.
     */
        volatile uint32_t ulWriteIndex;

        /*
     *! The ring buffer read index.
     */
        volatile uint32_t ulReadIndex;

        /*
     *! The ring buffer.
     */
        uint8_t *pucBuf;
    } tUSBRingBufObject;

    typedef struct
    {

        /*
     *USB Instance Number;
     */
        uint32_t uiUSBInstance;
        /*
     *Base address of the USB instance
     */
        uint32_t uiBaseAddr;
        /*
     *Base address of the USB sub system
     */
        uint32_t uiSubBaseAddr;
        /*
     *USB Interuppt number
     */
        uint32_t uiInterruptNum;
        /*
     *USB subsystem interrupt number
     */
        uint32_t uiSubInterruptNum;
        /*
     *PHY config register address
     */
        uint32_t uiPHYConfigRegAddr;

    } tUSBInstanceObject;
    extern tUSBInstanceObject g_USBInstance[];

    typedef struct _tUSBPerfInfo_
    {
        uint32_t ulBytesToTransfer;
        uint32_t ulTimeInTicks;
    } tUSBPerfInfo;

    extern tUSBPerfInfo g_USBPerfInfo[];
    extern uint32_t ulPerfInfoCounter;
    extern uint32_t fReadEnabled;
    extern uint32_t fWriteEnabled;
    extern uint32_t fUSBDisconnected;

    /******************************************************************************
 *
 * USB buffer API function prototypes.
 *
 ******************************************************************************/
    extern const tUSBBuffer *USBBufferInit(const tUSBBuffer *psBuffer);
    extern void USBBufferInfoGet(const tUSBBuffer *psBuffer,
                                 tUSBRingBufObject *psRingBuf);
    extern void *USBBufferCallbackDataSet(tUSBBuffer *psBuffer, void *pvCBData);
    extern uint32_t USBBufferWrite(const tUSBBuffer *psBuffer,
                                   const uint8_t *pucData,
                                   uint32_t ulLength);
    extern void USBBufferDataWritten(const tUSBBuffer *psBuffer,
                                     uint32_t ulLength);
    extern void USBBufferDataRemoved(const tUSBBuffer *psBuffer,
                                     uint32_t ulLength);
    extern void USBBufferFlush(const tUSBBuffer *psBuffer);
    extern uint32_t USBBufferRead(const tUSBBuffer *psBuffer,
                                  uint8_t *pucData,
                                  uint32_t ulLength);
    extern uint32_t USBBufferDataAvailable(const tUSBBuffer *psBuffer);
    extern uint32_t USBBufferSpaceAvailable(const tUSBBuffer *psBuffer);
    extern uint32_t USBBufferEventCallback(struct usbGadgetObj *pGadgetObject,
                                           void *pvCBData,
                                           uint32_t ulEvent,
                                           uint32_t ulMsgValue,
                                           void *pvMsgData);
    extern uint32_t USBRingBufFull(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufEmpty(tUSBRingBufObject *ptUSBRingBuf);
    extern void USBRingBufFlush(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufUsed(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufFree(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufContigUsed(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufContigFree(tUSBRingBufObject *ptUSBRingBuf);
    extern uint32_t USBRingBufSize(tUSBRingBufObject *ptUSBRingBuf);
    extern uint8_t USBRingBufReadOne(tUSBRingBufObject *ptUSBRingBuf);
    extern void USBRingBufRead(tUSBRingBufObject *ptUSBRingBuf,
                               uint8_t *pucData, uint32_t ulLength);
    extern void USBRingBufWriteOne(tUSBRingBufObject *ptUSBRingBuf,
                                   uint8_t ucData);
    extern void USBRingBufWrite(tUSBRingBufObject *ptUSBRingBuf,
                                const uint8_t *pucData,
                                uint32_t ulLength);
    extern void USBRingBufAdvanceWrite(tUSBRingBufObject *ptUSBRingBuf,
                                       uint32_t ulNumBytes);
    extern void USBRingBufAdvanceRead(tUSBRingBufObject *ptUSBRingBuf,
                                      uint32_t ulNumBytes);
    extern void USBRingBufInit(tUSBRingBufObject *ptUSBRingBuf,
                               uint8_t *pucBuf, uint32_t ulSize);

/******************************************************************************
 *
 * Close the Doxygen group.
 *! @}
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Mark the end of the C bindings section for C++ compilers.
 *
 ******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* PDK__USBLIB_H */
