#ifndef STEPPERS_HPP_INCLUDED
#define STEPPERS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <AccelStepper.h>
#include <Arduino.h>

namespace dps      {
namespace steppers {
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~    

//==============================================================================================================================================================
enum State
{
    Released,
    Pressed,
    LongPress,
    _NrStates
};
//==============================================================================================================================================================

//==============================================================================================================================================================
class Steppers
{
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  enum : int
  {
    InitDistance    = 500,
    InitPosition    = 200,
    MaxSpeed        = 2000,
    Acceleration    = 1000,
    EndstopPosition = 200
  };



  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  explicit Steppers() :
    Units {{{AccelStepper(AccelStepper::FULL4WIRE, 19, 18, 17, 16), 8},
            {AccelStepper(AccelStepper::FULL4WIRE, 26, 22, 21, 29), 7}}}
  {
    for (auto& u : Units)
    {
      pinMode(u.EndStopIO, INPUT_PULLUP);

      u.Stepper.setMaxSpeed    (MaxSpeed    );
      u.Stepper.setAcceleration(Acceleration);
      u.Stepper.setSpeed       (MaxSpeed    );
      u.Stepper.moveTo         (InitDistance);
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void operator()()
  {
    for (auto& u : Units)
    {
      if (!u.Initialised)
      {
        if (   !digitalRead        (u.EndStopIO)
            || !u.Stepper.isRunning()           )
        {
          // Endstopp durch Schalter oder Block erreicht
          u.Stepper.setCurrentPosition(EndstopPosition);
          u.Initialised = true;
        }
      }
      u.Stepper.run();
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void moveTo(int x, int y)
  {
    if (Units[0].Initialised)
    {
      x = std::min(std::max(x, (int) -EndstopPosition), (int) EndstopPosition);
      Units[0].Stepper.moveTo(x);
    }

    if (Units[1].Initialised)
    {
      y = std::min(std::max(y, (int) -EndstopPosition), (int) EndstopPosition);
      Units[0].Stepper.moveTo(y);
    }
  }


//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  struct TUnit
  {
    AccelStepper Stepper;
    int          EndStopIO;
    bool         Initialised = false;
  };
  
  std::array<TUnit, 2> Units;
};
//==============================================================================================================================================================


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~    
}} // namespace staircaselights::dps


#endif // STEPPERS_HPP_INCLUDED
