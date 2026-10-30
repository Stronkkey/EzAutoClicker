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
#include <src/input/input_listener_factory.hpp>

#include <fcntl.h>

#include <src/input/input_listener_evdev.hpp>
#include <src/input/input_listener_x11.hpp>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<EzAutoClicker::Input::HotkeyListenerX11> EzAutoClicker::Input::HotkeyListenerFactory::create_input_listener_x11() {
	auto input_listener_x11 = std::make_unique<EzAutoClicker::Input::HotkeyListenerX11>();
	input_listener_x11->start_reading_hotkey();
	return input_listener_x11;
}

#endif

std::unique_ptr<EzAutoClicker::Input::HotkeyListenerEvdev> EzAutoClicker::Input::HotkeyListenerFactory::create_input_listener_evdev() {
	auto input_listener_evdev = std::make_unique<EzAutoClicker::Input::HotkeyListenerEvdev>();
	input_listener_evdev->start_reading_hotkey();
	return input_listener_evdev;
}

std::unique_ptr<EzAutoClicker::Input::HotkeyListener> EzAutoClicker::Input::HotkeyListenerFactory::create_input_listener() {
	auto input_listener_evdev = create_input_listener_evdev();

	if (input_listener_evdev->is_reading_hotkey())
		return input_listener_evdev;

	#if EZAUTOCLICKER_X11_INPUT_ENABLED

	auto input_listener_x11 = create_input_listener_x11();
	if (input_listener_x11->is_reading_hotkey())
		return input_listener_x11;

	#endif

	return nullptr;
}