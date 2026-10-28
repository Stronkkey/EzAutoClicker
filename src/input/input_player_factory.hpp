/**
 * This file is part of EzAutoClicker.
 *
 * EzAutoClicker is free software: you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * EzAutoClicker is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with EzAutoClicker.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <memory>

namespace EzAutoClicker {

class InputPlayer;
class InputPlayerX11;
class InputPlayerUInput;

namespace InputPlayerFactory {

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<InputPlayerX11> create_input_player_x11();

#endif

std::unique_ptr<InputPlayerUInput> create_input_player_uinput();
std::unique_ptr<InputPlayer> create_compatible_input_player();

}

}