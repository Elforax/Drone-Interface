/* -- Lib reference --//
 * Modbus:    https://github.com/andresarmento/modbus-arduino
 * CAN:       https://github.com/sandeepmistry/arduino-CAN
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
 * PWM_input(){
 *  lees op CHANGE interuptbasis de pulse tijd van het inkomende PWM signaal
 *  sla dit op in een volitaile int variable
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
 * CAN_Read(){
 *  while(CAN beschikbaar){
 *    
 *  }
 * }
 * RS485_Read(){
 *  while(RS485 beschikbaar){
 *    
 *  }
 * }
 * Error_Cont(){
 *  
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
