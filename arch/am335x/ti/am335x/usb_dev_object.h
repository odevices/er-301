/**
 * \file    usb_dev_object.h
 *
 * \brief   This file contains device object data structure - a structure
 *          which is a wrapper that encapsulates the driver components used to
 *          manage a single device controller.
 */

/**
 *  Copyright (c) Texas Instruments Incorporated 2015-2019
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

#ifndef USB_DEV_OBJECT_H_
#define USB_DEV_OBJECT_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/am335x/usb_spec_defines.h>

#ifdef __cplusplus
extern "C"
{
#endif

     /* ========================================================================== */
     /*                                 Macros                                     */
     /* ========================================================================== */
     /** \brief Maximum number of gadget events that can be stored in the event
  *  Array. TBD - make this a list.
  */
#define MAX_GADGET_EVENTS (32U)
     /** \brief Max number of system DMA buffer Events.*/
#define MAX_DMA_EVENTS (1U)
     /** \brief Maximum number of DMA events.*/
#define MAX_NUM_REQUESTS (32U)
     /** \brief Maximum number of Requests we can process.*/

#define MAX_GADGET_NAME_LEN (30U)
     /** \brief length of string name in gadget obj */

/* length of buffer that is used by Gadget Obj */
#define MAX_GADGET_BUFF_LEN (512U)

     struct usbGadgetObj;
     /** \brief Forward declaration of usb gadget object .*/
     struct usbDevRequest;
     /** \brief Forward declaration of usb request object .*/
     struct usbDcd;
     /** \brief Forward declaration of dcd object .*/

     typedef struct usbDevRequest usbEndpt0Request_t;
     /** \brief  declaration of EP0 request.*/
     typedef struct usbDevRequest usbEndptRequest_t;
     /** \brief  declaration of Generic usb  request.*/

     /* ========================================================================== */
     /*                         Structures and Enums                               */
     /* ========================================================================== */

     /** \brief This structure defines the contiguous block of data which contains
 *         a group of descriptors that form a part of a contiguous descriptor
 *         for a device. It is assumed that a config section contains only
 *         whole descriptors. It is not valid to split a single descriptor
 *         across multiple sections.
 */
     typedef struct usbConfigSection
     {
          uint8_t size;
          /**< The number of bytes of descriptor data pointed by pointer pData */
          const uint8_t *pData;
          /**< A pointer to a block of data containing an integral number of
     * USB descriptors which form part of a larger configuration descriptor.
     */
     } usbConfigSection_t;

     /** \brief  This is the top level structure defining a USB device configuration
 *          descriptor.  A configuration descriptor contains a collection of device-
 *          specific descriptors in addition to the basic config, interface and
 *          endpoint descriptors.  To allow flexibility in constructing the
 *          configuration, the descriptor is described in terms of a list of data
 *          blocks.  The first block must contain the configuration descriptor itself
 *          and the following blocks are appended to this in order to produce the
 *          full descriptor sent to the host in response to a GetDescriptor request
 *          for the configuration descriptor.
 */

     typedef struct usbConfigHeader
     {
          uint8_t numSections;
          /**< The number of sections comprising the full descriptor for this
     * configuration.
     */

          usbConfigSection_t **pConfigSections;
          /**< A pointer to an array of ucNumSections section pointers which must
     *   be concatenated to form the configuration descriptor.
     */
     } usbConfigHeader_t;

     /**
 * \brief USB descriptor structure which presents a combined view of all
 *        descriptors used by a gadget/host. This data structure is common to
 *        both host and device stacks.In the host case , the protocol core would
 *        copy the descriptors received by the host into this structure. In the
 *        gadget stack , the function/gadget driver populates this structure.
 */
     typedef struct usbDescriptor
     {
          usbDeviceDesc_t *pDeviceDesc;
          /**< pointer to device descriptor of the Gadget.*/
          usbConfigHeader_t **ppConfigDesc;
          /**< A pointer to an array of configuration descriptor pointers.  Each entry
     * in the array corresponds to one configuration that the device may be set
     * to use by the USB host.  The number of entries in the array must
     * match the bNumConfigurations value in the device descriptor
     * array, pDeviceDesc.
     */
          usbInterfaceDesc_t *pInterfaceDesc;
          /**< pointer to Gadget's Interface descriptor.*/
          usbEndpointDesc_t *EndpointDescList[MAX_NUM_ENDPTS_PER_DEVICE];
          /**< array of pointers to endpoint descriptors. Mark with NULL for unused.*/
          uint8_t **ppStringDesc;
          /**< pointer to Gadget's String descriptor.*/
          uint32_t numStringDesc;
          /**< Number of string descriptors */
     } usbDescriptor_t;

     /**
 * \brief structure defining possible events on Endpoint zero .
 *        DCD would uses these events to communicate to the protocol Core.
 *
 *
 */
     typedef enum usbEndpt0event
     {
          USB_ENDPT0_EVENT_SETUP_PACKET_RECEIVED = 0U,
          /**< A setup packet was received.*/
          USB_ENDPT0_SOF_RECEIVED = 1U,
          /**< Incase we need the SOF interrupt for any contingency.*/
          //---------  control IN events -----------//

          USB_ENDPT0_EVENT_DATA_IN_NAK = 2U,
          /**< IN endpoint buffer currently has no data to transfer and hence
         NAKing.*/
          USB_ENDPT0_EVENT_DATA_IN_PARTIAL = 3U,
          /**< one transaction (any one)  transaction of a multi stage transaction is
         complete.*/
          USB_ENDPT0_EVENT_DATA_IN_COMPLETE = 4U,
          /**< Control data transaction is complete.*/

          //---------  control OUT events -----------//
          USB_ENDPT0_EVENT_DATA_OUT_NAK = 5U,
          /**< Out endpoint zero has no data and is being currently polled by the
         host.*/
          USB_ENDPT0_EVENT_DATA_OUT_PARTIAL = 6U,
          /**< one transaction (any one)  transaction of a multi stage transaction is
         complete.*/
          USB_ENDPT0_EVENT_DATA_OUT_COMPLETE = 7U,
          /**< Last requested OUT data transfer is complete.*/

          //--------- Errors -----------//
          USB_ENDPT0_EVENT_DATA_TOGGLE_ERROR = 8U,
          /**< Data toggle error has occured - retry the transfer.*/
          USB_ENDPT0_EVENT_TIMEOUT_ERROR = 9U,
          /**< Host has not responded to a transaction (a packet level ACK is missing)
         with in the USB mandated time.*/
          USB_ENDPT0_EVENT_BUS_ERROR = 10U
          /**< All other bus errors - eg transaction error, frame number mismatch .*/

     } usbEndpt0event_t;

     /**
 * \brief structure defining possible events on all other endpoints.
 *        DCD would uses these events to communicate to the protocol Core.
 *
 *
 */
     typedef enum usbGenericEvent
     {
          USB_GENERIC_EVENT_SOF_INTERRUPT = 0U,
          /**< Event representing the reception of an SOF frame.*/
          USB_GENERIC_EVENT_NAK_TIMEOUT = 1U,
          /**< host has stopped polling because of a timeout.*/

          //--------- IN events -----------//
          USB_GENERIC_EVENT_DATA_IN_NAK = 2U,
          /**< IN endpoint buffer currently has no data to transfer and hence
     *  NAKing.*/
          USB_GENERIC_EVENT_DATA_IN_PARTIAL = 3U,
          /**< one transaction (any one) of a multi stage transaction is done.*/
          USB_GENERIC_EVENT_DATA_IN_COMPLETE = 4U,
          /**< A data IN request made to dcd is complete.*/

          //------------ OUT events -----------//
          USB_GENERIC_EVENT_DATA_OUT_NAK = 5U,
          /**< Out endpoint zero has no data and is being currently polled by the
         host.*/
          USB_GENERIC_EVENT_DATA_OUT_PARTIAL = 6U,
          /**< one transaction (any one) transaction of a multi stage transaction is
         complete.*/
          USB_GENERIC_EVENT_DATA_OUT_COMPLETE = 7U,
          /**< Last requested OUT data transfer is complete.*/
          //--------------- Errors ---------------//
          USB_GENERIC_EVENT_DATA_TOGGLE_ERROR = 8U,
          /**< Data toggle error has occured - retry the transfer.*/
          USB_GENERIC_EVENT_TIMEOUT_ERROR = 9U,
          /**< Host has not responded to a transaction (a packet level ACK is missing)
         with in the USB mandated time.*/
          USB_GENERIC_EVENT_BUS_ERROR = 10U
          /**< All other bus errors - eg transaction error, frame number mismatch .*/

     } usbGenericEvent_t;

     /**
 * \brief Enum containing current state of the Gadget
 *
 *
 */
     typedef enum usbDeviceState
     {
          USB_DEVICE_STATE_UNKNOWN = 0U,
          /**< Unknown state - State of data structures unknown.*/
          USB_DEVICE_STATE_INITIALIZED = 1U,
          /**< Device data structures initialized, controller inite'd.*/
          USB_DEVICE_STATE_CONNECTED = 2U,
          /**< Connected to host.*/
          USB_DEVICE_STATE_RESET = 3U,
          /**< Host has driven reset.*/
          USB_DEVICE_STATE_ADDRESSED = 4U,
          /**< address set.*/
          USB_DEVICE_STATE_CONFIGURED = 5U,
          /**<  configuration value sent by host accepted - internal setConfig ()
          has returned successfully.*/
          USB_DEVICE_STATE_ENUM_COMPLETE = 6U,
          /**< enumeration is complete. If some custom enumeration commands are sent
         by host the this state is different from configured state.*/
          USB_DEVICE_STATE_STALLED_ENDPOINT = 7U,
          /**< One of the endpoints in the device is stalled.*/
          USB_DEVICE_STATE_INITIATE_SUSPENDED = 8U,
          /**< Device is starting the suspend procedure.*/
          USB_DEVICE_STATE_SUSPENDED = 9U,
          /**< Device is in suspend state.*/
          USB_DEVICE_STATE_LPM_ACTIVE = 10U,
          /**< Low power mode active.*/
          USB_DEVICE_STATE_ERROR = 11U
          /**< Unknown error in device.*/

     } usbDeviceState_t;

     /**<Status of the request  (0)- Complete, (1)- not processed yet,
  (2) being processed,(-1) Error */
     typedef enum usbDevReqStatus
     {
          DEV_REQ_STATUS_ERR = -1,
          DEV_REQ_STATUS_COMPLETE = 0,
          DEV_REQ_STATUS_SUBMIT = 1,
          DEV_REQ_STATUS_BEING_PROCESSED = 2,
          DEV_REQ_STATUS_MAX
     } usbDevReqStatus_t;

     /**
 * \brief Gadget action object structure which compiles a list of actions
 *        that can be performed on any USB function or pGadget. These functions
 *        would be called by the gadget driver.
 */
     typedef struct usbClassAction
     {
          uint32_t (*pFnInit)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which initialses the Gadget .*/
          uint32_t (*pFnStart)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which starts the gadget and initiates
         a connect on to the bus.*/
          uint32_t (*pFnReset)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which clears the gadget specific
         data structures and issues a device controller reset.*/
          uint32_t (*pFnSuspend)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which suspends the device - part of
         the power manger feature.*/
          uint32_t (*pFnResume)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which initiates gadget resume.*/
          uint32_t (*pFnStop)(struct usbGadgetObj *pGadget);
          /**< Function Pointer to function which stops the device and flushes
         the required data stuructures.*/
          void (*ep0Handler)(struct usbGadgetObj *pGadgetObject,
                             usbEndpt0event_t endptEvent, usbSetupPkt_t *setup);
          /**< Call back function which handles EP0 events.*/
          void (*epHandler)(struct usbGadgetObj *pGadgetObject,
                            usbGenericEvent_t endptEvent,
                            uint32_t epNo,
                            uint32_t length);
          /**< Call back function which handles generic endpoint events */
          void (*enumComplete)(struct usbGadgetObj *pGadgetObject);
          /**< Function pointer to inform the class driver that the enumeration
     * has completed
     */
     } usbClassAction_t;

     /**
 * \brief This structure defines the generic USB class object which
 *        is used by the protocol layer ot interact with the required class
 *        driver. The class needs to register with the protocol layer
 *        before this object can be used.
 */
     typedef struct usbClassObj
     {
          void *privateData;
          /**< Private data to hold the class object */
          usbClassAction_t classAction;
          /**< The class actions that can be called by the protocol layer to
         interact with the usb class */
     } usbClassObj_t;
     /**
 * \brief This structure defines the configurable paramters of an endpoint.
 *        The protocol core is expected to call the config routine with this
 *        structure populated as many times as there are endpoints.
 *
 */
     typedef struct usbDevEndptInfo
     {
          const void *pDesc;
          /**< pointer to descriptor Structure Incase descriptors are needed
         at some point of time.*/
          usbTransferType_t endpointType;
          /**< type of usb transfer this endpoint supports.*/
          usbTokenType_t endpointDirection;
          /**< IN , OUT or Setup .*/
          uint32_t MaxEndpointSize;
          /**< max ep size - 8, 64, 512, 1024 etc.*/
          uint32_t endpointNumber;
          /**< USB2.0 spec specifies 4 bits this needs to be extended for USB 3.0. */
          uint32_t endpointInterval;
          /**< dcd uses this field to configure the polling rate of the periodic
         endpoint. */
     } usbDevEndptInfo_t;

     /**
 * \brief Structure containing information regarding system DMA events
 *
 *
 */
     typedef struct usbDmaEvt
     {
          struct usbGadgetObj *pGadgetObject;
          /**< pointer to GadgetObject data structure.*/

          /* TODO: DMA events need to be added here */

          struct usbDevRequest *pReq;
          /**< Pointer to the request which caused this event. */
     } usbDmaEvt_t;

     /**
 * \brief Structure containing information regarding events that can be
 *        raised by the device control driver.
 *
 */
     typedef struct usbDevEvt
     {
          struct usbGadgetObj *pGadgetObject;
          /**< pointer to GadgetObject data structure.*/
          usbGenericEvent_t endptEvent;
          /**< Enum describing the event. */
          struct usbDevRequest *pReq;
          /**< Pointer to the request which caused this event. */
     } usbDevEvt_t;

     /*List of Call backs  */

     /* 1. handle Endpoint zero Request - pFnDevEndpt0Handler ( )
   2. Reset handler - pFnResetHandler ()
   3. Suspend Handler - pFnDevSuspendHandler ()
   4. Disconnect Handler - pFnDevDisconnectHandler ()
   5. Generic Interrupt handler - pFnDevGenericIntrHandler
   6. PM handler - pFnDevPMHandler()
*/

     /**
 * \brief This enum selcts the param the core wants the dcd to set. Initially
 *        the core sets the device speed and dsiables remote wake up.
 *
 *
 */
     typedef enum usbDeviceDcdChara
     {
          USB_DEVICE_DCD_CHARA_MIN = 1U,
          /**< min value of this enum .*/
          USB_DEVICE_DCD_CHARA_SPEED = USB_DEVICE_DCD_CHARA_MIN,
          /**< this param when selected sets dcd speed.*/
          USB_DEVICE_DCD_CHARA_ADDRESS = 2U,
          /**< this param when selected sets dcd address.*/
          USB_DEVICE_DCD_EP_CONFIG = 3U,
          /**< this param when selected EP config.*/
          USB_DEVICE_DCD_EP_INTERFACE = 4U,
          /**< this param when selected EP config.*/
          USB_DEVICE_DCD_CHARA_MAX = 5U,
          /**< max value of this enum .*/
          USB_DEVICE_DCD_CHARA_REMOTE_WAKE = USB_DEVICE_DCD_CHARA_MAX
          /**< this param when selected enables or disables remote wakeup.*/

     } usbDeviceDcdChara_t;
     /**
 * \brief  Function pointer to function which is used by the core to set device
 *     charectristics such as device speed, device address,Number of endpoints
 *    required and their endpoint numbers. Fifo config must be done by dcd.
 *
 */
     typedef struct usbDevChara
     {
          uint32_t configparam;
          /**< this field is used by the dcd to know which field it needs to use in
         the current run.*/

          usbSpeed_t devSpeed;
          /**< 1st Field - device speed .*/

          uint32_t deviceAddress;
          /**< Address assigned by the host- this needs to be configured in the dc. */

          uint32_t enableRemoteWakeup;
          /**< tell dcd to enable/disable remote wakeup signalling .*/

          usbDevEndptInfo_t *pDeviceEndptConfig;
          /**< Pointer to device endpoint configuration structure . */

          uint32_t epConfigNo; /* which configuration value used in 
                                        Set Configuration transfer */

          usbDevEndptInfo_t *pDeviceEndptInterface;
          /**< Pointer to device endpoint interface structure . */

          uint32_t epInterfaceNo; /* which interface value used in 
                                        Set Interface transfer */
     } usbDevChara_t;

     /**
 * \brief USB request structure which encapsulates information required by the
 *        dcd to genererate USB traffic.
 *
 *
 */
     typedef struct usbDevRequest
     {
          uint32_t deviceAddress;
          /**< Address assigned by the host. Note that this field would not be used.*/
          uint8_t *pbuf;
          /**<Pointer to data buffer - for incoming or out going traffic.*/
          uint32_t length;
          /**<How much of data needs to be sent or received.*/
          uint32_t dmaMode;
          /**<PIO mode or DMA mode of copy. If PIO mode(0)is set then , the CPU will
        copy data to controller FIFOs. If DMA mode (value 1)is set, then the
        CPU would not take any action to copy data to or from Controller
        FIFOs.*/
          usbDevEndptInfo_t deviceEndptInfo;
          /**< Pointer to device endpoint info structure . */
          uint32_t streamId;
          /**<USB 3.0 streamID  .*/
          uint32_t zeroLengthPacket;
          /**<whether this request is for a zero length packet or not.*/

#if 0
    void       (*reqComplete)(struct usbGadgetObj * pGadgetObject,
                              struct usbDevRequest *pReq);
    /**<Completion handler for this request.*/
#endif

          usbDevReqStatus_t status; /**<Status of the request */

          uint32_t actualLength;
          /**<number of bytes actually received or transmitted.*/

     } usbDevRequest_t;

     //------------------- Move to dcd interface ---------------
     /**
 * \brief Device object core call back functions which would be used by dcds to
 *        to communicate with the device protocol core - (device manager block
 *        being a part of it).
 *
 */
     typedef struct usbDevCoreCallback
     {
          uint32_t (*pFnResetHandler)(struct usbGadgetObj *pGadgetObject);
          /**< Call back function which handles device core reset on receiving a bus
         reset.*/
          uint32_t (*pFnDevSuspendHandler)(struct usbGadgetObj *pGadgetObject);
          /**< Call back function which handles device suspend logic on receiving bus
        suspend .*/
          uint32_t (*pFnDevDisconnectHandler)(struct usbGadgetObj *pGadgetObject);
          /**< Call back function which handles device disconnect.*/
          uint32_t (*pFnDevPMHandler)(struct usbGadgetObj *pGadgetObject);
          /**< Call back function which handles PM events.*/
          uint32_t (*pFnDevEndpt0Handler)(struct usbGadgetObj *pGadgetObject,
                                          usbEndpt0event_t endptEvent, usbSetupPkt_t *setup);
          /**< Call back function which handles EP0 events.*/
          uint32_t (*pFnDevGenericIntrHandler)(struct usbGadgetObj *pGadgetObject,
                                               usbGenericEvent_t endptEvent);
          /**< Call back function which handles Events on all endpoints except EP0.*/
     } usbDevCoreCallback_t;
     /**
 * \brief Dcd driver actions
 *        Structure which collates all actions required by the dcds to implement
 *        The protocol core call these actions to generate USB traffic.
 *
 */
     typedef struct usbDevDcdAction
     {
          void *pDcdObject;
          /**< pointer to device control driver object.*/
          uint32_t (*pFnDcInit)(struct usbDcd *pDcdObject);
          /**<Function pointer to init routine for the specific device
     controller instance.*/

          uint32_t (*pFnDcKill)(struct usbDcd *pDcdObject);
          /**< Function pointer to function which deallocates all mem and stops the
         controller.*/

          uint32_t (*pFnConfigDevChara)(struct usbDcd *pDcdObject,
                                        usbDevChara_t *pDevCharecteristic);
          /**<Function pointer to function which is used by the core to set device
     charectristics such as device speed, device address,Number of endpoints
     required and their endpoint numbers. Fifo config must be done by dcd.*/

          /* Function pointer for stalling EP0 */
          uint32_t (*pFnEndpt0Stall)(struct usbDcd *pDcdObject);

          /*================== Transfer Requests ====================*/
          /**< Function pointer to start an EP0 IN/OUT request.*/
          uint32_t (*pFnEndpt0Req)(struct usbDcd *pDcdObject,
                                   usbEndpt0Request_t *req);

          /**< Function pointer to start a non control request . The usbEndptRequest_t
     would have necessary details like add, endpoint number, type of trans,
     interval etc .*/
          uint32_t (*pFnEndptReq)(struct usbDcd *pDcdObject,
                                  usbEndptRequest_t *req);

     } usbDevDcdAction_t;
     /** Sample Device controller object - each Device controller instance would
           create an instance and register with the core  */

     typedef struct usbDcd
     {
          uint8_t name[MAX_GADGET_NAME_LEN + 1];
          /**< name of the dcd object - example "dwc3_instance0". */

          struct usbGadgetObj *pGadgetObject;
          /**< pointer to parent device controller data structure - used for
    callbacks. */

          void *privateData;
          /**< pointer to the hw specific dcd object.*/

          uint32_t registered;
          /**< Member which shows whether this dcd is registered with the core.*/

          usbDevDcdAction_t dcdActions;
          /**< structure containing dcd Actions.*/

          usbDevCoreCallback_t gadgetObjCallBack;
          /**< structure holding callbacks for dcd to communicate with the
         device/gadget protocol core.*/

          void *ptDeviceInfo;
          /**< pointer to the tDeviceInfo structure. */
     } usbDcd_t;
     //---------------------------------------------------------
     /**
 * \brief Device object structure which holds information related to
 *        a particular device controller - which assumes the role of one device
 *
 */
     typedef struct usbGadgetObj
     {
          char name[MAX_GADGET_NAME_LEN + 1];
          /**< Name of the Controller instance - copied from dcd object*/

          usbDcd_t dcd;
          /**< dcd core object. */

          usbClassObj_t usbClass;
          /**< usb class object */

          usbDevEvt_t eventBuf[MAX_GADGET_EVENTS];
          /**< array holding device events which are not time critical in nature -
         will be populated by DCD.*/

          usbDmaEvt_t eventBufDma[MAX_DMA_EVENTS];
          /**< This structure keeps a list of pending system DMA Events
         which need to be used to schedule system DMA transfers.*/

          usbDevRequest_t requestbuf[MAX_NUM_REQUESTS];
          /**< array holding requests to this device. This will be be populated
         by the function drivers with the help of Transaction manager.
         TODO: make this a list. A pointer to this array is used by the dcd to
         read the pending requests for this device.*/

          usbDescriptor_t pDesc;
          /**< Structure containing pointers to device desciptors -
      *  will be populated by the function driver.
      */

          void *pEpdata;
          /**< Pointer to endpoint data - Each function/gadget would have its
         own set.*/

          usbSpeed_t devSpeed;
          /**< Speed of the function/ gadget - will be populated by the function
         driver */

          usbDeviceState_t devState;
          /**< Current state of the function/ gadget */

          usbDevChara_t dcdCharecteristics;
          /**< dcd charecteristics object which is used to configure dcd */
          uint32_t objectUsed;
          /**< whether this object is in use or not 0 - not used , 1 -in use. */

          uint32_t deviceAddress;
          /**< Address assigned by the host. */
          uint32_t remoteWakeEnabled;
          /**< 1 - means remote wakeup is enabled , 0 - disabled. */

          uint32_t cmnInitialized;
          /**< True if common functionality has been initialized. */

          uint32_t gadgetInitialized;
          /**< True if Gadget has been initialized. */

          uint32_t dcdInitialized;
          /**< True if DCD has been initialized. */

          uint32_t irqInstalled;
          /**< True if common IRQ handler has been installed. */

          uint32_t hiberWaitConnect;
          /**< True if waiting for connect before resuming from hibernation. */

          uint32_t hiberWaitU0;
          /**< True if waiting for U0 state before sending remote wake. */

          /* temporary buffers for incoming or outgoing req 
     * Depending on SOC, this buffer might need to be in 
     * a non-cache area */
          uint8_t *gadgetBufferPtr;

     } usbGadgetObj_t;

     /* ========================================================================== */
     /*                         Global Variables Declarations                      */
     /* ========================================================================== */

     /* None */

     /* ========================================================================== */
     /*                          Function Declarations                             */
     /* ========================================================================== */

     /* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef USB_DEV_OBJECT_H_ */
