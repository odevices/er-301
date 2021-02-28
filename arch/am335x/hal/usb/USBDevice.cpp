#include "USBDevice.h"
#include <ti/am335x/musb.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/hw_types.h>
#include <ti/am335x/hw_usb.h>
#include <ti/am335x/hw_usbOtg_AM335x.h>
#include <ti/am335x/hw_usbphyGS70.h>
#include <ti/am335x/cppi41dma.h>
#include <hal/constants.h>
#include <hal/events.h>
#include <cstring>
#include <string>
#include <cstdio>

//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

#define BASE_ADDRESS USB0_BASE
#define SUBBASE_ADDRESS USB_0_OTGBASE
#define PHYBASE_ADDRESS CFGCHIP2_USBPHYCTRL

/****************************************************************************** */
/* Mask used to preserve various endpoint configuration flags. */
/****************************************************************************** */
#define EP_FLAGS_MASK (USB_EP_MODE_MASK | USB_EP_DEV_IN | \
                       USB_EP_DEV_OUT)

/* The buffer for sending/receiving data coming on EP0 */
static uint8_t gEP0InputBuffer[CACHE_ALIGNED_SIZE(MAX_PACKET_SIZE_EP0)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
static uint8_t gEP0OutputBuffer[CACHE_ALIGNED_SIZE(MAX_PACKET_SIZE_EP0)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

#ifdef BUILDOPT_VERBOSE
static const char *gStateLabels[] = {
    "IDLE",
    "TX",
    "RX",
    "STATUS",
    "STALL"};
#define setEP0State(state)                                     \
  if (mEP0State != state)                                      \
  {                                                            \
    const char *prevStateText = gStateLabels[mEP0State];       \
    const char *curStateText = gStateLabels[state];            \
    logDebug(3, "ep0(%s to %s)", prevStateText, curStateText); \
    mEP0State = state;                                         \
  }
#else
#define setEP0State(state) \
  if (mEP0State != state)  \
  {                        \
    mEP0State = state;     \
  }
#endif

namespace od
{

  const usbString0Desc_t gLanguageDescriptor = {
      /* Descriptor Length.*/
      .bLength = 4,
      /* Descriptor Type.*/
      .bDescriptorType = USB_DTYPE_STRING,
      /* The language code (LANGID) for the first supported language.  Note that
             * this descriptor may support multiple languages, in which case, the
             * number of elements in the wLANGID array will increase and bLength will
             * be updated accordingly.
             */
      .wLangId = {USB_LANG_EN_US},
  };

  USBDevice::USBDevice()
  {
    mpEP0InputBuffer = gEP0InputBuffer;
    mpEP0OutputBuffer = gEP0OutputBuffer;
  }

  USBDevice::~USBDevice()
  {
  }

  static inline uint32_t safeCopy(uint8_t *dst, uint32_t dstN, uint8_t *src, uint32_t srcN)
  {
    if (dst)
    {
      if (srcN > dstN)
      {
        return 0;
      }
      else
      {
        memcpy(dst, src, srcN);
      }
    }

    return srcN;
  }

  uint32_t USBDevice::copyDeviceDescriptor(uint8_t *buffer, uint32_t bufferLength)
  {
    if (mpDeviceDescriptor == 0)
    {
      return 0;
    }
    return safeCopy(buffer, bufferLength, (uint8_t *)mpDeviceDescriptor, mpDeviceDescriptor->bLength);
  }

  uint32_t USBDevice::copyConfigurationDescriptor(uint32_t index, uint8_t *buffer, uint32_t bufferLength)
  {
    usbConfigDesc_t *pConfig = getConfigurationDescriptorByIndex(index);
    if (pConfig)
    {
      return safeCopy(buffer, bufferLength, (uint8_t *)pConfig, pConfig->wTotalLength);
    }
    else
    {
      return 0;
    }
  }

  uint32_t USBDevice::copyStringDescriptor(uint32_t langId, uint32_t index, uint8_t *buffer, uint32_t bufferLength)
  {
    // Ignoring langId and always returning English strings.

    if (index == 0)
    {
      return safeCopy(buffer, bufferLength, (uint8_t *)&gLanguageDescriptor, gLanguageDescriptor.bLength);
    }

    index--; // convert to zero-based index
    if (index < mStringCount)
    {
      const std::string &text = mStringTable[index];
      uint32_t i = 0, n = text.size() * 2 + 2;
      if (n > bufferLength)
      {
        return n;
      }

      buffer[i++] = n;
      buffer[i++] = USB_DTYPE_STRING;
      // Copy string while converting to UTF-16
      for (char c : text)
      {
        buffer[i++] = c;
        buffer[i++] = 0;
      }

      return n;
    }
    else
    {
      return 0;
    }
  }

  static inline usbDescHeader_t *nextDescriptor(usbDescHeader_t *ptr)
  {
    uint8_t *tmp = (uint8_t *)ptr;
    ptr = (usbDescHeader_t *)(tmp + ptr->bLength);
    if (ptr->bLength > 0 && ptr->bDescriptorType > 0)
    {
      return ptr;
    }
    return 0;
  }

  usbConfigDesc_t *USBDevice::getConfigurationDescriptorByIndex(uint32_t index)
  {
    if (mConfigurationTable && index < mConfigurationCount)
    {
      return mConfigurationTable[index];
    }

    return 0;
  }

  usbConfigDesc_t *USBDevice::getConfigurationDescriptorByValue(uint32_t value)
  {
    if (mConfigurationTable)
    {
      // Search the configuration table for a configuration with the desired value.
      for (uint32_t i = 0; i < mConfigurationCount; i++)
      {
        usbConfigDesc_t *pConfig = mConfigurationTable[i];
        if (pConfig->bConfigurationValue == value)
        {
          return pConfig;
        }
      }
    }

    return 0;
  }

  usbInterfaceDesc_t *USBDevice::getInterfaceDescriptor(usbConfigDesc_t *pConfiguration, uint32_t interfaceId, uint32_t alternateSetting)
  {
    if (pConfiguration)
    {
      for (usbDescHeader_t *ptr = nextDescriptor((usbDescHeader_t *)pConfiguration);
           ptr != 0; ptr = nextDescriptor(ptr))
      {
        if (ptr->bDescriptorType == USB_DTYPE_INTERFACE)
        {
          usbInterfaceDesc_t *pInterface = (usbInterfaceDesc_t *)ptr;
          if ((pInterface->bInterfaceNumber == interfaceId) &&
              (pInterface->bAlternateSetting == alternateSetting))
          {
            return pInterface;
          }
        }
      }
    }

    return 0;
  }

  usbEndpointDesc_t *USBDevice::getEndpointDescriptor(usbInterfaceDesc_t *pInterface, uint32_t endpointIndex)
  {
    if (pInterface && endpointIndex < pInterface->bNumEndpoints)
    {
      for (usbDescHeader_t *ptr = nextDescriptor((usbDescHeader_t *)pInterface);
           ptr != 0; ptr = nextDescriptor(ptr))
      {
        if (ptr->bDescriptorType == USB_DTYPE_ENDPOINT)
        {
          if (endpointIndex == 0)
          {
            return (usbEndpointDesc_t *)ptr;
          }
          endpointIndex--;
        }
      }
    }

    return 0;
  }

  void USBDevice::handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup)
  {
    // TODO: stall?
    logWarn("not implemented (stall?)");
  }

  void USBDevice::handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length)
  {
    // TODO: stall?
    logWarn("not implemented (stall?)");
  }

  void USBDevice::handleEP0Data()
  {
    // TODO: stall?
    logWarn("not implemented (stall?)");
  }

  void USBDevice::onEP0RequestComplete()
  {
    usbEndptRequest_t *req = &(mRequests[0]);

    if (USB_TOKEN_TYPE_IN == req->deviceEndptInfo.endpointDirection)
    {
      if (1u == req->zeroLengthPacket)
      {
        /* Hardware sends ZLP automatically */
      }
      else
      {
        /* Reset request and enter IDLE state */
        req->pbuf = NULL;
        req->length = 0U;
        req->zeroLengthPacket = 1U;
        req->status = DEV_REQ_STATUS_SUBMIT;
        req->actualLength = 0U;
        req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
        req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_OUT;
        setEP0State(EndpointStates::IDLE);
      }
    }
    else
    {
      /* No Action required here */
    }
  }

  void USBDevice::handleEP0TransmitState()
  {
    setEP0State(EndpointStates::TX);

    /* Set the number of bytes to send this iteration. */
    uint32_t numBytes = mEP0DataRemain;

    /* Limit individual transfers to 64 bytes. */
    if (numBytes > MAX_PACKET_SIZE_EP0)
    {
      numBytes = MAX_PACKET_SIZE_EP0;
    }

    /* Save the pointer so that it can be passed to the USBEndpointDataPut() */
    /* function. */
    unsigned char *pData = (unsigned char *)pEP0Data;

    /* Advance the data pointer and counter to the next data to be sent. */
    mEP0DataRemain -= numBytes;
    pEP0Data += numBytes;

    /* Put the data in the correct FIFO. */
    if (USBEndpointDataPut(BASE_ADDRESS, USB_EP_0, pData, numBytes) == 0)
    {
      logDebug(1, "sending %d bytes, %d bytes remain.", numBytes, mEP0DataRemain);
    }
    else
    {
      logError("Failed to send. TxPktRdy bit is already set.");
      return;
    }

    /* If this is exactly 64 then don't set the last packet yet. */
    if (numBytes == MAX_PACKET_SIZE_EP0)
    {
      /* There is more data to send or exactly 64 bytes were sent, this */
      /* means that there is either more data coming or a null packet needs */
      /* to be sent to complete the transaction. */
      USBEndpointDataSend(BASE_ADDRESS, USB_EP_0, USB_TRANS_IN);

      /* More data (or ZLP) to send so stay in transmit state. */
    }
    else
    {
      /* Send the last bit of data. */
      USBEndpointDataSend(BASE_ADDRESS, USB_EP_0, USB_TRANS_IN_LAST);

      /* Now go to the status state and wait for the transmit to complete. */
      setEP0State(EndpointStates::STATUS);
    }
  }

  // USBMusbDcdEp0Req:
  void USBDevice::setupEP0Request(usbDevRequest_t *req)
  {
    switch (mEP0State)
    {
    case EndpointStates::IDLE:
      if (1U == req->zeroLengthPacket)
      {
        /* EP0 remains in IDLE state when there is no data phase */
        setEP0State(EndpointStates::IDLE);
      }
      else
      {
        /* This is a 2 state request */
        if (USB_TOKEN_TYPE_IN == req->deviceEndptInfo.endpointDirection)
        {
          /* This is a in data request */
          pEP0Data = req->pbuf;
          mEP0DataRemain = req->length;
          mOUTDataSize = req->length;
          // Enter the TX state and transmit first packet.
          handleEP0TransmitState();
        }
        else if (USB_TOKEN_TYPE_OUT == req->deviceEndptInfo.endpointDirection)
        {
          /* This is a out data request */
          pEP0Data = req->pbuf;
          mEP0DataRemain = req->length;
          mOUTDataSize = req->length;
          setEP0State(EndpointStates::RX);
        }
      }
      break;
    default:
      logError("Trying to setup EP0 request but EP0 is not idle.");
      break;
    }
  }

  // USBMusbDcdEpReq:
  void USBDevice::setupEPRequest(usbDevRequest_t *req)
  {
    uint32_t epNum = req->deviceEndptInfo.endpointNumber;

    logAssert(epNum != 0);

    if (devState != USB_DEVICE_STATE_CONFIGURED)
    {
      /* not yet configured. Don't expect to use unconfigured endpoints.         */
      logWarn("unconfigured device (stall?)");
      return;
    }

    if (USB_TOKEN_TYPE_IN == req->deviceEndptInfo.endpointDirection)
    {
      inEpReq = req;
      lastReqDirection = EP_DESC_DIR_IN;
      req->status = DEV_REQ_STATUS_BEING_PROCESSED;

      /* using DMA to dump data into USB core for sending data to host
           Associate application provided data buffer into a packet descriptor */
      doDmaTxTransfer(controllerId, req->pbuf,
                      req->length, INDEX_TO_USB_EP(epNum));
    }
    else
    {
      lastReqDirection = EP_DESC_DIR_OUT;
      outEpReq = req;

      /* No preparation needed for the hw to receive data. */
      /* actual receving is handling at the interrupt handler. */

      /* Need to replenish the RX queue with BD. associate the application 
        provided buffer to a BD and put it into RX free queue */
      doDmaRxTransfer(controllerId, req->length,
                      req->pbuf, INDEX_TO_USB_EP(epNum));

      /* mark request for being processed */
      req->status = DEV_REQ_STATUS_BEING_PROCESSED;
    }
  }

  void USBDevice::setupRequest(uint32_t epNum,
                               uint8_t *pData,
                               usbTokenType_t tokenType,
                               uint32_t length,
                               usbTransferType_t transferType)
  {
    logDebug(12, "EP%d", epNum);

    /* Setup endpoint request for the required endpoint */
    usbDevRequest_t *req = 0;

    /* getting pointer to the request_t for this endpoint */
    if (epNum < USBLIB_NUM_EP)
    {
      req = &(mRequests[epNum]);
    }
    else
    {
      logError("setupRequest invalid epNum (%d)", epNum);
      return;
    }

    req->pbuf = pData;
    req->length = length;
    req->zeroLengthPacket = 0U;
    req->status = DEV_REQ_STATUS_SUBMIT;
    req->actualLength = 0U;
    req->deviceEndptInfo.endpointType = transferType;
    req->deviceEndptInfo.endpointDirection = tokenType;
    req->deviceEndptInfo.endpointNumber = epNum;

    if (0U == epNum)
    {
      //req->reqComplete = usbEp0reqComplete;
      setupEP0Request(req);
    }
    else
    {
      //req->reqComplete = usbEpreqComplete;
      setupEPRequest(req);
    }
  }

  void USBDevice::cancelPendingRequest(uint32_t epNum)
  {
    if (epNum == 0)
    {
      return;
    }

    usbDevRequest_t *req = &(mRequests[epNum]);
    if (req->status != DEV_REQ_STATUS_BEING_PROCESSED)
    {
      return;
    }

    if (USB_TOKEN_TYPE_OUT == req->deviceEndptInfo.endpointDirection)
    {
      logDebug(1, "EP%d, canceling DMA transaction for pending OUT request.", epNum);
      Cppi41DmaRxChTeardown(controllerId, INDEX_TO_USB_EP(req->deviceEndptInfo.endpointNumber));
      req->status = DEV_REQ_STATUS_COMPLETE;
    }
    else
    {
      logDebug(1, "EP%d, canceling DMA transaction for pending IN request.", epNum);
      Cppi41DmaTxChTeardown(controllerId, INDEX_TO_USB_EP(req->deviceEndptInfo.endpointNumber));
      req->status = DEV_REQ_STATUS_COMPLETE;
    }
  }

  // Implements USB_coreIrqHandler(...)
  // which eventually falls into usbMusbDcdCoreIntrHandler(...)
  void USBDevice::handleUSBInterrupt()
  {
    uint32_t ulStatus = 0;

    /*Get Interrupt Controller Status*/
    ulStatus = USBGetEndpointIntrStatus(SUBBASE_ADDRESS, &ulIrqStatus, &epIrqStatus);

    /* Clear the interrupt status. */
    USBClearInt(SUBBASE_ADDRESS, ulStatus, epIrqStatus);

    ulIrqStatus |= USBIntStatusControl(BASE_ADDRESS);

    if ((ulIrqStatus & (USB_INTCTRL_RESET | USB_INTCTRL_DISCONNECT)) ||
        (epIrqStatus & (USB_INTEP_0 | 0xFFFEFFFE)) ||
        (dmaStatus & 0xF07))
    {
      handleUSBEvent(); // USBMusbDcdIntrHandler(...);
    }

    /* Clear the interrupt which occured */
    HWREG(SUBBASE_ADDRESS + USB_0_IRQ_EOI) = 0;
  }

  // Implements usbDmaIntrHandler(...)
  void USBDevice::handleDMAInterrupt()
  {
    dmaStatus = HWREG(USBSS_BASE + USBSS_IRQ_STATUS);

    /* Clear USBSS INT */
    HWREG(USBSS_BASE + USBSS_IRQ_STATUS) = dmaStatus;

    handleUSBInterrupt();

    /* Clear CPPI DMA Intr */
    /* so that any other ISR don't act on false DMA events */
    dmaStatus = 0;

    HWREG(USBSS_BASE + USBSS_IRQ_EOI) = 0;
  }

#if BUILDOPT_DEBUG_LEVEL
  static const char *_USBEventDebugString(uint32_t epIrqStatus, uint32_t ulIrqStatus, uint32_t dmaStatus)
  {
    static char buffer[64];
    char *ptr = buffer;
    int len = sizeof(buffer);

    /* USB device was disconnected. */
    if (ulIrqStatus & USB_INTCTRL_DISCONNECT)
    {
      int n = snprintf(ptr, len, "disconnect ");
      ptr += n;
      len -= n;
    }

    /* Received a reset from the host. */
    if (ulIrqStatus & USB_INTCTRL_RESET)
    {
      int n = snprintf(ptr, len, "reset ");
      ptr += n;
      len -= n;
    }

    /* Handle end point 0 interrupts. */
    if (epIrqStatus & USB_INTEP_0)
    {
      int n = snprintf(ptr, len, "ep0 ");
      ptr += n;
      len -= n;
    }

    /* Other endpoint and device interrupts */
    if (epIrqStatus & 0xFFFEFFFE)
    {
      int n = snprintf(ptr, len, "epN ");
      ptr += n;
      len -= n;
    }

    // DMAQueuePendHandler
    // Handles the CDMA Queue Pend Status for any reception or transmission
    if (dmaStatus & 0xF07)
    {
      int n = snprintf(ptr, len, "dma ");
      ptr += n;
      len -= n;
    }

    *ptr = 0;
    return buffer;
  }
#endif

  void USBDevice::handleUSBEvent(void)
  {
    mUsbEventCount++;
    logDebug(1, "#%d, addr=%d, %s",
             mUsbEventCount,
             USBDevAddrGet(BASE_ADDRESS),
             _USBEventDebugString(epIrqStatus, ulIrqStatus, dmaStatus));

    /* USB device was disconnected. */
    if (ulIrqStatus & USB_INTCTRL_DISCONNECT)
    {
      logInfo("USB disconnected.");
      onDisconnect();

      /* clean up DMA buffer descriptors that are in the RX Queue */
      /* doing this only after we have setup the DMA for the OUT transaction */
      /* skipping endpoint 0 since we don't do DMA on EP0 */
      for (uint32_t epNum = 1; epNum < USBLIB_NUM_EP; epNum++)
      {
        cancelPendingRequest(epNum);
      }

      resetConfiguration();
      resetAddress();
      devState = USB_DEVICE_STATE_RESET;
      setEP0State(EndpointStates::IDLE);
      return;
    }

    /* Received a reset from the host. */
    if (ulIrqStatus & USB_INTCTRL_RESET)
    {
      /* Disable remote wake up signaling (as per USB 2.0 spec 9.1.1.6). */
      ucStatus &= ~USB_STATUS_REMOTE_WAKE;
      bRemoteWakeup = false;

      resetConfiguration();

      devState = USB_DEVICE_STATE_RESET;
      setEP0State(EndpointStates::IDLE);
      return;
    }

    /* If there is a pending address change then set the address. */
    if (mUpdateDeviceAddress)
    {
      uint32_t currentAddress = USBDevAddrGet(BASE_ADDRESS);
      if (mPendingDeviceAddress != currentAddress)
      {
        USBDevAddrSet(BASE_ADDRESS, mPendingDeviceAddress);
        logAssert(mPendingDeviceAddress == USBDevAddrGet(BASE_ADDRESS));
        logDebug(1, "address(%d to %d)", currentAddress, mPendingDeviceAddress);
      }
      mUpdateDeviceAddress = false;
      devState = USB_DEVICE_STATE_ADDRESSED;
    }

    /* Start of Frame was received. */
    if (ulIrqStatus & USB_INTCTRL_SOF)
    {
      logDebug(99, "start-of-frame");
      // nop
    }

    /* Handle end point 0 interrupts. */
    if (epIrqStatus & USB_INTEP_0)
    {
      handleEP0Event(); // usbMusbDcdEp0EvntHandler(...);
    }

    // DMAQueuePendHandler
    // Handles the CDMA Queue Pend Status for any reception or transmission
    if (dmaStatus & 0xF07)
    {
      logDebug(10, "CDMA Queue Pend Status");
      handleEPEvent();
    }

    /* Other endpoint and device interrupts */
    if (epIrqStatus & 0xFFFEFFFE)
    {
      handleEPEvent();
    }
  }

  // USBDevSetupDispatcher
  void USBDevice::handleSetupPacket(usbSetupPkt_t *pSetup)
  {
    logDebug(1, "type=0x%x req=0x%x value=0x%x index=0x%x len=%d",
             pSetup->bmRequestType, pSetup->bRequest, pSetup->wValue, pSetup->wIndex, pSetup->wLength);
    /* See if this is a standard request or not.*/
    if ((pSetup->bmRequestType & USB_RTYPE_TYPE_M) != USB_RTYPE_STANDARD)
    {
      logDebug(1, "non-standard request");
      handleNonstandardRequest(USB_ENDPT0_EVENT_DATA_IN_COMPLETE, pSetup);
    }
    else
    {
      /* it is a standard request. */
      switch (pSetup->bRequest)
      {
      case 0: // get status
        logDebug(6, "[get status]");
        handleGetStatusRequest(pSetup);
        break;
      case 1: // clear feature
        logWarn("[clear feature] not implemented. Stall?");
        break;
      case 2:
        logWarn("unsupported request (0x%x). Stall?", pSetup->bRequest);
        break;
      case 3: // set feature
        logWarn("[set feature] not implemented. Stall?");
        break;
      case 4:
        logWarn("unsupported request (0x%x). Stall?", pSetup->bRequest);
        break;
      case 5: // set address
        logDebug(6, "[set address]");
        handleSetAddressRequest(pSetup);
        break;
      case 6: // get descriptor
        logDebug(6, "[get descriptor]");
        handleGetDescriptorRequest(pSetup);
        break;
      case 7: // set descriptor
        logWarn("[set descriptor] not implemented. Stall?");
        break;
      case 8: // get configuration
        logWarn("[get configuration] not implemented. Stall?");
        break;
      case 9: // set configuration
        logDebug(6, "[set configuration]");
        handleSetConfigurationRequest(pSetup);
        break;
      case 10: // get interface
        logWarn("[get interface] not implemented. Stall?");
        break;
      case 11: // set interface
        logDebug(6, "[set interface]");
        handleSetInterfaceRequest(pSetup);
        break;
      case 12: // sync frame
        logWarn("[sync frame] not implemented. Stall?");
        break;
      default:
        logError("unsupported request (0x%x). Stall?", pSetup->bRequest);
        break;
      }
    }
  }

  void USBDevice::handleGetStatusRequest(usbSetupPkt_t *pSetup)
  {
    uint16_t data = 0;
    usbDevRequest_t *req = &(mRequests[0]);

    /* Determine which type of status was requested */
    switch (pSetup->bmRequestType & USB_RTYPE_RECIPIENT_M)
    {
    case USB_RTYPE_DEVICE:
      /* Return the current status for the device */
      /* TODO Check how this status is to be stored and where */
      data = 0;
      break;
    case USB_RTYPE_INTERFACE:
      data = 0;
      break;
    case USB_RTYPE_ENDPOINT:
      /* TODO Check logic on how to return this status for endpoints */
      data = 0;
      break;
    default:
      /* Set endpoint stall */
      break;
    }

    mpEP0OutputBuffer[0] = data;

    /* Setup the request and send the USB status */
    req->pbuf = mpEP0OutputBuffer;
    req->length = 2U;
    req->zeroLengthPacket = 0U;
    req->status = DEV_REQ_STATUS_SUBMIT;
    req->actualLength = 0U;
    req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
    req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_IN;
    // req->reqComplete = usbEp0reqComplete;
    setupEP0Request(req);
  }

  void USBDevice::handleSetAddressRequest(usbSetupPkt_t *pSetup)
  {
    logDebug(6, "addr=%d.", pSetup->wValue);

    /* Transition directly to the status state since there is no data phase
            for this request.*/
    setEP0State(EndpointStates::STATUS);

    // Save the new address.  It will be committed on the next EP0 event.
    mPendingDeviceAddress = pSetup->wValue;
    mUpdateDeviceAddress = true;
  }

  void USBDevice::handleGetDescriptorRequest(usbSetupPkt_t *pSetup)
  {
    usbDevRequest_t *req = &(mRequests[0]);

    /* Which descriptor are we being asked for?*/
    uint32_t descriptorType = pSetup->wValue >> 8;
    switch (descriptorType)
    {
    case USB_DESC_TYPE_DEVICE:
      /*This request was for a device descriptor.*/
      logDebug(3, "DEVICE type");

      /* copy the device descriptor to a non-cached buffer that
             * is accessible by USB DMA */
      req->pbuf = mpEP0OutputBuffer;
      req->length = copyDeviceDescriptor(req->pbuf, MAX_PACKET_SIZE_EP0);
      if (req->length == 0)
      {
        logError("failed to copy DEVICE descriptor");
      }
      else
      {
        req->zeroLengthPacket = 0U;
        req->status = DEV_REQ_STATUS_SUBMIT;
        req->actualLength = 0U;
        req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
        req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_IN;
        //req->reqComplete = usbEp0reqComplete;
        setupEP0Request(req);
      }
      break;

    case USB_DESC_TYPE_CONFIGURATION:
      /*This request was for a configuration descriptor.*/
      logDebug(3, "CONFIGURATION type");

      /* copy the configuration descriptor to a non-cached buffer that
             * is accessible by USB DMA */
      req->pbuf = mpEP0OutputBuffer;
      req->length = copyConfigurationDescriptor(pSetup->wValue & 0xFF, req->pbuf, MAX_PACKET_SIZE_EP0);
      if (req->length == 0)
      {
        logError("failed to copy configuration descriptor (index=%d)", pSetup->wValue & 0xFF);
      }
      else
      {
        req->zeroLengthPacket = 0U;
        req->status = DEV_REQ_STATUS_SUBMIT;
        req->actualLength = 0U;
        req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
        req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_IN;

        if (0x09 == pSetup->wLength)
        {
          // Why?
          req->length = 9U;
        }

        setupEP0Request(req);
      }
      break;

    case USB_DESC_TYPE_STRING:
      /*This request was for a string descriptor.*/
      logDebug(3, "STRING type");

      /* copy the string descriptor to a non-cached buffer that
             * is accessible by USB DMA */
      req->pbuf = mpEP0OutputBuffer;
      req->length = copyStringDescriptor(pSetup->wIndex, pSetup->wValue & 0xFF, req->pbuf, MAX_PACKET_SIZE_EP0);
      if (req->length == 0)
      {
        logError("failed to copy STRING descriptor");
      }
      else
      {
        req->zeroLengthPacket = 0U;
        req->status = DEV_REQ_STATUS_SUBMIT;
        req->actualLength = 0U;
        req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
        req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_IN;

        setupEP0Request(req);
      }
      break;

    default:
      /*Any other request is not handled by the default enumeration handler
                so see if it needs to be passed on to another handler.*/
      logError("UNKNOWN type: %d", descriptorType);
      break;
    }
  }

  void USBDevice::handleSetConfigurationRequest(usbSetupPkt_t *pSetup)
  {
    logDebug(1, "wValue=%d", pSetup->wValue);
    if (getConfigurationDescriptorByValue(pSetup->wValue))
    {
      usbDevRequest_t *req = &(mRequests[0]);

      /* Return the ZLP(Zero length packet). */
      req->pbuf = NULL;
      req->length = 0U;
      req->zeroLengthPacket = 1U;
      req->status = DEV_REQ_STATUS_SUBMIT;
      req->actualLength = 0U;
      req->deviceEndptInfo.endpointType = USB_TRANSFER_TYPE_CONTROL;
      req->deviceEndptInfo.endpointDirection = USB_TOKEN_TYPE_OUT;
      setupEP0Request(req);

      if (pSetup->wValue)
      {
        /* Configure endpoints for the new configuration. */
        if (setConfiguration(pSetup->wValue))
        {
          devState = USB_DEVICE_STATE_CONFIGURED;
          onConfigurationChange();
          onEnumerationComplete();
          logInfo("Enumeration complete. Configuration #%d.", pSetup->wValue);
        }
        else
        {
          logError("configuration failed, stalling EP0.");
          stallEP0();
        }
      }
    }
    else
    {
      logError("invalid configuration, stalling EP0.");
      stallEP0();
    }
  }

  void USBDevice::stallEP0()
  {
    USBDevEndpointStall(BASE_ADDRESS, USB_EP_0, USB_EP_DEV_OUT);
    setEP0State(EndpointStates::STALL);
  }

  void USBDevice::handleSetInterfaceRequest(usbSetupPkt_t *pSetup)
  {
    if (setAlternateSetting(pSetup->wIndex, pSetup->wValue))
    {
      onInterfaceChange();
    }
    else
    {
      logError("failed, stalling EP0.");
      stallEP0();
    }
  }

  void USBDevice::onConfigurationChange()
  {
    // The default implementation does nothing.
  }

  void USBDevice::onInterfaceChange()
  {
    // The default implementation does nothing.
  }

  void USBDevice::onEnumerationComplete()
  {
    Events_push(EVENT_USB_CONNECT);
  }

  void USBDevice::onDisconnect()
  {
    Events_push(EVENT_USB_DISCONNECT);
  }

  void USBDevice::onEP0Stalled()
  {
    // The default implementation does nothing.
  }

  /****************************************************************************** */
  /* Given a maximum packet size and the user's FIFO scaling requirements, */
  /* determine the flags to use to configure the endpoint FIFO and the number */
  /* of bytes of FIFO space occupied. */
  /****************************************************************************** */
  static uint32_t quantizeEndpointFIFOSize(uint32_t maxPacketSize, uint32_t *pBytesUsed, int multiplier = 1, bool doubleBuffer = false)
  {
    /* A zero multiplier would not be a good thing. */
    logAssert(multiplier);

    /* What is the basic size required for a single buffered FIFO entry */
    /* containing the required number of packets? */
    uint32_t bytes = maxPacketSize * multiplier;

    /* Now we need to find the nearest supported size that accommodates the */
    /* requested size.  Step through each of the supported sizes until we */
    /* find one that will do. */
    for (int i = USB_FIFO_SZ_8; i <= USB_FIFO_SZ_8192; i++)
    {
      /* How many bytes does this FIFO value represent? */
      uint32_t fifoSize = USB_FIFO_SZ_TO_BYTES(i);

      /* Is this large enough to satisfy the request? */
      if (fifoSize >= bytes)
      {
        /* Yes - are we being asked to double-buffer the FIFO for this */
        /* endpoint? */
        if (doubleBuffer)
        {
          /* Yes - FIFO requirement is double in this case. */
          *pBytesUsed = fifoSize * 2;
          return (i | USB_FIFO_SIZE_DB_FLAG);
        }
        else
        {
          /* No double buffering so just return the size and associated */
          /* flag. */
          *pBytesUsed = fifoSize;
          return (i);
        }
      }
    }

    /* If we drop out, we can't support the FIFO size requested.  Signal a */
    /* problem by returning 0 in the pBytesUsed */
    *pBytesUsed = 0;
    return (USB_FIFO_SZ_8);
  }

  // usbMusbDcdDeviceConfig
  bool USBDevice::setConfiguration(uint32_t configValue)
  {
    logDebug(6, "%d", configValue);
    usbConfigDesc_t *pConfig = getConfigurationDescriptorByValue(configValue);
    if (pConfig == 0)
    {
      logError("Invalid config value");
      return false;
    }

    if (pConfig->bNumInterface > MaxInterfaceCount)
    {
      logError("%d interfaces exceeds the currently hard-coded maximum of %d interfaces.", pConfig->bNumInterface, MaxInterfaceCount);
      return false;
    }

    enum EndpointDirection
    {
      in_endpoint = 0,
      out_endpoint = 1
    };

    // Calculate the maximum packet size for each endpoint over all interfaces and alternative settings
    uint32_t fifoSizes[NUM_USB_EP - 1][2] = {0};
    for (usbDescHeader_t *ptr = nextDescriptor((usbDescHeader_t *)pConfig);
         ptr != 0; ptr = nextDescriptor(ptr))
    {
      if (ptr->bDescriptorType == USB_DTYPE_ENDPOINT)
      {
        usbEndpointDesc_t *pEndpoint = (usbEndpointDesc_t *)ptr;
        uint32_t epNum = pEndpoint->bEndpointAddress & USB_EP_DESC_NUM_M;

        if (epNum == 0 || epNum >= NUM_USB_EP)
        {
          logError("Invalid endpoint address (%d)", epNum);
          return false;
        }

        /* Does this endpoint have a max packet size requirement larger than */
        /* any previous use we have seen? */
        EndpointDirection epDir = (pEndpoint->bEndpointAddress & USB_EP_DESC_IN) ? in_endpoint : out_endpoint;
        uint32_t epIndex = epNum - 1;
        uint32_t maxPacketSize = pEndpoint->wMaxPacketSize & USB_EP_MAX_PACKET_COUNT_M;
        if (maxPacketSize > fifoSizes[epIndex][epDir])
        {
          /* Yes - increase the fifo size */
          fifoSizes[epIndex][epDir] = maxPacketSize;
        }
      }
    }

    // About to actually configure hardware, so forget the previous active configuration.
    resetConfiguration();

    // Configure the endpoints for each interface using the default alternate setting (zero).
    for (int i = 0; i < pConfig->bNumInterface; i++)
    {
      usbInterfaceDesc_t *pInterface = getInterfaceDescriptor(pConfig, i, 0);
      if (pInterface == 0)
      {
        logError("Interface(%d of %d) does not exist.", i, pConfig->bNumInterface);
        return false;
      }
      else if (pInterface->bInterfaceNumber >= MaxInterfaceCount)
      {
        logError("Interface number (%d) exceeds maximum allowed.", pInterface->bInterfaceNumber);
        return false;
      }

      if (!configureInterfaceEndpoints(pInterface, true))
      {
        return false;
      }
    }

    /* At this point, we have configured all the endpoints that are to be */
    /* used by this configuration's alternate setting 0.  Now we go on and */
    /* partition the FIFO based on the maximum packet size information we */
    /* extracted earlier.  Endpoint 0 is automatically configured to use the */
    /* first MAX_PACKET_SIZE_EP0 bytes of the FIFO so we start from there. */
    uint32_t fifoPosition = MAX_PACKET_SIZE_EP0;
    for (int i = 1; i < NUM_USB_EP; i++)
    {
      uint32_t requiredSize = fifoSizes[i - 1][in_endpoint];
      if (requiredSize)
      {
        /* What FIFO size flag do we use for this endpoint? */
        uint32_t bytesUsed = 0;
        uint32_t sizeFlag = quantizeEndpointFIFOSize(requiredSize, &bytesUsed);
        if (bytesUsed < requiredSize)
        {
          logError("IN endpoint(%d) failed to configure FIFO.", i);
          return false;
        }

        /* Now actually configure the FIFO for this endpoint. */
        USBFIFOConfigSet(BASE_ADDRESS,
                         INDEX_TO_USB_EP(i), fifoPosition,
                         sizeFlag, USB_EP_DEV_IN);

        fifoPosition += bytesUsed;

        /* enable TX DMA */
        enableCoreTxDMA(controllerId, INDEX_TO_USB_EP(i));
      }

      requiredSize = fifoSizes[i - 1][out_endpoint];
      if (requiredSize)
      {
        /* What FIFO size flag do we use for this endpoint? */
        uint32_t bytesUsed = 0;
        uint32_t sizeFlag = quantizeEndpointFIFOSize(requiredSize, &bytesUsed);
        if (bytesUsed < requiredSize)
        {
          logError("OUT endpoint(%d) failed to configure FIFO.", i);
          return false;
        }

        /* Now actually configure the FIFO for this endpoint. */
        USBFIFOConfigSet(BASE_ADDRESS,
                         INDEX_TO_USB_EP(i), fifoPosition,
                         sizeFlag, USB_EP_DEV_OUT);

        fifoPosition += bytesUsed;

        /* enable RX DMA */
        enableCoreRxDMA(controllerId, INDEX_TO_USB_EP(i));
      }
    }

    mpActiveConfiguration = pConfig;
    return true;
  }

  bool USBDevice::setAlternateSetting(uint32_t interfaceId, uint32_t alternateSetting)
  {
    logDebug(1, "interfaceId=%d alternateSetting=%d", interfaceId, alternateSetting);

    //    usbMusbDcdSetInterface
    if (mpActiveConfiguration == 0)
    {
      logError("No active configuration.");
      return false;
    }

    if (mAlternateSettings[interfaceId] == alternateSetting)
    {
      // This interface is already configured to the given alternate setting, so nothing to do.
      logDebug(6, "Already set, so nothing to do.");
      return true;
    }

    usbInterfaceDesc_t *pInterface = getInterfaceDescriptor(mpActiveConfiguration, interfaceId, alternateSetting);
    if (pInterface == 0)
    {
      logError("Not a valid interface.");
      return false;
    }

    // USBDConfigAlternate
    if (configureInterfaceEndpoints(pInterface, false))
    {
      mAlternateSettings[interfaceId] = alternateSetting;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool USBDevice::configureInterfaceEndpoints(usbInterfaceDesc_t *pInterface, bool resetFlags)
  {
    /* Walk through each endpoint in this interface and configure */
    /* it appropriately. */
    for (int i = 0; i < pInterface->bNumEndpoints; i++)
    {
      usbEndpointDesc_t *pEndpoint = getEndpointDescriptor(pInterface, i);
      if (pEndpoint == 0)
      {
        logError("Endpoint does not exist.");
        return false;
      }

      uint32_t epNum = pEndpoint->bEndpointAddress & USB_EP_DESC_NUM_M;
      uint32_t maxPacketSize = pEndpoint->wMaxPacketSize & USB_EP_MAX_PACKET_COUNT_M;
      uint32_t flags = (pEndpoint->bEndpointAddress & USB_EP_DESC_IN) ? USB_EP_DEV_IN : USB_EP_DEV_OUT;

      /* Set the endpoint mode. */
      switch (pEndpoint->bmAttributes & USB_EP_ATTR_TYPE_M)
      {
      case USB_EP_ATTR_CONTROL:
        flags |= USB_EP_MODE_CTRL;
        break;

      case USB_EP_ATTR_BULK:
        flags |= USB_EP_MODE_BULK;
        break;

      case USB_EP_ATTR_INT:
        flags |= USB_EP_MODE_INT;
        break;

      case USB_EP_ATTR_ISOC:
        flags |= USB_EP_MODE_ISOC;
        break;
      }

      if (!resetFlags)
      {
        /*
                * Get the existing endpoint configuration and mask in the
                * new mode and direction bits, leaving everything else
                * unchanged.
                */
        uint32_t oldFlags;
        uint32_t oldMaxPacketSize;
        USBDevEndpointConfigGet(BASE_ADDRESS,
                                INDEX_TO_USB_EP(epNum),
                                &oldMaxPacketSize,
                                &oldFlags);
        /* Mask in the previous DMA and auto-set bits. */
        flags = (flags & EP_FLAGS_MASK) | (oldFlags & ~EP_FLAGS_MASK);
      }

      /* Set the endpoint configuration. */
      USBDevEndpointConfigSet(BASE_ADDRESS,
                              INDEX_TO_USB_EP(epNum),
                              maxPacketSize, flags);
    }

    return true;
  }

  void USBDevice::handleEP0Event() // usbMusbDcdEp0EvntHandler
  {
    /* Get the end point 0 status. */
    uint32_t endpointStatus = USBEndpointStatus(BASE_ADDRESS, USB_EP_0);

    if (endpointStatus & USB_DEV_EP0_SENT_STALL)
    {
      logWarn("EP0 has stalled. Aborting any control transfer.");
      onEP0Stalled();
      USBDevEndpointStatusClear(BASE_ADDRESS, USB_EP_0, 0);
      setEP0State(EndpointStates::IDLE);
      //return;
    }

    if (endpointStatus & USB_DEV_EP0_SETUP_END)
    {
      logWarn("Current control transfer has ended prematurely.");
      onEP0Stalled();
      USBDevEndpointStatusClear(BASE_ADDRESS, USB_EP_0, 0);
      setEP0State(EndpointStates::IDLE);
      //return;
    }

    switch (mEP0State)
    {
    case EndpointStates::IDLE:
      /* In the IDLE state the code is waiting to receive data from the host. */

      /* Is there a packet waiting for us? */
      if (endpointStatus & USB_DEV_EP0_OUT_PKTRDY)
      {
        logDebug(6, "SETUP stage, received packet");
        uint32_t bufferSize = MAX_PACKET_SIZE_EP0;

        /* Get the data from the USB controller end point 0. */
        USBEndpointDataGet(BASE_ADDRESS, USB_EP_0, mpEP0InputBuffer, &bufferSize);
        USBDevEndpointDataAck(BASE_ADDRESS, USB_EP_0, true);

        /* If there was a null setup packet then just return. */
        if (!bufferSize)
        {
          logWarn("null setup packet, ignoring.");
          return;
        }

        handleSetupPacket((usbSetupPkt_t *)mpEP0InputBuffer);
      }
      break;

    case EndpointStates::STATUS:
      // The host is indicating that the previous request is complete.
      logDebug(6, "STATUS stage, previous setup request is complete.");

      /* completion of status phase of transfer */
      setEP0State(EndpointStates::IDLE);
      onEP0RequestComplete();

      /* If a new packet is already pending, we need to read it */
      /* and handle whatever request it contains. */
      if (endpointStatus & USB_DEV_EP0_OUT_PKTRDY)
      {
        logDebug(8, "Received packet during STATUS state, re-entering event handler.");
        handleEP0Event();
      }

      break;

    case EndpointStates::TX:
      /* Data is still being sent to the host */
      logDebug(6, "TX");
      handleEP0TransmitState();
      break;

    case EndpointStates::RX:
      /* Handle the receive state for commands that are receiving data on */
      /* endpoint zero. */
      logDebug(6, "RX");

      /* Set the number of bytes to get out of this next packet. */
      uint32_t numBytes;
      if (mEP0DataRemain > MAX_PACKET_SIZE_EP0)
      {
        /* Don't send more than EP0_MAX_PACKET_SIZE bytes. */
        numBytes = MAX_PACKET_SIZE_EP0;
      }
      else
      {
        /* There was space so send the remaining bytes. */
        numBytes = mEP0DataRemain;
      }

      /* Get the data from the USB controller end point 0. */
      if (USBEndpointDataGet(BASE_ADDRESS, USB_EP_0, pEP0Data, &numBytes) == 0)
      {
        logDebug(1, "received %d bytes, %d bytes remain.", numBytes, mEP0DataRemain);
      }
      else
      {
        logError("Failed to receive. RxPktRdy bit is not set.");
        return;
      }

      pEP0Data += numBytes;
      mEP0DataRemain -= numBytes;

      /* If there were less than */
      /* MAX_PACKET_SIZE_EP0 remaining then there still needs to be */
      /* null packet sent before this is complete. */
      if (mEP0DataRemain > MAX_PACKET_SIZE_EP0)
      {
        // Still more data coming...

        /* Need to ACK the data on end point 0 in this case */
        /* without setting data end because more data is coming. */
        /*USBDevEndpointDataAck(musb->uiBaseAddr, USB_EP_0, false); */

        onEP0RequestComplete();
      }
      else
      {
        /* Need to ACK the data on end point 0 in this case and set the */
        /* data end as this is the last of the data. */
        USBDevEndpointDataAck(BASE_ADDRESS, USB_EP_0, true);

        /* Return to the idle state. */
        setEP0State(EndpointStates::IDLE);

        onEP0RequestComplete();

        if (mOUTDataSize != 0)
        {
          /* Call the custom receive handler to handle the data */
          /* that was received. */
          handleEP0Data();

          /* Indicate that there is no longer any data being waited on. */
          mOUTDataSize = 0;
        }
        else
        {
          logWarn("Received no data.");
        }
      }
      break;
    case STALL:
      /* The device stalled endpoint zero so check if the stall needs to be */
      /* cleared once it has been successfully sent. */
      logDebug(1, "STALL");

      /* If we sent a stall then acknowledge this interrupt. */
      if (endpointStatus & USB_DEV_EP0_SENT_STALL)
      {
        /* Clear the Setup End condition. */
        USBDevEndpointStatusClear(BASE_ADDRESS, USB_EP_0, USB_DEV_EP0_SENT_STALL);

        /* Reset the global end point 0 state to IDLE. */
        setEP0State(EndpointStates::IDLE);
      }
      break;
    default:
      /* Halt on an unknown state, but only in DEBUG mode builds. */
      logError("UNKNOWN state");
    }
  }

  // usbMusbDcdEpEvntHandler(musb);
  void USBDevice::handleEPEvent()
  {
    uint32_t tx_eps = 0;
    uint32_t rx_eps = 0;
    uint32_t comp_eps = 0;

    usbDevRequest_t *req = 0;

    if ((epIrqStatus == 0) || (epIrqStatus == 1))
    {
      /* if we get here, the interrupt is from DMA engine. 
         * we can find which endpoint this interrupt is for by looking at the queue
         * pend registers and find which queue it comes from. The queue - ep mapping
         * is mentioned in table 16-27 in TRM */
      logDebug(12, "No EP IRQ Status. Using DMA to find EP.");
      comp_eps = CppiDmaGetEpNumber(controllerId);
    }
    else
    {
      /* find which endpoint this interrupt is for */
      comp_eps = epIrqStatus;
    }

    tx_eps = comp_eps & 0xFFFE;
    rx_eps = (comp_eps & 0xFFFE0000) >> 16;

    /* 
        MUSB can have one interrupt for many EPs at the same time. 
        Find out which EP this interrupt belongs to by looking at 
        which bit is set. 
        Since USB only has 16 eps, bitmaps should be a 16 bit number
        We handle EP0 separately so here the EPs we are interested in are 
        only from 1 to 15.
        */
    for (uint32_t epIndex = 1; epIndex < 15; epIndex++)
    {
      if (tx_eps & (1 << epIndex))
      {
        /* This is a request for in transfer complete */
        req = &(mRequests[epIndex]);

        if (epIndex == req->deviceEndptInfo.endpointNumber)
        {
          /* handle DMA completion for IN transaction here */
          if (dmaTxCompletion(controllerId, INDEX_TO_USB_EP(epIndex)) == DMA_TX_COMPLETED)
          {
            /* mark request being completed */
            req->status = DEV_REQ_STATUS_COMPLETE;

            handleEndpoints(USB_GENERIC_EVENT_DATA_IN_COMPLETE, epIndex, req->length);
          }
        }
        else if (req->deviceEndptInfo.endpointType == USB_TRANSFER_TYPE_ISOCH)
        {
          handleEndpoints(USB_GENERIC_EVENT_DATA_IN_COMPLETE, epIndex, 0);
        }
        else
        {
          // Can't find the request associted with this IRQ.
          logWarn("No associated request for IN event on EP%d. Ignoring.", epIndex);
        }
      }

      if (rx_eps & (1 << epIndex))
      {
        req = &(mRequests[epIndex]);

        if (epIndex == req->deviceEndptInfo.endpointNumber)
        {
          /* handling DMA completion for OUT transaction */
          dmaRxCompletion(controllerId, INDEX_TO_USB_EP(epIndex), &(req->length));

          /* data should have been written by DMA. no need to copy them out now.
                   Data buffer was associated to DMA transfer in USBMusbDcdEpReq() */

          /* mark request being completed */
          req->status = DEV_REQ_STATUS_COMPLETE;

          handleEndpoints(USB_GENERIC_EVENT_DATA_OUT_COMPLETE, epIndex, req->length);
        }
        else if (req->deviceEndptInfo.endpointType == USB_TRANSFER_TYPE_ISOCH)
        {
          handleEndpoints(USB_GENERIC_EVENT_DATA_OUT_COMPLETE, epIndex, 0);
        }
        else
        {
          // Can't find the request associted with this IRQ.
          logWarn("No associated request for OUT event on EP%d. Ignoring.", epIndex);
        }
      } /* rx_eps */
    }   /* for epNo */
  }

  void USBDevice::resetConfiguration()
  {
    mpActiveConfiguration = 0;
    for (int i = 0; i < MaxInterfaceCount; i++)
    {
      mAlternateSettings[i] = 0;
    }
  }

  void USBDevice::init()
  {
    logAssert(mConfigurationCount <= MaxConfigurationCount);
  }

  /* ****************************************************************************
    * DMA endpoint settings for MSC - need configure with all end points?
                                - since this config could be different from
                                the config that user application can set.
    ***************************************************************************** */

  static bool Cppi41DmaInitialized = false;
  static endpointInfo epInfo[] =
      {
          {
              USB_EP_TO_INDEX(USB_EP_1),
              CPDMA_DIR_RX,
#ifdef USE_GRNDIS
              CPDMA_MODE_SET_GRNDIS,
#else
              CPDMA_MODE_SET_TRANSPARENT,
#endif
          },

          {
              USB_EP_TO_INDEX(USB_EP_1),
              CPDMA_DIR_TX,
              CPDMA_MODE_SET_TRANSPARENT,
          },

          {
              USB_EP_TO_INDEX(USB_EP_2),
              CPDMA_DIR_RX,
#ifdef USE_GRNDIS
              CPDMA_MODE_SET_GRNDIS,
#else
              CPDMA_MODE_SET_TRANSPARENT,
#endif
          },

          {
              USB_EP_TO_INDEX(USB_EP_2),
              CPDMA_DIR_TX,
              CPDMA_MODE_SET_TRANSPARENT,
          }

  };

#define NUMBER_OF_ENDPOINTS 4

  void USBDevice::start()
  {
    USBReset(SUBBASE_ADDRESS);
    UsbPhyOn(PHYBASE_ADDRESS);

    /* Make sure we disconnect from the host for a while.  This ensures */
    /* that the host will enumerate us even if we were previously */
    /* connected to the bus. */
    USBDevDisconnect(BASE_ADDRESS);
    Task_sleep(100);

    // Reset internal state
    resetConfiguration();
    resetAddress();
    devState = USB_DEVICE_STATE_UNKNOWN;
    inEpReq = 0;
    outEpReq = 0;
    ulIrqStatus = 0;
    epIrqStatus = 0;
    dmaStatus = 0;
    ucStatus = 0;
    bRemoteWakeup = false;
    mEP0State = IDLE;
    pEP0Data = 0;
    mEP0DataRemain = 0;
    mOUTDataSize = 0;
    mUsbEventCount = 0;

    for (int i = 0; i < USBLIB_NUM_EP; i++)
    {
      memset(&mRequests[i], 0, sizeof(usbDevRequest_t));
      mRequests[i].deviceEndptInfo.endpointNumber = i;
    }    


    if (!Cppi41DmaInitialized)
    {
      Cppi41DmaInit(0, epInfo, NUMBER_OF_ENDPOINTS);
      Cppi41DmaInitialized = true;
    }

    USBDevConnect(BASE_ADDRESS);

    /* Enable USB Interrupts. */
    USBEnableOtgIntr(SUBBASE_ADDRESS);
    USBIntEnableControl(BASE_ADDRESS, USB_INTCTRL_RESET |
                                          USB_INTCTRL_DISCONNECT |
                                          USB_INTCTRL_RESUME |
                                          USB_INTCTRL_SUSPEND |
                                          USB_INTCTRL_SOF);
    USBIntEnableEndpoint(BASE_ADDRESS, USB_INTEP_ALL);
  }

  void USBDevice::stop()
  {
    /* Disable USB Interrupts. */
    USBIntDisableControl(BASE_ADDRESS, USB_INTCTRL_RESET |
                                           USB_INTCTRL_DISCONNECT |
                                           USB_INTCTRL_RESUME |
                                           USB_INTCTRL_SUSPEND |
                                           USB_INTCTRL_SOF);
    USBIntDisableEndpoint(BASE_ADDRESS, USB_INTEP_ALL);

    USBDevDisconnect(BASE_ADDRESS);
    UsbPhyOff(PHYBASE_ADDRESS);
  }

  void USBDevice::resetAddress()
  {
    mPendingDeviceAddress = 0;
    mUpdateDeviceAddress = false;
    USBDevAddrSet(BASE_ADDRESS, 0);
    logAssert(0 == USBDevAddrGet(BASE_ADDRESS));
    devState = USB_DEVICE_STATE_RESET;
  }

  bool USBDevice::isIdle()
  {
    return mEP0State == EndpointStates::IDLE;
  }

  bool USBDevice::hasAddress()
  {
    return USBDevAddrGet(BASE_ADDRESS) > 0;
  }

  bool USBDevice::isConfigured()
  {
    return devState == USB_DEVICE_STATE_CONFIGURED;
  }

  void USBDevice::printState()
  {
    // do nothing
  }

} /* namespace od */