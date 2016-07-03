// bienenforum.com ESP8266 (NodeMCU) Waage
// 
// OpenSoure - d.h. es kann jeder damit tun was er will ...
// Alles für die Bienen :-)


#include "HX711.h"

HX711 scale(12, 14); // // HX711-DT an Pin D6 (=12); HX711-SCK an Pin D5 (=14)

void setup() {

scale.set_scale(); 
scale.tare(); // Reset Waage auf 0

Serial.begin(115200);
Serial.println(F("Start HX711 Kalibrierung"));

}

void loop() {

Serial.println(F("Innerhalb von 20 Sekunden Referenzgewicht auf die Waage legen!"));
delay(20000);  
ESP.wdtDisable();
  
Serial.println(F("Kalbrierungswert:"));
Serial.println(scale.get_units(20));
ESP.wdtDisable();

Serial.println(F("Kalbrierungswert:"));
Serial.println(scale.get_units(20));
ESP.wdtDisable();

Serial.println(F("Kalbrierungswert:"));
Serial.println(scale.get_units(20));
ESP.wdtDisable();

Serial.println(F("Kalbrierungswert:"));
Serial.println(scale.get_units(20));
ESP.wdtDisable();

Serial.println(F(""));
Serial.println(F("Kalibrierungswert notieren. "));
Serial.println(F("Kalibrierungswert durch Referenzgewicht dividieren, z.B Kalibrierungswert -213245,9 / Referenzgewicht 10Kg = Wert zum eintragen -21324,59"));
Serial.println(F(""));
Serial.println(F("Innerhalb von 5 sek. das Gewicht wieder entfernen."));
delay(5000);
ESP.wdtDisable();

Serial.println(F("Jetzt noch den Offset Wert notieren, wird benötigt fuer die Nullstellung."));

Serial.println(F("Offset:"));
Serial.println(scale.read_average());
ESP.wdtDisable();

Serial.println(F("Offset:"));
Serial.println(scale.read_average());
ESP.wdtDisable();

Serial.println(F("Offset:"));
Serial.println(scale.read_average());
ESP.wdtDisable();

Serial.println(F("Offset:"));
Serial.println(scale.read_average());
ESP.wdtDisable();

}
