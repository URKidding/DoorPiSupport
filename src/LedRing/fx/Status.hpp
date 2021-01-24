#ifndef FX_STATUS_INCLUDED_HPP
#define FX_STATUS_INCLUDED_HPP

#include <algorithm>
#include "ILedFx.hpp"
#include "Statemachine/TimedStatemachine.hpp"
#include <Adafruit_NeoPixel.h>

namespace dps { namespace led { namespace fx {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class Status : public ILedFX
{
  enum
  {
    TargetLevel   =  100,
    Fade_ms       =  100,
  };

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  Status(Adafruit_NeoPixel& parent, uint32_t r, uint32_t g, uint32_t b, uint32_t stay_ms = 1000) : Parent(parent), R(r), G(g), B(b), Stay_ms(stay_ms)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator()() override
  {
    do {
      switch (Machine())
      {
      // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
      case States::FadeOut:
        {
          int t = Machine.dwell();

          auto fade_pph256 = (t * 256) / Fade_ms;

          for (auto i = 0; i < Parent.numPixels(); ++i)
          {
            auto r = mix(TargetLevel, 0, fade_pph256);
            auto g = mix(TargetLevel, 0, fade_pph256);
            auto b = mix(TargetLevel, 0, fade_pph256);

            Parent.setPixelColor(i, r, g, b);
          }
          Parent.show();
          
          if (t >= Fade_ms)
          {
            Machine = States::Color;
          }
        }
        break;
        
      // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
      case States::Color:
        if      (Machine.stateEntry())
        {
          for (auto i = 0; i < Parent.numPixels(); ++i)
          {
            Parent.setPixelColor(i, R, G, B);
          }
          Parent.show();
        }
        else if (Machine.hasExpired(Stay_ms))
        {
          Machine = States::FadeIn;
        }
        break;


        // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
      case States::FadeIn:
        {
          int t = Machine.dwell();

          auto fade_pph256 = (t * 256) / Fade_ms;

          for (auto i = 0; i < Parent.numPixels(); ++i)
          {
            auto r = mix(R, TargetLevel, fade_pph256);
            auto g = mix(G, TargetLevel, fade_pph256);
            auto b = mix(B, TargetLevel, fade_pph256);

            Parent.setPixelColor(i, r, g, b);
          }
          Parent.show();
          
          if (t >= Fade_ms)
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
  uint32_t mix(int32_t a, int32_t b, int32_t fade_pp256)
  {
    return a + ((b - a) * fade_pp256) / 256;
  }

  enum class States
  {
    FadeOut,
    Color,
    FadeIn,
    Finished
  };
  common::TimedStatemachine<States> Machine;

  Adafruit_NeoPixel& Parent;

  uint32_t R;
  uint32_t G;
  uint32_t B;
  uint32_t Stay_ms;
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}}} // namespace dps::led::fx

#endif // FX_STATUS_INCLUDED_HPP
