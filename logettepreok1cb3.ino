TaskHandle_t Task1;
TaskHandle_t Task2;


#include "core1.h"
#include "core2.h"

void setup() {
   delay(1000);
  Serial.begin(115200); 
  SerialAT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial2.begin(9600);
  
  modemRestart();
  pinMode(25,OUTPUT);
  digitalWrite(25,HIGH);
  pinMode(4, OUTPUT);
  secure_layer.setCACert(root_caa);
  pinMode(PIN_DS, OUTPUT);
  pinMode(PIN_STCP, OUTPUT);
  pinMode(PIN_SHCP, OUTPUT); 
  clearRegisters();
  writeRegisters();
 
  pinMode(DHTPIN,INPUT);
  pinMode(Pinfraude,INPUT);

  pinMode(2,OUTPUT);
  digitalWrite(2,1);
  pinMode(15,INPUT);
  
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,1);
  lcd.print("KIM ENGINEERING");
  lcd.setCursor(2,1);
  lcd.print("KIM BOX PREPAYEE");
  dht.begin(); 
 
    setRegisterPin(8, HIGH);
    setRegisterPin(6, HIGH);
    setRegisterPin(10, HIGH);
   
    writeRegisters();
    delay(1000);
    
    setRegisterPin(6, LOW);
    setRegisterPin(8, LOW);
    setRegisterPin(10, LOW);
    writeRegisters();
 
   if (!fram.begin(0x50)) {
    Serial.println("Erreur de connexion à la FRAM");
   // while (1);
  }

   delay(1000);
  
  
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
  capteurs();
  puissancefunction();
  etatfunction(); 
  compteur();
  buzzer();
  commutation();
  lcdfunction ();
  if(int(progress)==0){
  sleep();}
   
  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
   
    
     Serial.print("annulation : ");Serial.println(annulation);
     preferences.begin("serial_storage", false);
     validation=preferences.getUInt("validation",0);
     //preferences.putUInt("validation",0);
     preferences.end();
  Serial.print("validation : ");Serial.println(validation);
 if(validation==0){
     connexion();
     setupOTA(); 
     }
     if(validation==1){
     preferences.begin("serial_storage", false);
     annulation=preferences.getUInt("annulation",0);
     preferences.end();
       if(annulation==0){
      pzem2.resetEnergy(); pzem1.resetEnergy(); pzem3.resetEnergy();
      int zero=0;
      
 fram.write(0x0100, (uint8_t*)&zero, sizeof(zero));  fram.write(0x0200, (uint8_t*)&zero, sizeof(zero));
 fram.write(0x0300, (uint8_t*)&zero, sizeof(zero));  fram.write(0x0400, (uint8_t*)&zero, sizeof(zero)); 
 fram.write(0x0500, (uint8_t*)&zero, sizeof(zero));  fram.write(0x0600,0); 
 fram.write(0x0700, (uint8_t*)&zero, sizeof(zero));  fram.write(0x0750, (uint8_t*)&zero, sizeof(zero)); 
 fram.write(0x0800, 0);  fram.write(0x0900,0);  
 fram.write(0x1000,0);  fram.write(0x1100, (uint8_t*)&zero, sizeof(zero)); 

   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
   etatlogette=fram.read(0x0600);
   if(eng1==0 && eng2==0 && eng3==0 && energerestant==0 && energietotale==0 && etatlogette==0){
     preferences.begin("serial_storage", false);
     preferences.putUInt("annulation",1);
     preferences.end();
   }
     }
     connexion1();
     setupOTA1(); 
     }
   
       
  }
}
  
void loop() {
 
}
