

#include "App.hpp"
#include <esp_task_wdt.h>

static dps::App* App;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC

  App = new dps::App();

  esp_task_wdt_init(1, true); //enable panic so ESP32 restarts
  esp_task_wdt_add (nullptr); //add current thread to WDT watch  
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{
  (*App)();
  esp_task_wdt_init(1, true); //enable panic so ESP32 restarts
  esp_task_wdt_add (nullptr); //add current thread to WDT watch
}
