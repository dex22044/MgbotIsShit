#include "SparkFun_SGP30_Arduino_Library.h"

SGP30 sgp30;

void initCO2() {
  setBusChannel(0x05);
  if(sgp30.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1);
  }
  sgp30.initAirQuality();
}

void updateCO2() {
  setBusChannel(0x05);
  sgp30.measureAirQuality();
}

int getCO2_Val() {
  return sgp30.CO2;
}

int getTVOC_Val() {
  return sgp30.TVOC;
}
