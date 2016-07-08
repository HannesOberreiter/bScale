/*
 * BIG thanks for the example to
 * Example using the SparkFun HX711 breakout board with a scale
 * By: Nathan Seidle
 * SparkFun Electronics
 * Date: November 19th, 2014
*/
/*
 *  HX711.h library awesome bodge https://github.com/bogde/HX711
 */

#include "HX711.h"

#define DIGITALOUT  5     //HX711 DT
#define CLOCK       6     //HX711 CKL
float   SCALE =     690.64;  //200kg gauge weight, change this to your value
long    OFFSET =    71372; //Weight of the scale build up

HX711 scale(DIGITALOUT, CLOCK);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.set_scale();
  scale.set_offset(OFFSET);
  
  long zero_factor = scale.read_average(50); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(SCALE);
  Serial.print("Reading: ");
  Serial.print(scale.get_units(10), 1);
  Serial.print(" kg");
  Serial.print(" calibration_factor: ");
  Serial.print(SCALE);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      SCALE += 10;
    else if(temp == '-' || temp == 'z')
      SCALE -= 10;
  }
}
