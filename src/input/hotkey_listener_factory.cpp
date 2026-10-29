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
#include <src/input/hotkey_listener_factory.hpp>

#include <fcntl.h>

#include <src/input/hotkey_listener_evdev.hpp>
#include <src/input/hotkey_listener_x11.hpp>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<EzAutoClicker::Input::HotkeyListenerX11> EzAutoClicker::Input::HotkeyListenerFactory::create_hotkey_listener_x11() {
	auto hotkey_listener_x11 = std::make_unique<EzAutoClicker::Input::HotkeyListenerX11>();
	hotkey_listener_x11->start_reading();
	return hotkey_listener_x11;
}

#endif

std::unique_ptr<EzAutoClicker::Input::HotkeyListenerEvdev> EzAutoClicker::Input::HotkeyListenerFactory::create_hotkey_listener_evdev() {
	auto hotkey_listener_evdev = std::make_unique<EzAutoClicker::Input::HotkeyListenerEvdev>();
	hotkey_listener_evdev->start_reading();
	return hotkey_listener_evdev;
}

std::unique_ptr<EzAutoClicker::Input::HotkeyListener> EzAutoClicker::Input::HotkeyListenerFactory::create_hotkey_listener() {
	auto hotkey_listener_evdev = create_hotkey_listener_evdev();

	if (hotkey_listener_evdev->is_reading())
		return hotkey_listener_evdev;

	#if EZAUTOCLICKER_X11_INPUT_ENABLED

	auto hotkey_listener_x11 = create_hotkey_listener_x11();
	if (hotkey_listener_x11->is_reading())
		return hotkey_listener_x11;

	#endif

	return nullptr;
}