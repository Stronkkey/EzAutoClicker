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
#include <src/input/clicker_factory.hpp>
 
#include <fcntl.h>
 
#include <optional>

#include <src/session.hpp>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

std::unique_ptr<EzAutoClicker::Input::ClickerX11> EzAutoClicker::Input::ClickerFactory::create_clicker_x11() {
	auto input_player = std::make_unique<ClickerX11>();
	input_player->initialize();

	if (!input_player->is_connected())
		return nullptr;
	return input_player;
}

#endif

static std::optional<int> attempt_open_uinput_device(const char *path) {
	int fd = open(path, O_WRONLY | O_NONBLOCK);
	if (fd < 0)
		return std::nullopt;
	return fd;
}

static std::optional<int> get_mouse_fd() {
	std::optional<int> fd = attempt_open_uinput_device("/dev/uinput");
	if (!fd)
		fd = attempt_open_uinput_device("/dev/input/uinput");
	return fd;
}

std::unique_ptr<EzAutoClicker::Input::ClickerUInput> EzAutoClicker::Input::ClickerFactory::create_clicker_uinput() {
	std::optional<int> fd = get_mouse_fd();

	if (!fd)
		return nullptr;

	auto uinput_clicker = std::make_unique<Input::ClickerUInput>();
	uinput_clicker->set_mouse_file_descriptor(*fd);
	uinput_clicker->initialize();
	return uinput_clicker;
}

std::unique_ptr<EzAutoClicker::Input::Clicker> EzAutoClicker::Input::ClickerFactory::create_compatible_clicker() {
	Session::Type session_type = Session::get_type();
	auto input_player_uinput = create_clicker_uinput();

	if (input_player_uinput)
		return input_player_uinput;

	#if EZAUTOCLICKER_X11_INPUT_ENABLED

	if (session_type == Session::Type::X11 || session_type == Session::Type::WAYLAND_WITH_XWAYLAND) {
		auto input_player_x11 = create_clicker_x11();
		if (input_player_x11)
			return input_player_x11;
	}

	#endif

	return nullptr;
}