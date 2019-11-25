/* -- Lib reference --//
 * Modbus:    https://github.com/andresarmento/modbus-arduino
 * CAN:       https://github.com/sandeepmistry/arduino-CAN
 * EEPROM:    Internal EEPROM lib
 * Servo:     Internal servo lib
 * Interupt:  Internal interupt lib
 */

/*-- pseudo code --//
 * Inputs:
 * PWM inputs 2x 
 * CAN bus input (SPI)
 * RS485 bus (RX, TX)
 * 
 * Outputs:
 * PWM 1x (servo type)
 * Servo 1x
 * LED 1x
 * Buzzer 1x
 * 
 * Functions:
 * Digital_input(){
 *  lees op CHANGE interupt basis de Digitale inputs in en
 *  sla dit op in een volitaile bool variable.
 * }
 * 
 * PWM_output(){
 *  if(PWM signaal van lock servo > 1500 + deadBand){
 *    zet de lock servo PWM pin naar 2000ms (180 graden voor servo lib)
 *  }
 *  if(PWM signaal van lock servo < 1500 - deadBand){
 *    zet de lock servo PWM pin naar 1000ms (0 graden voor servo lib)
 *  }
 *  zet de PWM van de output PWM_aux naar de waarde van input PWM_aux
 * }
 * Indication(){
 *  indicatie types:
 *  / = 0.2s
 *  * = 0.5s
 *  - = 1s
 * 
 * 
 *  if(!Error_flag){
 *    if(lock servo == gesloten){
 *      zet lock LED aan
 *      laat de buzzer 1x / aangaan
 *    }else{
 *      zet lock LED uit
 *      laat de buzzer 1x / aangaan
 *    }
 *    if(lock servo == gesloten){
 *      zet lock LED aan
 *      laat de buzzer 1x / aangaan
 *    }else{
 *      zet lock LED uit
 *      laat de buzzer 1x / aangaan
 *    }
 *  }else{
 *    switch(error type){
 *      case 0: // CAN bus
 *        switch(error num){
 *          case 0: //CAN not OK
 *            laat led met **** interval  knipperen
 *            en de buzzer ook opdeze interval aangaan
 *          break;
 *          ...
 *        }
 *      break;
 *      case 1: // RS485 bus
 *        switch(error num){
 *          case 0: //RS485 not OK
 *            laat led met -*** interval  knipperen
 *            en de buzzer ook opdeze interval aangaan
 *          break;
 *          ...
 *        }
 *      break;
 *      default:  // Error type bestaat niet of is niet herkent
 *        laat led met //// interval  knipperen
 *        en de buzzer ook op deze interval aangaan maar dan geinverteerd
 *      break;
 *    }
 *  }
 * }
 * 
 * -- interupt free --
 * disableInterupts()
 * EEPROM_read(addr, data){
 *    scrijf data naar register op het gegeven addr.
 * }
 * 
 * EEPROM_write(addr){
 *    lees de infoud van addr en return dit.
 * }
 * 
 * CAN_Read(){
 *  Lees de CAN bericht ID.
 *  while(CAN beschikbaar){
 *    Lees de CAN bus informatie
 *  }
 * }
 * 
 * CAN_Write(ID){
 *  Write een bericht naar de can bus met het gegeven ID.
 *  
 * }
 * 
 * RS485_Read(){
 *  kijk op de rs485 bus of er registers en coils zijn aangepast.
 *  zo ja neem dit over in een locale array
 * }
 * 
 * RS485_write(){
 *    Update the local holding registers and coils
 * }
 * Error_Cont(){
 * 
 *    Set de Error flag array waardes naar true voor de speciefike error
 *    
 *    While(true){
 *      Roep indicatoren() op.
 *    }
 * }
 * enableInterupts()
 * -------------------
 * 
 * -----------------*/


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
