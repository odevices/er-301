#include <hal/log.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/eqep.h>
#include <ti/am335x/hw_types.h>
#include <hal/encoder.h>

static int lastValue = 0;

void Encoder_init(void)
{
  // CLKCONFIG Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_PWMSS0_REGS + PWMSS_CLKCONFIG);

    // enable eQEP clock
    HW_SET_FIELD(regVal, PWMSS_CLKCONFIG_EQEPCLK_EN, 1);

    HW_WR_REG32(SOC_PWMSS0_REGS + PWMSS_CLKCONFIG, regVal);
  }

  // wait for clock to enable
  while ((HW_RD_REG32(SOC_PWMSS0_REGS + PWMSS_CLKSTATUS) & PWMSS_CLKSTATUS_EQEP_CLK_EN_ACK_MASK) == 0)
  {
  }

  // QDECCTL Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QDECCTL);

    // choose quadrature count mode
    HW_SET_FIELD(regVal, EQEP_QDECCTL_QSRC, EQEP_QDECCTL_QSRC_EN_0X0);

    HW_WR_REG32(SOC_EQEP_0_REGS + EQEP_QDECCTL, regVal);
  }

  // QPOSINIT Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QPOSINIT);

    HW_SET_FIELD(regVal, EQEP_QPOSINIT_QPOSINIT, 0);

    HW_WR_REG32(SOC_EQEP_0_REGS + EQEP_QPOSINIT, regVal);
  }

  // QPOSMAX Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QPOSMAX);

    // choose max
    HW_SET_FIELD(regVal, EQEP_QPOSMAX_QPOSMAX, 0xFFFFFFFF);

    HW_WR_REG32(SOC_EQEP_0_REGS + EQEP_QPOSMAX, regVal);
  }

  logAssert(Encoder_getValue() == 0);

  // QCAPCTL Register
  {
    uint32_t regVal = HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QCAPCTL);

    // enable eQEP capture unit
    HW_SET_FIELD(regVal, EQEP_QCAPCTL_CEN, EQEP_QCAPCTL_CEN_EN_0X1);

    HW_WR_REG32(SOC_EQEP_0_REGS + EQEP_QCAPCTL, regVal);
  }

  // QEPCTL Register
  {
    uint16_t regVal = HW_RD_REG16(SOC_EQEP_0_REGS + EQEP_QEPCTL); // <-- DATA ABORT here

    // enable position counter
    HW_SET_FIELD(regVal, EQEP_QEPCTL_PHEN, EQEP_QEPCTL_PHEN_EN_0X1);

    // initialize position counter
    HW_SET_FIELD(regVal, EQEP_QEPCTL_SWI, EQEP_QEPCTL_SWI_EN_0X1);

    HW_WR_REG16(SOC_EQEP_0_REGS + EQEP_QEPCTL, regVal);
  }

  lastValue = HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QPOSCNT);
}

int Encoder_getValue(void)
{
  // requires 130ns
  return HW_RD_REG32(SOC_EQEP_0_REGS + EQEP_QPOSCNT);
}

int Encoder_getChange(void)
{
  int value = Encoder_getValue();
  int change = value - lastValue;
  lastValue = value;
  return change;
}
