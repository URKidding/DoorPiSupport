#ifndef TIMED_STATEMACHINE_HPP_INCLUDED
#define TIMED_STATEMACHINE_HPP_INCLUDED

#include "Delta/TimeDelta.hpp"
#include "Statemachine.hpp"


namespace common {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


//==============================================================================================================================================================
/// Templateklasse für gepollte Zustandsautomaten mit Zustandstimer.
///
/// ## Features ##
/// - konventionelle Transitionen und Schnelltransitionen (bei letzteren wird das Automatenkonstrukt nicht verlassen, sondern anstelle des Austritts aus dem
///   Automaten erfolgt ein weiterer Durchlauf, in dem der Zielzustand der Schnelltransition abgearbeitet wird.
/// - Entry- und Exit-Actions
/// @tparam TStates  Enumerationstyp mit Zustandsliteralen
template <typename TStates, size_t TimeBaseIndex = 0>
class TimedStatemachine : public Statemachine<TStates>
{
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  /// Konstruktor.
  /// @param initialState  (optinal) Initialzustand des Automaten
  explicit inline constexpr TimedStatemachine(TStates initialState = static_cast<TStates>(0))
    : Statemachine<TStates>(initialState)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Polling-Funktor.
  /// Führt die während der Abarbeitung der States eingeleiteten Transitionen aus.
  /// Muss zyklisch gepollt werden.
  inline TStates operator()()
  {
    TStates state = Statemachine<TStates>::operator()();

    if (stateEntry())
    {
      // Eintritt in State -> Timestamp erfassen.
      resetTimer();
    }

    return state;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Verweilzeit im aktuellen State seit Eintritt.
  /// @return Verweilzeit
  inline void resetTimer()
  {
    StateTimer.reset();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Verweilzeit im aktuellen State seit Eintritt.
  /// @return Verweilzeit
  inline uint32_t dwell() const
  {
    return StateTimer.get();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// prüft, ob die angegebene Zeit
  /// @return Verweilzeit
  inline bool hasExpired(uint32_t time) const
  {
    return dwell() >= time;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  using Statemachine<TStates>::operator=;
  using Statemachine<TStates>::stateEntry;
  using Statemachine<TStates>::stateExit;


//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  delta::TimeDelta<TimeBaseIndex> StateTimer;
};
//==============================================================================================================================================================


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common


#endif // TIMED_STATEMACHINE_HPP_INCLUDED
