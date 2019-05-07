# Local Modifications

2019-04-06 - RGS:

https://forum.arduino.cc/index.php?topic=585554.msg4106271#msg4106271

In the
    RTCZero-master/src/RTCZero.cpp
file in your library directory make two changes:
comment out the line 
    //config32kOSC();

find the line: 
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );

and change to:
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );

These two changes swap the RTC library over to using the internal, less accurate but lower power timer.
