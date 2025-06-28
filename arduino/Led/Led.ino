/// @file    Blink.ino
/// @brief   Blink the first LED of an LED strip
/// @example Blink.ino

// #include <FastLED.h>
#include <SoftwareSerial.h>

// #define NUM_LEDS 1
// #define DATA_PIN 5

#define PIN_RX 10
#define PIN_TX_INVALID 255

// // Define the array of leds
// CRGB leds[NUM_LEDS];

// void setup() { 
//   FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
// }

// void loop() { 
//   // Turn the LED on, then pause
//   leds[0] = CRGB::Red;
//   FastLED.show();
//   delay(500);
//   // Now turn the LED off, then pause
//   leds[0] = CRGB::Black;
//   FastLED.show();
//   delay(500);
// }

// Utilisation d'autres PIN parce que les PIN RX et TX sont utilisés pour la connexion USB
SoftwareSerial Serial1(PIN_RX, PIN_TX_INVALID);  // RX, TX

void setup() {
  Serial.begin(115200);  // Pour afficher vers le PC
  //Serial.println("Hello depuis l'Arduino !");
  Serial1.begin(115200); // Pour écouter l'appareil
  Serial.println("Test réception sur D10 démarré !");
}

void loop() {
  if (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);   // Affiche le caractère reçu
  }
}

