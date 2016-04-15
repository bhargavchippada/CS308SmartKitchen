#include "HX711.h"

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

HX711 scale(A1, A0);		// parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  // Setting appropriate baud rate
  Serial.begin(38400);
  // this value is obtained by calibrating the scale with known weights
  scale.set_scale(-204882.f);                    
}

void loop() {
  Serial.println(scale.get_units());
  scale.power_down();			        // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
