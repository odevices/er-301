#pragma once

#include <hal/usb/USBDevice.h>
#include <hal/usb/usbmsc.h>

namespace od
{

    class MassStorage : public USBDevice
    {
    public:
        MassStorage();
        virtual ~MassStorage();

        virtual void init();

        // Device-specific handler for requests on EP0.
        virtual void handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup);
        // Device-specific handler for request on EPn.
        virtual void handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length);

        virtual void onDisconnect();

    private:
        void receiveCBW();
        void sendBlocks();
        void receiveBlocks();
        void sendCSW();

        // SCSI Command Handlers
        bool scsiHandleCommand();
        void scsiInquiry();
        void scsiReadFormatCapacities();
        void scsiReadCapacity();
        void scsiRequestSense();
        void scsiTestUnitReady();
        void scsiRead10(tMSCCBW *pCBW);
        void scsiWrite10(tMSCCBW *pCBW);
        void scsiModeSense6(tMSCCBW *pCBW);
        void scsiStartStopUnit(tMSCCBW *pCBW);
        void scsiUnsupportedCommand(tMSCCBW *pCBW);

        enum SCSIStates
        {
            /* No command in progress */
            IDLE = 0,
            /* Sending and reading logical blocks. */
            SEND_BLOCKS,
            /* Receiving and writing logical blocks. */
            RECEIVE_BLOCKS,
            /* Send the status once the previous command is complete. */
            SEND_STATUS,
            /* Status was prepared to be sent and now waiting for it be transmitted. */
            SENT_STATUS
        };
        SCSIStates mSCSIState = IDLE;
        uint32_t mPendingTransmits = 0;

        bool mPingPong = false;
        uint32_t mPingSize = 0;
        uint32_t mPongSize = 0;
        uint32_t mReadAhead = 0;
        uint32_t mReadAheadGrowth = 0;

        uint32_t mBlocksToRead = 0;
        uint32_t mBlocksToSend = 0;

        uint32_t mBlocksToWrite = 0;
        uint32_t mBlocksToReceive = 0;
        uint32_t mCurrentLBA = 0;
        uint32_t mCurrentDrive = 1;
        bool mAllowMediumRemoval = false;
        bool mInhibitCardConnect = false;

        // Sense Data
        uint8_t mErrorCode;
        uint8_t mSenseKey;
        uint16_t mAddSenseCode;

        bool readPingPong();
        bool sendPingPong();
        void resetSCSIState();
        bool isCardConnected();
    };

} /* namespace od */
