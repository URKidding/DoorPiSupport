#ifndef FX_ACTIVATE_INCLUDED_HPP
#define FX_ACTIVATE_INCLUDED_HPP

#include <algorithm>
#include "ILedFx.hpp"
#include "Statemachine/TimedStatemachine.hpp"
#include <Adafruit_NeoPixel.h>

namespace dps { namespace led { namespace fx {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class Activate : public ILedFX
{
  enum
  {
    BlueFade_ms   = 100,
    BlueStep_ms   =  30,
    WhiteDelay_ms = 250,
    WhiteFade_ms  = 750,
  };

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  Activate(Adafruit_NeoPixel& parent, uint32_t brightness) : Parent(parent), Brightness(brightness)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator()() override
  {
    do {
      switch (Machine())
      {
      // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
      case States::Fade:
        {
          int t = Machine.dwell();
          int d = t  - WhiteDelay_ms;
          for (auto i = 0; i < Parent.numPixels(); ++i)
          {
            auto blue   = std::max(0, std::min((int)Brightness, static_cast<int>((t * Brightness) / BlueFade_ms )));
            auto bright = std::max(0, std::min((int)Brightness, static_cast<int>((d * Brightness) / WhiteFade_ms)));
            bright      = std::min(blue, bright);

            Parent.setPixelColor(i, bright, bright, blue);

            t          -= BlueStep_ms;
          }
          Parent.show();
          
          if (d >= (WhiteFade_ms + WhiteDelay_ms))
          {
            Machine = States::Finished;
          }
        }
        break;

      // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
      case States::Finished:
        break;
      }
    } while (Machine.loop());

    return Machine.currentState() == States::Finished;
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  enum class States
  {
    Fade,
    Finished
  };
  common::TimedStatemachine<States> Machine;

  Adafruit_NeoPixel& Parent;
  int32_t            Brightness;
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}}} // namespace dps::led::fx

#endif // FX_ACTIVATE_INCLUDED_HPP
