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
#if EZAUTOCLICKER_X11_INPUT_ENABLED

#include <src/input/input_player_x11.hpp>

#include <X11/keysym.h>
#include <X11/keysymdef.h>

EzAutoClicker::InputPlayerX11::~InputPlayerX11() {
	if (display)
		XCloseDisplay(display);
}

bool EzAutoClicker::InputPlayerX11::initialize() {
	display = XOpenDisplay(nullptr);
	if (display) {
		int unused;
		if (!XTestQueryExtension(display, &unused, &unused, &unused, &unused)) {
			XCloseDisplay(display);
			display = nullptr;
		} else
			return true;
	}

	return false;
}

/**
 * @brief Converts a Clicker mouse button to its X11 button number.
 * @return The X11 button number or 0 if the button is not supported (e.g. MOUSE_BUTTON::UNKNOWN).
 * @see http://xahlee.info/linux/linux_x11_mouse_button_number.html
 */
static int mouse_button_to_int(EzAutoClicker::Input::MOUSE_BUTTON mouse_button) {
	using MOUSE_BUTTON = EzAutoClicker::Input::MOUSE_BUTTON;

	switch (mouse_button) {
		case MOUSE_BUTTON::LEFT:
			return 1;
		case MOUSE_BUTTON::MIDDLE:
			return 2;
		case MOUSE_BUTTON::RIGHT:
			return 3;
		default:
			return 0;
	}
}

void EzAutoClicker::InputPlayerX11::send_input(const Input &input) {
	if (!display)
		return;

	int x11_button_number = mouse_button_to_int(input.button);
	if (x11_button_number == 0)
		return;

	if (input.set_mouse_location)
		XTestFakeMotionEvent(display, -1, input.x_position, input.y_position, CurrentTime);

	XTestFakeButtonEvent(display, x11_button_number, True, CurrentTime);
	XTestFakeButtonEvent(display, x11_button_number, False, CurrentTime);
	XFlush(display);
}

#endif