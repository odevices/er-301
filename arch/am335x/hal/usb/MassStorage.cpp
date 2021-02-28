#include <hal/usb/MassStorage.h>
#include <ti/am335x/usb_ids.h>
#include <hal/constants.h>
#include <hal/card.h>
#include <hal/events.h>
#include <cstring>
#include <cstdio>
#include "usbmsc.h"

//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 0
#include <hal/log.h>

#define BUF_ALIGN_SIZE (8)
/* These macros defines the sizes of USB transfers for Data and command */

/** \brief USB packet length */
#define USB_PACKET_LENGTH 512
#define DEVICE_BLOCK_SIZE 512
#define BLOCK_READ_AHEAD 16

/** \brief USB transfer size for transfer of data */
#define MAX_TRANSFER_SIZE USB_PACKET_LENGTH

/** \brief USB transfer size for transfer fo command */
#define COMMAND_BUFFER_SIZE 64

/** \brief These are fields that are used by the USB descriptors
 *         for the Mass Storage Class.
 */
#define USB_MSC_SUBCLASS_SCSI 0x6
#define USB_MSC_PROTO_BULKONLY 0x50

/** Endpoints to use for each of the endpoints in the driver */

/** \brief Data In endpoint */
#define DATA_IN_ENDPOINT 1

/** \brief Data out endpoint */
#define DATA_OUT_ENDPOINT 2

/** \brief Max packet size for bulk in endpoint */
#define DATA_IN_EP_MAX_SIZE MAX_TRANSFER_SIZE

/** \brief Max packet size for bulk out endpoint */
#define DATA_OUT_EP_MAX_SIZE MAX_TRANSFER_SIZE

/* number of LUN */
#define MSC_NUM_DRIVES (1U)

namespace od
{

    usbDeviceDesc_t mscDeviceDescriptor = {
        /* Descriptor Length.*/
        .bLength = sizeof(usbDeviceDesc_t),
        /* Descriptor Type.*/
        .bDescriptorType = USB_DTYPE_DEVICE,
        /* USB Specification version.*/
        .bcdUSB = 0x200,
        /* Each interface specifies its own class information .*/
        .bDeviceClass = 0,
        /* Each interface specifies its own Subclass information .*/
        .bDeviceSubClass = 0,
        /* Device Protocol .*/
        .bDeviceProtocol = 0,
        /* Maximum packet size for endpoint zero.*/
        .bMaxPacketSize = MAX_PACKET_SIZE_EP0,
        /* Vendor ID.*/
        .idVendor = USB_VID_STELLARIS,
        /* Product ID.*/
        .idProduct = USB_PID_MSC,
        /* device release number.*/
        .bcdDevice = 0x200,
        /* manufacturer string descriptor index.*/
        .iManufacturer = 1,
        /* product string descriptor index.*/
        .iProduct = 2,
        /* serial number string descriptor index.*/
        .iSerialNumber = 0,
        /* possible configurations .*/
        .bNumConfigurations = 1,
    };

    typedef struct mscConfiguration1
    {
        usbConfigDesc_t configuration;
        usbInterfaceDesc_t interface;
        usbEndpointDesc_t endpointIN;
        usbEndpointDesc_t endpointOUT;
        usbDescHeader_t nullDescriptor;
    } USB_STACK_PACKED mscConfiguration1_t;

    static mscConfiguration1_t mscConfiguration1 = {
        .configuration = {
            /* Descriptor Length.*/
            .bLength = sizeof(usbConfigDesc_t),
            /* Descriptor Type.*/
            .bDescriptorType = USB_DTYPE_CONFIGURATION,
            /* total length of data for this configuration.*/
            .wTotalLength = sizeof(usbConfigDesc_t) +
                            sizeof(usbInterfaceDesc_t) +
                            2 * sizeof(usbEndpointDesc_t),
            /* Number of interfaces supported by this configuration  .*/
            .bNumInterface = 1,
            /* Value to be used for selecting this configuration.*/
            .bConfigurationValue = 1,
            /* configuration string descriptor index .*/
            .iConfiguration = 3,
            /* Configuration characteristics.*/
            .bmAttributes = USB_CONF_ATTR_SELF_PWR,
            /*max current in 2 mA units .*/
            .bMaxPower = 0,
        },
        .interface = {
            /* Descriptor Length.*/
            .bLength = sizeof(usbInterfaceDesc_t),
            /* Descriptor Type.*/
            .bDescriptorType = USB_DTYPE_INTERFACE,
            /* number of this interface.*/
            .bInterfaceNumber = 0,
            /* value used to select the alternate setting for this interface .*/
            .bAlternateSetting = 0,
            /* number of endpoints used by this interface .*/
            .bNumEndpoints = 2,
            /* Class that this interface implements.*/
            .bInterfaceClass = USB_CLASS_MASS_STORAGE,
            /* Sub-Class that this interface implements.*/
            .bInterfaceSubClass = USB_MSC_SUBCLASS_SCSI,
            /* Protocol Used .*/
            .bInterfaceProtocol = USB_MSC_PROTO_BULKONLY,
            /*  interface string descriptor index .*/
            .iInterface = 0,
        },
        .endpointIN = {
            /* Descriptor Length.*/
            .bLength = sizeof(usbEndpointDesc_t),
            /* ENDPOINT Descriptor Type .*/
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            /* IN or OUT endpoint address.*/
            .bEndpointAddress = USB_EP_DESC_IN | DATA_IN_ENDPOINT,
            /* Endpoint type.*/
            .bmAttributes = USB_EP_ATTR_BULK,
            /* Maximum packet size for this endpoint.*/
            .wMaxPacketSize = DATA_IN_EP_MAX_SIZE,
            /* polling interval value in farmes.*/
            .bInterval = 0,
        },
        .endpointOUT = {
            /* Descriptor Length.*/
            .bLength = sizeof(usbEndpointDesc_t),
            /* ENDPOINT Descriptor Type .*/
            .bDescriptorType = USB_DTYPE_ENDPOINT,
            /* IN or OUT endpoint address.*/
            .bEndpointAddress = USB_EP_DESC_OUT | DATA_OUT_ENDPOINT,
            /* Endpoint type.*/
            .bmAttributes = USB_EP_ATTR_BULK,
            /* Maximum packet size for this endpoint.*/
            .wMaxPacketSize = DATA_IN_EP_MAX_SIZE,
            /* polling interval value in farmes.*/
            .bInterval = 0,
        },
        .nullDescriptor = {
            /* Descriptor Length.*/
            .bLength = 0,
            /* Descriptor Type.*/
            .bDescriptorType = 0,
        },
    };

    const char *mscStringTable[] = {
        "Orthogonal Devices", // 1: manufacturer
        "ER-301",             // 2: product
        "Mass Storage",       // 3: configuration 1
    };

    // For receiving the CBW...
    static uint8_t gCommandBuffer[CACHE_ALIGNED_SIZE(MAX_TRANSFER_SIZE)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

    // For transmission of blocks...
    static uint8_t gPing[CACHE_ALIGNED_SIZE(DEVICE_BLOCK_SIZE * BLOCK_READ_AHEAD)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
    static uint8_t gPong[CACHE_ALIGNED_SIZE(DEVICE_BLOCK_SIZE * BLOCK_READ_AHEAD)] __attribute__((aligned(CACHELINE_SIZE_MAX)));

    // For sending the CSW...
    static tMSCCSW gCSW __attribute__((aligned(CACHELINE_SIZE_MAX)));

    MassStorage::MassStorage()
    {
        mpDeviceDescriptor = &mscDeviceDescriptor;
        mConfigurationTable[0] = &mscConfiguration1.configuration;
        mConfigurationCount = 1;
        mStringTable = mscStringTable;
        mStringCount = sizeof(mscStringTable) / sizeof(const char *);
    }

    MassStorage::~MassStorage()
    {
    }

    void MassStorage::init()
    {
        USBDevice::init();
        resetSCSIState();
    }

    void MassStorage::resetSCSIState()
    {
        cancelPendingRequest(DATA_IN_ENDPOINT);
        cancelPendingRequest(DATA_OUT_ENDPOINT);
        mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        mSenseKey = SCSI_RS_KEY_NOT_READY;
        mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;
        memset(&gCSW, 0, sizeof(gCSW));
        gCSW.dCSWSignature = CSW_SIGNATURE;
        mSCSIState = IDLE;
        mPendingTransmits = 0;
    }

    void MassStorage::receiveCBW()
    {
        /* Setup request for next OUT transfer */
        setupRequest(DATA_OUT_ENDPOINT,
                     gCommandBuffer,
                     USB_TOKEN_TYPE_OUT,
                     MAX_TRANSFER_SIZE,
                     USB_TRANSFER_TYPE_BULK);
        mSCSIState = IDLE;
    }

    void MassStorage::handleEndpoints(usbGenericEvent_t endptEvent, uint32_t epNo, uint32_t length)
    {
        logDebug(30, "Pending transmits: %d", mPendingTransmits);

        switch (endptEvent)
        {
        case USB_GENERIC_EVENT_DATA_IN_COMPLETE:
            if (mPendingTransmits > 0)
            {
                mPendingTransmits--;
            }
            else
            {
                logWarn("IN COMPLETE event but no pending transmits. mSCSIState=%d", mSCSIState);
            }
            switch (mSCSIState)
            { 
            case SEND_BLOCKS:
                logDebug(30, "EP%d, send blocks", epNo);
                sendBlocks();
                break;
            case SEND_STATUS:
                logDebug(30, "EP%d, send status", epNo);
                sendCSW();
                break;
            case SENT_STATUS:
                logDebug(30, "EP%d, sent status", epNo);
                receiveCBW();
                break;
            default:
                break;
            }
            break;
        case USB_GENERIC_EVENT_DATA_OUT_COMPLETE:
            switch (mSCSIState)
            {
            case RECEIVE_BLOCKS:
                // Receiving from host and writing blocks to the storage device.
                logDebug(30, "EP%d, receive blocks", epNo);
                receiveBlocks();
                break;
            case IDLE:
                // We have received a new command...
                logDebug(30, "EP%d OUT event, idle", epNo);
                if (!scsiHandleCommand())
                {
                    /*
                    * Just return to the idle state since we are now out of
                    * sync with the host.  This should not happen, but this
                    * should allow the device to synchronize with the host
                    * controller.
                    */
                    logWarn("Out of sync with host.");
                    mSCSIState = IDLE;
                }

                break;
            /*
            * These cases should not occur as the being in the IDLE state due
            * to an OUT interrupt is invalid.
            */
            default:
                logWarn("Did not expect this EPn OUT event.");
                break;
            }

            break;

        /* Nothing to be done for these cases */
        case USB_GENERIC_EVENT_SOF_INTERRUPT:
            break;
        case USB_GENERIC_EVENT_NAK_TIMEOUT:
            break;
        case USB_GENERIC_EVENT_DATA_IN_NAK:
            break;
        case USB_GENERIC_EVENT_DATA_IN_PARTIAL:
            break;
        case USB_GENERIC_EVENT_DATA_OUT_NAK:
            break;
        case USB_GENERIC_EVENT_DATA_OUT_PARTIAL:
            break;
        case USB_GENERIC_EVENT_DATA_TOGGLE_ERROR:
            break;
        case USB_GENERIC_EVENT_TIMEOUT_ERROR:
            break;
        case USB_GENERIC_EVENT_BUS_ERROR:
            break;
        default:
            break;
        }
    }

    void MassStorage::handleNonstandardRequest(usbEndpt0event_t endptEvent, usbSetupPkt_t *pSetup)
    {
        switch (pSetup->bRequest)
        {
        case USBREQ_GET_MAX_LUN:
            // A Set Report request is received from the host when it sends an
            // Output report via endpoint 0.
            logDebug(30, "USBREQ_GET_MAX_LUN");
            mpEP0OutputBuffer[0] = MSC_NUM_DRIVES - 1;
            setupRequest(0U,
                         mpEP0OutputBuffer,
                         USB_TOKEN_TYPE_IN,
                         1U,
                         USB_TRANSFER_TYPE_CONTROL);
            break;

        case USBREQ_MASS_STORAGE_RESET:
            logDebug(1, "USBREQ_MASS_STORAGE_RESET");
            resetSCSIState();
            break;
        default:
            // This request was not recognized so stall.
            logDebug(1, "unrecognized request (%d), stall.", pSetup->bRequest);
            stallEP0();
            break;
        }

        receiveCBW();
    }

    void MassStorage::receiveBlocks()
    {
        Card_writeBlocks(mCurrentDrive, gPing, mCurrentLBA, 1);

        mBlocksToReceive--;
        mCurrentLBA++;

        if (mBlocksToReceive == 0)
        {
            // No more data coming.
            gCSW.bCSWStatus = 0;
            gCSW.dCSWDataResidue = 0;
            sendCSW();
        }
        else
        {
            // Prepare for next transmission.
            setupRequest(DATA_OUT_ENDPOINT,
                         gPing,
                         USB_TOKEN_TYPE_OUT,
                         MAX_TRANSFER_SIZE,
                         USB_TRANSFER_TYPE_BULK);
        }
    }

    void MassStorage::sendCSW()
    {
        logDebug(30, "tag=0x%x residue=%d status=%d", gCSW.dCSWTag, gCSW.dCSWDataResidue, gCSW.bCSWStatus);
        setupRequest(DATA_IN_ENDPOINT,
                     (uint8_t *)&gCSW,
                     USB_TOKEN_TYPE_IN,
                     sizeof(tMSCCSW),
                     USB_TRANSFER_TYPE_BULK);
        mPendingTransmits++;
        mSCSIState = SENT_STATUS;
    }

#if BUILDOPT_DEBUG_LEVEL
    static const char *describeCBW(tMSCCBW *pCBW)
    {
        static char buffer[64];
        snprintf(buffer, sizeof(buffer), "tag=0x%lx lun=%d tx=%lu",
                 pCBW->dCBWTag, pCBW->bCBWLUN, pCBW->dCBWDataTransferLength);
        return buffer;
    }
#endif

    bool MassStorage::scsiHandleCommand()
    {
        // New command has been received.
        tMSCCBW *pCBW = (tMSCCBW *)gCommandBuffer;

        if (pCBW->dCBWSignature != CBW_SIGNATURE)
        {
            logError("Invalid CBW signature.");
            return false;
        }

        // Prepare status
        gCSW.dCSWTag = pCBW->dCBWTag;
        gCSW.dCSWDataResidue = 0;
        gCSW.bCSWStatus = 0;

        // Save the transfer length because it may be overwritten by some calls.
        uint32_t transferLength = pCBW->dCBWDataTransferLength;
        // mCurrentDrive = pCBW->bCBWLUN;

        // Parse the command
        switch (pCBW->CBWCB[0])
        {
        case SCSI_INQUIRY_CMD:
            // Windows host sends INQUIRY_CMD then READ_CAPACITIES.
            // Linux host sends INQUIRY_CMD then TEST_UNIT_READY then READ_CAPACITY.
            logDebug(30, "[Inquiry] %s", describeCBW(pCBW));
            scsiInquiry();
            break;
        case SCSI_TEST_UNIT_READY:
            logDebug(30, "[Test Unit Ready] %s", describeCBW(pCBW));
            scsiTestUnitReady();
            break;
        case SCSI_READ_FORMAT_CAPACITIES:
            logDebug(30, "[Read Format Capacities] %s", describeCBW(pCBW));
            scsiReadFormatCapacities();
            break;
        case SCSI_READ_CAPACITY:
            logDebug(30, "[Read Capacity] %s", describeCBW(pCBW));
            scsiReadCapacity();
            break;
        case SCSI_REQUEST_SENSE:
            logDebug(30, "[Request Sense] %s", describeCBW(pCBW));
            scsiRequestSense();
            break;
        case SCSI_READ_10:
            logDebug(30, "[Read 10] %s", describeCBW(pCBW));
            scsiRead10(pCBW);
            break;
        case SCSI_WRITE_10:
            logDebug(30, "[Write 10] %s", describeCBW(pCBW));
            scsiWrite10(pCBW);
            break;
        case SCSI_MODE_SENSE_6:
            logDebug(30, "[Mode Sense 6] %s", describeCBW(pCBW));
            scsiModeSense6(pCBW);
            break;
        case SCSI_START_STOP_UNIT:
            logDebug(30, "[Start Stop Unit] %s", describeCBW(pCBW));
            scsiStartStopUnit(pCBW);
            break;
        case SCSI_VERIFY_10:
            logDebug(30, "[Verify 10] %s", describeCBW(pCBW));
            // Always verify
            gCSW.dCSWDataResidue = pCBW->dCBWDataTransferLength;
            mSCSIState = SEND_STATUS;
            break;
        case SCSI_REASSIGN_BLOCKS:
            logDebug(30, "[Reassign Blocks] %s", describeCBW(pCBW));
            scsiUnsupportedCommand(pCBW);
        case SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL:
            if (pCBW->CBWCB[4] && 0x3)
            {
                logDebug(30, "[Prevent Medium Removal] %s", describeCBW(pCBW));
                mAllowMediumRemoval = false;
            }
            else
            {
                logDebug(30, "[Allow Medium Removal] %s", describeCBW(pCBW));
                mAllowMediumRemoval = true;
            }
            break;
        default:
            logDebug(30, "[Unknown 0x%x] tag=0x%x lun=%d", pCBW->CBWCB[0], pCBW->dCBWTag, pCBW->bCBWLUN);
            scsiUnsupportedCommand(pCBW);
            break;
        }

        // If there is no data then send out the current status.
        if (transferLength == 0 || mSCSIState == SEND_STATUS)
        {
            sendCSW();
        }

        return true;
    }

    void MassStorage::scsiTestUnitReady()
    {
        gCSW.dCSWDataResidue = 0;
        // Assume not ready.
        gCSW.bCSWStatus = 1;

        if (Card_isPresent(mCurrentDrive))
        {

            if (mInhibitCardConnect)
            {
                logDebug(3, "present + inhibit connect");
                if (Card_getMode(mCurrentDrive) == CARD_MODE_FATFS)
                {
                    // Allow reconnect if we detect that the card is mounted locally.
                    mInhibitCardConnect = false;
                }
            }
            else
            {
                logDebug(3, "present + allow connect");
                if (Card_getMode(mCurrentDrive) == CARD_MODE_NOT_CONNECTED)
                {
                    Card_connect(mCurrentDrive, CARD_MODE_RAW);
                }
            }

            if (isCardConnected())
            {
                // Card is present and connected.
                gCSW.bCSWStatus = 0;
            }
        }
        else
        {
            logDebug(3, "not present");
            if (mInhibitCardConnect)
            {
                // Allow reconnect if we detect that no card is present.
                mInhibitCardConnect = false;
            }
        }
    }

    void MassStorage::scsiStartStopUnit(tMSCCBW *pCBW)
    {
        bool loadeject = (pCBW->CBWCB[4] & (0x1 << 1)) > 0;
        bool start = (pCBW->CBWCB[4] & 0x1) > 0;

        gCSW.dCSWDataResidue = 0;

        if (loadeject)
        {
            if (start)
            {
                logDebug(3, "start requested");
                // Allow this driver to reconnect to the card during [Test Unit Ready] command.
                mInhibitCardConnect = false;

                if (!isCardConnected())
                {
                    Card_connect(mCurrentDrive, CARD_MODE_RAW);
                }
                if (isCardConnected())
                {
                    gCSW.bCSWStatus = 0;
                }
                else
                {
                    gCSW.bCSWStatus = 1;
                }
            }
            else
            {
                logDebug(3, "stop requested");
                if (isCardConnected())
                {
                    Card_disconnect(mCurrentDrive);
                    // Inhibit this driver from reconnecting during [Test Unit Ready] command.
                    mInhibitCardConnect = true;
                }
                if (isCardConnected())
                {
                    gCSW.bCSWStatus = 1;
                }
                else
                {
                    gCSW.bCSWStatus = 0;
                }
            }
        }
    }

    void MassStorage::scsiUnsupportedCommand(tMSCCBW *pCBW)
    {
        mSCSIState = IDLE;
        gCSW.bCSWStatus = 1;
        gCSW.dCSWDataResidue = pCBW->dCBWDataTransferLength;

        // If there is data then there is more work to do.
        if (pCBW->dCBWDataTransferLength != 0)
        {
            if (pCBW->bmCBWFlags & CBWFLAGS_DIR_IN)
            {
                // TODO : Stall the IN endpoint
            }
            else
            {
                // TODO : Stall the OUT endpoint
            }
            mSCSIState = SEND_STATUS;
        }

        // Set the sense codes.
        mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
        mSenseKey = SCSI_RS_KEY_ILGL_RQST;
        mAddSenseCode = SCSI_ASC_PARAMETER_VALUE_INVALID;
    }

    void MassStorage::scsiInquiry()
    {
        const uint8_t Vendor[8 + 1] = "O|D     ";
        const uint8_t Product[16 + 1] = "ER-301          ";
        const uint8_t Version[4 + 1] = "0.00";

        // Direct Access device, Removable storage and SCSI 1 responses.
        gCommandBuffer[0] = SCSI_INQ_PDT_SBC;
        gCommandBuffer[1] = SCSI_INQ_RMB;
        gCommandBuffer[2] = 0x00;
        gCommandBuffer[3] = 0x00;

        // Additional Length is fixed at 31 bytes.
        gCommandBuffer[4] = 31;
        gCommandBuffer[5] = 0x00;
        gCommandBuffer[6] = 0x00;
        gCommandBuffer[7] = 0x00;

        // Copy the Vendor string.
        for (int i = 0; i < 8; i++)
        {
            gCommandBuffer[i + 8] = Vendor[i];
        }

        // Copy the Product string.
        for (int i = 0; i < 16; i++)
        {
            gCommandBuffer[i + 16] = Product[i];
        }

        // Copy the Version string.
        for (int i = 0; i < 4; i++)
        {
            gCommandBuffer[i + 32] = Version[i];
        }

        setupRequest(DATA_IN_ENDPOINT,
                     gCommandBuffer,
                     USB_TOKEN_TYPE_IN,
                     36,
                     USB_TRANSFER_TYPE_BULK);
        mPendingTransmits++;

        // Prepare for status transmission.
        gCSW.bCSWStatus = 0;
        gCSW.dCSWDataResidue = 0;
        mSCSIState = SEND_STATUS;
    }

    void MassStorage::scsiReadFormatCapacities()
    {
        if (isCardConnected())
        {
            uint32_t sizeInBlocks = Card_sizeInBlocks(mCurrentDrive);

            // Current/Maximum Capacity Descriptor

            gCommandBuffer[0] = 0x00; // reserved
            gCommandBuffer[1] = 0x00; // reserved
            gCommandBuffer[2] = 0x00; // reserved
            gCommandBuffer[3] = 0x08; // Capacity List Length

            // Fill in the number of blocks (big-endian).
            gCommandBuffer[4] = sizeInBlocks >> 24;
            gCommandBuffer[5] = 0xff & (sizeInBlocks >> 16);
            gCommandBuffer[6] = 0xff & (sizeInBlocks >> 8);
            gCommandBuffer[7] = 0xff & (sizeInBlocks);

            // Descriptor type: Formattable Media
            gCommandBuffer[8] = 0x02;

            // Fill in the block size, which is fixed at DEVICE_BLOCK_SIZE.
            gCommandBuffer[9] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
            gCommandBuffer[10] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
            gCommandBuffer[11] = 0xff & DEVICE_BLOCK_SIZE;

            setupRequest(DATA_IN_ENDPOINT,
                         gCommandBuffer,
                         USB_TOKEN_TYPE_IN,
                         12,
                         USB_TRANSFER_TYPE_BULK);
            mPendingTransmits++;

            gCSW.bCSWStatus = 0;
            gCSW.dCSWDataResidue = 0;
        }
        else
        {
            gCSW.bCSWStatus = 1;
            gCSW.dCSWDataResidue = 0;

            // Stall the IN endpoint
            // USBDevEndpointStall(g_USBInstance[ulIndex].uiBaseAddr, mINEndpoint, USB_EP_DEV_IN);

            // Mark the sense code as valid and indicate that these is no media present.
            mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            mSenseKey = SCSI_RS_KEY_NOT_READY;
            mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;
        }

        mSCSIState = SEND_STATUS;
    }

    void MassStorage::scsiReadCapacity()
    {
        if (isCardConnected())
        {
            uint32_t sizeInBlocks = Card_sizeInBlocks(mCurrentDrive);

            // One less than the maximum number is the last addressable block.
            if (sizeInBlocks > 0)
            {
                sizeInBlocks--;
            }

            // Last addressable block (big-endian)
            gCommandBuffer[0] = 0xff & (sizeInBlocks >> 24);
            gCommandBuffer[1] = 0xff & (sizeInBlocks >> 16);
            gCommandBuffer[2] = 0xff & (sizeInBlocks >> 8);
            gCommandBuffer[3] = 0xff & (sizeInBlocks);

            //
            gCommandBuffer[4] = 0;

            // Size of a block
            gCommandBuffer[5] = 0xff & (DEVICE_BLOCK_SIZE >> 16);
            gCommandBuffer[6] = 0xff & (DEVICE_BLOCK_SIZE >> 8);
            gCommandBuffer[7] = 0xff & DEVICE_BLOCK_SIZE;

            setupRequest(DATA_IN_ENDPOINT,
                         gCommandBuffer,
                         USB_TOKEN_TYPE_IN,
                         8U,
                         USB_TRANSFER_TYPE_BULK);
            mPendingTransmits++;

            gCSW.bCSWStatus = 0;
            gCSW.dCSWDataResidue = 0;
        }
        else
        {
            gCSW.bCSWStatus = 1;
            gCSW.dCSWDataResidue = 0;

            // TODO : Stall the IN endpoint

            // Mark the sense code as valid and indicate that these is no media present.
            mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            mSenseKey = SCSI_RS_KEY_NOT_READY;
            mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;
        }

        mSCSIState = SEND_STATUS;
    }

    void MassStorage::scsiRequestSense()
    {

        gCommandBuffer[0] = mErrorCode;
        gCommandBuffer[1] = 0;
        gCommandBuffer[2] = mSenseKey;
        gCommandBuffer[3] = 0;
        gCommandBuffer[4] = 0;
        gCommandBuffer[5] = 0;
        gCommandBuffer[6] = 0;

        gCommandBuffer[7] = 10;
        gCommandBuffer[8] = 0;
        gCommandBuffer[9] = 0;
        gCommandBuffer[10] = 0;
        gCommandBuffer[11] = 0;

        gCommandBuffer[12] = 0xff & (mAddSenseCode >> 8); // ASC (additional sense code)
        gCommandBuffer[13] = 0xff & (mAddSenseCode);      // ASCQ (qualifier)
        gCommandBuffer[14] = 0;
        gCommandBuffer[15] = 0;
        gCommandBuffer[16] = 0;
        gCommandBuffer[17] = 0;

        setupRequest(DATA_IN_ENDPOINT,
                     gCommandBuffer,
                     USB_TOKEN_TYPE_IN,
                     18,
                     USB_TRANSFER_TYPE_BULK);
        mPendingTransmits++;

        // Reset the valid flag on errors.
        mErrorCode = SCSI_RS_CUR_ERRORS;

        gCSW.bCSWStatus = 0;
        gCSW.dCSWDataResidue = 0;

        mSCSIState = SEND_STATUS;
    }

    bool MassStorage::readPingPong()
    {
        uint32_t n = mBlocksToRead;

        if (mReadAhead > BLOCK_READ_AHEAD)
        {
            mReadAhead = BLOCK_READ_AHEAD;
        }

        if (mReadAhead < n)
        {
            n = mReadAhead;
        }

        logDebug(100, "mBlocksToRead=%d n=%d", mBlocksToRead, n);
        // Read the new data and send it out.
        if (mPingPong)
        {
            if (!Card_readAlignedBlocks(mCurrentDrive, gPing, mCurrentLBA, n))
            {
                return false;
            }
            mPingSize = n;
        }
        else
        {
            if (!Card_readAlignedBlocks(mCurrentDrive, gPong, mCurrentLBA, n))
            {
                return false;
            }
            mPongSize = n;
        }

        mCurrentLBA += n;
        mBlocksToRead -= n;
        mReadAhead += mReadAheadGrowth;
        return true;
    }

    bool MassStorage::sendPingPong()
    {
        // Read the new data and send it out.
        if (mPingPong)
        {
            logDebug(100, "mBlocksToSend=%d mPingSize=%d", mBlocksToSend, mPingSize);
            // Setup request for next IN transfer
            setupRequest(
                DATA_IN_ENDPOINT,
                gPing,
                USB_TOKEN_TYPE_IN,
                mPingSize * DEVICE_BLOCK_SIZE,
                USB_TRANSFER_TYPE_BULK);
            mPendingTransmits++;
            mBlocksToSend -= mPingSize;
            mPingPong = false;
        }
        else
        {
            logDebug(100, "mBlocksToSend=%d mPongSize=%d", mBlocksToSend, mPongSize);
            // Setup request for next IN transfer
            setupRequest(
                DATA_IN_ENDPOINT,
                gPong,
                USB_TOKEN_TYPE_IN,
                mPongSize * DEVICE_BLOCK_SIZE,
                USB_TRANSFER_TYPE_BULK);
            mPendingTransmits++;
            mBlocksToSend -= mPongSize;
            mPingPong = true;
        }

        return true;
    }

    void MassStorage::scsiRead10(tMSCCBW *pCBW)
    {
        uint32_t blockCount = 0;

        if (isCardConnected())
        {
            // Values in the CBW are big-endian.

            mCurrentLBA = (pCBW->CBWCB[2] << 24) |
                          (pCBW->CBWCB[3] << 16) |
                          (pCBW->CBWCB[4] << 8) |
                          (pCBW->CBWCB[5] << 0);

            blockCount = (pCBW->CBWCB[7] << 8) | pCBW->CBWCB[8];
            logDebug(30, "sector=%d count=%d", mCurrentLBA, blockCount);

            // Read the first block...
            mPingPong = true;
            mBlocksToRead = blockCount;
            mReadAhead = blockCount / 8 + 1;
            mReadAheadGrowth = 4;
            if (!readPingPong())
            {
                logError("Failed to read from drive %d.  Unmounting.", mCurrentDrive);
                Card_unmount(mCurrentDrive);
            }
        }

        // Start transferring the data.
        if (isCardConnected())
        {
            // Schedule the remaining bytes to send.
            mBlocksToSend = blockCount;
            sendPingPong();
            mSCSIState = SEND_BLOCKS;
            if (mBlocksToRead > 0)
            {
                readPingPong();
            }
        }
        else
        {
            gCSW.bCSWStatus = 1;
            gCSW.dCSWDataResidue = 0;

            // Stall the IN endpoint
            // USBDevEndpointStall(g_USBInstance[ulIndex].uiBaseAddr, mINEndpoint, USB_EP_DEV_IN);

            // Mark the sense code as valid and indicate that these is no media present.
            mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            mSenseKey = SCSI_RS_KEY_NOT_READY;
            mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;

            mSCSIState = SEND_STATUS;
        }
    }

    void MassStorage::sendBlocks()
    {
        if (mBlocksToSend > 0)
        {
            sendPingPong();
        }
        else
        {
            // Send back the status once since this transfer is complete.
            gCSW.bCSWStatus = 0;
            gCSW.dCSWDataResidue = 0;
            sendCSW();
            return;
        }

        if (mBlocksToRead > 0)
        {
            readPingPong();
        }
    }

    void MassStorage::scsiWrite10(tMSCCBW *pCBW)
    {
        if (isCardConnected())
        {
            // Values in the CBW are big-endian.

            mCurrentLBA = (pCBW->CBWCB[2] << 24) |
                          (pCBW->CBWCB[3] << 16) |
                          (pCBW->CBWCB[4] << 8) |
                          (pCBW->CBWCB[5] << 0);

            uint32_t blockCount = (pCBW->CBWCB[7] << 8) | pCBW->CBWCB[8];
            mBlocksToReceive = blockCount;

            // Start receiving the data to be written.
            setupRequest(DATA_OUT_ENDPOINT,
                         gPing,
                         USB_TOKEN_TYPE_OUT,
                         DEVICE_BLOCK_SIZE,
                         USB_TRANSFER_TYPE_BULK);

            mSCSIState = RECEIVE_BLOCKS;
        }
        else
        {
            gCSW.bCSWStatus = 1;
            gCSW.dCSWDataResidue = 0;

            // TODO : Stall the OUT endpoint

            // Mark the sense code as valid and indicate that these is no media present.
            mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            mSenseKey = SCSI_RS_KEY_NOT_READY;
            mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;

            mSCSIState = SEND_STATUS;
        }
    }

    void MassStorage::scsiModeSense6(tMSCCBW *pCBW)
    {
        if (isCardConnected())
        {
            // Three extra bytes in this response.
            gCommandBuffer[0] = 3;
            gCommandBuffer[1] = 0;
            gCommandBuffer[2] = 0;
            gCommandBuffer[3] = 0;

            setupRequest(DATA_IN_ENDPOINT,
                         gCommandBuffer,
                         USB_TOKEN_TYPE_IN,
                         4,
                         USB_TRANSFER_TYPE_BULK);
            mPendingTransmits++;

            gCSW.bCSWStatus = 0;
            gCSW.dCSWDataResidue = pCBW->dCBWDataTransferLength - 4;
        }
        else
        {
            gCSW.bCSWStatus = 1;
            gCSW.dCSWDataResidue = 0;

            // TODO : Stall the IN endpoint

            // Mark the sense code as valid and indicate that these is no media present.
            mErrorCode = SCSI_RS_VALID | SCSI_RS_CUR_ERRORS;
            mSenseKey = SCSI_RS_KEY_NOT_READY;
            mAddSenseCode = SCSI_ASC_MED_NOT_PRSNT;
        }

        mSCSIState = SEND_STATUS;
    }

    bool MassStorage::isCardConnected()
    {
        if (Card_getMode(mCurrentDrive) == CARD_MODE_RAW)
        {
            mInhibitCardConnect = false;
            return true;
        }
        return false;
    }

    void MassStorage::onDisconnect()
    {
        if (isCardConnected())
        {
            Card_disconnect(mCurrentDrive);
        }
        USBDevice::onDisconnect();
    }

} /* namespace od */