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

#include <src/input/clicker_uinput.hpp>
#include <src/input/clicker_x11.hpp>

namespace EzAutoClicker {

namespace Input {

namespace ClickerFactory {

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<ClickerX11> create_clicker_x11();

#endif

std::unique_ptr<ClickerUInput> create_clicker_uinput();
std::unique_ptr<Clicker> create_compatible_clicker();

}

}

}