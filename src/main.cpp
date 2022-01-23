#include "App.hpp"
//#include <esp_task_wdt.h>
#include "pico/stdlib.h"

static dps::App* App;


#define LED LED_BUILTIN




#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "Pico_WS2812/WS2812.hpp"

#define CLK_PIN       2 // 14
#define MOSI_PIN      3 // 13
#define MISO_PIN      4 // 12
#define RST_PIN       0 // 26           // Configurable, see typical pin layout above
#define SS_PIN        1 // 25           // Configurable, see typical pin layout above
#define IRQ_PIN       5 // 27           // Configurable, depends on hardware


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup()
{
  pinMode(25, OUTPUT);
  digitalWrite(25, true);

  Serial.begin(); // Initialize serial communications with the PC

//   App = new dps::App();




//   // xTaskCreatePinnedToCore(
//   //             StepperTask, /* Task function. */
//   //             "Task1",   /* name of task. */
//   //             10000,     /* Stack size of task */
//   //             NULL,      /* parameter of the task */
//   //             1,         /* priority of the task */
//   //             &StepperTaskHandle,    /* Task handle to keep track of created task */
//   //             0);        /* pin task to core 0 */

//   //esp_task_wdt_init(1, true); //enable panic so ESP32 restarts
//   //esp_task_wdt_add (nullptr); //add current thread to WDT watch  


SPI.setSCK(CLK_PIN);
SPI.setRX (MISO_PIN);
SPI.setTX (MOSI_PIN);


//  sleep_ms(10000);
  App = new dps::App();
}


unsigned long previousMillis = 0; 
unsigned long slowMillis = 0; 

bool onoff;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= dps::App::Polling_ms)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    (*App)();
    //esp_task_wdt_reset();  
  }
  App->pollSteppers();
}

// void StepperTask(void*)
// {
//   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   using TPoints = std::array<int16_t, 4>;


//   AccelStepper StepperX(AccelStepper::FULL4WIRE, 15,  4,  2, 16);
//   AccelStepper StepperY(AccelStepper::FULL4WIRE, 17, 18,  5, 19);

//   StepperX.setMaxSpeed    ( 2000.0);
//   StepperX.setAcceleration( 1000.0);
//   StepperX.setSpeed       ( 2000.0);
//   StepperY.setMaxSpeed    ( 2000.0);
//   StepperY.setAcceleration( 1000.0);
//   StepperY.setSpeed       ( 2000.0);


//   TPoints X = {{ -200,  200, 200, -200}};//,  200,  200,  200, -200 }};
//   TPoints Y = {{ -200, -200, 200,  200}};//, -200,  200,  200, -200 }};
//   TPoints::const_iterator NextX = X.cbegin();
//   TPoints::const_iterator NextY = Y.cbegin();

//   while(true)
//   {
//     if ((StepperX.distanceToGo() == 0)
//         &&
//         (StepperY.distanceToGo() == 0))
//     {
//       if (NextX == X.cend())
//       {
//         NextX = X.cbegin();
//         NextY = Y.cbegin();
//       }

//       StepperX.moveTo(*NextX++);
//       StepperY.moveTo(*NextY++);

//     }

//     StepperX.run();
//     StepperY.run();

//     yield();
//     delay(1);
//   }
// }
