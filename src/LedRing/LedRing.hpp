#ifndef LED_RING_INCLUDED_HPP
#define LED_RING_INCLUDED_HPP

#include <memory> 
#include "Pico_WS2812/WS2812.hpp"
#include "Delta/TimeDelta.hpp"
#include "fx/ILedFx.hpp"
#include "fx/Activate.hpp"
#include "fx/Booting.hpp"
#include "fx/Deactivate.hpp"
#include "fx/Status.hpp"

namespace dps { namespace led {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class LedRing //: public Adafruit_NeoPixel
{
  enum
  {
    NrPixels = 12
  };

  using TFx = std::unique_ptr<fx::ILedFX>;

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  LedRing(uint8_t pin) //: Adafruit_NeoPixel(NrPixels, pin, NEO_GRB + NEO_KHZ800)
    : LedDriver(
        pin,                // Data line is connected to pin 0. (GP0)
        12,                 // 12 LEDs.
        pio0,               // Use PIO 0 for creating the state machine.
        0,                  // Index of the state machine that will be created for controlling the LED strip
                            // You can have 4 state machines per PIO-Block up to 8 overall.
                            // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Pixel format used by the LED strip
    )
  {
    LedDriver.fill( WS2812::RGB(10, 10, 0) );
    LedDriver.show();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator=(uint32_t brightness)
  {
    Brightness = brightness;
    return true;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator=(String const& fx)
  {
    if      (fx == "activate")
    {
      CurrentFx = TFx(new fx::Activate(LedDriver, Brightness));
    }
    else if (fx == "deactivate")
    {
      CurrentFx = TFx(new fx::Deactivate(LedDriver, Brightness));
    }
    else if (fx == "pass")
    {
      CurrentFx = TFx(new fx::Status(LedDriver, 0, Brightness, 0));
    }
    else if (fx == "fail")
    {
      CurrentFx = TFx(new fx::Status(LedDriver, Brightness, 0, 0));
    }
    else if (fx == "check")
    {
      CurrentFx = TFx(new fx::Status(LedDriver, Brightness / 2, Brightness / 2, 0));
    }
    else if (fx == "bell")
    {
      CurrentFx = TFx(new fx::Status(LedDriver, 0, 0, Brightness));
    }
    else if (fx == "startup")
    {
      CurrentFx = TFx(new fx::Booting(LedDriver, true));
    }
    else if (fx == "shutdown")
    {
      CurrentFx = TFx(new fx::Booting(LedDriver, false));
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
  WS2812   LedDriver;

  TFx      CurrentFx;
  uint32_t Brightness = 100;

};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}} // namespace dps::led

#endif // LED_RING_INCLUDED_HPP
