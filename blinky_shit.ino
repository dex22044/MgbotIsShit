#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>

char auth[] = "83hqwqIDEahyGY63o4lbRS9XZuro31YQ";
char ssid[] = "DNK";
char pass[] = "DNK12345";
// 0 - вперёд до стены
// 1 - вправо
// 2 - влево
char cmds[] {0, 2, 0, 2, 0, 2, 0, 1, 0};
int cmdsCount = 9;
int cmdIdx = 0;

int triggerDist = 100;
int delayLeft = 0;
int delayRight = 0;
int rotateSpeed = 0;

float spdX = 0.5, spdY = 0.5;
float spdX2 = 0.5, spdY2 = 0.5;
float spdLeft = 1;
float spdRight = 1;

bool isBlynkControl = false;

TaskHandle_t core2taskhandle;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  initMotors();
  initRgbLed();
  initIR();
  initCO2();
  initDistSensor();
  initFireSensor();

  Blynk.connectWiFi(ssid, pass);
  Blynk.config(auth, "172.16.1.155", 8080);
}

BLYNK_WRITE(V0)
{
  rgbLedWrite(param[0].asInt(), param[1].asInt(), param[2].asInt());
}

bool w, a, s, d;

BLYNK_WRITE(V4) {
  w = param.asInt();
}
BLYNK_WRITE(V3) {
  s = param.asInt();
}
BLYNK_WRITE(V6) {
  a = param.asInt();
}
BLYNK_WRITE(V5) {
  d = param.asInt();
}

int LX = 0;
int LY = 0;

BLYNK_WRITE(V7) {
  spdY2 = param.asInt() / 1000.0f;
}
BLYNK_WRITE(V8) {
  spdX2 = param.asInt() / 1000.0f;
}
BLYNK_WRITE(V12) {
  triggerDist = param.asInt();
}
BLYNK_WRITE(V13) {
  delayLeft = param.asInt();
}
BLYNK_WRITE(V14) {
  delayRight = param.asInt();
}
BLYNK_WRITE(V15) {
  rotateSpeed = param.asInt();
}
BLYNK_WRITE(V16) {
  spdLeft = param.asInt() / 1000.0f;
}
BLYNK_WRITE(V17) {
  spdRight = param.asInt() / 1000.0f;
}

int tmr1last = 0;
int lastIRCode = -1;
int sendDataCounter = 0;
bool isAutoMode = false;
int cmdStartTime = 0;

void rotateLeft() {
  motorA_setpower(0x20 * 40.95, false);
  motorB_setpower(0x20 * 40.95, false);
  delay(delayLeft);
  motorA_setpower(-2047, false);
  motorB_setpower(-2047, false);
  delay(30);
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

void rotateRight() {
  motorA_setpower(0x20 * 40.95, true);
  motorB_setpower(0x20 * 40.95, true);
  delay(delayRight);
  motorA_setpower(-2047, true);
  motorB_setpower(-2047, true);
  delay(30);
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

void fastStop() {
  motorA_setpower(1000, false);
  motorB_setpower(1000, true);
  delay(150);
  motorA_setpower(0, false);
  motorB_setpower(0, false);
}

void processCmd() {
  if (!isAutoMode || cmdIdx >= cmdsCount) return;
  int cmd = cmds[cmdIdx];
  if (cmd == 0) {
    if (getDistance() > triggerDist) {
      writeJoystick(0, -4095, 100000.0f);
    } else {
      fastStop();
      delay(700);
      cmdIdx++;
      cmdStartTime = millis();
      Serial.println("End cmd 0");
    }
  }
  if (cmd == 1) {
    rotateLeft();
    delay(700);
    cmdIdx++;
    cmdStartTime = millis();
    Serial.println("End cmd 1");
  }
  if (cmd == 2) {
    rotateRight();
    delay(700);
    cmdIdx++;
    cmdStartTime = millis();
    Serial.println("End cmd 2");
  }
}

BLYNK_WRITE(V11) {
  if (param.asInt()) {
    isAutoMode = true;
    cmdStartTime = millis();
    cmdIdx = 0;
  }
  else {
    isAutoMode = false;
  }
}

void loop()
{
  Blynk.run();

  if (millis() - 30 > tmr1last) {
    Serial.println(millis() - tmr1last);
    updateDistance();
    tmr1last = millis();
    float dist = getDistance();
    
    sendDataCounter++;
    if (sendDataCounter > 10) {
      updateCO2();
      updateFireSensor();
      int CO2Val = getCO2_Val();
    
      if(getIRData() > 1000) Blynk.notify("Пахнет жареным");
      if(CO2Val > 2000) Blynk.notify("Пахнет углекислотой");

      int r = 0, g = 0, b = 0;
      if (getIRData() > 1000) r = 255;
      if (CO2Val > 2000) g = 255;
      rgbLedWrite(r, g, b);
    
      //Blynk.virtualWrite(V1, dist);
      //Blynk.virtualWrite(V2, CO2Val);
      //Blynk.virtualWrite(V9, getIRData());
      //Blynk.virtualWrite(V10, getVisibleData());
      sendDataCounter = 0;
    }
    processCmd();
  }

  if (!isAutoMode) {
    if ((lastIRCode = receiveIR()) != -1) {
      Serial.println(lastIRCode, HEX);
      if (lastIRCode == 0xFF30CF) {
        w = 1; s = 0; a = 1; d = 0;
      }
      if (lastIRCode == 0xFF18E7) {
        w = 1; s = 0; a = 0; d = 0;
      }
      if (lastIRCode == 0xFF7A85) {
        w = 1; s = 0; a = 0; d = 1;
      }
      if (lastIRCode == 0xFF5AA5) {
        w = 0; s = 0; a = 0; d = 1;
      }
      if (lastIRCode == 0xFF52AD) {
        w = 0; s = 1; a = 1; d = 0;
      }
      if (lastIRCode == 0xFF4AB5) {
        w = 0; s = 1; a = 0; d = 0;
      }
      if (lastIRCode == 0xFF42BD) {
        w = 0; s = 1; a = 0; d = 1;
      }
      if (lastIRCode == 0xFF10EF) {
        w = 0; s = 0; a = 1; d = 0;
      }
      
      if (lastIRCode == 0xFFE01F) {
        spdY2 -= 0.05;
      }
      
      if (lastIRCode == 0xFFA857) {
        spdY2 += 0.05;
      }
      
      if (lastIRCode == 0xFF22DD) {
        spdX2 -= 0.05;
      }
      
      if (lastIRCode == 0xFF02FD) {
        spdX2 += 0.05;
      }
      
      if (lastIRCode == 0xFF38C7) {
        w = 0; s = 0; a = 0; d = 0;
      }
    }

    float force = 100000.0f;
    if (w && !s) LY = -4095;
    else if (!w && s) LY = 4095;
    else LY = 0;
  
    if (a && !d) LX = 4095;
    else if (!a && d) LX = -4095;
    else LX = 0;
  
    spdX = spdX2;
    spdY = spdY2;
  
    if (getDistance() < triggerDist) {
      LY = 2500;
      force = 100000.0f;
    }
    writeJoystick(LX, LY, force);
  }
}
