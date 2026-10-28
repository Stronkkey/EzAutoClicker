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
#include <src/input/input_player_factory.hpp>
#include <src/input/input_player_uinput.hpp>
#include <src/input/input_player_x11.hpp>
#include <src/session.hpp>

#include <fcntl.h>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<EzAutoClicker::InputPlayerX11> EzAutoClicker::InputPlayerFactory::create_input_player_x11() {
	auto input_player = std::make_unique<EzAutoClicker::InputPlayerX11>();
	input_player->initialize();

	if (!input_player->is_connected())
		return nullptr;
	return input_player;
}

#endif

struct uinput_devices {
	bool successful;
	int mouse_fd;
	int keyboard_fd;
	int errno_mouse;
	int errno_keyboard;
};

static void attempt_open_uinput_device(const char *path, uinput_devices &file_descriptors) {
	if (file_descriptors.successful)
		return;

	int mouse_fd = open(path, O_WRONLY | O_NONBLOCK);
	if (mouse_fd < 0)
		return;

	file_descriptors.mouse_fd = mouse_fd;
	file_descriptors.errno_mouse = errno;

	file_descriptors.keyboard_fd = open(path, O_WRONLY | O_NONBLOCK);
	file_descriptors.errno_keyboard = errno;
	file_descriptors.successful = true;
}

static uinput_devices get_mouse_keyboard_file_descriptors() {
	uinput_devices file_descriptors;
	file_descriptors.successful = false;

	attempt_open_uinput_device("/dev/uinput", file_descriptors);
	attempt_open_uinput_device("/dev/input/uinput", file_descriptors);

	return file_descriptors;
}

std::unique_ptr<EzAutoClicker::InputPlayerUInput> EzAutoClicker::InputPlayerFactory::create_input_player_uinput() {
	uinput_devices uinput_device = get_mouse_keyboard_file_descriptors();

	if (!uinput_device.successful)
		return nullptr;

	auto uinput_clicker = std::make_unique<InputPlayerUInput>();
	uinput_clicker->set_mouse_file_descriptor(uinput_device.mouse_fd);
	uinput_clicker->set_keyboard_file_descriptor(uinput_device.keyboard_fd);
	uinput_clicker->initialize();
	return uinput_clicker;
}

std::unique_ptr<EzAutoClicker::InputPlayer> EzAutoClicker::InputPlayerFactory::create_compatible_input_player() {
	Session::Type session_type = Session::get_type();
	auto input_player_uinput = create_input_player_uinput();

	if (input_player_uinput)
		return input_player_uinput;

	#if EZAUTOCLICKER_X11_INPUT_ENABLED

	if (session_type == Session::Type::X11 || session_type == Session::Type::WAYLAND_WITH_XWAYLAND) {
		auto input_player_x11 = create_input_player_x11();
		if (input_player_x11)
			return input_player_x11;
	}

	#endif

	return nullptr;
}