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

#include <src/session.hpp>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<EzAutoClicker::HotkeyListenerX11> EzAutoClicker::HotkeyListenerFactory::create_hotkey_listener_x11() {
	return std::make_unique<EzAutoClicker::HotkeyListenerX11>();
}

#endif

std::unique_ptr<EzAutoClicker::HotkeyListenerEvdev> EzAutoClicker::HotkeyListenerFactory::create_hotkey_listener_evdev() {
	return std::make_unique<EzAutoClicker::HotkeyListenerEvdev>();
}

std::unique_ptr<EzAutoClicker::HotkeyListener> EzAutoClicker::HotkeyListenerFactory::create_compatible_hotkey_listener() {
	Session::Type session_type = Session::get_type();
	auto hotkey_listener_evdev = create_hotkey_listener_evdev();

	if (hotkey_listener_evdev)
		return hotkey_listener_evdev;

	#if EZAUTOCLICKER_X11_INPUT_ENABLED

	if (session_type == Session::Type::X11 || session_type == Session::Type::WAYLAND_WITH_XWAYLAND) {
		auto hotkey_listener_x11 = create_hotkey_listener_x11();
		if (hotkey_listener_x11)
			return hotkey_listener_x11;
	}

	#endif

	return nullptr;
}