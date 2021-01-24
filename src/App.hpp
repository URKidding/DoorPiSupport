#ifndef APP_INCLUDED_HPP
#define APP_INCLUDED_HPP

#include <Arduino.h>
#include <ArduinoJson.hpp>
#include "Button/Button.hpp"
#include "LedRing/LedRing.hpp"
#include "PIR/Pir.hpp"
#include "Rfid/Reader.hpp"
#include <sstream>
#include <iomanip>

namespace dps {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class App
{
  enum
  {
    Led_DIn     = 33,
    PIR_IO      = 39,

    NrButtons   = 3,
    S1_IO       = 34,
    S2_IO       = 35,
    S3_IO       = 32,

    Servo1_IO   = 22,
    Servo2_IO   = 23,
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
  App() : PIR(PIR_IO), Ring(Led_DIn)
  {
    Ring.setBrightness(100);
    Ring = "booting";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Polling-Funktor.
  void operator()()
  {
    common::delta::TimeDelta<>::tick(Polling_ms);

    handleUsart();

    if (Enabled)
    {
      handleButtons();

      {
        auto uid = Reader();
        if (uid != nullptr)
        {
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

    Ring();
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
          std::string action = msg["action"];

          if      (action == "set")
          {
            setFromJson(msg);
          }
          else if (action == "enable")
          {
            Ring       = "";
            Enabled    = true;
          }          
          else if (action == "reboot")
          {
            while(true) {};
          }        
        }

        InputBuffer.clear();
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
    auto doc = createDoc("event");

    auto rfid = doc.createNestedObject("tag");
    {
      std::stringstream ss;
      ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uid.sak);
      rfid["sak"] = ss.str();
    }
    {
      std::stringstream ss;
      for (byte i = 0; i <  uid.size; i++)
      {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uid.uidByte[i]);
      }
      rfid["uid"] = ss.str();
    }

    serializeJson(doc, Serial);
    Serial.write('\n');
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
      std::string led = doc["led"];

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

  pir::Pir     PIR;
  led::LedRing Ring;
  rfid::Reader Reader;
  std::string  InputBuffer;

  bool         Enabled    = false;
  bool         StayActive = false;
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
} // namespace dps

#endif // APP_INCLUDED_HPP
