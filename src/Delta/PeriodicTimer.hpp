#ifndef common_DELTA_PERIODIC_TIMER_INCLUDED
#define common_DELTA_PERIODIC_TIMER_INCLUDED
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


#include "TimeDelta.hpp"

namespace common { namespace delta {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

/// Periodisch ablaufender, sich wiederholender Software-Timer.
///
/// Zeitversätze durch asynchrone Verarbeitung werden automatisch kompensiert, so dass eine konstante, präzise Zykluszeit gewährleistet bleibt.
/// Auch wenn die die Abtastung so selten erfolgt, dass ein oder mehrere Intervalle übergangen wurden, verbleibt der Timer im konfigurierten Raster ohne
/// Phasenversatz.
///
/// @tparam TimeBaseIndex  Index der gemeinsamen Zeitbasis
template <size_t TimeBaseIndex = 0>
class PeriodicTimer : public TimeDelta<TimeBaseIndex>
{
  using TTimer = TimeDelta<TimeBaseIndex>;
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  /// setzt eine neue Referenzzeitmarke.
  using TTimer::reset;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Konstruktor.
  /// @param cycleTime   Zykluszeit
  /// @param offsetTime  ggf. Offset
  inline PeriodicTimer(uint32_t cycleTime,
                       uint32_t offsetTime = 0)
    : TTimer   (offsetTime),
      CycleTime(cycleTime )
  {
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Arbeitet den periodischen Timer ab.
  /// Startet automatisch das Folgeintervall unter Berücksichtigung eines ggf. aufgelaufenen Zeitverzugs, so dass sich Zeitabweichungen durch
  /// verspäteten Aufruf nicht summieren. Ausgelassene Zeitscheiben werden NICHT nachgeholt.
  /// @return true = Zykluszeit ist abgelaufen
  inline bool elapsed() const
  {
    return TTimer::get() >= CycleTime;
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Arbeitet den periodischen Timer ab.
  /// Startet automatisch das Folgeintervall unter Berücksichtigung eines ggf. aufgelaufenen Zeitverzugs, so dass sich Zeitabweichungen durch
  /// verspäteten Aufruf nicht summieren. Ausgelassene Zeitscheiben werden NICHT nachgeholt.
  /// @return true = Zykluszeit ist abgelaufen
  bool operator()()
  {
    auto diff = TTimer::get();
    if (diff >= CycleTime)
    {
      diff -= CycleTime;
      reset((diff < CycleTime) ? diff : (diff % CycleTime));
      return true;
    }

    return false;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// setzt die Zykluszeit.
  /// @param cycleTime  Zykluszeit
  inline void     setCycleTime(uint32_t cycleTime) { CycleTime = cycleTime; }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Zykluszeit.
  /// @return  Zykluszeit
  inline uint32_t getCycleTime() const             { return CycleTime;      }



//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  uint32_t CycleTime;  ///< Zykluszeit
};



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
}} // namespace common::delta


#endif // common_DELTA_PERIODIC_TIMER_INCLUDED
