#include <VL53L0X.h>
VL53L0X distSensor;

float dist = 0;

void initDistSensor() {
  setBusChannel(0x06);
  distSensor.init();
  distSensor.setTimeout(500);
  distSensor.setSignalRateLimit(0.1);
  distSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  distSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
}

void updateDistance() {
  setBusChannel(0x06);
  dist = distSensor.readRangeSingleMillimeters();
}

float getDistance() {
  return dist;
}
