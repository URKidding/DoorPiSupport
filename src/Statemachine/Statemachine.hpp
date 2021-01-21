#ifndef STATEMACHINE_HPP_INCLUDED
#define STATEMACHINE_HPP_INCLUDED

namespace common {
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


//==============================================================================================================================================================
/// Zustandsautomat.
/// @tparam TStates  Enumerationstyp mit Zustandsliteralen
template <typename TStates>
class Statemachine
{
//==============================================================================================================================================================
public:
//==============================================================================================================================================================
  /// Konstruktor.
  /// @param initialState  (optinal) Initialzustand des Automaten
  explicit inline constexpr Statemachine(TStates initialState = static_cast<TStates>(0))
    : CurrentState(initialState),
      NextState   (initialState),
      Flags       (Transition  )
  {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Polling-Funktor.
  /// Führt die während der Abarbeitung der States eingeleiteten Transitionen aus.
  /// Muss zyklisch gepollt werden.
  inline TStates operator()()
  {
    // durch das Shifting wird eine im letzten Durchlauf angemeldete Transition -> StateChanged geschoben
    Flags = (StatusFlags) ((Flags >> 1) & StateChanged);
    if (Flags)
    {
      CurrentState = NextState;
    }
    return CurrentState;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// leitet eine Transition in den gewünschten State ein.
  /// @param state  gewünschter Folgestate
  /// @return übergebener State
  inline TStates operator=(TStates state)
  {
    NextState  = state;
    Flags      = (StatusFlags) (Flags | Transition);
    return state;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// leitet eine Schnelltransition in den gewünschten State ein.
  /// Eine Schnelltransition wird direkt nach Abschluss des aktuellen States ausgeführt, ohne das umgebende do..while-Konstrukt zu verlassen.
  /// @param state  gewünschter Folgestate
  /// @return übergebener State
  inline TStates operator>>=(TStates state)
  {
    NextState = state;
    Flags     = (StatusFlags) (Flags | Transition | QuickTransition);
    return state;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// prüft, ob der aktuelle State gerade frisch betreten wurde (zur Ausführung einer Entry-Action)
  /// @attention auch eine explizite Transition aus einem State in den selben State führt zu einem positiven Ergebnis (erwünschtes Verhalten!).
  /// @return true = Stateeintritt
  inline bool stateEntry() const
  {
    return (Flags & StateChanged) != 0;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// prüft, ob der aktuelle State verlassen wird (zur Ausführung einer Exit-Action)
  /// @attention auch eine explizite Transition aus einem State in den selben State führt zu einem positiven Ergebnis (erwünschtes Verhalten!).
  /// @return true = Stateaustritt
  inline bool stateExit() const
  {
    return (Flags & Transition) || (Flags & QuickTransition);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// prüft, ob eine Schnelltransition ansteht.
  /// Wird im umgebenden do..while-Konstrukt eingesetzt, um Schnelltransitionen ohne Verlassen der Statemachine auszuführen.
  /// @return true = Schnelltransaktion steht an
  inline bool loop() const
  {
    return (Flags & QuickTransition) != 0;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert den aktuellen State.
  /// @return aktueller State
  inline TStates currentState() const { return CurrentState;  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// liefert den Folgestate.
  /// @return Folgestate
  inline TStates nextState() const    { return NextState;  }


//==============================================================================================================================================================
private:
//==============================================================================================================================================================
  /// Transitionsflags
  enum StatusFlags
  {
    None            = 0b000, ///< keine Transitionsflags
    StateChanged    = 0b001, ///< Transition hat stattgefunden (-> Entry-Action)
    Transition      = 0b010, ///< Transition steht an (-> Exit-Action)
    QuickTransition = 0b100  ///< Schnelltransition steht an (-> Exit-Action)
  };

  TStates     CurrentState;  ///< aktueller State
  TStates     NextState;     ///< Folgestate
  StatusFlags Flags;         ///< Transitionsflags dieser Instanz
};
//==============================================================================================================================================================


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
} // namespace common


#endif // STATEMACHINE_HPP_INCLUDED
