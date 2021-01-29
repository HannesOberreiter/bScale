#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include "HX711.h"
//#include "DHT.h"
#include <Arduino.h>
#include "SHTC3.h"


//Connection PINS to GSM Shield, change if needed
SoftwareSerial mySerial(10, 11);

//Url to btree API, change if you use your own
const char URL[] PROGMEM = "www.xxxxxxx.pl";
const char API[] PROGMEM = "/send_data_ident.php?";
//Resquestb.in Testing Request
//const char URL[] PROGMEM = "requestb.in";
//const char API[] PROGMEM = "YOU_REQUEST_BIN_URL";
//change this constants as needed
const char APN[] PROGMEM = "internet"; //APN from your provider, given is internet (PLAY)
const char KEY[] PROGMEM= "-"; //API Key from your profile page www.btree.at/app
const char ACTION[] PROGMEM = "CREATE"; //Use CREATE_DEMO if you just want to check the connection
const char TIMEZONE[] PROGMEM = "Europe/Warsaw"; //php timezoneformat, used to save with the correct current date/time

//This is the name of your Hive, www.btree.at/app will create a  new if it doenst exist
const char IDENT[] PROGMEM = "ScaleHouse";

//Array Table for our fixed strings
const char * const MARRAY[7] PROGMEM =
{
  APN,
  URL,
  API,
  KEY,
  ACTION,
  TIMEZONE,
  IDENT,
};

//Sensor Data
char weight[7];
char temp1[6]= "0.00";
char temp2[6] = "0.00";
char rain[6] = "0.00";
char hum[7] = "0.00";
char ident[] = "0001";

//Battery Data
char charging[1]= "0";
char remains[3] = "00";
char voltage[5] = "00.00";



//Code Variables
float helper;
char conv[150];
char postdata[150];
char response[150];

//atTiny Message PIN
#define FINISHED 2 //Tell the atTiny we are finished

//Scale
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 6;
//#define DIGITALOUT  5     //HX711 DT - stara wesja library HX711 - wyłączone
//#define CLOCK       6     //HX711 CKL - stara wesja library HX711 - wyłączone
//HX711 scale(DIGITALOUT, CLOCK); //- stara wesja library HX711 - wyłączone
HX711 scale;

//ENTER YOUR CALIBRATED DATA HERE
//as example is my first try given, with 200kg load cell and 3.3V calibrated
float SCALE = 20794.48;
long offset = 78600;

//DHT22
// #define DHTPIN 10     // what pin we're connected to  /*****/
// #define DHTTYPE DHT22   // DHT 22                     /*****/

//SHTC3
SHTC3 sht(Wire);

void setup()
{
delay(500);
mySerial.begin(9600);
Serial.begin(9600);
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
delay(1000);
Power_UP();
delay(2000);
/*if(sendATcommand2("AT+CPIN?", "+CPIN: READY", "NOTHING", 100) == 0) {
    Serial.println(F("Error SIM locked, please unlock SIM"));
  } else {
    Serial.println(F("Good, SIM is unlocked!"));
  }*/
//Serial.println(F("Find provider!"));
//sendATcommand2("AT+COPS?", "OK", "NOTHING", 100);
Serial.println(F("Connecting .... "));
while( sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 1000)== 0 );
Serial.println(F("Setup finished!"));
delay(2000);
Request();
}

void loop()
{
  //Nothing in Loop
}

void Power_UP()
{
  uint8_t answer=0;
  Serial.println(F("Checking if GSM is running ..."));
  answer = sendATcommand2("AT", "OK", "NOTHING", 2000);
  if (answer == 0)
  {
    // power on pulse
    //Serial.println("Starting GSM ...");
    pinMode(9, OUTPUT);
    digitalWrite(9,LOW);
    delay(1000);
    digitalWrite(9,HIGH);
    delay(2000);
    digitalWrite(9,LOW);
    delay(8000);
    // waits for an answer from the module
    while(answer == 0){     // Send AT every two seconds and wait for the answer
      answer = sendATcommand2("AT", "OK", "NOTHING", 2000);
    }
  }
  Serial.println(F("GSM ready!"));
  delay(1000);
  
  //  Read battery capacity
  sendATcommand2("AT+CBC", "OK", "NOTHING", 500);
  Serial.print("Battery capacity: [%]  ");
  Serial.println(String(response).substring(17,19));
  Serial.print("Battery voltage: [V]  ");
  Serial.print(String(response).substring(20,20)+String(response).substring(21,24));

 
  
  
  
  
  
  
  
  
  //Print GSM Reciving Strenth
  Serial.println(F("Get access strength .."));
  sendATcommand2("AT+CSQ", "OK", "NOTHING", 1000);
}


int8_t sendATcommand2(char const* ATcommand, char const* expected_answer1, char const* expected_answer2, unsigned int timeout){

    uint8_t x=0, answer=0;
    unsigned long previous;

    memset(response, '\0', 150);    // Initialize the string
    delay(100);
    while( mySerial.available() > 0) mySerial.read();    // Clean the input buffer
    mySerial.println(ATcommand);    // Send the AT command
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(mySerial.available() != 0){
            response[x] = mySerial.read();
            x++;
            if (x >= 150) { //Overflow protection
              Serial.println(response);
              memset(response, 0, 150);    // Initialize the string
              x=0;
            }
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)
            {
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
        while((answer == 0) && ((millis() - previous) < timeout));
      Serial.println(response);
    return answer;
  }


void Request()
{
  if(sendATcommand2("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E setting connection to GPRS!"));
    delay(2000);
    Request();
  }

  snprintf_P(conv, sizeof(conv), PSTR("AT+SAPBR=3,1,\"APN\",\"%S\""), (char*) pgm_read_word(&MARRAY[0]));
  if(sendATcommand2(conv, "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E APN!"));
    delay(2000);
    Request();
  }

  if(sendATcommand2("AT+SAPBR=1,1", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E already attached to GPRS"));
    //return;
  }

  if(sendATcommand2("AT+SAPBR=2,1", "OK", "ERROR", 1000) != 1) {
    Serial.println(F("#E IP!"));
    delay(2000);
    Request();
  }


  /*if(sendATcommand2("AT+HTTPTERM", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E nothing to terminate"));
    //return;
  }*/



  if(sendATcommand2("AT+HTTPINIT", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E HTTP!"));
    delay(2000);
    //Request();
  }


  if(sendATcommand2("AT+HTTPPARA=\"CID\",1", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E BEARER!"));
    delay(2000);
    Request();
  }


  if(sendATcommand2("AT+HTTPPARA=\"REDIR\",1", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E setting redirect"));
    //return;
  }

  //------------------------------------------------
  //-------------------Sensor Readings--------------
  //------------------------------------------------
  //HX711 definitions
  delay(2000);
  scale.set_scale();
  scale.set_offset(offset);
  scale.set_scale(SCALE);
  delay(2000);

  //Get weight and convert to char
  dtostrf(scale.get_units(5), 0, 2, weight);
  Serial.println(F("Weight:\t"));
  Serial.println(weight);

  //************SHTC3 SENSOR****************
  delay(2000);
  Wire.begin();
  sht.begin(true);
  sht.sample();
  float t = sht.readTempC();
  float h = sht.readHumidity();
  
  dtostrf(t, 5, 2, temp1);
  Serial.println(F("Temperature[°C]:"));
  Serial.println(temp1);

  dtostrf(h, 5, 2, hum);
  Serial.println(F("Humidity[%]:"));
  Serial.println(hum);
  
  //------------------------------------------------
  //-------------------Sensor Readings--------------
  //------------------------------------------------

  memset(postdata, '\0', 150);    // Initialize the string
 
  
  //snprintf(postdata, sizeof(postdata), "weight=%s&temp1=%s&temp2=%s&hum=%s&rain=%s&ident=%s", weight, temp1, temp2, hum, rain, ident);
  snprintf(postdata, sizeof(postdata), "weight=%s&temp1=%s&temp2=%s&hum=%s&rain=%s&ident=%s&charging=%s", weight, temp1, temp2, hum, rain, ident, charging);
  snprintf(conv, sizeof(conv), "AT+HTTPPARA=\"URL\", \"http://%S%S%s\"", (char*) pgm_read_word(&MARRAY[1]), (char*) pgm_read_word(&MARRAY[2]), postdata);
  if(sendATcommand2(conv, "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E URL!"));
    delay(2000);
    Request();
  }

  if(sendATcommand2("AT+HTTPPARA=\"UA\",\"arduino\"", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E USERAGENT!"));
    delay(2000);
    Request();
  }

  if(sendATcommand2("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded;\"", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E HTTP Content-Type!"));
    Request();
  }
  memset(postdata, '\0', 150);    // Initialize the string
  snprintf(postdata, sizeof(postdata), "HTTP_X_API=%S&ident=%S&timezone=%S&action=%S", (char*) pgm_read_word(&MARRAY[3]), (char*) pgm_read_word(&MARRAY[6]), (char*) pgm_read_word(&MARRAY[5]), (char*) pgm_read_word(&MARRAY[4]));
  snprintf(conv, sizeof(conv), "AT+HTTPDATA=%d, 20000", strlen(postdata));
  if(sendATcommand2(conv, "DOWNLOAD", "ERROR", 3500) != 1) {
    Serial.println(F("#E prepare sending"));
    delay(2000);
    Request();
  }
  Serial.println(String(postdata));
  if(sendATcommand2(postdata, "OK", "ERROR", 3500) != 1) {
    Serial.println(F("#E POSTING"));
    delay(2000);
    Request();
  }
  /*if(sendATcommand2("AT+HTTPSCONT?", "OK", "ERROR", 3000) != 1) {
    Serial.println("#E READING CONTENT!");
    return;
  }*/
  delay(500);
  if(sendATcommand2("AT+HTTPACTION=1", "NOTHING", "ERROR", 6000) != 0) {
    Serial.println(F("#E EXECUTING POST!"));
    Request();
  }
  delay(3000);
  if(sendATcommand2("AT+HTTPREAD", "OK", "ERROR", 5000) !=1) {
    Serial.println(F("#E READING!"));
    delay(2000);
    Request();
  }

  if(sendATcommand2("AT+HTTPTERM", "OK", "ERROR", 500) != 1) {
    Serial.println(F("#E TERMINATE!"));
    delay(2000);
    Request();
  }

  done();
}
/*void battery()
  //************BATTERY STATUS****************
  dtostrf(h, 5, 2, charging);
   Serial.println(F("cHARGING[%]:"));
  Serial.println(charging); */
  

void done(){
    //Shutdown GSM
    pinMode(9, OUTPUT);
    digitalWrite(9,LOW);
    delay(1000);
    digitalWrite(9,HIGH);
    delay(2000);
    digitalWrite(9,LOW);
    delay(3000);

    //Shutdown Scale
    scale.power_down();

    //Tell atTiny we are finished (should disconnect Arduino and following could should be unnessaccery)
    pinMode(FINISHED, OUTPUT);
    digitalWrite(FINISHED,LOW);
    delay(10000); // musi byc > 8 sekund zeby ATTiny85 zrobilo odczyt, zanim Arduino pojdzie w sleep mode

    //Set Arduino Sleep mode and send to sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
}