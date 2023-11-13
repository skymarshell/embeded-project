#ifndef ESP32_Servo_h
#define ESP32_Servo_h


#define DEFAULT_uS_LOW 544    // 544us
#define DEFAULT_uS_HIGH 2400  // 2400us


#define DEFAULT_TIMER_WIDTH 10          //ความกว้างpulse pwm ที่ส่งไปในservo
#define DEFAULT_TIMER_WIDTH_TICKS 1024  //ความกว้างpulse pwm ที่ออกจากservo


#define MIN_PULSE_WIDTH 500             // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH 2500            // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH 1500        // default pulse width when servo is attached
#define DEFAULT_PULSE_WIDTH_TICKS 4825  // default position of the servo when it's first attached or initialized
#define REFRESH_CPS 50                  //ค่าสูงจะทำให้หมุนเร็วขึ้น แต่จะหมุนผิดตำแหน่ง เช่น ให้เป็น 100 สั่งหมุน 180 จะได้ประมาณ 40-50 องศา
#define REFRESH_USEC 20000              //refresh rate of the servo in counts per microseconds ใช้อัปเดตสัญญาณ servo

#define MAX_SERVOS 16  // no. of PWM channels in ESP32


class Servo {
public:
  Servo();
  int myAttach(int pin);                    // attach the given pin to the next free channel, returns channel number or 0 if failure
  int myAttach(int pin, int min, int max);  // as above but also sets min and max values for writes.
  void angle(int value);                    // if value is < MIN_PULSE_WIDTH its treated as an angle, otherwise as pulse width in microseconds
  void writeMicroseconds(int value);        // Write pulse width in microseconds
  

private:
  int usToTicks(int usec);  //เป็นฟังก์ชันที่ใช้สำหรับแปลงค่าความยาวของพัลส์ PWM จากไมโครวินาทีไปเป็น ticks หรือแบบกลับกัน.
  int ticksToUs(int ticks);
  int servoChannel = 0;                               // channel number for this servo
  int min = DEFAULT_uS_LOW;                           // minimum pulse width for this servo
  int max = DEFAULT_uS_HIGH;                          // maximum pulse width for this servo
  int pinNumber = 0;                                  // GPIO pin assigned to this channel
  int timer_width = DEFAULT_TIMER_WIDTH;              // ESP32 allows variable width PWM timers
  int ticks = DEFAULT_PULSE_WIDTH_TICKS;              // current pulse width on this channel
  int timer_width_ticks = DEFAULT_TIMER_WIDTH_TICKS;  // no. of ticks at rollover; varies with width
};
#endif