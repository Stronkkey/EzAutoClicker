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
#include <src/input/input_player_uinput.hpp>

#include <fcntl.h>
#include <unistd.h>

#include <libevdev/libevdev-uinput.h>

bool EzAutoClicker::InputPlayerUInput::send_mouse_input(int type, int code, int value) {
	return libevdev_uinput_write_event(mouse_uinput_device, type, code, value);
}

bool EzAutoClicker::InputPlayerUInput::send_keyboard_input(int type, int code, int value) {
	return libevdev_uinput_write_event(keyboard_uinput_device, type, code, value);
}

bool EzAutoClicker::InputPlayerUInput::synchronise_mouse_inputs() {
	return send_mouse_input(EV_SYN, SYN_REPORT, 0);
}

bool EzAutoClicker::InputPlayerUInput::synchronise_keyboard_inputs() {
	return send_keyboard_input(EV_SYN, SYN_REPORT, 0);
}

/**
 * @brief Converts a Clicker mouse button to its uinput button number.
 * @return The uinput button number or BTN_LEFT if the button is not supported (e.g. MOUSE_BUTTON::UNKNOWN).
 */
int mouse_button_to_code(EzAutoClicker::Input::MOUSE_BUTTON button) {
	using MOUSE_BUTTON = EzAutoClicker::Input::MOUSE_BUTTON;

	switch (button) {
		case MOUSE_BUTTON::LEFT:
			return BTN_LEFT;
		case MOUSE_BUTTON::RIGHT:
			return BTN_RIGHT;
		case MOUSE_BUTTON::MIDDLE:
			return BTN_MIDDLE;
		default:
			return BTN_LEFT;
	}
}

EzAutoClicker::InputPlayerUInput::~InputPlayerUInput() {
	if (mouse_uinput_device)
		libevdev_uinput_destroy(mouse_uinput_device);
	if (mouse_device)
		libevdev_free(mouse_device);
	if (keyboard_uinput_device)
		libevdev_uinput_destroy(keyboard_uinput_device);
	if (keyboard_device)
		libevdev_free(keyboard_device);
	if (mouse_uinput_fd > -1)
		close(mouse_uinput_fd);
	if (keyboard_uinput_fd > -1)
		close(keyboard_uinput_fd);
}

bool EzAutoClicker::InputPlayerUInput::initialize() {
	if (mouse_uinput_fd < 0)
		mouse_uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

	if (keyboard_uinput_fd < 0)
		keyboard_uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

	if (mouse_uinput_fd < 0) {
		if (keyboard_uinput_fd > -1)
			close(keyboard_uinput_fd);
		return false;
	}

	if (keyboard_uinput_fd < 0) {
		if (mouse_uinput_fd > -1)
			close(mouse_uinput_fd);
		return false;
	}

	mouse_device = libevdev_new();
	keyboard_device = libevdev_new();

	libevdev_set_name(mouse_device, "EzAutoClicker-Mouse-Device");
	libevdev_enable_event_type(mouse_device, EV_KEY);
	libevdev_enable_event_type(mouse_device, EV_REL);
	libevdev_enable_event_type(mouse_device, EV_ABS);

	libevdev_enable_event_code(mouse_device, EV_KEY, BTN_LEFT, nullptr);
	libevdev_enable_event_code(mouse_device, EV_KEY, BTN_MIDDLE, nullptr);
	libevdev_enable_event_code(mouse_device, EV_KEY, BTN_RIGHT, nullptr);
	libevdev_enable_event_code(mouse_device, EV_REL, REL_X, nullptr);
	libevdev_enable_event_code(mouse_device, EV_REL, REL_Y, nullptr);
	libevdev_uinput_create_from_device(mouse_device, mouse_uinput_fd, &mouse_uinput_device);

	libevdev_set_name(keyboard_device, "EzAutoClicker-Keyboard-Device");
	libevdev_enable_event_type(keyboard_device, EV_KEY);
	libevdev_uinput_create_from_device(keyboard_device, keyboard_uinput_fd, &keyboard_uinput_device);

	connected = mouse_uinput_device && keyboard_uinput_device;
	return connected;
}

bool EzAutoClicker::InputPlayerUInput::is_connected() const {
	return connected;
}

void EzAutoClicker::InputPlayerUInput::send_input(const Input &input) {
	if (!connected)
		return;

	int mouse_code = mouse_button_to_code(input.button);

	if (input.set_mouse_location) {
		send_mouse_input(EV_REL, REL_X, -9999);
		send_mouse_input(EV_REL, REL_Y, -9999);
		synchronise_mouse_inputs();
		send_mouse_input(EV_REL, REL_X, input.x_position);
		send_mouse_input(EV_REL, REL_Y, input.y_position);
		synchronise_mouse_inputs();
	}

	send_mouse_input(EV_KEY, mouse_code, 1);
	synchronise_mouse_inputs();
	send_mouse_input(EV_KEY, mouse_code, 0);
	synchronise_mouse_inputs();
}