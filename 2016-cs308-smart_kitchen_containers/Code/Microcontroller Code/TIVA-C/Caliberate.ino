#include "HX711.h"

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

HX711 scale(A1, A0);		// parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  // Setting appropriate baud rate
  Serial.begin(38400);
  scale.set_scale();
  scale.tare();
}

void loop() {
  Serial.println("Now place some known standard weight");
  // Waiting for standard weight to be placed
  delay(5000);
  // factor = standard weight/value printed from statement below
  Serial.println(scale.get_units());
  // put the ADC in sleep mode
  scale.power_down();			        
  delay(5000);
  scale.power_up();
}
