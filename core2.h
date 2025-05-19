 #include <Arduino.h>

#define TINY_GSM_MODEM_SIM7600
#define INCOM_PIN 18
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
#define SerialAT Serial1
#define TINY_GSM_DEBUG SerialMon
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN ""
const char apn[] = "kim-apn"; 
const char gprsUser[] = "";
const char gprsPass[] = "";
//String serie= "20240916111445000100";
String serie= "20241105175928000900";
String seriep="20241111103814000200";

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <Ticker.h>

#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


Ticker tick;

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 60          // Time ESP32 will go to sleep (in seconds)




#define uS_TO_S_FACTOR      1000000ULL  
#define TIME_TO_SLEEP       10         
#define UART_BAUD           115200
#define PIN_DTR             25
#define MODEM_TX            27
#define MODEM_RX            26
#define PWR_PIN             4



int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

/// If Certificate needed https://github.com/jimmisitepu88/SSL-HTTPS-SIM7600/blob/main/ssl_test_cert/ssl_test_cert.ino
#include "Arduino.h"
#include <TinyGsmClient.h>
#include <CRC32.h>
#include <Update.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoHttpClient.h>
#include "SSLClient.h"
const char root_caa[]PROGMEM =
"-----BEGIN CERTIFICATE-----\n"
"MIIDjTCCAxKgAwIBAgISBAIpOcUTzTQVXgXnJ2SnxAMKMAoGCCqGSM49BAMDMDIx\n"
"CzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQDEwJF\n"
"MTAeFw0yMzA5MDQxNDAyMjNaFw0yMzEyMDMxNDAyMjJaMCAxHjAcBgNVBAMTFWtp\n"
"bWVuZ2luZWVyaW5nLm9ubGluZTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABPSz\n"
"UE9YkBhIiSB0dAKhc0he7hyL0aSRJBcy2AOZZI2Xp7TMb1assvzPpp89yU6Fp6dP\n"
"NWfJzN3Oj4toOBS5wF+jggIYMIICFDAOBgNVHQ8BAf8EBAMCB4AwHQYDVR0lBBYw\n"
"FAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFNWx\n"
"qqsP12g+/utMX9fZeBqdRRkKMB8GA1UdIwQYMBaAFFrz7Sv8NsI3eblSMOpUb89V\n"
"yy6sMFUGCCsGAQUFBwEBBEkwRzAhBggrBgEFBQcwAYYVaHR0cDovL2UxLm8ubGVu\n"
"Y3Iub3JnMCIGCCsGAQUFBzAChhZodHRwOi8vZTEuaS5sZW5jci5vcmcvMCAGA1Ud\n"
"EQQZMBeCFWtpbWVuZ2luZWVyaW5nLm9ubGluZTATBgNVHSAEDDAKMAgGBmeBDAEC\n"
"ATCCAQUGCisGAQQB1nkCBAIEgfYEgfMA8QB2ALc++yTfnE26dfI5xbpY9Gxd/ELP\n"
"ep81xJ4dCYEl7bSZAAABimC3XLEAAAQDAEcwRQIgcIEncR/d8UgD7Liw+aK2mPNk\n"
"ipcf3gRRqRfWacHGp90CIQDNuhshIVEBC8lq1ad2DsTu4EHMukJ7N0KdzidwR+6j\n"
"HwB3AOg+0No+9QY1MudXKLyJa8kD08vREWvs62nhd31tBr1uAAABimC3XNEAAAQD\n"
"AEgwRgIhAOr9THBxfToLZrZeNMbEbR3KebVs+XHIOirqxwFLXWI8AiEAnX/v0fAd\n"
"En7UKle9Q8wcnqQqcAbYA5XzQp73hc+CHIUwCgYIKoZIzj0EAwMDaQAwZgIxAJ25\n"
"B85JTKnQxb48MR52i3bU9BsK7BxMidaQdOpXRj8WuH6HPG6WBLJsBt0hQZTgSgIx\n"
"AOvl53EJknPS/nVFHzeEYVVYExQdC2J74n6MPVc8JqraMJFZw1/kbe2+aNX+bE6s\n"
"Hg==\n"
"-----END CERTIFICATE-----\n";

boolean firmFlag=false;
//const int port = 80;
const int port =443;
const char server[]="test.loginapp.kimboxplatform.com";
const char serverp[]="loginapp.kimboxplatform.com";
//const char server[] = "espstack.com";
// const char firmResource[] = "/OTA/firmware.bin";
// const char versionResource[] = "/OTA/version.txt";
const char firmResource[] ="/api/firmwares/prepayment";;
const char versionResource[] ="/api/getversion/prepayment";
const uint32_t CHUNK_SIZE =1024;  // Define chunk size for reading and writing
unsigned long timeElapsed = 0;
uint32_t knownCRC32 = 0x6f50d767;
uint32_t knownFileSize = 2408000; // In case server does not send it
float activeFirmwareVer=1.3;
float serverFirmwareVer=1.3;
int verifieversion;

float lat2,lon2;
int zzzz=7;

void performUpdate(File updateSource, size_t updateSize);
void doOTA();
void doOTA1();
// HTTPS Transport
TinyGsmClient base_client(modem);
SSLClient secure_layer(&base_client);
HttpClient client  = HttpClient(secure_layer, server, port);
HttpClient clienta = HttpClient(secure_layer, server, port);
HttpClient clientr = HttpClient(secure_layer, serverp, port);
HttpClient clientra = HttpClient(secure_layer, serverp, port);
int validation=0;
int annulation=0;

void light_sleep(uint32_t sec )
{
    esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
    esp_light_sleep_start();
}

void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(100);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(150);
  digitalWrite(PWR_PIN, HIGH);
}


void modemRestart(){
  modemPowerOff();
  delay(100);
  modemPowerOn();
}

String gsm_send_serial(String command, int delay)
{
  String buff_resp = "";
  Serial.println("Send ->: " + command);
  SerialAT.println(command);
  long wtimer = millis();
  while (wtimer + delay > millis())
  {
    while (SerialAT.available())
    {
      buff_resp = SerialAT.readString();
      Serial.println(buff_resp);
    }
    if (String(buff_resp).indexOf("+CCHOPEN: 0,0") > -1 || String(buff_resp).indexOf("ERROR") > -1)
      break;
  }
  Serial.println();

  return buff_resp;
}
int gg1=3;
int gg2=1;


void parseResponse(String response) {
  // Find the position of the first occurrence of "+CCHRECV: DATA,0,"
  Serial.println ("Parsing Response: "+response);
  int startIndex = response.lastIndexOf("+CCHRECV: DATA,0,");
  if (startIndex == -1) {
    // If "+CCHRECV: DATA,0," is not found, exit the function
    return;
  }

  // Skip the "+CCHRECV: DATA,0," line
  startIndex = response.indexOf("\n", startIndex) + 1;
Serial.println("Start Index "+(String)startIndex);
  // Find the position of the next occurrence of "+CCHRECV: DATA,0,"
  int endIndex = response.indexOf("+CCHRECV: DATA,0,", startIndex);
  if (endIndex == -1) {
    // If no further occurrence is found, use the end of the response
    endIndex = response.length();
  }
Serial.println("endIndex "+(String)endIndex);
  // Extract the substring containing the version number
  String versionString = response.substring(startIndex, endIndex);
  // Trim leading and trailing whitespace
  versionString.trim();
activeFirmwareVer=versionString.toFloat();

  // Print the extracted version number
  SerialMon.println("Version Number: " + (String)activeFirmwareVer);

  
}

void setupOTA() {
   DBG("Initializing modem.. up .");
   if (!modem.init()) {
    modem.restart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting up " );
    return;
  } 
         modem.setNetworkMode(54);
         modem.isNetworkConnected();
         modem.gprsConnect(apn, gprsUser, gprsPass);
         modem.isGprsConnected();

          if (!modem.waitForNetwork()) {
    SerialMon.println(" fail up ");
    delay(100);
    return;
  }
  SerialMon.println(" success up ");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected up ");
  }

  SerialMon.print(F("Connecting to up "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail up ");
    
    delay(1000);
    
  }
  SerialMon.println(" success up ");


      if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected up ");
  
  }

        DBG("Connecting to ", server);
  


  Serial.print("Active firmware version:");
  Serial.println(activeFirmwareVer);
  //InitSSL();
TinyGsmClient base_client(modem);
SSLClient secure_layer(&base_client);
HttpClient client = HttpClient(secure_layer, server, port);
 
  
  DBG("Connecting to ", server);
  
    client.get(versionResource);
      String gette= client.responseBody();
      String key = "version";
      int keyIndex = gette.indexOf(key);
     
    // Trouver le début de la valeur
    int startIndex = gette.indexOf(':', keyIndex) + 2; // +2 pour ignorer ':"'
    
    // Trouver la fin de la valeur
    int endIndex = gette.indexOf('"', startIndex);
    
    // Extraire la valeur
    float  versionrecu = gette.substring(startIndex, endIndex).toFloat();
    verifieversion=versionrecu;
    // Afficher la valeur
    Serial.print("Version : ");
    Serial.println(versionrecu);
 
 
  
      Serial.print("active version: "); Serial.println(activeFirmwareVer); 
     if((versionrecu!=activeFirmwareVer) && (verifieversion>0))
     {
      Serial.println("Found New Firmware Time to do OTA");
     doOTA();
     }
  }


   void setupOTA1() {
   DBG("Initializing modem.. up .");
   if (!modem.init()) {
    modem.restart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting up " );
    return;
  } 
         modem.setNetworkMode(54);
         modem.isNetworkConnected();
         modem.gprsConnect(apn, gprsUser, gprsPass);
         modem.isGprsConnected();

          if (!modem.waitForNetwork()) {
    SerialMon.println(" fail up ");
    delay(100);
    return;
  }
  SerialMon.println(" success up ");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected up ");
  }

  SerialMon.print(F("Connecting to up "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail up ");
    
    delay(1000);
    
  }
  SerialMon.println(" success up ");


      if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected up ");
  
  }

        DBG("Connecting to ", serverp);
  


  Serial.print("Active firmware version:");
  Serial.println(activeFirmwareVer);
  //InitSSL();
TinyGsmClient base_client(modem);
SSLClient secure_layer(&base_client);
HttpClient clientr = HttpClient(secure_layer, serverp, port);
 
  
  DBG("Connecting to ", serverp);
  
    clientr.get(versionResource);
      String gette= clientr.responseBody();
      String key = "version";
      int keyIndex = gette.indexOf(key);
     
    // Trouver le début de la valeur
    int startIndex = gette.indexOf(':', keyIndex) + 2; // +2 pour ignorer ':"'
    
    // Trouver la fin de la valeur
    int endIndex = gette.indexOf('"', startIndex);
    
    // Extraire la valeur
    float  versionrecu = gette.substring(startIndex, endIndex).toFloat();
     verifieversion=versionrecu ;
    // Afficher la valeur
    Serial.print("Version : ");
    Serial.println(versionrecu);
 
 
  
      Serial.print("active version: "); Serial.println(activeFirmwareVer); 
     if((versionrecu!=activeFirmwareVer) && (verifieversion>0))
     {
      Serial.println("Found New Firmware Time to do OTA");
     doOTA1();
     }
  }
   
 

void doOTA() {
if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
 if ( SPIFFS.remove("/firmware.bin")) {
    Serial.println("SPIFFS Remove File Success");
    
  }
  else
  {
 Serial.println("SPIFFS Remove File Failed");

  }
  SerialMon.print(F("Connecting to "));
  SerialMon.print(server);
client.setTimeout(60000*10);
client.stop();

TinyGsmClient base_client(modem);
SSLClient secure_layer(&base_client);
HttpClient client = HttpClient(secure_layer, server, port);
 
  if (!client.connect(server, port)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }

  SerialMon.println(" success");
//client.setTimeout(60000*10);
  // Make a HTTP GET request
  client.print(String("GET ") + firmResource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();

  SerialMon.println(F("Waiting for response header"));

  // While we are still looking for the end of the header, continue to read data into the buffer
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;
  File file = SPIFFS.open("/firmware.bin", FILE_APPEND);

  while (!finishedHeader) {
    if (client.available()) {
      while (client.available()) {
        char c = client.read();
        headerBuffer += c;

        if (headerBuffer.indexOf(F("\r\n")) >= 0) break;
      }
    }

    int nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0) {
      headerBuffer.toLowerCase();
      if (headerBuffer.startsWith(F("content-length:"))) {
       contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        SerialMon.print(F("Got Content Length: "));  
        SerialMon.println(contentLength);
      }
      headerBuffer.remove(0, nlPos + 2);
    } else if (nlPos == 0) {
      finishedHeader = true;
    }
  }

  uint32_t readLength = 0;
  CRC32 crc;

  SerialMon.println(F("Reading response data"));

  while (readLength < contentLength && client.connected()) {
    while (client.available()) {

      if (!file.print(char(client.read())))
            {
                Serial.println("Appending file");
            }
      uint8_t chunk[CHUNK_SIZE];
      size_t bytesRead = client.readBytes(chunk, CHUNK_SIZE);

      if (bytesRead > 0) {
        file.write(chunk, bytesRead);
        crc.update(chunk, bytesRead);
        readLength += bytesRead;

        // Calculate and print progress
        progress = (float)readLength / contentLength * 100;
        SerialMon.print("Download Progress: ");
        SerialMon.print(progress, 2);
        SerialMon.println("%");
      }
    }
  }

  file.close();

  timeElapsed = millis() - timeElapsed;

  SerialMon.println(F("Server disconnected"));

  // Call update function after file download completes
  performUpdate(SPIFFS.open("/firmware.bin", FILE_READ), contentLength);
}
void doOTA1() {
if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
 if ( SPIFFS.remove("/firmware.bin")) {
    Serial.println("SPIFFS Remove File Success");
    
  }
  else
  {
 Serial.println("SPIFFS Remove File Failed");

  }
  SerialMon.print(F("Connecting to "));
  SerialMon.print(serverp);
clientr.setTimeout(60000*10);
clientr.stop();

TinyGsmClient base_client(modem);
SSLClient secure_layer(&base_client);
HttpClient clientr = HttpClient(secure_layer, serverp, port);
 
  if (!clientr.connect(serverp, port)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }

  SerialMon.println(" success");
//client.setTimeout(60000*10);
  // Make a HTTP GET request
  clientr.print(String("GET ") + firmResource + " HTTP/1.0\r\n");
  clientr.print(String("Host: ") + serverp + "\r\n");
  clientr.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();

  SerialMon.println(F("Waiting for response header"));

  // While we are still looking for the end of the header, continue to read data into the buffer
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;
  File file = SPIFFS.open("/firmware.bin", FILE_APPEND);

  while (!finishedHeader) {
    if (clientr.available()) {
      while (clientr.available()) {
        char c = clientr.read();
        headerBuffer += c;

        if (headerBuffer.indexOf(F("\r\n")) >= 0) break;
      }
    }

    int nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0) {
      headerBuffer.toLowerCase();
      if (headerBuffer.startsWith(F("content-length:"))) {
       contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        SerialMon.print(F("Got Content Length: "));  
        SerialMon.println(contentLength);
      }
      headerBuffer.remove(0, nlPos + 2);
    } else if (nlPos == 0) {
      finishedHeader = true;
    }
  }

  uint32_t readLength = 0;
  CRC32 crc;

  SerialMon.println(F("Reading response data"));

  while (readLength < contentLength && clientr.connected()) {
    while (clientr.available()) {

      if (!file.print(char(clientr.read())))
            {
                Serial.println("Appending file");
            }
      uint8_t chunk[CHUNK_SIZE];
      size_t bytesRead = clientr.readBytes(chunk, CHUNK_SIZE);

      if (bytesRead > 0) {
        file.write(chunk, bytesRead);
        crc.update(chunk, bytesRead);
        readLength += bytesRead;

        // Calculate and print progress
        progress = (float)readLength / contentLength * 100;
        SerialMon.print("Download Progress: ");
        SerialMon.print(progress, 2);
        SerialMon.println("%");
      }
    }
  }

  file.close();

  timeElapsed = millis() - timeElapsed;

  SerialMon.println(F("Server disconnected"));

  // Call update function after file download completes
  performUpdate(SPIFFS.open("/firmware.bin", FILE_READ), contentLength);
}

void performUpdate(File updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      Serial.println("Writes : " + String(written) + " successfully");
    } else {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end()) {
      Serial.println("OTA finished!");
      if (Update.isFinished()) {
        Serial.println("Restart ESP device!");
        ESP.restart();
      } else {
        Serial.println("OTA not fiished");
      }
    } else {
      Serial.println("Error occured #: " + String(Update.getError()));
    }
  } else {
    Serial.println("Cannot begin update");
  }
}
void connexion(){

  DBG("Initializing modem...");
   if (!modem.init()) {
    modem.restart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

   if(gg1==gg2){
    Serial.println("pas de gps ");
  }
  else{
    Serial.println("gps activer ");
  DBG("Enabling GPS/GNSS/GLONASS");
  modem.enableGPS();
  float speed2    = 0;
  float alt2      = 0;
  int   vsat2     = 0;
  int   usat2     = 0;
  float accuracy2 = 0;
  int   year2     = 0;
  int   month2    = 0;
  int   day2      = 0;
  int   hour2     = 0;
  int   min2      = 0;
  int   sec2      = 0;
  DBG("Requesting current GPS/GNSS/GLONASS location");
  
for(int i=0;i<20;i++){   
  if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,

                     &year2, &month2, &day2, &hour2, &min2, &sec2)) {
                       
      DBG("Latitude:", String(lat2, 8), "\tLongitude:", String(lon2, 8));
      DBG("Retrieving GPS/GNSS/GLONASS location again as a string");
  String gps_raw = modem.getGPSraw();
  DBG("GPS/GNSS Based Location String:", gps_raw);
  DBG("Disabling GPS");
  
  modem.disableGPS();
  gg1=gg2;
  Serial.println("donnes gps ok ");
  }
  
  
  
  else{Serial.println("pas de gps");
  }}}

  


         modem.setNetworkMode(54);
         modem.isNetworkConnected();
         modem.gprsConnect(apn, gprsUser, gprsPass);
         modem.isGprsConnected();

          if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(100);
    return;
  }
  SerialMon.println(" success");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    
    delay(1000);
    
  }
  SerialMon.println(" success");


      if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  
  }

       
        // Make a HTTPS POST request:
        TinyGsmClient base_client(modem);
        SSLClient secure_layer(&base_client);
       
        HttpClient clienta = HttpClient(secure_layer, server, port);
         Serial.print("net");  Serial.println( modem.isNetworkConnected());
        Serial.print("isgps");   Serial.println( modem.isGprsConnected());
        Serial.print("client");   Serial.println(clienta);
        
        
    if(clienta==1 && modem.isNetworkConnected()==1&& modem.isGprsConnected()==1){ 
  preferences.begin("serial_storage", false);
  // Stocker la chaîne dans la mémoire
  if(validation==0){
  preferences.putString("serie", serie);}
   if(validation==1){
  preferences.putString("serie", seriep);}
  Serial.println("Numéro de série stocké : " + serie);
  String storedSerie = preferences.getString("serie", "non trouvé");
  Serial.println("Numéro de série récupéré : " + storedSerie);
  preferences.end();   
  String resourcea ="/api/logetteprepayee/"+storedSerie+"/getparameters";
    clienta.get(resourcea);
    int status_codes = clienta.responseStatusCode();
    String str = clienta.responseBody();
    
    int energAchatWebIndex = str.indexOf("EnergAchatWeb") + 15; // +15 pour sauter "EnergAchatWeb": 
    int etatLogetteIndex = str.indexOf("etat_logette") + 14; // +14 pour sauter "etat_logette": 
    int selectL1Index = str.indexOf("selectL1") + 10; // +11 pour sauter "selectL1": 
    int selectL2Index = str.indexOf("selectL2") + 10; // +11 pour sauter "selectL2": 
    int selectL3Index = str.indexOf("selectL3") + 10; // +11 pour sauter "selectL3":
    int puissanceIndex = str.indexOf("puissancemax") + 14;
    int testindex=str.indexOf("test_logette") + 14;
    // Trouver les indices de fin pour chaque valeur  
    int energAchatWebEndIndex = str.indexOf(",", energAchatWebIndex);
    int etatLogetteEndIndex = str.indexOf(",", etatLogetteIndex);
    int selectL1EndIndex = str.indexOf(",", selectL1Index);
    int selectL2EndIndex = str.indexOf(",", selectL2Index);
    int selectL3EndIndex = str.indexOf(",", selectL3Index);
    int puissanceEndIndex = str.indexOf(",",puissanceIndex);
    int testend = str.indexOf("}",testindex);
    // Extraire les valeurs  
    String energAchatWeb = str.substring(energAchatWebIndex, energAchatWebEndIndex);
    String etatLogette = str.substring(etatLogetteIndex, etatLogetteEndIndex);
    String selectL1 = str.substring(selectL1Index, selectL1EndIndex);
    String selectL2 = str.substring(selectL2Index, selectL2EndIndex);
    String selectL3 = str.substring(selectL3Index, selectL3EndIndex);
    String puissanCe= str.substring(puissanceIndex, puissanceEndIndex);
    String Teste=str.substring(testindex,testend);
    validation=Teste.toInt();
    ea = str.substring(energAchatWebIndex, energAchatWebEndIndex).toFloat();
 float eet;
 preferences.begin("serial_storage", false);
 preferences.putUInt("validation",validation);
 preferences.end();
    // Afficher les valeurs  
    Serial.print("validation: ");
    Serial.println(validation);
    Serial.print("Teste: ");
    Serial.println(Teste);
    Serial.print("EnergAchatWeb: ");
    Serial.println(energAchatWeb);
    Serial.print("Etat Logette: ");
    Serial.println(etatLogette);
    Serial.print("Select L1: ");
    Serial.println(selectL1);
    Serial.print("Select L2: ");
    Serial.println(selectL2);
    Serial.print("Select L3: ");
    Serial.println(selectL3);
    Serial.print("puissance: ");
    Serial.println(puissanCe);
    Serial.println(str);
    Serial.print("satut : ");Serial.println(status_codes);
   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
  
 
   if(status_codes==200){
 etatweb=etatLogette.toInt();
  puissancesouscrite=puissanCe.toInt();  
   fram.write(0x0750, (uint8_t*)&puissancesouscrite, sizeof(1)); 
   fram.write(0x0900,etatweb);
    selectTension1=selectL1.toInt();
    selectTension2=selectL2.toInt();
    selectTension3=selectL3.toInt();
   
   Serial.print("etatweb ");Serial.println(etatweb);
    int aaaa;
    fram.read(0x0750, (uint8_t*)&aaaa, sizeof(1));
   Serial.print("puissancesouscrite ");Serial.println(aaaa);
    
   }
   
         

 if(ea==0.00){
    statutenergie=0;
    ea1=0;
    ea2=0;
  }
  else{

      ea2=ea;
      if(ea1==ea2){
        Serial.println(" energie deja enregistrée ");
      }
       else{
    if(energietotale==0){
     energietotale=(eng1+eng2+eng3)+ea;
    
     Serial.print("energ total ini  : ");Serial.println( energietotale);
     }
     else{
      
     if(eng1==0 && eng2==0 && eng3==0 ){
      if(sans==1){
        energietotale=energietotale+ea;
       
        zzzz=3;
      }
      if(sans==3){
       energietotale=energerestant+ea;
       
        sans=1;
        zzzz=4;
       }
     }
     else{
     energietotale=energietotale+ea;} 
       Serial.print("energ total pas ini: ");Serial.println( energietotale);
     }
    
    
      fram.write(0x0500, (uint8_t*)&energietotale, sizeof(1)); 
      delay(500);
     statutenergie=1;
    
  }
  
  
   }
   
   ea1=ea2;
    eet=eet+ea;
    Serial.print(" energie totale : ");Serial.println(eet);
 
  String reponse="tensionligne1=" +String(tensionw1)+"&tensionligne2="+String(tensionw2)+"&tensionligne3="+String(tensionw3)+"&energieligne1="+String(energieb1)+"&energieligne2="+String(energieb2)+"&energieligne3="+String(energieb3)+"&puissanceligne1="+String(puissanceb1)+"&puissanceligne2="+String(puissanceb2)+"&puissanceligne3="+String(puissanceb3)+"&courantligne1="+String(courant1)+"&courantligne2="+String(courant2)+"&courantligne3="+String(courant3)+"&facteur_puissanceligne1="+String( facteurPuissance1)+"&facteur_puissanceligne2="+String( facteurPuissance2)+"&facteur_puissanceligne3="+String( facteurPuissance3)+"&frequenceligne1="+String( frequence1)+"&frequenceligne2="+String( frequence2)+"&frequenceligne3="+String( frequence3)+"&humidity="+String(humidite)+"&temperature="+String(temperature)+"&status_coque="+String(fraude)+"&status_branchement="+String(phase)+"&latitude="+String(lat2,8)+"&longitude="+String(lon2,8)+"&statutenergie="+String(statutenergie)+"&status_fraude="+String(fraude);        
  clienta.post("/api/logetteprepayee/"+storedSerie+"/postparameters","application/x-www-form-urlencoded" , reponse);
  Serial.println("post est bon");
  int  statusc = clienta.responseStatusCode();
  String response = clienta.responseBody();
  Serial.println(statusc);
  Serial.println(response);
 
    if(clienta==0 || modem.isNetworkConnected()==0 || modem.isGprsConnected()==0 ||( status_codes==-3)){ 
    modem.restart();
  }
 
  }
 clienta.stop(); 
   
}


void connexion1(){

  DBG("Initializing modem...");
   if (!modem.init()) {
    modem.restart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

   if(gg1==gg2){
    Serial.println("pas de gps ");
  }
  else{
    Serial.println("gps activer ");
  DBG("Enabling GPS/GNSS/GLONASS");
  modem.enableGPS();
  float speed2    = 0;
  float alt2      = 0;
  int   vsat2     = 0;
  int   usat2     = 0;
  float accuracy2 = 0;
  int   year2     = 0;
  int   month2    = 0;
  int   day2      = 0;
  int   hour2     = 0;
  int   min2      = 0;
  int   sec2      = 0;
  DBG("Requesting current GPS/GNSS/GLONASS location");
  
for(int i=0;i<20;i++){   
  if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,

                     &year2, &month2, &day2, &hour2, &min2, &sec2)) {
                       
      DBG("Latitude:", String(lat2, 8), "\tLongitude:", String(lon2, 8));
      DBG("Retrieving GPS/GNSS/GLONASS location again as a string");
  String gps_raw = modem.getGPSraw();
  DBG("GPS/GNSS Based Location String:", gps_raw);
  DBG("Disabling GPS");
  
  modem.disableGPS();
  gg1=gg2;
  Serial.println("donnes gps ok ");
  }
  
  
  
  else{Serial.println("pas de gps");
  }}}

  


         modem.setNetworkMode(54);
         modem.isNetworkConnected();
         modem.gprsConnect(apn, gprsUser, gprsPass);
         modem.isGprsConnected();

          if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(100);
    return;
  }
  SerialMon.println(" success");
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    
    delay(1000);
    
  }
  SerialMon.println(" success");


      if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  
  }

       
        // Make a HTTPS POST request:
        TinyGsmClient base_client(modem);
        SSLClient secure_layer(&base_client);
       
        HttpClient clientra = HttpClient(secure_layer, serverp, port);
         Serial.print("net");  Serial.println( modem.isNetworkConnected());
        Serial.print("isgps");   Serial.println( modem.isGprsConnected());
        Serial.print("client");   Serial.println(clientra);
        
        
    if(clientra==1 && modem.isNetworkConnected()==1&& modem.isGprsConnected()==1){ 
  preferences.begin("serial_storage", false);
  if(validation==0){
  preferences.putString("serie", serie);}
   if(validation==1){
  preferences.putString("serie", seriep);}
  Serial.println("Numéro de série stocké : " + serie);
  String storedSerie = preferences.getString("serie", "non trouvé");
  Serial.println("Numéro de série récupéré : " + storedSerie);
  preferences.end();   
  String resourcea ="/api/logetteprepayee/"+storedSerie+"/getparameters";
    clientra.get(resourcea);
    int status_codes = clientra.responseStatusCode();
    String str = clientra.responseBody();
    
    int energAchatWebIndex = str.indexOf("EnergAchatWeb") + 15; // +15 pour sauter "EnergAchatWeb": 
    int etatLogetteIndex = str.indexOf("etat_logette") + 14; // +14 pour sauter "etat_logette": 
    int selectL1Index = str.indexOf("selectL1") + 10; // +11 pour sauter "selectL1": 
    int selectL2Index = str.indexOf("selectL2") + 10; // +11 pour sauter "selectL2": 
    int selectL3Index = str.indexOf("selectL3") + 10; // +11 pour sauter "selectL3":
    int puissanceIndex = str.indexOf("puissancemax") + 14;
    int testindex=str.indexOf("test_logette") + 14;
    // Trouver les indices de fin pour chaque valeur  
    int energAchatWebEndIndex = str.indexOf(",", energAchatWebIndex);
    int etatLogetteEndIndex = str.indexOf(",", etatLogetteIndex);
    int selectL1EndIndex = str.indexOf(",", selectL1Index);
    int selectL2EndIndex = str.indexOf(",", selectL2Index);
    int selectL3EndIndex = str.indexOf(",", selectL3Index);
    int puissanceEndIndex = str.indexOf(",",puissanceIndex);
    int testend = str.indexOf("}",testindex);
    // Extraire les valeurs  
    String energAchatWeb = str.substring(energAchatWebIndex, energAchatWebEndIndex);
    String etatLogette = str.substring(etatLogetteIndex, etatLogetteEndIndex);
    String selectL1 = str.substring(selectL1Index, selectL1EndIndex);
    String selectL2 = str.substring(selectL2Index, selectL2EndIndex);
    String selectL3 = str.substring(selectL3Index, selectL3EndIndex);
    String puissanCe= str.substring(puissanceIndex, puissanceEndIndex);
    String Teste=str.substring(testindex,testend);
    validation=1;
    ea = str.substring(energAchatWebIndex, energAchatWebEndIndex).toFloat();
 float eet;
 preferences.begin("serial_storage", false);
 preferences.putUInt("validation",validation);
 preferences.end();
    // Afficher les valeurs  
    Serial.print("validation: ");
    Serial.println(validation);
    Serial.print("Teste: ");
    Serial.println(Teste);
    Serial.print("EnergAchatWeb: ");
    Serial.println(energAchatWeb);
    Serial.print("Etat Logette: ");
    Serial.println(etatLogette);
    Serial.print("Select L1: ");
    Serial.println(selectL1);
    Serial.print("Select L2: ");
    Serial.println(selectL2);
    Serial.print("Select L3: ");
    Serial.println(selectL3);
    Serial.print("puissance: ");
    Serial.println(puissanCe);
    Serial.println(str);
    Serial.print("satut : ");Serial.println(status_codes);
   fram.read(0x0100, (uint8_t*)&eng1, sizeof(1));
   fram.read(0x0200, (uint8_t*)&eng2, sizeof(1));
   fram.read(0x0300, (uint8_t*)&eng3, sizeof(1));
   fram.read(0x0400, (uint8_t*)&energerestant, sizeof(1));
   fram.read(0x0500, (uint8_t*)&energietotale, sizeof(1));
   etatlogette=fram.read(0x0600);
  
   
   if(status_codes==200){
 etatweb=etatLogette.toInt();
  puissancesouscrite=puissanCe.toInt();  
   fram.write(0x0750, (uint8_t*)&puissancesouscrite, sizeof(1)); 
   fram.write(0x0900,etatweb);
    selectTension1=selectL1.toInt();
    selectTension2=selectL2.toInt();
    selectTension3=selectL3.toInt();
   
   Serial.print("etatweb ");Serial.println(etatweb);
    int aaaa;
    fram.read(0x0750, (uint8_t*)&aaaa, sizeof(1));
   Serial.print("puissancesouscrite ");Serial.println(aaaa);
    
   }
   
         

 if(ea==0.00){
    statutenergie=0;
    ea1=0;
    ea2=0;
  }
  else{

      ea2=ea;
      if(ea1==ea2){
        Serial.println(" energie deja enregistrée ");
      }
       else{
    if(energietotale==0){
     energietotale=(eng1+eng2+eng3)+ea;
    
     Serial.print("energ total ini  : ");Serial.println( energietotale);
     }
     else{
      
     if(eng1==0 && eng2==0 && eng3==0 ){
      if(sans==1){
        energietotale=energietotale+ea;
       
        zzzz=3;
      }
      if(sans==3){
       energietotale=energerestant+ea;
       
        sans=1;
        zzzz=4;
       }
     }
     else{
     energietotale=energietotale+ea;} 
       Serial.print("energ total pas ini: ");Serial.println( energietotale);
     }
    
    
      fram.write(0x0500, (uint8_t*)&energietotale, sizeof(1)); 
      delay(500);
     statutenergie=1;
    
  }
  
  
   }
   
   ea1=ea2;
    eet=eet+ea;
    Serial.print(" energie totale : ");Serial.println(eet);
 
  String reponse="tensionligne1=" +String(tensionw1)+"&tensionligne2="+String(tensionw2)+"&tensionligne3="+String(tensionw3)+"&energieligne1="+String(energieb1)+"&energieligne2="+String(energieb2)+"&energieligne3="+String(energieb3)+"&puissanceligne1="+String(puissanceb1)+"&puissanceligne2="+String(puissanceb2)+"&puissanceligne3="+String(puissanceb3)+"&courantligne1="+String(courant1)+"&courantligne2="+String(courant2)+"&courantligne3="+String(courant3)+"&facteur_puissanceligne1="+String( facteurPuissance1)+"&facteur_puissanceligne2="+String( facteurPuissance2)+"&facteur_puissanceligne3="+String( facteurPuissance3)+"&frequenceligne1="+String( frequence1)+"&frequenceligne2="+String( frequence2)+"&frequenceligne3="+String( frequence3)+"&humidity="+String(humidite)+"&temperature="+String(temperature)+"&status_coque="+String(fraude)+"&status_branchement="+String(phase)+"&latitude="+String(lat2,8)+"&longitude="+String(lon2,8)+"&statutenergie="+String(statutenergie)+"&status_fraude="+String(fraude);        
  clientra.post("/api/logetteprepayee/"+storedSerie+"/postparameters","application/x-www-form-urlencoded" , reponse);
  Serial.println("post est bon");
  int  statusc = clientra.responseStatusCode();
  String response = clientra.responseBody();
  Serial.println(statusc);
  Serial.println(response);
 
    if(clientra==0 || modem.isNetworkConnected()==0 || modem.isGprsConnected()==0 ||( status_codes==-3)){ 
    modem.restart();
  }
 
  }
 clientra.stop(); 
   
}
