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

bool const DEVMODE = true;          // Activeerd de serial poort voor debugging

bool lock_servo = false;            // Status van de lock servo (true = Locked; false = Open) 
bool state_change = false;          // When a status changes this will be set true

//CAN bus variables
unsigned int msg_id = 0x7FF;        // recieved Message ID
unsigned int packet_size = 0;       // recieved Packet size
byte Data[8];                       // recieved Data array of 8 bytes

long debouncing_time = 50;          // Debouncing Time in Milliseconds
volatile unsigned long last_micros; // Elapsed time while debouncing
bool interruptOn = true;            // Saves the state of the interupts routine

Servo Lock;                         // Lock servo object
Servo Ext_servo;                    // External servo object

void(* resetFunc) (void) = 0;       // Arduino internal reset routine

void setup() {
  if(DEVMODE){                      // Opens serial if dev mode is true
    Serial.begin(115200);
    Serial.println("Dev mode active!");
  }

  while (!CAN.begin(500E3)) {                 // Open CAN bus interface
    if(DEVMODE){
      Serial.println("Starting CAN failed!"); // if opening CAN fails and dev mode is true write to serial
    }
  }
  
  //Pin mode setup
  pinMode(LOCK_SWITCH, INPUT_PULLUP);
  pinMode(EXT_INTERUPT0, INPUT_PULLUP);
  pinMode(EXT_INTERUPT1, INPUT_PULLUP);
  
  pinMode(LOCK_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LOCK_SERVO, OUTPUT);
  pinMode(EXT_PWM, OUTPUT);

  // servo objectes pin attachment
  Lock.attach(LOCK_SERVO);
  Ext_servo.attach(EXT_PWM);

  lock_toggle(1);                     // Lock de Servo on startup (!! Needs to change to when the button is pressed!!)
  ext_servo_control(0, false);        // Sets the External servo to 0 degrees
  can_write(0x7FF);                   // --------- > function is incompleter needes to be finished!!!!
  delay(10);
  interupt_activate();                // Starting interrupt routines
}

void loop() {
  if(state_change){                   // Update indicators when State has changed
    Indication();
    state_change = false;
  }
  noInterrupts();                     // turn off interrupts
  interruptOn = false;                // set interrupt state too false
  can_controller();                   // 
  interrupts();                       // turn on interrupts
  interruptOn = true;                 // set interrupt state too true
}


void can_controller(){
  can_read();                                 // Read the CAN bus
  if(packet_size > 0){                        // If a package is found
    if(0x100 <= msg_id && msg_id <= 0x109){   // Test if the Message in a request
      can_request();                          // Run the request function for te CAN bus
    }
    if(0x110 <= msg_id && msg_id <= 0x120){   // Test if the Message in a input
      can_input();                            // Run the input function for te CAN bus
    }
  }
}

void can_request(){                           // Procces the CAN requests based on there Message ID
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

void can_input(){                             // Procces the CAN inputs based on there Message ID
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

void can_write(unsigned int id){              // Write Information to the CAN bus with a specific ID -------------> Needs finishing still !!!!
  if(interruptOn){
    noInterrupts();
  }
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
    case 0x7FF:
      Serial.println("Starting packet");
      //Arduino online
      CAN.beginPacket(id);
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
  if(interruptOn){
    interrupts();
  }
}

void can_read(){                              // Read the CAN bus if a message is avaliable
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
  interruptOn = true;
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
