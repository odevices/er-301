#pragma once

#include <hal/usb/USBDevice.h>
#include <hal/usb/usbmsc.h>
#include <hal/concurrency/EventFlags.h>

namespace od
{

    class SerialPort : public USBDevice
    {
    public:
        SerialPort();
        virtual ~SerialPort();

        virtual void init();

        bool canWrite();
        bool startWrite(const char *buffer, size_t length);
        size_t waitForWriteCompletion(int timeout);
        size_t waitForWriteCompletion();

        bool canRead();
        bool startRead(char *buffer, size_t length);
        size_t waitForReadCompletion(int timeout);
        size_t waitForReadCompletion();

        // Device-specific handler for requests on EP0.
        virtual void handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup);
        virtual void handleEP0Data();
        // Device-specific handler for request on EPn.
        virtual void handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length);

        virtual void onEnumerationComplete();
        virtual void onDisconnect();
        virtual void onEP0Stalled();

        virtual void printState();

    private:
        enum CDCStates
        {
            UNCONFIGURED,
            IDLE,
            WAIT_EP0_DATA,
            WAIT_TX_COMPLETE,
            WAIT_RX_COMPLETE,
            WAIT_CLIENT,
        };
        volatile CDCStates mCDCState = UNCONFIGURED;
        uint32_t mPendingRequest = 0;
        uint32_t mBytesReceived = 0;
        uint32_t mBytesWritten = 0;
        char * mUserReadBuffer = 0;

        EventFlags mEvents;
        const uint32_t writeDone = EventFlags::flag00;
        const uint32_t readDone = EventFlags::flag01;
    };
} /* namespace od */
