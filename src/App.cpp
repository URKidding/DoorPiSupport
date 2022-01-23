#include "App.hpp"


using namespace dps;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Konstruktor.
App::App() : PIR         (PIR_IO ),
        Ring        (Led_DIn),
        RoutineTimer(15000  ),
        AliveTimer  (500    )
{
  Ring = 100;
  Ring = "startup";

  pinMode(LED_BUILTIN, OUTPUT);

  Enabled = true;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void App::operator()()
{
  common::delta::TimeDelta<>::tick(Polling_ms);

  // Alive-LED blinken
  digitalWrite(LED_BUILTIN, AliveTimer());

  // eingehende Nachrichten verarbeiten
  handleUsart ();

  Enabled = true;

  if (Enabled)
  {
    // Klingeltaster verarbeiten
    handleButtons();

    // RFID-Reader
    {
      auto uid = Reader();

      if (uid != nullptr)
      {
        // Tag wurde gescannt
        Ring = "check";
        transmitUid(*uid);
      }
    }

    // PIR-Sensor
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

  // zyklische Routineausgabe
  if (RoutineTimer())
  {
    JsonDoc doc = createDoc("status");
    doc["PIR"]  = *PIR ? "true" : "false";

    for (auto& button : Buttons)
    {
      doc[button.Name] = button.getStateText();
    }

    serializeJson(doc, Serial);
    Serial.write ('\n');    
  }

  // LED-Ring
  Ring();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void App::handleUsart()
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
void App::transmitPIR()
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
void App::handleButtons()
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
      Serial.write ('\n');        
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
void App::transmitUid(MFRC522::Uid& uid)
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
void App::setFromJson(JsonDoc const& doc)
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
