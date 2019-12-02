/* -- Lib reference --//
 * Modbus:    https://github.com/andresarmento/modbus-arduino
 * CAN:       https://github.com/sandeepmistry/arduino-CAN
 * EEPROM:    Internal EEPROM lib
 * Servo:     Internal servo lib
 * Interupt:  Internal interupt lib
 */
 
#include <Servo.h>
#include <EEPROM.h>
#include <CAN.h>

//- Pin defenitions -//
#define LOCK_SWITCH A0
#define EXT_INTERUPT0 2
#define EXT_INTERUPT1 6

#define EXT_PWM 3
#define LOCK_SERVO 5
#define BUZZER A3
#define LOCK_LED A2
//-------------------//

bool const DEVMODE = true;

bool lock_servo = false;

Servo Lock;

void lock_toggle(int);

void setup() {
  // put your setup code here, to run once:
  pinMode(LOCK_SWITCH, INPUT_PULLUP);
  pinMode(EXT_INTERUPT0, INPUT_PULLUP);
  pinMode(EXT_INTERUPT1, INPUT_PULLUP);
  
  pinMode(LOCK_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK_SERVO, OUTPUT);
  pinMode(EXT_PWM, OUTPUT);

  Lock.attach(LOCK_SERVO);

  lock_toggle(1);

  if(DEVMODE){
    Serial.begin(115200);
    Serial.println("Dev mode active!");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  Indication();
  //lock_toggle(-1);
  delay(1000);
  
}

void lock_toggle(int state){
  switch(state){
    case -1:
      lock_servo = !lock_servo;
      if(lock_servo){
        Lock.write(180);
      }else{
        Lock.write(0);
      }
      break;
    case 0:
      lock_servo = false;
      Lock.write(0);
      break;
    case 1:
      lock_servo = true;
      Lock.write(180);
      break;
    default:
      if(DEVMODE){
        Serial.println("State ID not known (lock_toggle)");
      }
      break;
  }
}

void Indication(){
  if(lock_servo){
    digitalWrite(LOCK_LED, HIGH);
  }else{
    digitalWrite(LOCK_LED, LOW);
  }
}
