#ifndef APP_INCLUDED_HPP
#define APP_INCLUDED_HPP

#include <ArduinoJson.hpp>
#include "LedRing/LedRing.hpp"
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
    PIR_Pin     = 39,
    S1_Pin      = 34,
    S2_Pin      = 35,
    S3_Pin      = 32,
  };

  using JsonDoc = ArduinoJson::StaticJsonDocument<512>;

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  enum
  {
    Polling_ms = 10,
  };

  App() : Ring(Led_DIn)
  {
    Ring.setBrightness(100);
    delay(500);

    pinMode(PIR_Pin, INPUT);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()()
  {
    common::delta::TimeDelta<>::tick(Polling_ms);

    handleUsart();

    {
      auto pir = digitalRead(PIR_Pin);
      if (pir != PirActive)
      {
        Serial.print(pir);
        PirActive = pir;
        Ring = pir ? "activate" : "deactivate";
      }
    }
    
    auto uid = Reader();
    if (uid)
    {
      handleUid(*uid);
      Ring = "pass";
    }

    Ring();
    delay(Polling_ms);
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
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
  void handleUid(MFRC522::Uid& uid)
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
  void setFromJson(JsonDoc const& doc)
  {
    if (doc.containsKey("bright"))
    {
      uint32_t brightness = doc["bright"];
      Ring                = brightness;
    }
    if (doc.containsKey("fx"))
    {
      std::string fx = doc["fx"];
      Ring           = fx;
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  JsonDoc createDoc(char const* action)
  {
    JsonDoc doc;
    doc["action"] = action;

    return doc;
  }
  led::LedRing Ring;
  rfid::Reader Reader;
  std::string  InputBuffer;

  bool PirActive = false;

};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
} // namespace dps

#endif // APP_INCLUDED_HPP
