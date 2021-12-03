#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwmMotors = Adafruit_PWMServoDriver(0x70);

void initMotors() {
  pwmMotors.begin();
  pwmMotors.setPWMFreq(100);
  pwmMotors.setPWM(8, 0, 4096);
  pwmMotors.setPWM(9, 0, 4096);
  pwmMotors.setPWM(10, 0, 4096);
  pwmMotors.setPWM(11, 0, 4096);
}

float smooth(float current, float target, float stp) {
  if(target > current) {
    if(current + stp < target) current += stp;
    else current = target;
  }
  if(target < current) {
    if(current - stp > target) current -= stp;
    else current = target;
  }
  return current;
}

int currX = 0, currY = 0;

void writeJoystick(float x, float y, float stp) {
  currX = smooth(currX, x, stp);
  currY = smooth(currY, y, stp);

  int LX = currX * spdX;
  int LY = currY * spdY;

  int dutyR = LY + LX;
  int dutyL = LY - LX;
  dutyR = constrain(dutyR, -4095, 4095);
  dutyL = constrain(dutyL, -4095, 4095);

  motorA_setpower(dutyL * spdLeft, false);
  motorB_setpower(-dutyR * spdRight, false);
}

void motorA_setpower(float pwr, bool invert)
{
  if (invert) pwr = -pwr;
  if (pwr < -4095) pwr = -4095;
  if (pwr > 4095) pwr = 4095;
  int pwmvalue = fabs(pwr);
  if (pwr < 0)
  {
    pwmMotors.setPWM(10, 0, 4096);
    pwmMotors.setPWM(11, 0, pwmvalue);
  }
  else
  {
    pwmMotors.setPWM(11, 0, 4096);
    pwmMotors.setPWM(10, 0, pwmvalue);
  }
}

void motorB_setpower(float pwr, bool invert)
{
  if (invert) pwr = -pwr;
  if (pwr < -4095) pwr = -4095;
  if (pwr > 4095) pwr = 4095;
  int pwmvalue = fabs(pwr);
  if (pwr < 0)
  {
    pwmMotors.setPWM(8, 0, 4096);
    pwmMotors.setPWM(9, 0, pwmvalue);
  }
  else
  {
    pwmMotors.setPWM(9, 0, 4096);
    pwmMotors.setPWM(8, 0, pwmvalue);
  }
}
