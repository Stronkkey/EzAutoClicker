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
#include <src/session.hpp>

#include <cstdlib>

EzAutoClicker::Session::Type EzAutoClicker::Session::get_type() {
	const char *x11_display_env = std::getenv("DISPLAY");
	const char *wayland_display_env = std::getenv("WAYLAND_DISPLAY");

	if (wayland_display_env) {
		if (x11_display_env)
			return Session::Type::WAYLAND_WITH_XWAYLAND;
		return Session::Type::WAYLAND;
	}

	return x11_display_env ? Session::Type::X11 : Session::Type::INVALID;
}