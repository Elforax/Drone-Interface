/* -- Lib reference --//
 * Modbus:    https://github.com/andresarmento/modbus-arduino
 * CAN:       https://github.com/sandeepmistry/arduino-CAN
 * EEPROM:    Internal EEPROM lib
 * Servo:     Internal servo lib
 * Interupt:  Internal interupt lib
 */
 
#include <Servo.h>
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
bool state_change = false;

//CAN bus variables
unsigned int msg_id = 0xFFF;
unsigned int packet_size = 0;
byte Data[8];

long debouncing_time = 50; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

Servo Lock;
Servo Ext_servo;

void(* resetFunc) (void) = 0;

void setup() {
  if(DEVMODE){
    Serial.begin(115200);
    Serial.println("Dev mode active!");
  }

  if (!CAN.begin(500E3) && DEVMODE) {
    Serial.println("Starting CAN failed!");
  }
  
  //Pin mode setup
  pinMode(LOCK_SWITCH, INPUT_PULLUP);
  pinMode(EXT_INTERUPT0, INPUT_PULLUP);
  pinMode(EXT_INTERUPT1, INPUT_PULLUP);
  
  pinMode(LOCK_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK_SERVO, OUTPUT);
  pinMode(EXT_PWM, OUTPUT);

  // servo objectes
  Lock.attach(LOCK_SERVO);
  Ext_servo.attach(EXT_PWM);

  lock_toggle(1);
  ext_servo_control(0, false);
  can_write(0xF00);
  delay(10);
  interupt_activate();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(state_change){
    Indication();
    state_change = false;
  }
  noInterrupts();
  can_controller();
  interrupts();
}


void can_controller(){
  can_read();
  if(packet_size > 0){
    if(0x100 <= msg_id && msg_id <= 0x109){
      can_request();
    }
    if(0x110 <= msg_id && msg_id <= 0x120){
      can_input();
    }
  }
}

void can_request(){
  switch(msg_id){
    case 0x100:
      //Lock status
      can_write(0x090);
      break;
    case 0x101:
      //Current Ext_PWM value
      can_write(0x091);
      break;
    case 0x102:
      //EEPROM register read
      can_write(0x092);
      break;
    case 0x103:
      //Indicator status
      can_write(0x093);
      break;
    case 0x104:
      // retrive the error list;
      break;
  }
}

void can_input(){
    switch(msg_id){
    case 0x110:
      if(Data[0] == 0 || Data[0] == 1){
        lock_toggle(Data[0]);
      }
      break;
    case 0x111:
      if(0 <= Data[0] && Data[0] <= 255){
        int angle = map(Data[0], 0, 255, 0, 180);
        ext_servo_control(angle, false);
      }
      break;
    case 0x112:
      if(0 <= Data[0] && Data[0] <= 180){
        ext_servo_control(Data[0], false);
      }
      break;
    case 0x113:
      if(Data[0] == 0){
        ext_servo_control(Data[0], false);
      }
      if(Data[0] == 1){
        ext_servo_control(180, false);
      }
      break;
    case 0x114:
      // Writing EEPROM data
      break;
    case 0x115:
      int time_ms = 0;
      for(int i=0; i<sizeof(Data);i++){
        time_ms += Data[i];
      }
      buzzer_active(time_ms);
      break;
    case 0x116:
      // for error clearing
      break;
    case 0x117:
      resetFunc(); // resets the arduino
      break;
  }
}

void can_write(unsigned int id){
  Serial.print("Sending id 0x");
  Serial.println(id, HEX);
  switch(id){
    case 0x090:
      //Lock status response
      CAN.beginPacket(id);
      CAN.write(lock_servo);
      CAN.endPacket();
      break;
    case 0x091:
      //Current Ext_PWM value response
      CAN.beginPacket(id);
      CAN.write(Ext_servo.read());
      CAN.endPacket();
      break;
    case 0x092:
      //EEPROM register response
      CAN.beginPacket(id);
      CAN.write('0');
      CAN.endPacket();
      break;
    case 0x093:
      //EEPROM register response
      CAN.beginPacket(id);
      CAN.write(digitalRead(14));
      CAN.write(digitalRead(2));
      CAN.write(digitalRead(6));
      CAN.write(digitalRead(16));
      CAN.write(digitalRead(18));
      CAN.endPacket();
      break;
    case 0xF00:
      Serial.println("Starting packet");
      //Arduino online
      CAN.beginPacket(0xF00);
      CAN.write('S');
      CAN.write('A');
      CAN.write('X');
      CAN.write('I');
      CAN.write('O');
      CAN.write('N');
      CAN.endPacket();
      Serial.println("Ending packet");
      break;
  }
  delay(100);
}

void can_read(){
  packet_size = CAN.parsePacket();
  //Serial.println(packet_size);

  if (packet_size) {
    // received a packet
    Serial.print("Received ");

    CAN.packetExtended();
    CAN.packetRtr();

    Serial.print("packet with id 0x");
    msg_id = CAN.packetId();
    Serial.print(msg_id, HEX);

    if (CAN.packetRtr()) {
      CAN.packetDlc();
    } else {
      Serial.print(" and length ");
      Serial.println(packet_size);
      
      int i = 0;
      while (CAN.available()) {
        if(i < sizeof(Data)){
          Data[i] = CAN.read();
          Serial.print(Data[i]);
        }
      }
      Serial.println();
    }
    
    if(packet_size != 8){
      for(int i = packet_size-1; i<sizeof(Data); i++){
        Data[i] = 0;
      }
    }
    Serial.println();
  }
}

void interupt_activate(){
  PCICR |= (1 << PCIE2);        //Sets pin change interupt for PORTD to true
  PCMSK2 |= (1 << PCINT22);     //adds in D6 to the interupt vector PCINT2
  PCMSK2 |= (1 << PCINT18);     //adds in D2 to the interupt vector PCINT2

  PCICR |= (1 << PCIE1);        //Sets pin change interupt for PORTC to true
  PCMSK1 |= (1 << PCINT8);      //adds in A0 (D14) to the interupt vector PCINT1
}

ISR(PCINT1_vect){
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    if(!digitalRead(14)){  //14 = A0
      lock_toggle(-1);
    }
    last_micros = micros();
  }
}

ISR(PCINT2_vect){
  Serial.println("interupt 2");
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    if(!digitalRead(EXT_INTERUPT0)){
      ext_servo_control(0, true);
    }
    if(!digitalRead(EXT_INTERUPT1)){
      ext_servo_control(0, true);
    }
    last_micros = micros();
  }
}

void ext_servo_control(int angle, bool is_sw){
  if(is_sw){
    if(Ext_servo.read() <= 90){
      Ext_servo.write(150);
    }else{
      Ext_servo.write(0);
    }
  }else{
    if(0 <= angle && angle <= 180){
      Ext_servo.write(angle);
    }
  }
}

void lock_toggle(int state){
  int old_lock = lock_servo;
  switch(state){
    case -1:
      lock_servo = !lock_servo;
      if(lock_servo){
        Lock.write(150);
      }else{
        Lock.write(0);
      }
      break;
    case 0:
      Lock.write(0);
      lock_servo = false;
      break;
    case 1:
      Lock.write(150);
      lock_servo = true;
      break;
    default:
      if(DEVMODE){
        Serial.println("State ID not known (lock_toggle)");
      }
      break;
  }

  if(old_lock != lock_servo){
    state_change = true;
  }
  
  if(DEVMODE){
    Serial.print("Lock servo state: ");
    Serial.print(lock_servo);
    Serial.println(" (lock_toggle)");
  }
}

void Indication(){
  if(lock_servo){
    digitalWrite(LOCK_LED, HIGH);
    buzzer_active(200);
  }else{
    digitalWrite(LOCK_LED, LOW);
    buzzer_active(200);
  }
}

void buzzer_active(int ms){
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW); 
}
