#ifndef common_TIME_DELTA_INCLUDED
#define common_TIME_DELTA_INCLUDED
///#############################################################################################################################################################
///
/// @file
///
/// @brief Messung von Zeitdifferenzen.
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


namespace common { namespace delta {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


/// Klasse zur Messung von Zeitdifferenzen.
///
/// Alle TimeDelta-Instanzen einer spezifischen, auf einem periodisch inkrementierten und ggf. überlaufenden Zähler beruhender Zeitbasis erlauben die
/// voneinander unabhängig Messung von Zeitdifferenzen gegenüber einem triggerbaren Referenzpunkt (reset()).
///
/// Die Zeitbasis muss zentral durch Aufruf der statischen tick()-Methode aktualisiert werden.
///
/// @tparam TimeBaseIndex  Index der gemeinsamen Zeitbasis
template <size_t TimeBaseIndex = 0>
class TimeDelta
{

//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  //===== Konstruktion =========================================================================================================================================
  /// Konstruktor.
  /// @param offset  ggf. Offset, der die initiale Referenzzeitmake um diesen Offset in die Vergangenheit verschiebt
  inline TimeDelta(uint32_t  offset = 0)
  {
    reset(offset);
  }



  //===== Rücksetzen/Auswerten =================================================================================================================================
  /// setzt eine neue Referenzzeitmarke für spätere get()-Operationen.
  /// @param offset  verschiebt die Referenzzeitmake um diesen Offset in die Vergangenheit
  inline void reset(uint32_t offset = 0)
  {
    Capture = Counter - offset;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Zeitdifferenz zur zuletzt gesetzten Referenzzeitmarke.
  /// @return Zeitdifferenz.
  inline uint32_t get() const
  {
    return Counter - Capture;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert die Zeitdifferenz zur zuletzt gesetzten Referenzzeitmarke.
  /// @return Zeitdifferenz.
  inline uint32_t operator*() const
  {
    return get();
  }


  //===== gemeinsame Zeitbasis =================================================================================================================================
  /// Inkrementiert die gemeinsame Zeitbasis um eine Bestimmte Anzahl von Zeiteinheiten.
  ///
  /// @attention Jede Zeitbasis, die durch eine Instanz von TimeDelta oder seinen Derivaten zum Einatz kommt, muss an zentraler Stelle fortlaufend
  ///            durch Aufruf dieser Methode aktualisiert werden.
  ///
  /// Der Aufruf kann streng periodisch unter Angabe einer konstanten Zeitdifferenz erfolgen (z.B. tick(100) für eine Mikrosekunden-Zeitbasis,
  /// aufgerufen durch einen 100µs Hardware-Timerinterrupt), oder durch hinreichend häufige Aufrufe unter Angabe einer variablen Zahl von Zeiteinheiten,
  /// die sei dem letzten Aufruf vergangen sind.
  /// 
  ///
  /// @param timeUnitsPassed  Anzahl der Zeiteinheiten, die seit dem letzten Tick vergangen sind.
  static void inline tick(uint32_t timeUnitsPassed = 1)
  {
    Counter += timeUnitsPassed;
  }

//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  static uint32_t          Counter; ///< gemeinsamer Systemzeitzähler für die Zeitbasis \p TimeBaseIndex
  uint32_t                 Capture; ///< Referenzzeitmarke (@see reset())
};

template <size_t TimeBaseIndex>
uint32_t TimeDelta<TimeBaseIndex>::Counter = 0;


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} } // namespace common::delta


#endif // common_TIME_DELTA_INCLUDED
