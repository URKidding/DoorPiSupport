#ifndef common_CYCLICAL_HPP_INCLUDED
#define common_CYCLICAL_HPP_INCLUDED
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


#include "common/Delta/Delta.hpp"

namespace common::delta {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


//==============================================================================================================================================================
/// zyklischer Timer.
/// @tparam T  Systemzeitzählertyp
template <typename T = uint32_t>
class Cyclical : public Delta<T>
{
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  using Delta<T>::capture;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Konstruktor.
  /// @param systemTime  Pointer auf Systemzeitquelle
  /// @param cycleTime   Zykluszeit
  /// @param offsetTime  ggf. Offset
  inline Cyclical(T const volatile& systemTime,
                  T                 cycleTime,
                  T                 offsetTime = 0)
    : Delta<T>(systemTime, offsetTime), CycleTime(cycleTime)
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Arbeitet den zyklischen Timer ab.
  /// Startet automatisch das Folgeintervall unter Berücksichtigung eines ggf. aufgelaufenen Zeitverzugs, so dass sich Zeitabweichungen durch
  /// verspäteten Aufruf nicht summieren. Ausgelassene Zeitscheiben werden NICHT nachgeholt.
  /// @return true = Zykluszeit ist abgelaufen
  bool operator()()
  {
    auto diff = Delta<T>::get();
    if (diff >= CycleTime)
    {
      diff -= CycleTime;
      capture((diff < CycleTime) ? diff : (diff % CycleTime));
      return true;
    }

    return false;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// setzt die Zykluszeit.
  /// @param cycleTime  gewünschte Zykluszeit
  inline void setCycleTime(T cycleTime) { CycleTime = cycleTime; }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die eingestellte Zykluszeit.
  /// @return Zykluszeit
  inline T    getCycleTime() const      { return CycleTime;      }



//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  T CycleTime;
};
//==============================================================================================================================================================


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common::delta


#endif // common_CYCLICAL_HPP_INCLUDED
