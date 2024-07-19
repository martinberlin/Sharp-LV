#include <bb_captouch.h>
#include <Wire.h>

BBCapTouch bbct;
int iWidth, iHeight;

#define TOUCH_SDA 0
#define TOUCH_SCL 1
#define TOUCH_INT 2
#define TOUCH_RST 3

void setup() {
  pinMode(TOUCH_INT, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Let'S test this TOUCH");

  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();
  Serial.printf("Sensor type = %d\n", iType);
}

TOUCHINFO ti;

void loop() {
  if (bbct.getSamples(&ti)) {
    for (int i=0; i<ti.count; i++){
      Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
      Serial.print("  x: ");Serial.print(ti.x[i]);
      Serial.print("  y: ");Serial.print(ti.y[i]);
      Serial.print("  size: ");Serial.println(ti.area[i]);
      Serial.println(' ');
    } // for each touch point
  }
  // INT Seems to be unuseful, is always LOW, maybe needs external PullUP?
  // if (digitalRead(TOUCH_INT) == 0) {
  //   Serial.println("Touched");
  // }


}
