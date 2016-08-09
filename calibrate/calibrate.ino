/*
 * BIG thanks for the example to
 * Example using the SparkFun HX711 breakout board with a scale
 * By: Nathan Seidle
 * SparkFun Electronics
 * Date: November 19th, 2014
 * https://github.com/sparkfun/HX711-Load-Cell-Amplifier
*/
/*
 *  HX711.h library awesome bodge https://github.com/bogde/HX711
 */

#include "HX711.h"

#define DIGITALOUT  5     //HX711 DT
#define CLOCK       6     //HX711 CKL


float   SCALE =     false;  //Set to 'false' on your first try, then enter your calibration factor
long    OFFSET =    false; //Set to 'false' if you dont know your offset / zero factor.

HX711 scale(DIGITALOUT, CLOCK);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or s to decrease calibration factor");
  Serial.println("Press t to tare and set scale to zero");


  scale.set_scale();

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor / OFFSET: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor / OFFSET: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  if(OFFSET){
      scale.set_offset(OFFSET); //set offset
  } else {
      scale.tare(); //Else reset scale to zero
  }
}

void loop() {
  if(SCALE){
      scale.set_scale(SCALE);
  }
  Serial.print("Reading: ");
  Serial.print(scale.get_units(5));
  Serial.print(" weight");
  Serial.print(" calibration_factor: ");
  Serial.print(SCALE);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      SCALE += 10;
    else if(temp == '-' || temp == 's')
      SCALE -= 10;
    else if(temp == 't'){
      scale.tare();
      SCALE = 0.00;
    }
  }
}
