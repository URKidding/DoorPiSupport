#ifndef common_DELTA_BLINKER_INCLUDED
#define common_DELTA_BLINKER_INCLUDED
///#############################################################################################################################################################
///
/// @file
///
/// Projekt: common Standard
///
///#############################################################################################################################################################
///
/// (C) 2019 Magnet Schultz GmbH & Co. KG
///
///#############################################################################################################################################################


#include "PeriodicTimer.hpp"

namespace common::delta {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


//==============================================================================================================================================================
/// Blinkfunktion, die an einen zentralen Delta-Timer gekoppelt ist.
/// Generiert ein Blinksignal.
/// Die Instnz muss zentrl per operator()() aktualisiert werden. Der Blinkzustand kann per blinkState() ermittelt werden.
/// @tparam TimeBaseIndex  Index der gemeinsamen Zeitbasis
template <size_t TimeBaseIndex = 0>
class Blinker : private PeriodicTimer<TimeBaseIndex>
{
  using TPeriodicTimer = PeriodicTimer<TimeBaseIndex>;
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  using TPeriodicTimer::reset;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Konstruktor.
  /// @param cycleTime   Zykluszeit
  /// @param offsetTime  ggf. Offset
  inline Blinker(uint32_t cycleTime,
                 uint32_t offsetTime = 0)
    : TPeriodicTimer(cycleTime, offsetTime)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Funktor zur Rechenzeitzuteilung.
  /// Muss zyklisch an zentraler Stelle aufgerufen werden.
  inline bool operator()()
  {
    if (TPeriodicTimer::operator()())
    {
      State = !State;
    }
    return State;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// setzt den Blinkzyklus zurück.
  /// @param state       gewünschter Blinkzustand.
  /// @param offsetTime  Zeitversatz zum Beginn einer Blinkperiode.
  inline void reset(bool state, uint32_t offsetTime = 0)
  {
    State = state;
    TPeriodicTimer::reset(offsetTime);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  using TPeriodicTimer::setCycleTime;
  using TPeriodicTimer::getCycleTime;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert den Blinkzustand.
  /// @return Blinkzustand
  static inline bool blinkState()
  {
    return State;
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  static bool State; ///< Blinkzustand
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <size_t TimeBaseIndex>
bool Blinker<TimeBaseIndex>::State = true;
//==============================================================================================================================================================


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common::delta


#endif // common_DELTA_BLINKER_INCLUDED
