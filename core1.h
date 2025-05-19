
#include <Preferences.h>
Preferences preferences;
// la partie  registre a decalage 
#define PIN_DS 12   //pin 14  75HC595    
#define PIN_STCP 18 //pin 12  75HC595
#define PIN_SHCP 5//pin 11  75HC595
#define numberOf74hc595 2 
#define numOfRegisterPins numberOf74hc595 * 8
boolean registers[numOfRegisterPins];
int ousomme;
 int aaaa=0;
 int verification;
 float progress=0;
 int proc1,proc2;
 int procee=1;
 bool buzz=LOW;
 int resetesp=0;
 int comptour=0;
 int etatpuissance;
 int etatweb;
 int etatphase1,etatphase2,etatphse3;
  int a,b,c;
 

unsigned long previousMillis = 0; 
const long interval = 100; // intervalle post 
  int cca2,cca3,cca1a,cca2a,cca3a,cca1;

void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 
void writeRegisters(){
  digitalWrite(PIN_STCP, LOW);
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(PIN_SHCP, LOW);
    int val = registers[i];
    digitalWrite(PIN_DS, val);
    digitalWrite(PIN_SHCP, HIGH);

  }
  digitalWrite(PIN_STCP, HIGH);

}
void setRegisterPin(int index, int value){
  registers[index] = value;
}
// entete pour le capteur dht brache au gpio 0

#include <DHT.h>             // librarie
#define DHTPIN 0           // broche
#define DHTTYPE DHT11       //type donc dht11
DHT dht(DHTPIN,DHTTYPE);  // configuration

int temperature;
int humidite;

//phase

bool phase;

// fraude
int  fraude;

int Pinfraude=34;

// entete pour le capteur pzem
#include <HardwareSerial.h>

HardwareSerial MySerial(2); 

#include <PZEM004Tv30.h>
#define PZEM_RX_PIN 23
#define PZEM_TX_PIN 19


PZEM004Tv30 pzem1(Serial2,PZEM_RX_PIN,PZEM_TX_PIN,0x4);
PZEM004Tv30 pzem2(Serial2,PZEM_RX_PIN,PZEM_TX_PIN,0x15);
PZEM004Tv30 pzem3(Serial2,PZEM_RX_PIN,PZEM_TX_PIN,0x30);

float energie1w,energie2w,energie3w,courant1,courant2,courant3,facteurPuissance1,facteurPuissance2,facteurPuissance3;
int   energie1,energie2,energie3,tension1,tension2,tension3,puissance1,puissance2,puissance3,frequence1,frequence2,frequence3;

int tensionw1,tensionw2,tensionw3,tensionw1a,tensionw2a,tensionw3a;
int selectTension1=1; int selectTension2=1;int selectTension3=1;
int nbEnergie1,nbEnergie2,nbEnergie3;
int nbP1,nbP2,nbP3;
int puissanceb1,puissanceb2,puissanceb3;
float energieb1,energieb2,energieb3;
float ea;
float ea1,ea2;
int n=0;
int energie1n,energie2n,energie3n,energie1an,energie2an,energie3an=0;
int t1,t2,t3;
int commutationvalide=0;


  int puissancesouscrite;
// variable intermediare pour la commutaion
bool b1,c1,a2,b2,c2,l1,l2=0;
bool a1=1;
bool enn1,enn2;
bool phase1,phase2,fraude1,fraude2;
int actuelle,precedent;
int nvariable=2;
int ncompte;

// entete pour lcd
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0X27,20,4);
// les variables tempo pour reset l ecran lcd en fonction des caracters de la puissance et de 3 tensions
int cp1,cp2,cten1,cten2,cten1a,cten2a,cten3=0;
int cten3a=1;

// variable permettant de stocker la valeur de energie une seile fois 
int sans=3;
// sleep variable
int slp=0;

int temps=1000;
int tempc=3000;
// memmoire eeprom

#include <Adafruit_FRAM_I2C.h>
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
float eng1,eng2,eng3;
float energietotale;
float energerestant;
int etatlogette;
bool statutenergie;
void puissancefunction(){
 
  fram.read(0x0750, (uint8_t*)&puissancesouscrite, sizeof(1));
  etatpuissance = fram.read(0x0800);
 
  Serial.print("puissance souscrite : ");Serial.println(puissancesouscrite);
 
  int puissancemax=puissance1+puissance2+puissance3;
 
  bool ten1=tension1;
  bool ten2=tension2;
  bool ten3=tension3;
  int addition=ten1+ten2+ten3;

  if(addition==3){
    if(puissancemax>=puissancesouscrite){
     etatpuissance=0;
    }
  else{
    etatpuissance=1;
  }
  }

  
  if(addition==2){
    if(puissancemax>=(puissancesouscrite*0.66)){
      etatpuissance=0;
    }
  else{
     etatpuissance=1;
  }
  }

  
  if(addition==1){
    if(puissancemax>=(puissancesouscrite*0.4)){
     etatpuissance=0;
    }
  else{
     etatpuissance=1;
  }
  }Serial.println(".............................................");
  Serial.print("puissance souscrite : ");Serial.println(puissancesouscrite);
  Serial.print("puissance max : ");Serial.println(puissancemax);
    Serial.print("addition :"); Serial.println(addition);
   Serial.print(" etatpuissance :"); Serial.println( etatpuissance);
   Serial.println(".............................................");
  fram.write(0x0800,etatpuissance);
}
void capteurs(){
  
   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
   etatlogette=fram.read(0x0600);
  

   Serial.println("........................................");
   Serial.print(" energie 1 cap  : "); Serial.println(pzem1.energy());
   Serial.print(" energie 2  cap : "); Serial.println(pzem2.energy());
   Serial.print(" energie 3 : cap  "); Serial.println(pzem3.energy());

   Serial.println("........................................");
// la partie dht
 
   if(isnan(dht.readTemperature())){
         temperature=0;
        } 
        else{
         temperature=dht.readTemperature();
        }
        if(isnan(dht.readHumidity())){
         humidite=0;
        } 
        else{
         humidite=dht.readHumidity();
      }
     
     
                //la partie pzem 
// tension
 
  //les tensiow a envoyé au web : tensionw
  if(isnan(pzem1.voltage())) 
    {
      tensionw1a=0; 
    } 
  else{
      tensionw1a=pzem1.voltage();
      }
      
  if(isnan(pzem2.voltage())) 
    {
      tensionw2a=0; 
    } 
  else{
      tensionw2a=pzem2.voltage();
      }
  if(isnan(pzem3.voltage()))
    {
      tensionw3a=0; 
    } 
   else{
      tensionw3a=pzem3.voltage();
      
   }
    String nbtp1=String(tensionw1a);
    String nbtp2=String(tensionw2a);
    String nbtp3=String(tensionw3a);

    int nbt1=nbtp1.length();
    int nbt2=nbtp2.length();
    int nbt3=nbtp3.length();
     if(nbt1>=4){ 
    }
    else{
    tensionw1=tensionw1a;
    }
      if(nbt2>=4){ 
    }
    else{
    tensionw2=tensionw2a;
    }
      if(nbt3>=4){ 
    }
    else{
    tensionw3=tensionw3a;
    }
   // phase
   if((tensionw1>280) ||(tensionw2>280) || (tensionw3>280)){
    phase=1;
   }
   else{
    phase=0;
   }
  resetesp =fram.read(0x1000);
  
  Serial.print("n : "); Serial.print(n);
  Serial.print("resetesp: "); Serial.print(resetesp);
   
  if((n==0) && (resetesp!=4)){
    if((tensionw1==0) && (tensionw2==0) && (tensionw3==0)){
      resetesp=4;
        fram.write(0x1000,resetesp);
       delay(100);
       Serial.println(" on a restart ");
      // ESP.restart();
       
    }
  }
  if(n!=0){
      resetesp=3;
   fram.write(0x1000,resetesp);
  } 
  
  // tensions pour la commutation , tensions filtrées
 
  if((isnan(pzem1.voltage())) || (pzem1.voltage()>260.0) || (pzem1.voltage()<80.0)|| (selectTension1==0))
    {
      tension1=0; 
      etatphase1=100;
     
    } 
  else{
      tension1=pzem1.voltage();
      etatphase1=0; 
      }
      
  if((isnan(pzem2.voltage())) || (pzem2.voltage()>260.0) || (pzem2.voltage()<80.0)|| (selectTension2==0))
    {
      tension2=0; 
      etatphase2=10;
      
    } 
  else{
      tension2=pzem2.voltage();
      etatphase2=0;
     
      }
  if((isnan(pzem3.voltage())) || (pzem3.voltage()>260.0)|| (pzem3.voltage()<80.0) || (selectTension3==0))
    {
      tension3=0; 
      etatphse3=1;
    } 
   else{
      tension3=pzem3.voltage();
      etatphse3=0;
      }

//energie

 if(isnan(pzem1.energy()))
    {
      energie1=0; 
      energie1w=0; 
     } 
  else{
      energie1=pzem1.energy();
      energie1w=pzem1.energy();
      }
  if(isnan(pzem2.energy()))
    {
      energie2=0; 
      energie2w=0; 
    } 
  else{
      energie2=pzem2.energy();
      energie2w=pzem2.energy();
      }
  if(isnan(pzem3.energy()))
    {
      energie3=0;
      energie3w=0;
    } 
   else{
          energie3=pzem3.energy();
          energie3w=pzem3.energy();
       }
    String nbE1=String(energie1);
    String nbE2=String(energie2);
    String nbE3=String(energie3);

    nbEnergie1=nbE1.length();
    nbEnergie2=nbE2.length();
    nbEnergie3=nbE3.length();

    if(n % 2==0){
      energie1n=energie1;
      energie2n=energie2;
      energie3n=energie3;
    }
    else{
      energie1an=energie1;
      energie2an=energie2;
      energie3an=energie3;
    }
    int diffenergie1=abs(energie1n-energie1an);
    int diffenergie2=abs(energie2n-energie2an);
    int diffenergie3=abs(energie3n-energie3an);
   
    if(n==1000000){
      n=0;
    }
 
   // stockage energie dans eeprom

   if((nbEnergie1<5)&&(diffenergie1<10)&&(tension1<260)&&(tension1>80))
   {
    
    energieb1=energie1w;
 
  fram.write(0x0100, (uint8_t*)&energieb1, sizeof(energieb1));  
  delay(500);
  Serial.print(" en 1 ecrit :");Serial.println(energieb1);
   }
   if((nbEnergie2<5)&&(diffenergie2<10)&&(tension2<260)&&(tension2>80))
   {
    energieb2=energie2w;
   
   fram.write(0x0200, (uint8_t*)&energieb2, sizeof(1)); 
   delay(500);
    Serial.print(" en 2 ecrit :");Serial.println(energieb2);
    
   }
   if((nbEnergie3<5)&&(diffenergie3<10)&&(tension3<260)&&(tension3>80))
   {
    energieb3=energie3w;
  
     fram.write(0x0300, (uint8_t*)&energieb3, sizeof(1)); 
     delay(500);
      Serial.print(" en 3 ecrit :");Serial.println(energieb3);
   }
        

//courant 

  if(isnan(pzem1.current()))
    {
      courant1=0; 
    } 
  else{
      courant1=pzem1.current();
          }
          
  if(isnan(pzem2.current()))
    {
      courant2=0; 
    } 
    else{
      courant2=pzem2.current();
          }
          
    if(isnan(pzem3.current()))
    {
      courant3=0; 
    } 
    else{
      courant3=pzem3.current();
          }

  // frequence

   if(isnan(pzem1.frequency()))
    {
      frequence1=0; 
    } 
    else{
      frequence1=pzem1.frequency();
          }
          
   if(isnan(pzem2.frequency()))
    {
      frequence2=0; 
    } 
    else{
      frequence2=pzem2.frequency();
          }
           
   if(isnan(pzem3.frequency()))
    {
      frequence3=0; 
    } 
    else{
      frequence3=pzem3.frequency();
          }

// puissance

    if(isnan(pzem1.power()))
     {
      puissance1=0; 
     } 
    else{
     puissance1=pzem1.power();
     }
          
    if(isnan(pzem2.power()))
     {
      puissance2=0;
     } 
    else{
     puissance2=pzem2.power();
     }
    if(isnan(pzem3.power()))
     {
      puissance3=0; 
      } 
    else{
      puissance3=pzem3.power();
      }

    String nbpp1=String(puissance1);
    String nbpp2=String(puissance2);
    String nbpp3=String(puissance3);

    nbP1=nbpp1.length();
    nbP2=nbpp2.length();
    nbP3=nbpp3.length();
    if(nbP1>6){
      
    }
    else{
      puissanceb1=puissance1;
    }
     if(nbP2>6){
      
    }
    else{
      puissanceb2=puissance2;
    }
     if(nbP3>6){
      
    }
    else{
      puissanceb3=puissance3;
    }
 // facteur de puissance
  if(isnan(pzem1.pf()))
    {
      facteurPuissance1=0; 
    } 
   else{
       facteurPuissance1=pzem1.pf();
       }
   if(isnan(pzem2.energy()))
      {
      facteurPuissance2=0; 
      } 
    else{
          facteurPuissance2=pzem2.pf();
        }
          
    if(isnan(pzem3.pf()))
      {
        facteurPuissance3=0; 
      } 
    else{
        facteurPuissance3=pzem3.pf();
        } 
  Serial.print("n a : ");Serial.println(n); 
       n++;    
     Serial.print("n: ");Serial.println(n); 
         
  Serial.print("energie1: ");Serial.println(energie1);  Serial.print("courant: ");Serial.println(courant1);
 Serial.print("puissance1: ");Serial.println(puissanceb1);  Serial.print("eng1: ");Serial.println(eng1);
 Serial.print("tensionw3: ");Serial.println(tensionw3);  Serial.print("tension3: ");Serial.println(tension3);
   
  Serial.print("tension 3 cap: ");Serial.println(pzem3.voltage());  
  delay(1000);     
}


void lcdfunction (){
    Serial.print("tension 1 cap: ");Serial.println(pzem1.voltage());  
    Serial.print("tension 2 cap: ");Serial.println(pzem2.voltage());  
    Serial.print("tension 3 cap: ");Serial.println(pzem3.voltage()); 
   
  fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
  int puissancet=puissanceb1+puissanceb2+puissanceb3;
   
 Serial.print("progress : "); Serial.println(progress);
  
  String cara;
  int energielcd=energerestant;
  String pstr=String(puissancet);
  String aaa=String(tensionw1);
  String bbb=String(tensionw2);
  String ccc=String(tensionw3);


   
  if((selectTension1==0) && (tensionw1!=0)){
    cara="x";
  }
  else{
    cara="";
  
  }

    String cara2;
  if((selectTension2==0)&& (tensionw2!=0)){
    cara2="x";
  }
  else{
    cara2="";
   
  }

    String cara3;
  if((selectTension3==0)&& (tensionw3!=0)){
    cara3="x";
  }
  else{
    cara3="";
   
  }
  cca1=cca1a; cca2=cca2a; cca3=cca3a; cca1a=selectTension1; cca2a=selectTension2; cca3a=selectTension3; 
 Serial.print(cca1);Serial.print( cca1==cca1a);Serial.print(" selecte1 : "); ;Serial.println(selectTension1);
 
    if(cp1==cp2 && cten1==cten1a && cten2==cten2a && cten3==cten3a && cca1==cca1a && cca2==cca2a && cca3==cca3a && proc1==proc2 ){
   Serial.println("cp1 == cp2");
     
  }else{
    lcd.init();
  }
     cp1=cp2 ;
     cten1=cten1a;
     cten2=cten2a;
     cten3=cten3a;  
     cp2=pstr.length();
     cten1a=aaa.length();
     cten2a=bbb.length();
     cten3a=ccc.length();
    if(progress==0){
    lcd.setCursor(6,0);
    lcd.print("KIM BOX");
    lcd.setCursor(0,1);
    lcd.print("L1=");
    lcd.print(tensionw1);
    lcd.print("V");
    lcd.setCursor(7,1);
    lcd.print(cara);

    lcd.setCursor(9,1);
    lcd.print("T=");
    lcd.print(temperature);
    lcd.print("C");
    lcd.setCursor(15,1);
    lcd.print("H=");
    lcd.print(humidite);
    lcd.print("%");

    lcd.setCursor(0,2);
    lcd.print("L2=");
    lcd.print(tensionw2);
    lcd.print("V");
    lcd.setCursor(7,2);
    lcd.print(cara2);
    
    lcd.setCursor(9,2);
    lcd.print("E=");
    lcd.print(energerestant,1);
    lcd.print("KWH");
    lcd.setCursor(0,3);
    lcd.print("L3=");
    lcd.print(tensionw3);
    lcd.print("V");
    lcd.setCursor(7,3);
    lcd.print(cara3);
    
    lcd.setCursor(9,3);
    lcd.print("P=");
    lcd.print(puissancet);
    lcd.print("W");
    procee=1;}
    else{
      if(procee==1){
   lcd.init();
   lcd.backlight();
  }
    lcd.setCursor(6,0);
    lcd.print("KIM BOX");
    lcd.setCursor(0,2);
    lcd.print("Mise a jour: ");
    lcd.print(progress);
    lcd.print("%");
    procee=3;
    }
    proc1=proc2;
    if(progress==0){
      proc2=4;
    }
    else{
      proc2=2;
    }
}


void compteur(){
  Serial.print("tensionw3: ");Serial.println(tensionw3);  Serial.print("tension3: ");Serial.println(tension3);
 Serial.print("courant1: ");Serial.print(courant1);Serial.print("  courant2: ");Serial.print(courant2);
 Serial.print("  courant3: ");Serial.println(courant3);
   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
   etatlogette=fram.read(0x0600);
  
   Serial.println("........................................");
   Serial.print(" energie 1 : "); Serial.println(eng1);
   Serial.print(" energie 2 : "); Serial.println(eng2);
   Serial.print(" energie 3 : "); Serial.println(eng3);
   Serial.print(" energie RE : "); Serial.println(energerestant);
   Serial.print(" energie to : "); Serial.println(energietotale);
   Serial.print(" etat  : "); Serial.println(etatlogette);
   Serial.println("........................................");
 
  if(energietotale!=0){
    Serial.println("et existe ");
    if(eng1==0 && eng2==0 && eng3 ==0 && energerestant==0){
      energerestant=energietotale;
      fram.write(0x0400, (uint8_t*)&energerestant, sizeof(energerestant)); 
    }
   else{
    if(eng1!=0 || eng2!=0 || eng3 !=0 ){
      energerestant=energietotale-(eng1+eng2+eng3);
      Serial.println("normal");
      
      if(energerestant<0){
        energerestant=0;
      }
       fram.write(0x0400, (uint8_t*)&energerestant, sizeof(energerestant)); 
       delay(500);
        sans=3;
    }
    
       if(eng1==0 && eng2==0 && eng3 ==0 ){
      if(sans==1){
        energerestant=energietotale;
       fram.write(0x0400, (uint8_t*)&energerestant, sizeof(energerestant)); 
       delay(500);
       }
      if(sans==3){
       energietotale=energerestant;
      
      fram.write(0x0500, (uint8_t*)&energietotale, sizeof(energietotale)); 
      delay(500);
        sans=1;
       }
     }
    }
  }
  else{
     Serial.println("et  n existe  pas");
  }
  
}


void sleep(){
  if((tension1==0)&&(tension2==0) &&(tension3==0) ){
    slp++;
  }
  else{
    slp=0;
  }
  
  if(slp>45){
   digitalWrite(4,0); 
   delay(200); 
   digitalWrite(4,1);

  }
   if(slp>47){
   digitalWrite(2,0);
    }
  if(slp>49){

 esp_sleep_enable_ext0_wakeup(GPIO_NUM_15,HIGH);  
 int ret = esp_light_sleep_start();
 delay(200);
 esp_deep_sleep_start();
 
 Serial.print("light_sleep:");  
 Serial.println(ret);
 slp=0;
 }
 if(slp<45){
    digitalWrite(2,1);
  } 
   
  Serial.print(" broche 15 : ");Serial.println(digitalRead(15));
 }
void buzzer(){
  Serial.print("tensionw3: ");Serial.println(tensionw3);  Serial.print("tension3: ");Serial.println(tension3);
 Serial.print("courant1: ");Serial.print(courant1);Serial.print("  courant2: ");Serial.print(courant2);
 Serial.print("  courant3: ");Serial.println(courant3);
  int essai;
   fraude=digitalRead(34);
   
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   Serial.print("energie restante buzzer : ");Serial.println(energerestant);
   int ennner=energerestant;
   if(ennner<=1){
    essai=2;
    setRegisterPin(13,HIGH);
    writeRegisters();
    delay(200);
    setRegisterPin(13,LOW); 
    writeRegisters();
    delay(200);
   }

   if((fraude==1)||(phase==1)){
    essai=3;
    setRegisterPin(13,HIGH);
    writeRegisters();
   }
   if(fraude==0 && phase==0 && ennner>1){
    setRegisterPin(13,LOW);
    writeRegisters();
   }
   
  Serial.print(essai);
 Serial.print("fraude : "); Serial.println(fraude);
}

void commutation(){
     precedent=actuelle;
     actuelle=abs(etatphase1+etatphase2+etatphse3);
    
  if((actuelle-precedent)>0){
      if((precedent==100 && actuelle==110) ||(precedent==100 && actuelle==111) ||(precedent==100 && actuelle==101)||(precedent==010 && actuelle==110) ||(precedent==010 && actuelle==111) ||(precedent==010 && actuelle==011)||
      (precedent==001 && actuelle==011) ||(precedent==001 && actuelle==111) ||(precedent==001 && actuelle==101)||(precedent==110 && actuelle==111) ||(precedent==101 && actuelle==111) ||(precedent==011 && actuelle==111))
     {
      commutationvalide=1;
     }
     else{
      commutationvalide=0;
     }
  }
  if((actuelle-precedent)<0){
    commutationvalide=0;
  }
  if(commutationvalide==1){
    ncompte++;
    if(ncompte>=60){
   a=tension1;  b=tension2; c=tension3;
    ncompte=0;
    }
  }
  if(commutationvalide==0){
   a=tension1;  b=tension2; c=tension3;
    if(ncompte>=40){
   a=tension1;  b=tension2; c=tension3;
    ncompte=0;
    }
   
  }
  if(ncompte>10000){
    ncompte=0;
  }
 
  Serial.print("tensionw3: ");Serial.println(tensionw3);  Serial.print("tension3: ");Serial.println(tension3);
 Serial.print("courant1: ");Serial.print(courant1);Serial.print("  courant2: ");Serial.print(courant2);
 Serial.print("  courant3: ");Serial.println(courant3);
  Serial.print(" sleep : "); Serial.println(slp);
   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
   etatlogette=fram.read(0x0600);
  
  
 float somme=eng1+eng2+eng3;
 int zero=0;
  
   
   int counter=etatlogette; float enn;
   Serial.println(energerestant); Serial.println(energietotale);Serial.println(somme);
   Serial.print(" avant : ");Serial.print(counter);Serial.print(phase);Serial.println(fraude);

    buzzer();
    if(a1==a2 && b1==b2 && c1==c2 && l1==l2 && phase1==phase2 && fraude1==fraude2){
   Serial.println("merci egale ");
   comptour=0;
   ousomme=1;
   
     
  }else{
    Serial.println("pas egale ");
  
    ousomme=2;
        if((a>80&b>80&c>80)&&(counter==1)&& (phase==0) && (fraude==0)){
    setRegisterPin(4, HIGH);
    setRegisterPin(2, HIGH);
    writeRegisters(); 
    delay(temps);
    setRegisterPin(4, LOW);
    setRegisterPin(2, LOW);
    writeRegisters();  
    
    delay(temps);
    setRegisterPin(9, HIGH);
    setRegisterPin(7, HIGH);
    setRegisterPin(5, HIGH);
    writeRegisters(); 
    delay(temps);
    setRegisterPin(9, LOW);
    setRegisterPin(7, LOW);
    setRegisterPin(5, LOW);
    writeRegisters();
    
  }
   if((a>80&b>80&c<=80)&&(counter==1)&& (phase==0) && (fraude==0)){
    ousomme=2;
    setRegisterPin(6, HIGH);
    setRegisterPin(4, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(6, LOW);
    setRegisterPin(4, LOW);
    writeRegisters();
     
    delay(temps);
    setRegisterPin(9, HIGH);
    setRegisterPin(1, HIGH);
    setRegisterPin(7, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(9, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(7, LOW);
    writeRegisters();
    
  }

   if((a>80&b<=80&c>80)&&(counter==1)&& (phase==0) && (fraude==0)){
    ousomme=2;
    setRegisterPin(8, HIGH);
    setRegisterPin(2, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(8, LOW);
    setRegisterPin(2, LOW);
    writeRegisters();
    
    delay(temps);
    setRegisterPin(9, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(3, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(9, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(5, LOW);
    writeRegisters();
    
  }

   if((a<=80&b>80&c>80)&&(counter==1)&& (phase==0) && (fraude==0)){
    ousomme=2;
    setRegisterPin(10, HIGH);
    setRegisterPin(2, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(2, LOW);
    setRegisterPin(10,LOW);
    writeRegisters();
     
    delay(temps);
    setRegisterPin(3, HIGH);
    setRegisterPin(7, HIGH);
    setRegisterPin(5, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(3, LOW);
    setRegisterPin(7, LOW);
    setRegisterPin(5, LOW);
    writeRegisters();
    
  }

   if((a>80&b<=80&c<=80)&&(counter==1)&& (phase==0) && (fraude==0)){
    
    setRegisterPin(8, HIGH);
    setRegisterPin(6, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(8, LOW);
    setRegisterPin(6, LOW);
    writeRegisters();
     
    delay(temps);
    setRegisterPin(9, HIGH);
    setRegisterPin(3, HIGH);
    setRegisterPin(1, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(9, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(1, LOW);
    writeRegisters();
    
    ousomme=6;
  }

   if((a<=80&b<=80&c>80)&&(counter==1)&& (phase==0) && (fraude==0)){
    ousomme=2;
    setRegisterPin(10, HIGH);
    setRegisterPin(8, HIGH);
    writeRegisters();
    delay(temps);
     setRegisterPin(10, LOW);
    setRegisterPin(8, LOW);
    writeRegisters();
     
    delay(temps);
    setRegisterPin(3, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(1, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(3, LOW);
    setRegisterPin(5, LOW);
    setRegisterPin(1, LOW);
    writeRegisters();
  }
   if((a<=80&b>80&c<=80)&&(counter==1)&& (phase==0) && (fraude==0)){
    ousomme=2;
    setRegisterPin(10, HIGH);
    setRegisterPin(6, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(10, LOW);
    setRegisterPin(6, LOW);
    writeRegisters();
     
    delay(temps);
    setRegisterPin(1, HIGH);
    setRegisterPin(3, HIGH);
    setRegisterPin(7, HIGH);
    writeRegisters();
    delay(temps);
    setRegisterPin(1, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(7, LOW);
    writeRegisters();
  
  }

   if((a<=80&b<=80&c<=80) ||(counter==0)|| (phase==1) || (fraude==1) ){
  
    setRegisterPin(8, HIGH);
    setRegisterPin(6, HIGH);
    setRegisterPin(10, HIGH);
   
    writeRegisters();
    delay(1000);
   
    setRegisterPin(6, LOW);
    setRegisterPin(8, LOW);
    setRegisterPin(10, LOW);
    writeRegisters();
  }
  
  }
       
        a2=a1;
        b2=b1;
        c2=c1;
        l2=l1;
        phase2=phase1;
        fraude2=fraude1;
         
  
        a1=a;
        b1=b;
        c1=c;
        l1=counter;
        phase1=phase;
        fraude1=fraude;
  Serial.print(" compt ");Serial.println(comptour);
  Serial.print("commuite : ");Serial.print(a<=80&b<=80&c<=80);Serial.print(counter);Serial.print(fraude);Serial.print(phase);
  
  }


void etatfunction(){
  Serial.print("tensionw3: ");Serial.println(tensionw3);  Serial.print("tension3: ");Serial.println(tension3);
 Serial.print("courant1: ");Serial.print(courant1);Serial.print("  courant2: ");Serial.print(courant2);
 Serial.print("  courant3: ");Serial.println(courant3);
  
  etatpuissance = fram.read(0x0800);
  etatweb = fram.read(0x0900);
  fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));

  if(etatpuissance==1 && etatweb==1 && energerestant!=0 && progress==0){
    etatlogette=1;
  }
  else{
    etatlogette=0;
  }
  fram.write(0x0600,etatlogette);
  Serial.println("..............................");
  Serial.print(" etatpuissance : ");Serial.println(etatpuissance);
  Serial.print(" energerestant : ");Serial.println(energerestant);
  Serial.print(" etatweb : ");Serial.println(etatweb);
  Serial.print(" etatlogette : ");Serial.println(etatlogette);
  Serial.println("..............................");
}
