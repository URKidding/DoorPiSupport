#ifndef common_DELTA_HPP_INCLUDED
#define common_DELTA_HPP_INCLUDED
///#############################################################################################################################################################
///
/// @file
///
/// @brief Delta-Berechnung für überlaufende Zähler
///
/// Projekt: common Standard
///
///#############################################################################################################################################################
///
/// (C) 2019 Magnet Schultz GmbH & Co. KG
///
///#############################################################################################################################################################


#include <stdint.h>
#include <type_traits>


namespace common::delta {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


//==============================================================================================================================================================
/// Delta-Klasse.
///
/// Dient der Berechnung der Differenz eines (capturebaren) Wertes von einem fortlaufenden, am Ende des Wertebreichs umbrechenden Zähler.
/// Typische Anwendung: Detektion abgelaufener Zeitspannen auf Basis eines fortlaufenden Systemzeitzählers, dessen Stand per capture() durch Klasse
/// aufgezeichnet, und die seither vergangene Zeit per get() bestimmt werden kann.
/// @tparam T  Zählertyp
template <typename T = uint32_t>
class Delta
{
  static_assert(!std::is_signed<T>::value, "vorzeichenbehaftete Zähler werden nicht unterstützt!");

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  /// Konstruktor.
  inline Delta(
               T const volatile& counter,   ///< Pointer auf Zählerquelle
               T                 offset = 0 ///< ggf. Offset
              )
    : Counter(counter)
  {
    capture(offset);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// sichert den aktuellen Zählerstand als Referenz für spätere get()-Operationen mit dem angegebenen Offset.
  inline void capture(T offset = 0)
  {
    Capture = Counter - offset;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Differenz des aktuellen Zählerstandes vom zuletzt per capture() aufgezeichneten Stand.
  /// @return Zählerdifferenz.
  inline auto get() const
  {
    return Counter - Capture;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Differenz des aktuellen Zählerstandes vom zuletzt per capture() aufgezeichneten Stand.
  /// @return Zählerdifferenz.
  inline T operator*() const
  {
    return get();
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// sichert den aktuellen Zählerstand als Referenz für spätere get()-Operationen.
  ///
  /// Der Offset wird eigenständig aus der tatsächlichen Zählerdifferenz und der erwarteten Differenz (interval) berücksichtigt, und der Capture-Wert auf
  /// diese Weise entsprechend vorgespannt.
  ///
  /// Typische Anwendung:
  /// Ein freilaufender Zähler wird für Timingzwecke verwendet. Beim Prüfen der Zähler-/Zeitdifferenz ist das gewünschte Intervall m
  /// bereits um n Zählerinkrementeüberschritten überschritten. Mittels captureInterval([instance], m) wird der Capture-Wert um n vorgespannt, so
  /// dass das nächste Intervall bereits nach m - n Zählerinkrementen erreicht ist, und somit ein Aufaddieren der Abweichungen unterbunden wird.
  ///
  /// @param interval   Soll-Intervall/aktuelles Soll-Delta
  inline void captureInterval(T interval)
  {
    T offset = get() - interval;


    capture((offset < interval) ? offset : (offset % interval));
  }


//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  T const volatile& Counter; ///< überlaufender Zähler
  T                 Capture; ///< zuletzt gemerkter Zählerwert
};
//==============================================================================================================================================================



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common::delta


#endif // common_DELTA_HPP_INCLUDED
