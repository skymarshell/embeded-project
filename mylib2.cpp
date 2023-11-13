#include "mylib2.h"
////////////////////////////////////consttruct/destruct
//5 trig 18 echo
void HCSR04::init(int out, int echo[]) {
  this->out = out;
  this->echo = echo;
  pinMode(this->out, OUTPUT);
  for (int i = 0; i < sizeof(echo); i++) {
    pinMode(this->echo[i], INPUT);  //
  }
}
HCSR04::HCSR04(int out, int echo) {
  this->init(out, new int[1]{ echo });
}
HCSR04::HCSR04(int out, int echo[]) {
  this->init(out, echo);
}

///////////////////////////////////////////////////dist
float HCSR04::dist(int n) const {
  digitalWrite(this->out, LOW);                //ล้างสัญญาณจากก่อนหน้า
  delayMicroseconds(2);                        //หน่วง
  digitalWrite(this->out, HIGH);               //ส่งสัญญาณไปsensor start
  delayMicroseconds(10);                       //หน่วงเพื่อให้เซ็นเซอร์เพียงพอสำหรับการวัดระยะทาง
  digitalWrite(this->out, LOW);                //ล้างสัญญาณ
  return pulseIn(this->echo[n], HIGH) / 58.0;  //(Pulse duration)/58 แปลง pulse เป็น cm
}
float HCSR04::dist() const {
  return this->dist(0);
}
