#include "types.h"
#include "hw_usbphyGS70.h"
#include "hw_types.h"

/**  
 * \brief  turn on USB PHY
 *
**/
void UsbPhyOn(uint32_t uiPHYConfigRegAddr)
{
    uint32_t usbphycfg = 0;

    usbphycfg = HWREG(uiPHYConfigRegAddr);
    usbphycfg &= ~(USBPHY_CM_PWRDN | USBPHY_OTG_PWRDN);
    usbphycfg |= (USBPHY_OTGVDET_EN | USBPHY_OTGSESSEND_EN);

    HWREG(uiPHYConfigRegAddr) = usbphycfg;
}



/**
 * \brief This function will switch off  the USB Phy  
 *          
 *
 * \param    None
 *
 * \return   None
 *
  **/
void UsbPhyOff(uint32_t uiPHYConfigRegAddr)
{
    uint32_t  usbphycfg = 0;

    usbphycfg = HWREG(uiPHYConfigRegAddr);
    usbphycfg |= (USBPHY_CM_PWRDN | USBPHY_OTG_PWRDN);
    HWREG(uiPHYConfigRegAddr) = usbphycfg;
}