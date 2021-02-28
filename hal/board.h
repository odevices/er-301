#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void Board_init();

    /* Not enabled by default */
    void Board_enableI2C2();
    void Board_enableUSB();
    void Board_disableUSB();

    /* These share the same pins. */
    void Board_pinmuxI2C2();
    void Board_pinmuxUART0();

#ifdef __cplusplus
}
#endif
