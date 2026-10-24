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

	if (input.use_global_mouse_position)
		XTestFakeMotionEvent(display, -1, input.mouse_x_offset, input.mouse_y_offset, CurrentTime);

    XTestFakeButtonEvent(display, x11_button_number, True, CurrentTime);
	XTestFakeButtonEvent(display, x11_button_number, False, CurrentTime);
	XFlush(display);
}

#endif