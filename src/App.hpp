#ifndef APP_INCLUDED_HPP
#define APP_INCLUDED_HPP

#include <AccelStepper.h>
#include <Arduino.h>
#include <ArduinoJson.hpp>
#include "Button/Button.hpp"
#include "Delta/PeriodicTimer.hpp"
#include "LedRing/LedRing.hpp"
#include "PIR/Pir.hpp"
#include "Rfid/Reader.hpp"



namespace dps {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class App
{
  enum
  {
    // Led_DIn     = 33,
    // PIR_IO      = 39,

    // NrButtons   = 3,
    // S1_IO       = 34,
    // S2_IO       = 35,
    // S3_IO       = 32,

    Led_DIn        = 27, // 33,
    PIR_IO         = 9,  // 39,

    NrButtons      = 3,
    S1_IO          = 13,
    S2_IO          = 12,
    S3_IO          = 11,

  };

  using JsonDoc = ArduinoJson::StaticJsonDocument<512>;

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  enum
  {
    Polling_ms = 10,
  };

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Konstruktor.
  App() : PIR         (PIR_IO ),
          Ring        (Led_DIn),
          RoutineTimer(15000  ),
          Steppers    {{AccelStepper(AccelStepper::FULL4WIRE, 19, 18, 17, 16),
                        AccelStepper(AccelStepper::FULL4WIRE, 26, 22, 21, 29)}}
  {
    sleep_ms(100);
    Ring = 100;
    Ring = "startup";

    for (auto& s : Steppers)
    {
      s.setMaxSpeed    ( 200.0);
      s.setAcceleration( 100.0);
      s.setSpeed       ( 200.0);
      s.moveTo(10000);
    }
  }

  bool onoff = false;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Polling-Funktor.
  void operator()()
  {
    common::delta::TimeDelta<>::tick(Polling_ms);

    handleUsart();

  
Enabled = true;
    if (Enabled)
    {
      handleButtons();

      {
        auto uid = Reader();

        if (uid != nullptr)
        {
          digitalWrite(25, onoff);
          onoff = !onoff;



          // Tag wurde gescannt

          Ring = "check";
          transmitUid(*uid);
        }
      }

      if (PIR())
      {
        // Änderung am Bewegungsmelder -> übretragen + Ring aktivieren
        transmitPIR();
        if (!StayActive)
        {
          Ring = *PIR ? "activate" : "deactivate";
        }
      }
    }

    if (RoutineTimer())
    {
      JsonDoc doc = createDoc("status");
      doc["PIR"]  = *PIR ? "true" : "false";

      for (auto& button : Buttons)
      {
        doc[button.Name] = button.getStateText();
      }

      serializeJson(doc, Serial);
      Serial.write('\n');    
    }

    Ring();
  }

  void pollSteppers()
  {
    for (auto& s : Steppers)
    {
      s.run();
    }
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  /// eingehende Nachrichten vom USART behandeln.
  void handleUsart()
  {
    while (Serial.available()) 
    {
      auto c = Serial.read();
      Serial.write(c);

      if (c != '\n')
      {
        // Zeichen hinzufügen
        InputBuffer += c;
      }
      else
      {
        // Nachricht komplett -> parsen
        JsonDoc msg;
        if (deserializeJson(msg, InputBuffer) == ArduinoJson::DeserializationError::Ok) 
        {          
          String action = msg["action"];

          if      (action == "set")
          {
            setFromJson(msg);
          }
          else if (action == "enable")
          {
            Ring       = "";
            Enabled    = true;
          }          
          else if (action == "disable")
          {
            Ring       = "shutdown";
            Enabled    = false;
          }          
          else if (action == "reboot")
          {
            while(true) {};
          }        
        }

        InputBuffer = "";
      }
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Überträgt den Zustand des Bewegungsmelders.
  void transmitPIR()
  {
    auto state = *PIR;

    JsonDoc event = createDoc("event");
    event["PIR"]  = state ? "true" : "false";

    serializeJson(event, Serial);
    Serial.write('\n');

    RoutineTimer.reset();   
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Klingeltaster auswerten.
  void handleButtons()
  {
    bool anyPressed = false;

    for (auto& button : Buttons)
    {
      if (button())
      {
        JsonDoc event      = createDoc("event");
        event[button.Name] = button.getStateText();

        if (*button == button::State::Pressed)
        {
          anyPressed = true;
        }

        serializeJson(event, Serial);
        Serial.write('\n');        
        RoutineTimer.reset();   
      }
    }

    if (anyPressed)
    {
      Ring = "bell"; 
    };
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// UID des gelesenen Tags übertragen.
  void transmitUid(MFRC522::Uid& uid)
  {
    char buffer[100];
    auto doc = createDoc("event");

    auto rfid = doc.createNestedObject("tag");
    {
      char* end = buffer;
      end += sprintf(end, "%02x", uid.sak);
      *end = '\0';

      String s    = buffer;
      rfid["sak"] = s;
    }
    {
      char* end = buffer;
      for (byte i = 0; i <  uid.size; i++)
      {
        end += sprintf(end, "%02x", uid.uidByte[i]);
      }
      *end = '\0';
      String s    = buffer;
      rfid["uid"] = s;
    }

    serializeJson(doc, Serial);
    Serial.write('\n');
    RoutineTimer.reset();
  }



  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// eingehende JSON-Sollwertkommandos behandeln.
  void setFromJson(JsonDoc const& doc)
  {
    if (doc.containsKey("bright"))
    {
      uint32_t brightness = doc["bright"];
      Ring                = brightness;
    }
    if (doc.containsKey("led"))
    {
      String led = doc["led"];

      if (led == "activate")
      {
        if (!*PIR)
        {
          Ring     = led;
        }
        StayActive = true;
      }
      else if (led == "deactivate")
      {
        if (!*PIR)
        {
          Ring     = led;
        }
        StayActive = false;
      }
      else
      {
        Ring       = led;
      }

    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// erstellt ein neues JSON-Dokument.
  /// @param action  Inhalt des action-Keys
  JsonDoc createDoc(char const* action)
  {
    JsonDoc doc;
    doc["action"] = action;

    return doc;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  std::array<button::Button, NrButtons> Buttons =
  {{
    button::Button(S1_IO, "S1"),
    button::Button(S2_IO, "S2"),
    button::Button(S3_IO, "S3")
  }};

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  std::array<AccelStepper, 2> Steppers;
  

  pir::Pir     PIR;
  rfid::Reader Reader;
  String       InputBuffer;

  common::delta::PeriodicTimer<> RoutineTimer;

  bool         Enabled    = false;
  bool         StayActive = false;


  led::LedRing Ring;

};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
} // namespace dps

#endif // APP_INCLUDED_HPP


