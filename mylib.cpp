#include "Arduino.h"
#include "mylib.h"
//#include "esp32-hal-ledc.h"
#include <iostream>
#include <chrono>
#include <ctime>

//   ถ้าใช้ servo หลายตัวเราต้องแยก channel กััน
//   int Servo::ChannelUsed[MAX_SERVOS];// 16 ช่อง 0-15
//   for(int i=0;i<16;i++)
//   {
//      ChannelUsed[i]= -1 ;
//   }
//   Servo::Servo() {
//   this->servoChannel = 0;
//   // see if there is a servo channel available for reuse
//   bool foundChannelForReuse = false;
//   for (int i = 1; i < MAX_SERVOS + 1; i++) {
//     if (ChannelUsed[i] == -1) {
//       // reclaim this channel
//       ChannelUsed[i] = 1;
//       this->servoChannel = i;
//       break;
//     }
//   }
//
//     // initialize this channel with plausible values, except pin # (we set pin # when attached)
//     this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
//     this->timer_width = DEFAULT_TIMER_WIDTH;
//     this->pinNumber = -1;  // make it clear that we haven't attached a pin to this channel
//     this->min = DEFAULT_uS_LOW;
//     this->max = DEFAULT_uS_HIGH;
//     this->timer_width_ticks = pow(2, this->timer_width);
//   }
// }

// สร้าง object class Servo
// คุณสมบัติ servoChannel ticks timer_width pinNumber min max timer_width_ticks
Servo::Servo() {
  this->servoChannel = 0;
  this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
  this->timer_width = DEFAULT_TIMER_WIDTH;
  this->pinNumber = -1;  // make it clear that we haven't attached a pin to this channel
  this->min = DEFAULT_uS_LOW;
  this->max = DEFAULT_uS_HIGH;
  this->timer_width_ticks = pow(2, this->timer_width);
}

// กำหนด Ex 23
int Servo::myAttach(int pin) {
  return (this->myAttach(pin, DEFAULT_uS_LOW, DEFAULT_uS_HIGH));
}

// ส่งต่อมาจากด้านบน
int Servo::myAttach(int pin, int min, int max) {

  this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
  this->timer_width = DEFAULT_TIMER_WIDTH;
  this->timer_width_ticks = pow(2, this->timer_width);

  //กำหนดให้ object นี้ pinNumber = 23
  this->pinNumber = pin;

  // min/max checks
  // เป็นการเช็คว่า MIN,MAX_PULSE_WIDTH อยู่ในค่าที่กำหนดหรือไม่
  // เช่น user อยากกำหนด min_PULSE_WIDTH 250 myAttach(23,300,2500) --> 300 น้อยไปขั้นตํ่าต้อง 500
  // กรณีนี้เราไม่ได้กำหนด min คือ min เลย 500
  if (min < MIN_PULSE_WIDTH)
    min = MIN_PULSE_WIDTH;
  if (max > MAX_PULSE_WIDTH)
    max = MAX_PULSE_WIDTH;
  this->min = min;  //store this value in uS 500
  this->max = max;  //store this value in uS 2500
  // Set up this channel
  // if you want anything other than default timer width, you must call setTimerWidth() before attach
  ledcSetup(this->servoChannel, REFRESH_CPS, this->timer_width);  // channel #, 50 Hz, timer width //timer_width 10
  ledcAttachPin(this->pinNumber, this->servoChannel);             // GPIO pin assigned to channel
  return 1;
}

//.write(90)
void Servo::angle(int value) {
  //90<500 ?
  if (value < MIN_PULSE_WIDTH) {
    if (value < 0)
      value = 0;
    else if (value > 180)
      value = 180;
    //value=90
    value = map(value, 0, 180, this->min, this->max);
  }
  // 90 >> 1500
  this->writeMicroseconds(value);
}


void Servo::writeMicroseconds(int value) {
  //1500 < 500 ?
  if (value < this->min)  // ensure pulse width is valid
    value = this->min;
  //1500 > 500 ? yes
  else if (value > this->max)
    value = this->max;

  value = usToTicks(value);  // convert to ticks
  // 1500--> 76
  this->ticks = value;
  // do the actual write
  //ค่าความกว้างพัลส์เป็น 76 ติ๊ก
  ledcWrite(this->servoChannel, this->ticks);
}


int Servo::usToTicks(int usec) {
  return (int)((float)usec / ((float)REFRESH_USEC / (float)this->timer_width_ticks));
}

int Servo::ticksToUs(int ticks) {
  return (int)((float)ticks * ((float)REFRESH_USEC / (float)this->timer_width_ticks));
}


