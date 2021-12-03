#include <TLC59108.h>

#define HW_RESET_PIN 0
#define I2C_ADDR TLC59108::I2C_ADDR::BASE
TLC59108 leds(I2C_ADDR + 0);

void initRgbLed() {
  setBusChannel(0x07);
  leds.init(HW_RESET_PIN);
  leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);
}

void rgbLedWrite(int r, int g, int b) {
  setBusChannel(0x07);
  leds.setBrightness(3, r);
  leds.setBrightness(2, g);
  leds.setBrightness(5, b);
}
