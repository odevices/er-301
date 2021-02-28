#include <hal/usb/SerialPort.h>
#include <ti/am335x/usb_ids.h>
#include <hal/constants.h>
#include <hal/card.h>
#include <cstring>
#include <cstdio>
#include "usbcdc.h"

//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

/* ***************************************************************************** */
/* Endpoints to use for each of the required endpoints in the driver. */
/* ***************************************************************************** */
#define CONTROL_ENDPOINT (3)
#define DATA_IN_ENDPOINT (1)
#define DATA_OUT_ENDPOINT (2)

#define CTL_IN_EP_MAX_SIZE (16)
#define DATA_OUT_EP_MAX_SIZE (512)
#define DATA_IN_EP_MAX_SIZE (512)

/* ***************************************************************************** */
/* The following are the USB interface numbers for the CDC serial device. */
/* ***************************************************************************** */
#define SERIAL_INTERFACE_CONTROL 0
#define SERIAL_INTERFACE_DATA 1

static uint8_t gReceiveBuffer[CACHE_ALIGNED_SIZE(DATA_OUT_EP_MAX_SIZE)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

namespace od
{

  static usbDeviceDesc_t gDeviceDescriptor = {
      .bLength = sizeof(usbDeviceDesc_t),
      .bDescriptorType = USB_DTYPE_DEVICE,
      .bcdUSB = 0x200,
      .bDeviceClass = 0,
      .bDeviceSubClass = 0,
      .bDeviceProtocol = 0,
      .bMaxPacketSize = MAX_PACKET_SIZE_EP0,
      .idVendor = USB_VID_STELLARIS,
      .idProduct = USB_PID_SERIAL,
      .bcdDevice = 0x200,
      .iManufacturer = 1,
      .iProduct = 2,
      .iSerialNumber = 0,
      .bNumConfigurations = 1,
  };

  typedef struct gConfiguration1
  {
    usbConfigDesc_t configuration;

#define PART_OF_COMPOSITE_DEVICE 0
#if PART_OF_COMPOSITE_DEVICE
    usbInterfaceAssociationDesc_t interfaceAssociation;
#endif

    // CDC Control Interface
    usbInterfaceDesc_t interfaceControl;
    usbCDCFunctionalHeaderDesc_t cdcFunctionalHeader;
    usbCDCFunctionalACMDesc_t cdcFunctionalACM;
    usbCDCFunctionalUnionDesc_t cdcFunctionalUnion;
    // TODO: Call Management descriptor needed?
    usbEndpointDesc_t endpointControl;

    // CDC DataInterface
    usbInterfaceDesc_t interfaceData;
    usbEndpointDesc_t endpointIN;
    usbEndpointDesc_t endpointOUT;

    usbDescHeader_t nullDescriptor;
  } USB_STACK_PACKED mscConfiguration1_t;

  static mscConfiguration1_t gConfiguration1 = {
      .configuration = {
          .bLength = sizeof(usbConfigDesc_t),
          .bDescriptorType = USB_DTYPE_CONFIGURATION,
#if PART_OF_COMPOSITE_DEVCE
          .wTotalLength = sizeof(usbConfigDesc_t) +
                          sizeof(usbInterfaceAssociationDesc_t) +
                          sizeof(usbInterfaceDesc_t) +
                          sizeof(usbCDCFunctionalHeaderDesc_t) +
                          sizeof(usbCDCFunctionalACMDesc_t) +
                          sizeof(usbCDCFunctionalUnionDesc_t) +
                          sizeof(usbEndpointDesc_t) +
                          sizeof(usbInterfaceDesc_t) +
                          2 * sizeof(usbEndpointDesc_t),
#else
          .wTotalLength = sizeof(usbConfigDesc_t) +
                          sizeof(usbInterfaceDesc_t) +
                          sizeof(usbCDCFunctionalHeaderDesc_t) +
                          sizeof(usbCDCFunctionalACMDesc_t) +
                          sizeof(usbCDCFunctionalUnionDesc_t) +
                          sizeof(usbEndpointDesc_t) +
                          sizeof(usbInterfaceDesc_t) +
                          2 * sizeof(usbEndpointDesc_t),
#endif
          .bNumInterface = 2,
          .bConfigurationValue = 1,
          .iConfiguration = 3,
          .bmAttributes = USB_CONF_ATTR_SELF_PWR,
          .bMaxPower = 0,
      },
#if PART_OF_COMPOSITE_DEVICE
      .interfaceAssociation = {
          .bLength = sizeof(usbInterfaceAssociationDesc_t),
          .bDescriptorType = USB_DTYPE_INTERFACE_ASC,
          .bFirstInterface = 0,
          .bInterfaceCount = 2,
          .bFunctionClass = USB_CLASS_CDC,
          .bFunctionSubClass = USB_CDC_SUBCLASS_ABSTRACT_MODEL,
          .bFunctionProtocol = USB_CDC_PROTOCOL_V25TER,
          .iFunction = 0,
      },
#endif
      .interfaceControl = {
          .bLength = sizeof(usbInterfaceDesc_t),
          .bDescriptorType = USB_DTYPE_INTERFACE,
          .bInterfaceNumber = SERIAL_INTERFACE_CONTROL,
          .bAlternateSetting = 0,
          .bNumEndpoints = 1,
          .bInterfaceClass = USB_CLASS_CDC,
          .bInterfaceSubClass = USB_CDC_SUBCLASS_ABSTRACT_MODEL,
          .bInterfaceProtocol = USB_CDC_PROTOCOL_V25TER,
          .iInterface = 0,
      },
      .cdcFunctionalHeader = {
          .bFunctionLength = sizeof(usbCDCFunctionalHeaderDesc_t),
          .bDescriptorType = USB_CDC_CS_INTERFACE,
          .bDescriptorSubType = USB_CDC_FD_SUBTYPE_HEADER,
          .bcdCDC = 0x110,
      },
      .cdcFunctionalACM = {
          .bFunctionLength = sizeof(usbCDCFunctionalACMDesc_t), .bDescriptorType = USB_CDC_CS_INTERFACE, .bDescriptorSubType = USB_CDC_FD_SUBTYPE_ABSTRACT_CTL_MGMT,
          .bmCapabilities = 0, //USB_CDC_ACM_SUPPORTS_LINE_PARAMS | USB_CDC_ACM_SUPPORTS_SEND_BREAK,
      },
      .cdcFunctionalUnion = {
          .bFunctionLength = sizeof(usbCDCFunctionalUnionDesc_t),
          .bDescriptorType = USB_CDC_CS_INTERFACE,
          .bDescriptorSubType = USB_CDC_FD_SUBTYPE_UNION,
          .bMasterInterface = SERIAL_INTERFACE_CONTROL,
          .bSlaveInterface0 = SERIAL_INTERFACE_DATA,
      },
      .endpointControl = {
          .bLength = sizeof(usbEndpointDesc_t),
          .bDescriptorType = USB_DTYPE_ENDPOINT,
          .bEndpointAddress = USB_EP_DESC_IN | CONTROL_ENDPOINT,
          .bmAttributes = USB_EP_ATTR_INT,
          .wMaxPacketSize = CTL_IN_EP_MAX_SIZE,
          .bInterval = 10,
      },
      .interfaceData = {
          .bLength = sizeof(usbInterfaceDesc_t),
          .bDescriptorType = USB_DTYPE_INTERFACE,
          .bInterfaceNumber = SERIAL_INTERFACE_DATA,
          .bAlternateSetting = 0,
          .bNumEndpoints = 2,
          .bInterfaceClass = USB_CLASS_CDC_DATA,
          .bInterfaceSubClass = 0,
          .bInterfaceProtocol = USB_CDC_PROTOCOL_NONE,
          .iInterface = 0,
      },
      .endpointIN = {
          .bLength = sizeof(usbEndpointDesc_t),
          .bDescriptorType = USB_DTYPE_ENDPOINT,
          .bEndpointAddress = USB_EP_DESC_IN | DATA_IN_ENDPOINT,
          .bmAttributes = USB_EP_ATTR_BULK,
          .wMaxPacketSize = DATA_IN_EP_MAX_SIZE,
          .bInterval = 0,
      },
      .endpointOUT = {
          .bLength = sizeof(usbEndpointDesc_t),
          .bDescriptorType = USB_DTYPE_ENDPOINT,
          .bEndpointAddress = USB_EP_DESC_OUT | DATA_OUT_ENDPOINT,
          .bmAttributes = USB_EP_ATTR_BULK,
          .wMaxPacketSize = DATA_OUT_EP_MAX_SIZE,
          .bInterval = 0,
      },
      .nullDescriptor = {
          .bLength = 0,
          .bDescriptorType = 0,
      },
  };

  const char *gStringTable[] = {
      "Orthogonal Devices", // 1: manufacturer
      "ER-301",             // 2: product
      "Serial Port",        // 3: configuration 1
  };

  tLineCoding gLineCoding = {
      .ulRate = 115200,
      .ucStop = 8,
      .ucParity = USB_CDC_PARITY_NONE,
      .ucDatabits = USB_CDC_STOP_BITS_1,
  };

  SerialPort::SerialPort()
  {
    mpDeviceDescriptor = &gDeviceDescriptor;
    mConfigurationTable[0] = &gConfiguration1.configuration;
    mConfigurationCount = 1;
    mStringTable = gStringTable;
    mStringCount = sizeof(gStringTable) / sizeof(const char *);
  }

  SerialPort::~SerialPort()
  {
  }

  void SerialPort::init()
  {
    USBDevice::init();
  }

  bool SerialPort::canRead()
  {
    return isConfigured() && mCDCState == IDLE;
  }

  bool SerialPort::startRead(char *buffer, size_t length)
  {
    if (canRead())
    {
      mBytesReceived = 0;
      mUserReadBuffer = buffer;
      mEvents.clear(readDone);
      if (length > DATA_OUT_EP_MAX_SIZE)
      {
        length = DATA_OUT_EP_MAX_SIZE;
      }
      setupRequest(DATA_OUT_ENDPOINT,
                   gReceiveBuffer,
                   USB_TOKEN_TYPE_OUT,
                   length,
                   USB_TRANSFER_TYPE_BULK);
      mCDCState = WAIT_RX_COMPLETE;
      return true;
    }
    else
    {
      return false;
    }
  }

  size_t SerialPort::waitForReadCompletion()
  {
    if (mEvents.waitForAny(readDone) & readDone)
    {
      size_t j = 0;
      if (mBytesReceived > 0 && mUserReadBuffer)
      {
        //memcpy(mUserReadBuffer, gReceiveBuffer, mBytesReceived);

        for (uint32_t i = 0; i < mBytesReceived; i++)
        {
          if (gReceiveBuffer[i] == '\r')
          {
            mUserReadBuffer[j++] = '\n';
          }
          mUserReadBuffer[j++] = gReceiveBuffer[i];
        }
      }
      mUserReadBuffer = 0;
      return j;
    }
    else
    {
      return 0;
    }
  }

  size_t SerialPort::waitForReadCompletion(int timeout)
  {
    if (mEvents.waitForAny(readDone, timeout) & readDone)
    {
      size_t j = 0;
      if (mBytesReceived > 0 && mUserReadBuffer)
      {
        //memcpy(mUserReadBuffer, gReceiveBuffer, mBytesReceived);

        for (uint32_t i = 0; i < mBytesReceived; i++)
        {
          if (gReceiveBuffer[i] == '\r')
          {
            mUserReadBuffer[j++] = '\n';
          }
          mUserReadBuffer[j++] = gReceiveBuffer[i];
        }
      }
      mUserReadBuffer = 0;
      return j;
    }
    else
    {
      return 0;
    }
  }

  bool SerialPort::canWrite()
  {
    return isConfigured() && mCDCState == IDLE;
  }

  bool SerialPort::startWrite(const char *buffer, size_t length)
  {
    if (canWrite())
    {
      mBytesWritten = 0;
      mEvents.clear(writeDone);
      setupRequest(DATA_IN_ENDPOINT,
                   (uint8_t *)buffer,
                   USB_TOKEN_TYPE_IN,
                   length,
                   USB_TRANSFER_TYPE_BULK);
      mCDCState = WAIT_TX_COMPLETE;
      return true;
    }
    else
    {
      return false;
    }
  }

  size_t SerialPort::waitForWriteCompletion()
  {
    if (mEvents.waitForAny(writeDone) & writeDone)
      return mBytesWritten;
    else
      return 0;
  }

  size_t SerialPort::waitForWriteCompletion(int timeout)
  {
    if (mEvents.waitForAny(writeDone, timeout) & writeDone)
      return mBytesWritten;
    else
      return 0;
  }

  void SerialPort::handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length)
  {
    if (epNo == CONTROL_ENDPOINT)
    {
      // do nothing
      logDebug(1, "EP%d evt=%d len=%d", epNo, endptEvent, length);
    }
    else
    {
      logDebug(12, "EP%d evt=%d len=%d", epNo, endptEvent, length);
      switch (endptEvent)
      {
      case USB_GENERIC_EVENT_DATA_IN_COMPLETE:
        // TX
        mCDCState = IDLE;
        mBytesWritten = length;
        mEvents.post(writeDone);
        break;
      case USB_GENERIC_EVENT_DATA_OUT_COMPLETE:
        // RX
        mCDCState = IDLE;
        mBytesReceived = length;
        mEvents.post(readDone);
        break;
      default:
        break;
      }
    }
  }

  void SerialPort::handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup)
  {
    switch (pSetup->bRequest)
    {
    case USB_CDC_SEND_ENCAPSULATED_COMMAND:
    {
      /* This implementation makes use of no communication protocol so */
      /* this request is meaningless.  We stall endpoint 0 if we receive */
      /* it. */
      logWarn("USB_CDC_SEND_ENCAPSULATED_COMMAND not expected, stall.");
      stallEP0();
      break;
    }

    case USB_CDC_GET_ENCAPSULATED_RESPONSE:
    {
      /* This implementation makes use of no communication protocol so */
      /* this request is meaningless.  We stall endpoint 0 if we receive */
      /* it. */
      logWarn("USB_CDC_GET_ENCAPSULATED_RESPONSE not expected, stall.");
      stallEP0();
      break;
    }

    case USB_CDC_SET_COMM_FEATURE:
    case USB_CDC_GET_COMM_FEATURE:
    case USB_CDC_CLEAR_COMM_FEATURE:
      /* This request is apparently required by an ACM device but does */
      /* not appear relevant to a virtual COM port and is never used by */
      /* Windows (or, at least, is not seen when using Hyperterminal or */
      /* TeraTerm via a Windows virtual COM port).  We stall endpoint 0 */
      /* to indicate that we do not support the request. */
      logWarn("XXX_COMM_FEATURE not expected (%d), stall.", pSetup->bRequest);
      stallEP0();
      break;

    /* Set the serial communication parameters. */
    case USB_CDC_SET_LINE_CODING:
#if 0
      logWarn("USB_CDC_SET_LINE_CODING (ignored, stalling)");
      stallEP0();
#else
      mPendingRequest = USB_CDC_SET_LINE_CODING;
      mCDCState = WAIT_EP0_DATA;
      setupRequest(0U,
                   (uint8_t *)&gLineCoding,
                   USB_TOKEN_TYPE_OUT,
                   sizeof(tLineCoding),
                   USB_TRANSFER_TYPE_CONTROL);
#endif
      break;

    /* Return the serial communication parameters. */
    case USB_CDC_GET_LINE_CODING:
      logDebug(1, "USB_CDC_GET_LINE_CODING");
      setupRequest(0U,
                   (uint8_t *)&gLineCoding,
                   USB_TOKEN_TYPE_IN,
                   sizeof(tLineCoding),
                   USB_TRANSFER_TYPE_CONTROL);
      break;

    case USB_CDC_SET_CONTROL_LINE_STATE:
      logWarn("USB_CDC_SET_CONTROL_LINE_STATE ignored");
      stallEP0();
      break;
    case USB_CDC_SEND_BREAK:
      logWarn("USB_CDC_SEND_BREAK ignored");
      stallEP0();
      break;

    /* These are valid CDC requests but not ones that an ACM device should */
    /* receive. */
    case USB_CDC_SET_AUX_LINE_STATE:
    case USB_CDC_SET_HOOK_STATE:
    case USB_CDC_PULSE_SETUP:
    case USB_CDC_SEND_PULSE:
    case USB_CDC_SET_PULSE_TIME:
    case USB_CDC_RING_AUX_JACK:
    case USB_CDC_SET_RINGER_PARMS:
    case USB_CDC_GET_RINGER_PARMS:
    case USB_CDC_SET_OPERATION_PARMS:
    case USB_CDC_GET_OPERATION_PARMS:
    case USB_CDC_SET_LINE_PARMS:
    case USB_CDC_GET_LINE_PARMS:
    case USB_CDC_DIAL_DIGITS:
    case USB_CDC_SET_UNIT_PARAMETER:
    case USB_CDC_GET_UNIT_PARAMETER:
    case USB_CDC_CLEAR_UNIT_PARAMETER:
    case USB_CDC_GET_PROFILE:
    case USB_CDC_SET_ETHERNET_MULTICAST_FILTERS:
    case USB_CDC_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
    case USB_CDC_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
    case USB_CDC_SET_ETHERNET_PACKET_FILTER:
    case USB_CDC_GET_ETHERNET_STATISTIC:
    case USB_CDC_SET_ATM_DATA_FORMAT:
    case USB_CDC_GET_ATM_DEVICE_STATISTICS:
    case USB_CDC_SET_ATM_DEFAULT_VC:
    case USB_CDC_GET_ATM_VC_STATISTICS:
      logWarn("recognized but unhandled request (%d), stall.", pSetup->bRequest);
      stallEP0();
      break;

    default:
      // This request was not recognized so stall.
      logWarn("unrecognized request (%d), stall.", pSetup->bRequest);
      stallEP0();
      break;
    }
  }

  void SerialPort::handleEP0Data()
  {
    if (mCDCState != WAIT_EP0_DATA)
    {
      logWarn("not waiting for data");
      mCDCState = IDLE;
      return;
    }

    if (mOUTDataSize == 0)
    {
      logWarn("no data received");
      mCDCState = IDLE;
      return;
    }

    switch (mPendingRequest)
    {
    case USB_CDC_SET_LINE_CODING:
      if (mOUTDataSize != sizeof(tLineCoding))
      {
        logWarn("USB_CDC_SET_LINE_CODING wrong data size (%d bytes), stall.", mOUTDataSize);
        stallEP0();
        return;
      }
      else
      {
        logInfo("Received line coding (%d, %d, %d, %d).",
                gLineCoding.ulRate,
                gLineCoding.ucStop,
                gLineCoding.ucParity,
                gLineCoding.ucDatabits);
      }
      mCDCState = IDLE;

      break;
    default:
      logWarn("Unhandled request (%d)", mPendingRequest);
      break;
    }
  }

  void SerialPort::onEnumerationComplete()
  {
    mCDCState = IDLE;
  }

  void SerialPort::onDisconnect()
  {
    mCDCState = UNCONFIGURED;
  }

  void SerialPort::onEP0Stalled()
  {
    switch (mCDCState)
    {
    case WAIT_EP0_DATA:
      mCDCState = IDLE;
    default:
      break;
    }
  }

  void SerialPort::printState()
  {
    switch (mCDCState)
    {
    case UNCONFIGURED:
      logInfo("UNCONFIGURED");
      break;
    case IDLE:
      logInfo("IDLE");
      break;
    case WAIT_EP0_DATA:
      logInfo("WAIT_EP0_DATA");
      break;
    case WAIT_TX_COMPLETE:
      logInfo("WAIT_TX_COMPLETE");
      break;
    case WAIT_RX_COMPLETE:
      logInfo("WAIT_RX_COMPLETE");
      break;
    case WAIT_CLIENT:
      logInfo("WAIT_CLIENT");
      break;
    }
  }

} /* namespace od */