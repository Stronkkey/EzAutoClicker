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

#include <src/input/clicker_x11.hpp>

#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include <src/input/click.hpp>

EzAutoClicker::Input::ClickerX11::~ClickerX11() {
	if (display)
		XCloseDisplay(display);
}

bool EzAutoClicker::Input::ClickerX11::initialize() {
	if (XInitThreads() == 0)
		return false;

	display = XOpenDisplay(nullptr);
	if (display) {
		int unused;
		if (!XTestQueryExtension(display, &unused, &unused, &unused, &unused))
			return true;

		XCloseDisplay(display);
		display = nullptr;
	}

	return false;
}

/**
 * @brief Converts a Clicker mouse button to its X11 button number.
 * @return The X11 button number or 0 if the button is not supported (e.g. MOUSE_BUTTON::UNKNOWN).
 * @see http://xahlee.info/linux/linux_x11_mouse_button_number.html
 */
static int mouse_button_to_int(EzAutoClicker::Input::Click::MouseButton mouse_button) {
	using MouseButton = EzAutoClicker::Input::Click::MouseButton;

	switch (mouse_button) {
		case MouseButton::LEFT:
			return 1;
		case MouseButton::MIDDLE:
			return 2;
		case MouseButton::RIGHT:
			return 3;
		default:
			return 0;
	}
}

void EzAutoClicker::Input::ClickerX11::send_click(const Click &click) {
	if (!display)
		return;

	int x11_button_number = mouse_button_to_int(click.button);
	if (x11_button_number == 0)
		return;

	if (click.set_mouse_location)
		XTestFakeMotionEvent(display, -1, click.x_position, click.y_position, CurrentTime);

	if (click.only_move) {
		XFlush(display);
		return;
	}

	XTestFakeButtonEvent(display, x11_button_number, True, CurrentTime);
	XTestFakeButtonEvent(display, x11_button_number, False, CurrentTime);
	XFlush(display);
}

#endif