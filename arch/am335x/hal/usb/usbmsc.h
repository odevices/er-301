/**
 *  \file  usbmsc.h
 *
 *  \brief Generic types and defines use by the mass storage class.
 *
 *
 *  \copyright Copyright (C) 2013 Texas Instruments Incorporated -
 *             http://www.ti.com/
 */

/*
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

#ifndef USBMSC_H_
#define USBMSC_H_

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

/** \brief The request for maximum number of logical units on a mass storage
 *         device.
 */
#define USBREQ_GET_MAX_LUN 0xfe
#define USBREQ_MASS_STORAGE_RESET 0xff

   /**
 * \brief The signatures used by the USB MSC class specification
 */

#define CBW_SIGNATURE 0x43425355
#define CSW_SIGNATURE 0x53425355

/**
 * \brief Flag for bmCBWFlags of tMSCCBW
 */
#define CBWFLAGS_DIR_M 0x80
#define CBWFLAGS_DIR_IN 0x80
#define CBWFLAGS_DIR_OUT 0x00

   /** \brief Flag for the bCSWStatus member of tMSCCSW */

#define CSWSTATUS_CMD_SUCCESS 0
#define CSWSTATUS_CMD_FAILED 1
#define CSWSTATUS_PHASE_ERROR 2

/** \brief SCSI command return codes */
#define SCSI_CMD_STATUS_PASS 0x00
#define SCSI_CMD_STATUS_FAIL 0x01

/** \brief SCSI Commands */
#define SCSI_TEST_UNIT_READY 0x00
#define SCSI_REQUEST_SENSE 0x03
#define SCSI_INQUIRY_CMD 0x12
#define SCSI_MODE_SENSE_6 0x1a
#define SCSI_READ_FORMAT_CAPACITIES 0x23
#define SCSI_READ_CAPACITY 0x25
#define SCSI_READ_10 0x28
#define SCSI_WRITE_10 0x2a
#define SCSI_VERIFY_10 0x2f
#define SCSI_REASSIGN_BLOCKS 0x07
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1e
#define SCSI_START_STOP_UNIT 0x1b

/** \brief Size of the SCSI inquiry response data */
#define SCSI_INQUIRY_DATA_SZ 36

/* Offset 0 of the Inquiry Data. */

/** \brief Peripheral Qualifier mask */
#define SCSI_INQ_PQ_M 0xe0
/** \brief Device connected */
#define SCSI_INQ_PQ_CNCT 0x00
/** \brief Devce disconnected */
#define SCSI_INQ_PQ_DISC 0x20
/** \brief Preipheral device type mask */
#define SCSI_INQ_PDT_M 0x1f
/** \brief Direct Access device */
#define SCSI_INQ_PDT_SBC 0x00

/** Offset 1 of the Inquiry Data. */
/** \brief Device is removable */
#define SCSI_INQ_RMB 0x80

/** \brief Macro to check if removable */
#define SCSIIsRemovable(pData) \
   (((uint8_t *)pData)[1] & SCSI_INQ_RMB)

/** \brief Size of the SCSI Read capacity response data */
#define SCSI_READ_CAPACITY_SZ 0x08

/** SCSI Mode Sense definitions, these are passed in via the ulFlags parameter
 * of the SCSIModeSense() function call.
 */

/** \brief Discable block descriptors */
#define SCSI_MS_DBD 0x00000800

/** \brief Page code values used in combination with page control values */
#define SCSI_MS_PC_VENDOR 0x00000000
#define SCSI_MS_PC_DISCO 0x00020000
#define SCSI_MS_PC_CONTROL 0x000a0000
#define SCSI_MS_PC_LUN 0x00180000
#define SCSI_MS_PC_PORT 0x00190000
#define SCSI_MS_PC_POWER 0x001a0000
#define SCSI_MS_PC_INFORM 0x001c0000
#define SCSI_MS_PC_ALL 0x003f0000

/** \brief Page control values */
#define SCSI_MS_PC_CURRENT 0x00000000
#define SCSI_MS_PC_CHANGEABLE 0x00400000
#define SCSI_MS_PC_DEFAULT 0x00800000
#define SCSI_MS_PC_SAVED 0x00c00000

/** Request Sense Definitions. */

/** \brief Size of the data returned by the Request Sense command. */
#define SCSI_REQUEST_SENSE_SZ 18

/** \brief Sense Key offset */
#define SCSI_RS_SKEY 2
/** \brief Additional Sense Key offset */
#define SCSI_RS_SKEY_AD_SKEY 12

/** Offset 0 in the Request Sense response. */
/** \brief Response is valid */
#define SCSI_RS_VALID 0x80
/** Current errors returned */
#define SCSI_RS_CUR_ERRORS 0x70
/** \brief Deferred errors returned. */
#define SCSI_RS_DEFER_ERRORS 0x71

/** Offset 2 in the Request Sense response. */
/** \brief Sense Key */
#define SCSI_RS_KEY_M 0x0f
/** \brief No Sense data */
#define SCSI_RS_KEY_NO_SENSE 0x00
/** \brief Recovered Error */
#define SCSI_RS_KEY_RECOVRD_ERR 0x01
/** \brief Not ready */
#define SCSI_RS_KEY_NOT_READY 0x02
/** \brief Error in the media */
#define SCSI_RS_KEY_MEDIUM_ERR 0x03
/** \brief Hardware error, non recoverable */
#define SCSI_RS_KEY_HW_ERR 0x04
/** \brief Illegal request */
#define SCSI_RS_KEY_ILGL_RQST 0x05
/** \brief Unit changed or reset */
#define SCSI_RS_KEY_UNIT_ATTN 0x06
/** \brief Write protect error */
#define SCSI_RS_KEY_DATA_PROT 0x07
/** \brief Write once error, block not clear */
#define SCSI_RS_KEY_BLANK_CHK 0x07
/** \brief Last command was aborted */
#define SCSI_RS_KEY_ABORT 0x0b
// Indicates that the source data did not match the data read from the medium.
#define SCSI_RS_KEY_MISCOMPARE 0x0e

// Additional Sense Codes (ASC|ASCQ)
/** \brief  Medium not present */
#define SCSI_ASC_MED_NOT_PRSNT 0x3a00
/** \brief Not ready to ready transition */
#define SCSI_ASC_NOTRDY2RDY 0x2800
/** \brief Parameter value invalid */
#define SCSI_ASC_PARAMETER_VALUE_INVALID 0x2602

/** Additional information for SCSI_RS_KEY_NOT_READY */
/** \brief Media not present */
#define SCSI_RS_KEY_NOTPRSNT 0x3A
   /* ========================================================================== */
   /*                         Structures and Enums                               */
   /* ========================================================================== */

   /**
 * \brief This structure is used to access the Command Block wrapper
 *        (CBW) data structure that is used when the communicating
 *        with the mass storage class device.
 */

   typedef struct
   {
      uint32_t dCBWSignature;
      /**< Signature that helps identify this data packet as a CBW.  The signature
     * field shall contain the value 0x43425355 (little endian), indicating a
     * CBW.
     */

      uint32_t dCBWTag;
      /**< The Command Block Tag sent by the host controller.  The device shall
     * echo the contents of this field back to the host in the dCSWTag field
     * of the associated CSW.  The dCSWTag positively associates a CSW with the
     * corresponding CBW.
     */

      uint32_t dCBWDataTransferLength;
      /**< The number of bytes of data that the host expects to transfer on the
     * Bulk-In or Bulk-Out endpoint (as indicated by the Direction bit) during
     * the execution of this command.  If this field is zero, the device and
     * the host will not transfer data between the CBW and the associated CSW,
     * and the device will ignore the value of the Direction bit in
     * bmCBWFlags.
     */

      uint8_t bmCBWFlags;
      /**< The device will ignore these bits if the dCBWDataTransferLength value
     * is set to 0.
     *
     * The bits of this field are defined as follows:
     * Bit 7 Direction
     *   0 = Data-Out from host to the device,
     *   1 = Data-In from the device to the host.
     * Bit 6 Obsolete - The host shall set this bit to zero.
     * Bits 5..0 Reserved - the host shall set these bits to zero.
     */

      uint8_t bCBWLUN;
      /**< The device Logical Unit Number (LUN) to which the command block is being
     * sent.  For devices that support multiple LUNs, the host shall place into
     * this field the LUN to which this command block is addressed.  Otherwise,
     * the host shall set this field to zero.
     *
     *
     * The valid length of the CBWCB in bytes.  This defines the valid length
     * of the command block.  The only legal values are 1 through 16.  All
     * other values are reserved.
     */
      uint8_t bCBWCBLength;

      uint8_t CBWCB[16];
      /**< This array holds the command block to be executed by the device.  The
     * MSC device will interpret the first bCBWCBLength bytes in this field as
     * a command block as defined by the command set identified by
     * bInterfaceSubClass.  If the command set supported by the device uses
     * command blocks of fewer than 16 bytes in length, the significant bytes
     * shall be transferred first, beginning with the byte at offset 15.  The
     * device will ignore the content of the CBWCB field past the byte at
     * offset (15 + bCBWCBLength - 1).
     */
   } USBLIB_PACKED tMSCCBW;

   /**
 * \brief This structure encapsulates the Command Status Word (CSW) structure that is
 *        sent in response to all CBW commands.
 */
   typedef struct
   {
      uint32_t dCSWSignature;
      /**< Signature that identifies this data packet as a CSW.  The signature
     * field must contain the value 53425355h (little endian) to indicate CSW.
     */

      uint32_t dCSWTag;
      /**< The device will set this field to the value received in the dCBWTag of
     * the associated CBW.
     */

      uint32_t dCSWDataResidue;
      /**< For OUT transactions the device will fill the dCSWDataResidue field with
     * the difference between the amount of data expected as stated in the
     * dCBWDataTransferLength, and the actual amount of data processed by the
     * device.  For IN transactions the device will fill the dCSWDataResidue
     * field with the difference between the amount of data expected as stated
     * in the dCBWDataTransferLength and the actual amount of relevant data
     * sent by the device.  The dCSWDataResidue will not exceed the value sent
     * in the dCBWDataTransferLength.
     */

      uint8_t bCSWStatus;
      /**< The bCSWStatus field indicates the success or failure of the command.
     *   The device shall set this byte to zero if the command completed
     *   successfully.  A non-zero value shall indicate a failure during command
     *   execution.
     */
   } USBLIB_PACKED tMSCCSW;

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

#endif /* #ifndef USBMSC_H_ */
