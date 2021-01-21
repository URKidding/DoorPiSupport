#ifndef common_DELTA_START_STOP_TIMER_INCLUDED
#define common_DELTA_START_STOP_TIMER_INCLUDED
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

/// Periodisch ablaufender, sich wiederholender Software-Timer.
///
/// Zeitversätze durch asynchrone Verarbeitung werden automatisch kompensiert, so dass eine konstante, präzise Zykluszeit gewährleistet bleibt.
/// Auch wenn die die Abtastung so selten erfolgt, dass ein oder mehrere Intervalle übergangen wurden, verbleibt der Timer im konfigurierten Raster ohne
/// Phasenversatz.
///
/// @tparam TimeBaseIndex  Index der gemeinsamen Zeitbasis
template <size_t TimeBaseIndex = 0>
class StartStopTimer : public PeriodicTimer<TimeBaseIndex>
{
  using TTimer = PeriodicTimer<TimeBaseIndex>;
  
  enum class Modes
  {
    Stopped,
    Expired,
    Started,
    Cyclical,
  };
  
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  /// setzt eine neue Referenzzeitmarke.
  using TTimer::reset;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Konstruktor.
  /// @param cycleTime   Zykluszeit
  /// @param offsetTime  ggf. Offset
  inline StartStopTimer(uint32_t cycleTime  = 0,
                        uint32_t offsetTime = 0)
    : TTimer   (cycleTime, offsetTime),
      Mode     (Modes::Stopped)
  {
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void stop()
  {
    Mode = Modes::Stopped;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void start()
  {
    reset();
    Mode = Modes::Started;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void start(uint32_t cycleTime)
  {
    reset();
    setCycleTime(cycleTime);
    Mode = Modes::Started;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void startCyclical()
  {
    reset();
    Mode = Modes::Cyclical;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline void startCyclical(uint32_t cycleTime, uint32_t offsetTime = 0)
  {
    reset(offsetTime);
    setCycleTime(cycleTime);
    Mode = Modes::Cyclical;
  }
  
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  inline bool running() const
  {
    return (Mode != Modes::Stopped) && (Mode != Modes::Expired);
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Arbeitet den periodischen Timer ab.
  /// Startet automatisch das Folgeintervall unter Berücksichtigung eines ggf. aufgelaufenen Zeitverzugs, so dass sich Zeitabweichungen durch
  /// verspäteten Aufruf nicht summieren. Ausgelassene Zeitscheiben werden NICHT nachgeholt.
  /// @return true = Zykluszeit ist abgelaufen
  bool operator()()
  {
    switch (Mode)
    {
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    case Modes::Stopped:
      return false;
      
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    case Modes::Expired:
      return true;
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    default:
      if (Mode == Modes::Started)
      {
        if (elapsed())
        {
          Mode = Modes::Expired;
          return true;
        }
      }
      else
      {
        return TTimer::operator()();
      }
    }
    return false;
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  Modes    Mode = Modes::Stopped;
};



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common::delta


#endif // common_DELTA_START_STOP_TIMER_INCLUDED
