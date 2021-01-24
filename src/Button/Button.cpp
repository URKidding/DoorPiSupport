#include "Button.hpp"

using namespace dps::button;

std::array<char const*, State::_NrStates> Button::StateNames =
{
  "released",
  "pressed",
  "long"
};
