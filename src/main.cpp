#include "App.hpp"
#include "pico/stdlib.h"

#include <stdint.h>

static dps::App*  App;
static uint32_t   PreviousMillis = 0; 


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup()
{
  // USB-Serial starten
  Serial.begin();

  // SPI vorkonfigurieren
  SPI.setSCK(CLK_PIN);
  SPI.setRX (MISO_PIN);
  SPI.setTX (MOSI_PIN);

  App = new dps::App();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - PreviousMillis >= dps::App::Polling_ms)
  {
    // save the last time you blinked the LED
    PreviousMillis = currentMillis;

    (*App)();
  }

  App->pollSteppers();
}
