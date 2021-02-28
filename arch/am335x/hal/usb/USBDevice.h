#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <ti/am335x/usblib.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>

namespace od
{

    class USBDevice
    {
    public:
        USBDevice();
        virtual ~USBDevice();

        // Device-specific handler for requests on EP0.
        virtual void handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup);

        // Device-specific handler for request on EPn.
        virtual void handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length);

        // Data requested from EP0 has been received.
        virtual void handleEP0Data();

        // Optional event notifications
        virtual void onConfigurationChange();
        virtual void onInterfaceChange();
        virtual void onEnumerationComplete();
        virtual void onDisconnect();
        virtual void onEP0Stalled();

        virtual void init();
        void start();
        void stop();
        bool isIdle();
        bool hasAddress();
        bool isConfigured();

        virtual void printState();

        void handleUSBInterrupt();
        void handleDMAInterrupt();

        void setupRequest(uint32_t epNum, uint8_t *pData, usbTokenType_t tokenType,
                          uint32_t length, usbTransferType_t transferType);

        const uint32_t controllerId = 0;
        usbDevRequest_t *inEpReq;  /* Current endpoint request for in transfer */
        usbDevRequest_t *outEpReq; /* Current endpoint request for out transfer */
        uint32_t lastReqDirection; /* last request is an IN/OUT req */

        uint32_t ulIrqStatus; /* 15 TX FIFO interrupts and device interrupt */
        uint32_t epIrqStatus; /* endpoint interrupt status */
        uint32_t dmaStatus;   /* USBSS IRQ status */

        /**< Address assigned by the host. */
        uint32_t mPendingDeviceAddress = 0;
        bool mUpdateDeviceAddress = false;

        /* Current state of the function gadget */
        usbDeviceState_t devState;

        /* Holds the current device status. */
        unsigned char ucStatus;

        /* This flag indicates whether or not remote wake up signaling is in progress. */
        bool bRemoteWakeup;

        enum EndpointStates
        {
            /* The USB device is waiting on a request from the host controller on */
            /* endpoint zero. */
            IDLE = 0,

            /* The USB device is sending data back to the host due to an IN request. */
            TX,

            /* The USB device is receiving data from the host due to an OUT */
            /* request from the host. */
            RX,

            /* The USB device has completed the IN or OUT request and is now waiting */
            /* for the host to acknowledge the end of the IN/OUT transaction.  This */
            /* is the status phase for a USB control transaction. */
            STATUS,

            /* This endpoint has signaled a stall condition and is waiting for the */
            /* stall to be acknowledged by the host controller. */
            STALL
        };

        /* The current state of endpoint zero. */
        EndpointStates mEP0State;

        /* This is the pointer to the current data being sent out or received */
        /* on endpoint zero. */
        unsigned char *pEP0Data;

        /* This is the number of bytes that remain to be sent from or received */
        /* into the g_sUSBDeviceState.pEP0Data data buffer. */
        uint32_t mEP0DataRemain;

        /* The amount of data being sent/received due to a custom request. */
        uint32_t mOUTDataSize;

    protected:
        uint32_t mUsbEventCount = 0;
        usbDevRequest_t mRequests[USBLIB_NUM_EP];

        // To be provided by device implementation:
        usbDeviceDesc_t *mpDeviceDescriptor = 0;
        static const int MaxConfigurationCount = 8;
        usbConfigDesc_t *mConfigurationTable[MaxConfigurationCount] = {0}; // Each configuration must be terminated by a null descriptor.
        uint32_t mConfigurationCount = 0;
        const char **mStringTable = 0;
        uint32_t mStringCount = 0;

        // May need to be non-cached and aligned.  At least 512 bytes.
        uint8_t *mpEP0InputBuffer = 0;
        uint8_t *mpEP0OutputBuffer = 0;

        usbConfigDesc_t *mpActiveConfiguration = 0;
        static const int MaxInterfaceCount = 8;
        uint32_t mAlternateSettings[MaxInterfaceCount] = {0};

        void stallEP0();

        void handleUSBEvent();
        void handleEP0Event();
        void handleEPEvent();

        void resetAddress();
        void handleSetupPacket(usbSetupPkt_t *pSetup);

        void setupEP0Request(usbDevRequest_t *req);
        void setupEPRequest(usbDevRequest_t *req);
        void cancelPendingRequest(uint32_t epNum);

        void handleEP0TransmitState();
        void onEP0RequestComplete();

        // standard (setup) requests on EP0
        void handleGetStatusRequest(usbSetupPkt_t *pSetup);
        void handleSetAddressRequest(usbSetupPkt_t *pSetup);
        void handleGetDescriptorRequest(usbSetupPkt_t *pSetup);
        void handleSetConfigurationRequest(usbSetupPkt_t *pSetup);
        void handleSetInterfaceRequest(usbSetupPkt_t *pSetup);

        bool setConfiguration(uint32_t configId);
        void resetConfiguration();
        bool setAlternateSetting(uint32_t interfaceId, uint32_t alternateSetting);
        bool configureInterfaceEndpoints(usbInterfaceDesc_t *pInterface, bool resetFlags);

        // Copy descriptor into the given buffer and return the number of bytes copied.
        uint32_t copyDeviceDescriptor(uint8_t *buffer, uint32_t bufferLength);
        uint32_t copyConfigurationDescriptor(uint32_t index, uint8_t *buffer, uint32_t bufferLength);
        uint32_t copyStringDescriptor(uint32_t langId, uint32_t index, uint8_t *buffer, uint32_t bufferLength);

        // Descriptor access
        usbConfigDesc_t *getConfigurationDescriptorByIndex(uint32_t index);
        usbConfigDesc_t *getConfigurationDescriptorByValue(uint32_t value);
        usbInterfaceDesc_t *getInterfaceDescriptor(usbConfigDesc_t *pConfiguration, uint32_t interfaceId, uint32_t alternateSetting);
        usbEndpointDesc_t *getEndpointDescriptor(usbInterfaceDesc_t *pInterface, uint32_t endpointIndex);
    };

} /* namespace od */
