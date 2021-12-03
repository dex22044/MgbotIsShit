#include <Wire.h>
#define sensor_addr 0x39

float ir_data = 0;
float vis_data = 0;

void initFireSensor() {
  setBusChannel(0x03);
  Wire.begin();
  Wire.beginTransmission(sensor_addr);
  Wire.write(0x81);       // Регистр времени интегрирования АЦП
  Wire.write(0b00111111); // 180 мс, 65535 циклов
  Wire.endTransmission();
  Wire.beginTransmission(sensor_addr);
  Wire.write(0x83);       // Регистр времени ожидания
  Wire.write(0b00111111); // 180 мс
  Wire.endTransmission();
  Wire.beginTransmission(sensor_addr);
  Wire.write(0x90);         // Регистр усиления
  Wire.write(0b00000000);   // Усиление 1x
  Wire.endTransmission();
  Wire.beginTransmission(sensor_addr);
  Wire.write(0x80);       // Регистр управления питанием
  Wire.write(0b00001011); // Включение ожидания, генератора, АЦП и ALS сенсора
  Wire.endTransmission();
}

void updateFireSensor() {
  setBusChannel(0x03);
  unsigned int sensor_data[4];
  Wire.beginTransmission(sensor_addr);
  Wire.write(0x94); // Начальный адрес регистров данных
  Wire.endTransmission();
  Wire.requestFrom(sensor_addr, 4);
  if (Wire.available() == 4) {
    sensor_data[0] = Wire.read();
    sensor_data[1] = Wire.read();
    sensor_data[2] = Wire.read();
    sensor_data[3] = Wire.read();
  }
  ir_data   = sensor_data[3] * 256.0 + sensor_data[2];
  vis_data = sensor_data[1] * 256.0 + sensor_data[0];
}

float getIRData() { return ir_data; }
float getVisibleData() { return vis_data; }
