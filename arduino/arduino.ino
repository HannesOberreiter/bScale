#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include "HX711.h"

//Connection PINS to GSM Shield, change if needed
SoftwareSerial mySerial(7, 8);

//Url to btree API, change if you use your own
const char URL[] PROGMEM = "www.btree.at";
const char API[] PROGMEM = "/app/api/ext/scale.php?";
//Resquestb.in Testing Request
//const char URL[] PROGMEM = "requestb.in";
//const char API[] PROGMEM = "/u2e1hdu2?";
//change this constants as needed
const char APN[] PROGMEM = "webaut"; //APN from your provider, given is HOT (Hofer)
const char KEY[] PROGMEM= "API-KEY"; //API Key from your profile page www.btree.at/app
const char ACTION[] PROGMEM = "CREATE"; //Use CREATE_DEMO if you just want to check the connection
const char TIMEZONE[] PROGMEM = "Europe/Vienna"; //php timezoneformat, used to save with the correct current date/time

//This is the name of your Hive, www.btree.at/app will create a  new if it doenst exist
const char IDENT[] PROGMEM = "Test_Scale";

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
char weight[7] = "120.90";
char temp1[6]= "12.00";
char temp2[6] = "12.00";
char rain[6] = "13.4";
char hum[7] = "80.00";

//Code Variables
char conv[150];
char postdata[150];
char response[150];

//Scale
HX711 scale(12, 14); // // HX711-DT an Pin D6 (=12); HX711-SCK an Pin D5 (=14)
long calibrate = -21389;
long offset = 8319960;


void setup()
{
mySerial.begin(19200);  
Serial.begin(19200);
//HX711 definitions
scale.set_scale(calibrate);          
scale.set_offset(offset);    

delay(1000);
Power_UP();
delay(3000);
/*if(sendATcommand2("AT+CPIN?", "+CPIN: READY", "NOTHING", 100) == 0) {
    Serial.println(F("Error SIM locked, please unlock SIM"));
  } else {
    Serial.println(F("Good, SIM is unlocked!"));
  }*/
delay(500);
//Serial.println(F("Find provider!"));
//sendATcommand2("AT+COPS?", "OK", "NOTHING", 100);
Serial.println(F("Connecting .... "));
while( sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 1000)== 0 );
Serial.println(F("Setup finished!"));
delay(5000);
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
    delay(3000);
    // waits for an answer from the module
    while(answer == 0){     // Send AT every two seconds and wait for the answer
      answer = sendATcommand2("AT", "OK", "NOTHING", 2000);    
    }
  }
  Serial.println(F("GSM ready!"));
  delay(3000);
  //Print GSM Reciving Strenth
  Serial.println(F("Get access strength .."));
  sendATcommand2("AT+CSQ", "OK", "NOTHING", 100);
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
  
  if(sendATcommand2("AT+SAPBR=2,1", "OK", "ERROR", 500) != 1) {
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
    Request();
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
  //sprintf(weight, "%.2f", scale.get_units(20)); //Get weight readings and save to char with two after comma
  //Serial.println(F("Weight:\t")); 
  //Serial.println(weight);
  //delay (1000);
  sprintf(weight, "%s", "10");
  //------------------------------------------------
  //-------------------Sensor Readings--------------
  //------------------------------------------------

  
  memset(postdata, '\0', 150);    // Initialize the string
  snprintf(postdata, sizeof(postdata), "weight=%s&temp1=%s&temp2=%s&hum=%s&rain=%s", weight, temp1, temp2, hum, rain);
  
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
  if(sendATcommand2(conv, "DOWNLOAD", "ERROR", 5000) != 1) {
    Serial.println(F("#E prepare sending")); 
    delay(2000);
    Request();
  }
  delay(800);
  Serial.println(String(postdata));
  if(sendATcommand2(postdata, "OK", "ERROR", 5000) != 1) {
    Serial.println(F("#E POSTING")); 
    delay(2000);
    Request();
  }
  /*if(sendATcommand2("AT+HTTPSCONT?", "OK", "ERROR", 3000) != 1) {
    Serial.println("#E READING CONTENT!"); 
    return;
  }*/
  delay(2000);
  if(sendATcommand2("AT+HTTPACTION=1", "NOTHING", "ERROR", 10000) != 0) {
    Serial.println(F("#E EXECUTING POST!")); 
    Request();
  }
  delay(5000);
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

void done(){
    //Shutdown GSM
    pinMode(9, OUTPUT); 
    digitalWrite(9,LOW);
    delay(1000);
    digitalWrite(9,HIGH);
    delay(2000);
    digitalWrite(9,LOW);
    delay(3000);  
    //Set Arduino Sleep mode and send to sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();   
    sleep_mode();       
}

