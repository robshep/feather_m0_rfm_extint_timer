# Testing LowPower on a FeatherM0 + LoRa RFM95 board.

## RTC Zero

Bundled patched version of RTCZero SAMD21 standby library

Lower precision oscillator used to save power.

Taken from

 - https://forum.arduino.cc/index.php?topic=585554.msg4106271#msg4106271
 - http://forcetronic.blogspot.com/2016/09/reducing-power-consumption-on-arduino_24.html


```
--- RTCZero.cpp.orig	2019-05-07 12:57:40.000000000 +0100
+++ RTCZero.cpp	2019-05-07 12:58:13.000000000 +0100
@@ -44,7 +44,7 @@
   uint16_t tmp_reg = 0;

   PM->APBAMASK.reg |= PM_APBAMASK_RTC; // turn on digital interface clock
-  config32kOSC();
+  //config32kOSC();

   // If the RTC is in clock mode and the reset was
   // not due to POR or BOD, preserve the clock time
@@ -449,7 +449,7 @@
   GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
   while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
     ;
-  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
+  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
   while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
     ;
   GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
```

## RadioHead

install RadioHead library from Zip (not included)

Used here just to call `radio.sleep()`

## External Interrupt

Has a primitive debouncer on the external interrupt pin `D10`

Noise affects the interrupt a lot.  Sometimes it triggers when just touching the board, needs better investigation.

*currently a PULLDOWN input type with HIGH switch interrupt, for use with NO_HIGH Grove button*

## Pin power

A few uA shaved off by setting all pins to `OUTPUT` + `LOW` which is a lower power consumption than (a) the cpu default of tri-state floating and (b) the platform's init mode of `INPUT`

```
for (uint32_t ul = 0 ; ul < NUM_DIGITAL_PINS ; ul++ )
  {
    pinMode( ul, OUTPUT ) ;
    digitalWrite(ul, LOW);
  }
```

This might be fixed even further if this patch makes its way upstream.

 - https://github.com/adafruit/Adafruit_SleepyDog/issues/17#issuecomment-481798215
 - https://github.com/adafruit/ArduinoCore-samd/commit/bd082fa1f613cfad6b8cf0eb6eebce31005b2881

