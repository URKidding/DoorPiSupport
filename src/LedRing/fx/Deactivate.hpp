#ifndef FX_DEACTIVATE_INCLUDED_HPP
#define FX_DEACTIVATE_INCLUDED_HPP

#include <algorithm>
#include "ILedFx.hpp"
#include "Statemachine/TimedStatemachine.hpp"
#include "Pico_WS2812/WS2812.hpp"

namespace dps { namespace led { namespace fx {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class Deactivate : public ILedFX
{
  enum
  {
    BlueFade_ms   = 100,
    BlueStep_ms   =  30,
    WhiteDelay_ms =  50,
    FadeOff_ms    = 750,
  };

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  Deactivate(WS2812& parent, uint32_t brightness) : Parent(parent), Brightness(brightness)
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
            auto blue   = std::max(0, std::min((int)Brightness, static_cast<int>((d * Brightness) / FadeOff_ms )));
            auto bright = std::max(0, std::min((int)Brightness, static_cast<int>((t * Brightness) / BlueFade_ms)));

            Parent.setPixelColor(i, Brightness - bright, Brightness - bright, Brightness - blue);

            t -= BlueStep_ms;
          }
          Parent.show();
          
          if (d >= (FadeOff_ms + WhiteDelay_ms))
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

  WS2812&            Parent;
  int32_t            Brightness;
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}}} // namespace dps::led::fx

#endif // FX_DEACTIVATE_INCLUDED_HPP
