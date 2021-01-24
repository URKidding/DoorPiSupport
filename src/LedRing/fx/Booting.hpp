#ifndef FX_BOOTING_HPP_INCLUDED
#define FX_BOOTING_HPP_INCLUDED

#include <algorithm>
#include "ILedFx.hpp"
#include "Statemachine/TimedStatemachine.hpp"
#include <Adafruit_NeoPixel.h>
#include "Delta/PeriodicTimer.hpp"
namespace dps { namespace led { namespace fx {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

//==============================================================================================================================================================
class Booting : public ILedFX
{
  enum
  {
    TargetLevel   =  100,
    T1            =  67,
    T2            =  27
  };

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  Booting(Adafruit_NeoPixel& parent) : Parent(parent), Timer1(T1), Timer2(T2)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  bool operator()() override
  {
    if (Timer1())
    {
      Pos1 %= Parent.numPixels();
      States[Pos1] = !States[Pos1];      
      ++Pos1;
    }
    if (Timer2())
    {
      Pos2 %= Parent.numPixels();
      States[Pos2] = !States[Pos2];
      ++Pos2;
    }

    for (auto i = 0; i < Parent.numPixels(); ++i)
    {
      auto& c = Levels[i];
      if (States[i])
      {
        c = std::min(50, c + 20);
      }
      else
      {
        c = std::max(0, c - 10);
      }

      Parent.setPixelColor(i, 0, c, c);
      Parent.show();
    }

    return false;
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================


  int                            Pos1 = 0;
  int                            Pos2 = 0;

  std::array<bool, 12>           States = {{ false }};
  std::array<uint8_t, 12>        Levels = {{ 0 }};

  Adafruit_NeoPixel&             Parent;
  common::delta::PeriodicTimer<> Timer1;
  common::delta::PeriodicTimer<> Timer2;
};


//==============================================================================================================================================================

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
}}} // namespace dps::led::fx

#endif // FX_BOOTING_HPP_INCLUDED
