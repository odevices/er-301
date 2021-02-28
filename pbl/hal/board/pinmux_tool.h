/**
 * \file   pinmux.h
 *
 * \brief  This file contains structure declarations for representing pinmux 
 *         data and API prototype declarations of pinmux API's. These API's 
 *         are generic irrespective of the bit field position of the IO PAD 
 *         registers.
 *
 *         The pinmux data from the pinmux tool is to be dumped in the below 
 *         format.
 *
 *         The Pin mux data is organised based on Board/Application
 *             Board/Appln -> Modules -> Instance pin config -> 
 *                                                   -> Individual pin data.
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

#ifndef PINMUX_H_
#define PINMUX_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

//#include "chipdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Invalid pin number */
#define PINMUX_INVALID_PIN			(-1)

/** \brief Sub system identifier for ICSS PRU0 */
#define PINMUX_SS_PRU_ICSS_PRU0                    (0x1U) 
/** \brief Sub system identifier for ICSS PRU1 */
#define PINMUX_SS_PRU_ICSS_PRU1                    (0x2U) 
/** \brief Sub system identifier for ICSS ECAP0 */
#define PINMUX_SS_PRU_ICSS_ECAP0                   (0x3U) 
/** \brief Sub system identifier for ICSS UART0 */
#define PINMUX_SS_PRU_ICSS_UART0                   (0x4U) 
/** \brief Sub system identifier for ICSS UART1 */
#define PINMUX_SS_PRU_ICSS_UART1                   (0x5U) 
/** \brief Sub system identifier for ICSS MII0 */
#define PINMUX_SS_PRU_ICSS_MII0                    (0x6U) 
/** \brief Sub system identifier for ICSS MII1 */
#define PINMUX_SS_PRU_ICSS_MII1                    (0x7U) 
/** \brief Sub system identifier for ICSS ECAT */
#define PINMUX_SS_PRU_ICSS_ECAT                    (0x8U) 
/** \brief Sub system identifier for ICSS MDIO */
#define PINMUX_SS_PRU_ICSS_MDIO                    (0x9U) 
 
/** \brief Sub system identifier for PWMSS ECAP0 */
#define PINMUX_SS_PWMSS_ECAP0                      (0xAU) 
/** \brief Sub system identifier for PWMSS ECAP1 */
#define PINMUX_SS_PWMSS_ECAP1                      (0xBU) 
/** \brief Sub system identifier for PWMSS ECAP2 */
#define PINMUX_SS_PWMSS_ECAP2                      (0xCU) 
 
/** \brief Sub system identifier for PWMSS EQEP0 */
#define PINMUX_SS_PWMSS_EQEP0                      (0xDU) 
/** \brief Sub system identifier for PWMSS EQEP1 */
#define PINMUX_SS_PWMSS_EQEP1                      (0xEU) 
/** \brief Sub system identifier for PWMSS EQEP2 */
#define PINMUX_SS_PWMSS_EQEP2                      (0xFU) 
 
/** \brief Sub system identifier for PWMSS EHRPWM0 */
#define PINMUX_SS_PWMSS_EHRPWM0                    (0x10U)
/** \brief Sub system identifier for PWMSS EHRPWM1 */
#define PINMUX_SS_PWMSS_EHRPWM1                    (0x11U)
/** \brief Sub system identifier for PWMSS EHRPWM2 */
#define PINMUX_SS_PWMSS_EHRPWM2                    (0x12U)
/** \brief Sub system identifier for PWMSS EHRPWM3 */
#define PINMUX_SS_PWMSS_EHRPWM3                    (0x13U)
/** \brief Sub system identifier for PWMSS EHRPWM4 */
#define PINMUX_SS_PWMSS_EHRPWM4                    (0x14U)
/** \brief Sub system identifier for PWMSS EHRPWM5 */
#define PINMUX_SS_PWMSS_EHRPWM5                    (0x15U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Structure defining the pin configuration parameters.
 *
 */
typedef struct pinmuxPerCfg
{
    uint16_t pinOffset;
    /**< Register offset for configuring the pin */
    uint16_t optParam;
    /**< Optional param to hold the peripheral specific data */
    uint32_t pinSettings;
    /**< Value to be configured,
		  - Active mode configurations like Mux mode, pull config, Rx enable & 
		    slew rate
		  - Sleep mode configurations like Deep sleep enable, o/p value &  
	        pull config
		  - Wake up enable/disable
		Refer TRM section "7.2.1 Pad Control Registers" for more details.
	*/      
}pinmuxPerCfg_t;

/**
 *  \brief Structure defining the pin configuration for different instances of 
 *         a module.
 */
typedef struct pinmuxModuleCfg
{
    uint16_t modInstNum;
    /**< Instance number of the ip */    
    uint16_t doPinConfig;
    /**< Flag indicating whether this instance has to be configured. This flag 
         can be altered with separate API (PinMuxConfigEnable()).
         Default configuration will be set to TRUE, but can be altered for 
         different scenarios (like power management). */
    pinmuxPerCfg_t* instPins;
    /**< Pointer to list of pins corresponding to this instance */    
}pinmuxModuleCfg_t;

/**
 *  \brief Structure defining the pin configuration of a board.
 */
typedef struct pinmuxBoardCfg
{
    chipdbModuleID_t moduleId;
    /**< Module ID */   
    pinmuxModuleCfg_t* modulePinCfg;
    /**< Pin config info of a module: #pinmuxModuleCfg_t */   
}pinmuxBoardCfg_t;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   Configures IO pins needed by for module. The pin mux configurations 
 *          needed for module of a board are represented based on the above 
 *          structures. This API uses the pin mux db data and configures the 
 *          pinmux of requested module and instance.
 *
 * Note: Applications wanting to go to low power state shall call this API to 
 *       set pinmux for modules, rather than calling the overall API 
 *       (#PINMUXBoardConfig). This is to have controlled configuration of  
 *       pins for the needed peripherals only.
 *
 * \param   moduleId    Module ID of the peripheral
 * \param   instNum     Instance number of the module
 * \param   pParam1     Optional parameter for scalability
 *
 * \retval S_PASS           On success.
 * \retval E_FAIL           Generic failure
 * \retval E_INVALID_PARAM  Invalid parameter passed.
 **/
int32_t PINMUXModuleConfig(chipdbModuleID_t moduleId, uint32_t instNum, 
                                                              void* pParam1);

/**
 * \brief   Configures IO pins corresponding to all the peripherals connected 
 *          in a board. This can be called during initialization time where 
 *          individual call to configure specific module is not needed.
 *
 * Note:    Peripherals with "pinConfigFlag" set as 'TRUE' only will be 
 *          configured.
 *
 * \retval S_PASS           On success.
 * \retval E_FAIL           Generic failure
 **/                                                            
int32_t PINMUXBoardConfig(void);


/**
 * \brief   Enable or disable peripheral pin configuration.
 *
 * Note:    Peripherals with "pinConfigFlag" set as 'TRUE' only will be 
 *          configured. This API can be used to selectively enable/disable 
 *          peripheral pin config when high level API's #PINMUXModuleConfig
 *          or #PINMUXSuspendConfig are called.
 *
 * \param   moduleId   Module ID of the peripheral
 * \param   instNum    Instance number of the module
 * \param   toEnable   Flag to indicate whether to enable the module or not
 *
 * \retval S_PASS           On success.
 * \retval E_FAIL           Generic failure
 **/                              
int32_t PINMUXPerConfigEnable(chipdbModuleID_t moduleId, uint32_t instNum,
                                                    uint16_t toEnable);


/**
 * \brief   Configures IO pins corresponding to all the peripherals connected 
 *          on a board. This API configures the specific pin config value 
 *          (from pinmux DB) for all the pins whose modules are enabled for 
 *          pinmux config. This API shall be called to set the pins to optimum 
 *          configuration before entering low power mode.
 *
 * Note:    Peripherals with "pinConfigFlag" set as 'TRUE' only will be 
 *          configured.
 *
 * \retval S_PASS           On success.
 * \retval E_FAIL           Generic failure
 **/                                                            
int32_t PINMUXSuspendConfig(uint32_t pinConfigValue);
                                                    

/**
 * \brief   Configures IO pad of a specific pin. Pin level control is needed 
 *          for specific use cases like power management.
 *
 * \param   pinOffset     Register offset of the pin to be configured
 * \param   pinSettings   Pin configuration value
 *
 * \retval S_PASS           On success.
 * \retval E_FAIL           Generic failure
 * \retval E_INVALID_PARAM  Invalid parameter passed.
 **/
int32_t PINMUXPinConfig(int32_t pinOffset, uint32_t pinSettings);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* PINMUX_H_ */
