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

namespace Input {

class HotkeyListener;
class HotkeyListenerEvdev;
class HotkeyListenerX11;

namespace HotkeyListenerFactory {

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<HotkeyListenerX11> create_hotkey_listener_x11();

#endif

std::unique_ptr<HotkeyListenerEvdev> create_hotkey_listener_evdev();
std::unique_ptr<HotkeyListener> create_hotkey_listener();

}

}

}