#ifndef LED_RING_INCLUDED_HPP
#define LED_RING_INCLUDED_HPP

#include <memory> 
#include <Adafruit_NeoPixel.h>
#include "Delta/TimeDelta.hpp"
#include "fx/ILedFx.hpp"
#include "fx/Activate.hpp"
#include "fx/Booting.hpp"
#include "fx/Deactivate.hpp"
#include "fx/Status.hpp"

namespace dps { namespace led {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class LedRing : public Adafruit_NeoPixel
{
  enum
  {
    NrPixels = 12
  };

  using TFx = std::unique_ptr<fx::ILedFX>;

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  LedRing(uint8_t pin) : Adafruit_NeoPixel(NrPixels, pin, NEO_GRB + NEO_KHZ800)
  {
    begin();
    setBrightness(50);
    show(); // Initialize all pixels to 'off'
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator=(uint32_t brightness)
  {
    Brightness = brightness;
    return true;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator=(std::string const& fx)
  {
    if      (fx == "activate")
    {
      CurrentFx = TFx(new fx::Activate(*this, Brightness));
    }
    else if (fx == "deactivate")
    {
      CurrentFx = TFx(new fx::Deactivate(*this, Brightness));
    }
    else if (fx == "pass")
    {
      CurrentFx = TFx(new fx::Status(*this, 0, Brightness, 0));
    }
    else if (fx == "fail")
    {
      CurrentFx = TFx(new fx::Status(*this, Brightness, 0, 0));
    }
    else if (fx == "check")
    {
      CurrentFx = TFx(new fx::Status(*this, Brightness / 2, Brightness / 2, 0));
    }
    else if (fx == "bell")
    {
      CurrentFx = TFx(new fx::Status(*this, 0, 0, Brightness));
    }
    else if (fx == "booting")
    {
      CurrentFx = TFx(new fx::Booting(*this));
    }
    else
    {
      CurrentFx.release();
      return false;
    }

    return true;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator()()
  {
    if (CurrentFx)
    {
      if ((*CurrentFx)())
      {
        // Effekt fertig ausgeführt
        CurrentFx.release();
      }
      return false;
    }

    return true;
  }



//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  TFx      CurrentFx;
  uint32_t Brightness = 100;

};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}} // namespace dps::led

#endif // LED_RING_INCLUDED_HPP
