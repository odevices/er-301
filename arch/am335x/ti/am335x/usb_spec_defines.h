/**
 *  file   usb_spec_defines.h
 *
 *  This file contains USB spec related definitions. This is a common file
 *  for both the device and the host stacks.This header file does not
 *  contain any function prototypes and merely acts a collection of USB2.0
 *  spec related definitions.
 *
 */
/**
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

#ifndef USB_SPEC_DEFINES_H_
#define USB_SPEC_DEFINES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
#define USB_STACK_PACKED __attribute__((packed))
    /** \brief Packing attribute. TODO define for other compilers. */

#define STRING_DESC_MAX_LENGTH (48U)
    /** \brief Maximum size of a unicode string in the string descriptor
 * might need to be adapted */

#define MAX_NUM_ENDPTS_PER_DEVICE (32U)
    /** \brief Maximum number of Endpoints as per USB spec.*/

#define SPEC_VERSION_USB_2_0 (0x0200)
    /** \brief BCD USB field = USB 2.0. */

#define USB_DEVICE_DESCRIPTOR_SIZE (18U)
/** \brief size of device descriptor. */

/* Configuration descriptor field macros  */
#define CONFIG_DESC_BM_ATT_ONE (1 << 7)
/** \brief Reserved bit must be set. */
#define CONFIG_DESC_BM_ATT_SELFPOWER (1 << 6)
/** \brief self powered. */
#define CONFIG_DESC_BM_ATT_WAKEUP (1 << 5)
/** \brief can wakeup by remote signalling. */
#define CONFIG_DESC_BM_ATT_BATTERY (1 << 4)
/** \brief battery powered. */
#define CONFIG_DESC_POWER_FACTOR (2U)
    /** \brief Max power consumed in this config in multiples of 2 . */

#define USB_CONFIG_DESCRIPTOR_SIZE (9U)
    /** \brief size of Configuration descriptor. */

#define USB_INTERFACE_DESCRIPTOR_SIZE (9U)
/** \brief size of Interface descriptor. */

/* Endpoint descriptor field macros  */
#define EP_DESC_GET_DIR(a) ((a)&0x80)
/** \brief Get direction from Endpoint Address. */
#define EP_DESC_SET_DIR(a, d) ((a) | (((d)&1) << 7))
/** \brief Set direction in Ep address field. */
#define EP_DESC_DIR_IN (0x80)
/** \brief macro representing IN Direction. */
#define EP_DESC_DIR_OUT (0x00)
/** \brief macro representing OUT Direction. */
#define EP_DESC_ENDPOINT_ADDR (0x0F)
/** \brief macro representing endpoint address field. */
#define EP_DESC_GET_NUMBER(a) ((a)&EP_DESC_ENDPOINT_ADDR)
/** \brief Get endpoint Number from Endpoint address. */
#define EP_DESC_XFERTYPE (0x03)
/** \brief macro representing transfer type. */
#define EP_DESC_GET_XFERTYPE(a) ((a)&EP_DESC_XFERTYPE)
/** \brief Get the transfer type from bmAttrributes. */
#define USB_ENDPOINT_DESCRIPTOR_SIZE (7U)
    /** \brief size of Endpoint descriptor. */

#define USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE (6U)
    /** \brief size of SS Endpoint Companion descriptor. */

    /*
The following defines are used with the bmRequestType

Request types have 3 bit fields:
4:0 - Is the recipient type.
6:5 - Is the request type.
7 - Is the direction of the request.
*/

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

// EP0 data size
#define MAX_PACKET_SIZE_EP0 64

    /* ========================================================================== */
    /*                         Structures and Enums                               */
    /* ========================================================================== */
    /**
* \brief Enum encapulating all USB descriptor types
*
*/
    typedef enum usbDescType
    {
        USB_DESC_TYPE_NOT_DESC = 0U,
        /**< not a descriptor.*/
        USB_DESC_TYPE_DEVICE = 1U,
        /**< device descriptor.*/
        USB_DESC_TYPE_CONFIGURATION = 2U,
        /**< Configuration descriptor.*/
        USB_DESC_TYPE_STRING = 3U,
        /**< String descriptor.*/
        USB_DESC_TYPE_INTERFACE = 4U,
        /**< Interface descriptor.*/
        USB_DESC_TYPE_ENDPOINT = 5U,
        /**< Endpoint descriptor.*/
        USB_DESC_TYPE_DEVICE_QUALIFIER = 6U,
        /**< device qualifier descriptor.*/
        USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION = 7U,
        /**< Other speed config descriptor.*/
        USB_DESC_TYPE_INTERFACE_POWER = 8U,
        /**< Interface power descriptor.*/
        USB_DESC_TYPE_OTHER = 9U
        /**< any other descriptor.*/
    } usbDescType_t;

    /**
* \brief Enumeration for different usb speeds. Support depends on hardware and
*         phy also.
*/
    typedef enum usbSpeed
    {
        USB_SPEED_LOW_SPEED = 0U,
        /**< Low speed - 1.5Mbps.*/
        USB_SPEED_FULL_SPEED = 1U,
        /**< full speed - 12Mbps.*/
        USB_SPEED_HIGH_SPEED = 2U,
        /**< high speed - 480Mbps.*/
        USB_SPEED_SUPER_SPEED_30 = 3U,
        /**< Super speed 3.0 spec - 5Gbps.*/
        USB_SPEED_SUPER_SPEED_31 = 4U
        /**< Super speed 3.1 spec - 10Gbps.*/
    } usbSpeed_t;

    /**
 * \brief Enumeration for different types of USB transfers.
 */
    typedef enum usbTransferType
    {
        USB_TRANSFER_TYPE_CONTROL = 0U,
        /**< Control transfer.*/
        USB_TRANSFER_TYPE_ISOCH = 1U,
        /**< Isochronous transfer.*/
        USB_TRANSFER_TYPE_BULK = 2U,
        /**< bulk transfer.*/
        USB_TRANSFER_TYPE_INTERRUPT = 3U
        /**< interrupt transfer.*/

    } usbTransferType_t;

    /**
 * \brief token type or direction field.
 */
    typedef enum usbTokenType
    {
        USB_TOKEN_TYPE_OUT = 0U,
        /**< out token.*/
        USB_TOKEN_TYPE_IN = 1U,
        /**< in token.*/
        USB_TOKEN_TYPE_SOF = 2U,
        /**< sof token - for future use.*/
        USB_TOKEN_TYPE_SETUP = 3U
        /**< setup token - in case a setup transfer needs to be configured by the
         protocol Core.*/
    } usbTokenType_t;

    /* chapter 9.3 usb spec */
    /**
* \brief Enum encapulating all Standard Request Codes
*
*/
    typedef enum usbBrequest
    {
        USB_BREQUEST_GET_STATUS = 0U,
        /**< Get status brequest Code.*/
        USB_BREQUEST_CLEAR_FEATURE = 1U,
        /**< clear feature brequest Code.*/
        USB_BREQUEST_RESERVED_1 = 2U,
        /**< reserved .*/
        USB_BREQUEST_SET_FEATURE = 3U,
        /**< set feature brequest Code.*/
        USB_BREQUEST_RESERVED_2 = 4U,
        /**<  reserved brequest Code.*/
        USB_BREQUEST_SET_ADDRESS = 5U,
        /**< set address brequest Code.*/
        USB_BREQUEST_GET_DESCRIPTOR = 6U,
        /**< get descriptor brequest Code.*/
        USB_BREQUEST_SET_DESCRIPTOR = 7U,
        /**< set descriptor brequest Code.*/
        USB_BREQUEST_GET_CONFIGURATION = 8U,
        /**< get configureation brequest Code.*/
        USB_BREQUEST_SET_CONFIGURATION = 9U,
        /**< set configuration brequest Code.*/
        USB_BREQUEST_GET_INTERFACE = 10U,
        /**< get interface brequest Code.*/
        USB_BREQUEST_SET_INTERFACE = 11U,
        /**< set interface brequest Code.*/
        USB_BREQUEST_SYNCH_FRAME = 12U
        /**< synch frame brequest Code.*/
    } usbBrequest_t;

    /* Descriptor Types */

    /** \brief  This structure describes the USB string descriptor for index 0 as defined
 * in USB 2.0 specification section 9.6.7.  Note that the number of language
 * IDs is variable and can be determined by examining bLength.  The number of
 * language IDs present in the descriptor is given by ((bLength - 2) / 2).
 */
    typedef struct usbString0Desc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
        uint16_t wLangId[1];
        /**<  The language code (LANGID) for the first supported language.  Note that
     * this descriptor may support multiple languages, in which case, the
     * number of elements in the wLANGID array will increase and bLength will
     * be updated accordingly.
     */
    } USB_STACK_PACKED usbString0Desc_t;

    /** \brief Structure defining descriptor Header .*/
    typedef struct usbDescHeader
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
    } USB_STACK_PACKED usbDescHeader_t;

    /** \brief Structure defining USB Device Descriptor.*/
    typedef struct usbDeviceDesc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
        uint16_t bcdUSB;
        /**<   USB Specification version.*/
        uint8_t bDeviceClass;
        /**< Each interface specifies its own class information .*/
        uint8_t bDeviceSubClass;
        /**< Each interface specifies its own Subclass information .*/
        uint8_t bDeviceProtocol;
        /**< Device Protocol .*/
        uint8_t bMaxPacketSize;
        /**< Maximum packet size for endpoint zero.*/
        uint16_t idVendor;
        /**<   Vendor ID.*/
        uint16_t idProduct;
        /**<   Product ID.*/
        uint16_t bcdDevice;
        /**< device release number.*/
        uint8_t iManufacturer;
        /**< manufacturer string descriptor index.*/
        uint8_t iProduct;
        /**< product string descriptor index.*/
        uint8_t iSerialNumber;
        /**< serial number string descriptor index.*/
        uint8_t bNumConfigurations;
        /**< possible configurations .*/

    } USB_STACK_PACKED usbDeviceDesc_t;

    /** \brief Structure defining USB Configuration Descriptor.*/
    typedef struct usbConfigDesc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
        uint16_t wTotalLength;
        /**< total length of data for this configuration.*/
        uint8_t bNumInterface;
        /**< Number of interfaces supported by this configuration  .*/
        uint8_t bConfigurationValue;
        /**< Value to be used for selecting this configuration.*/
        uint8_t iConfiguration;
        /**< configuration string descriptor index .*/
        uint8_t bmAttributes;
        /**< Configuration characteristics.*/
        uint8_t bMaxPower;
        /**<max current in 2 mA units .*/
    } USB_STACK_PACKED usbConfigDesc_t;

    /** \brief Structure defining USB Interface Descriptor.*/
    typedef struct usbInterfaceDesc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
        uint8_t bInterfaceNumber;
        /**< number of this interface.*/
        uint8_t bAlternateSetting;
        /**< value used to select the alternate setting for this interface .*/
        uint8_t bNumEndpoints;
        /**< number of endpoints used by this interface .*/
        uint8_t bInterfaceClass;
        /**< Class that this interface implements.*/
        uint8_t bInterfaceSubClass;
        /**< Sub-Class that this interface implements.*/
        uint8_t bInterfaceProtocol;
        /**< Protocol Used .*/
        uint8_t iInterface;
        /**<  interface string descriptor index .*/
    } USB_STACK_PACKED usbInterfaceDesc_t;

    /** \brief Structure defining USB Endpoint Descriptor.*/
    typedef struct usbEndpointDesc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< ENDPOINT Descriptor Type .*/
        uint8_t bEndpointAddress;
        /**< IN or OUT endpoint address.*/
        uint8_t bmAttributes;
        /**< Endpoint type.*/
        uint16_t wMaxPacketSize;
        /**< Maximum packet size for this endpoint.*/
        uint8_t bInterval;
        /**< polling interval value in farmes.*/
    } USB_STACK_PACKED usbEndpointDesc_t;

    /** \brief Structure defining USB SS Endpoint companion Descriptor.*/
    typedef struct usbssEndptCompanionDesc
    {
        uint8_t bLength;
        /**< Descriptor Length.*/
        uint8_t bDescriptorType;
        /**< Descriptor Type.*/
        uint8_t bMaxBurst;
        /**< Max num of bursts possible.*/
        uint8_t bmAttributes;
        /**< attributes of the stream endpoint .*/
        uint16_t wBytesPerInterval;
        /**< number of bytes per polling interval.*/
    } USB_STACK_PACKED usbssEndptCompanionDesc_t;

    /** \brief Structure defining USB Setup Packet.*/
    typedef struct usbSetupPkt
    {
        uint8_t bmRequestType;
        /**< USB Standard request type.*/
        uint8_t bRequest;
        /**< Request code.*/
        uint16_t wValue;
        /**< USB 2.0 WValue.*/
        uint16_t wIndex;
        /**< USB 2.0 WIndex.*/
        uint16_t wLength;
        /**< USB 2.0 WLength.*/
    } usbSetupPkt_t;

    /** \brief Standard USB Interface Association Descriptor (USB-IF naming conventions).
     *
     *  Type define for a standard Interface Association Descriptor. This structure uses the relevant standard's given
     *  element names to ensure compatibility with the standard.
     *
     *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
     *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
     *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
     *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
     *  function. Read the ECN for more information.
     *
     *  \see \ref USB_Descriptor_Interface_Association_t for the version of this type with non-standard LUFA specific
     *       element names.
     *
     *  \note Regardless of CPU architecture, these values should be stored as little endian.
     */
    typedef struct usbInterfaceAssociationDesc
    {
        uint8_t bLength;           /**< Size of the descriptor, in bytes. */
        uint8_t bDescriptorType;   /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
				                          *   given by the specific class.
				                          */
        uint8_t bFirstInterface;   /**< Index of the first associated interface. */
        uint8_t bInterfaceCount;   /**< Total number of associated interfaces. */
        uint8_t bFunctionClass;    /**< Interface class ID. */
        uint8_t bFunctionSubClass; /**< Interface subclass ID. */
        uint8_t bFunctionProtocol; /**< Interface protocol ID. */
        uint8_t iFunction;         /**< Index of the string descriptor describing the
				                    *   interface association.
				                    */
    } USB_STACK_PACKED usbInterfaceAssociationDesc_t;

    /** \brief CDC class-specific Functional Header Descriptor (USB-IF naming conventions).
     *
     *  Type define for a CDC class-specific functional header descriptor. This indicates to the host that the device
     *  contains one or more CDC functional data descriptors, which give the CDC interface's capabilities and configuration.
     *  See the CDC class specification for more details.
     *
     *  \see \ref USB_CDC_Descriptor_FunctionalHeader_t for the version of this type with non-standard LUFA specific
     *       element names.
     *
     *  \note Regardless of CPU architecture, these values should be stored as little endian.
     */
    typedef struct usbCDCFunctionalHeaderDesc
    {
        uint8_t bFunctionLength;    /**< Size of the descriptor, in bytes. */
        uint8_t bDescriptorType;    /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
			                           *   given by the specific class.
			                           */
        uint8_t bDescriptorSubType; /**< Sub type value used to distinguish between CDC class-specific descriptors,
			                              *   must be \ref CDC_DSUBTYPE_CSInterface_Header.
			                              */
        uint16_t bcdCDC;            /**< Version number of the CDC specification implemented by the device, encoded in BCD format.
			                  *
			                  *   \see \ref VERSION_BCD() utility macro.
			                  */
    } USB_STACK_PACKED usbCDCFunctionalHeaderDesc_t;

    /** \brief CDC class-specific Functional ACM Descriptor (USB-IF naming conventions).
     *
     *  Type define for a CDC class-specific functional ACM descriptor. This indicates to the host that the CDC interface
     *  supports the CDC ACM subclass of the CDC specification. See the CDC class specification for more details.
     *
     *  \see \ref USB_CDC_Descriptor_FunctionalACM_t for the version of this type with non-standard LUFA specific
     *       element names.
     *
     *  \note Regardless of CPU architecture, these values should be stored as little endian.
     */
    typedef struct usbCDCFunctionalACMDesc
    {
        uint8_t bFunctionLength;    /**< Size of the descriptor, in bytes. */
        uint8_t bDescriptorType;    /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
			                          *   given by the specific class.
			                          */
        uint8_t bDescriptorSubType; /**< Sub type value used to distinguish between CDC class-specific descriptors,
			                             *   must be \ref CDC_DSUBTYPE_CSInterface_ACM.
			                             */
        uint8_t bmCapabilities;     /**< Capabilities of the ACM interface, given as a bit mask. For most devices,
			                         *   this should be set to a fixed value of 0x06 - for other capabilities, refer
			                         *   to the CDC ACM specification.
			                         */
    } USB_STACK_PACKED usbCDCFunctionalACMDesc_t;

    /** \brief CDC class-specific Functional Union Descriptor (USB-IF naming conventions).
     *
     *  Type define for a CDC class-specific functional Union descriptor. This indicates to the host that specific
     *  CDC control and data interfaces are related. See the CDC class specification for more details.
     *
     *  \see \ref USB_CDC_Descriptor_FunctionalUnion_t for the version of this type with non-standard LUFA specific
     *       element names.
     *
     *  \note Regardless of CPU architecture, these values should be stored as little endian.
     */
    typedef struct usbCDCFunctionalUnionDesc
    {
        uint8_t bFunctionLength;    /**< Size of the descriptor, in bytes. */
        uint8_t bDescriptorType;    /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
			                          *   given by the specific class.
			                          */
        uint8_t bDescriptorSubType; /**< Sub type value used to distinguish between CDC class-specific descriptors,
			                             *   must be \ref CDC_DSUBTYPE_CSInterface_Union.
			                             */
        uint8_t bMasterInterface;   /**< Interface number of the CDC Control interface. */
        uint8_t bSlaveInterface0;   /**< Interface number of the CDC Data interface. */
    } USB_STACK_PACKED usbCDCFunctionalUnionDesc_t;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef USB_SPEC_DEFINES_H_ */
