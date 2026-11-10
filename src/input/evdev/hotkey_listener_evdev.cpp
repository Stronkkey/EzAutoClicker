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
#include <src/input/evdev/hotkey_listener_evdev.hpp>

#include <fcntl.h>
#include <unistd.h>

#include <filesystem>

#include <glibmm/main.h>

EzAutoClicker::Input::HotkeyListenerEvdev::~HotkeyListenerEvdev() {
	stop_reading();
}

void EzAutoClicker::Input::HotkeyListenerEvdev::for_input_device(const std::string_view &input_device_path) {
	if (input_device_path.find("event") != std::string_view::npos)
		return;

	int fd = open(input_device_path.data(), O_RDONLY | O_NONBLOCK);
	if (fd < 0)
		return;

	libevdev *device;
	int evdev_intialized = libevdev_new_from_fd(fd, &device) == 0;

	if (fd < 0) {
		if (evdev_intialized)
			libevdev_free(device);
		return;
	}

	if (!evdev_intialized) {
		if (fd > -1)
			close(fd);
		return;
	}

	if (!device_has_key(device)) {
		if (fd > -1)
			close(fd);
		libevdev_free(device);
		return;
	}

	devices.push_back(device);
	watches.push_back(Glib::signal_io().connect(
		[this, device](Glib::IOCondition) { return listen_for_device(device); },
		fd, Glib::IOCondition::IO_IN));
}

bool EzAutoClicker::Input::HotkeyListenerEvdev::device_has_key(libevdev *device) const {
	return libevdev_has_event_type(device, EV_KEY);
}

bool EzAutoClicker::Input::HotkeyListenerEvdev::is_reading() const {
	return !devices.empty();
}

void EzAutoClicker::Input::HotkeyListenerEvdev::start_reading() {
	stop_reading();

	if (!std::filesystem::exists("/dev/input"))
		return;

	using directory_iterator = std::filesystem::directory_iterator;
	constexpr const auto option = std::filesystem::directory_options::follow_directory_symlink;

	for (const auto &entry: directory_iterator("/dev/input", option))
		for_input_device(entry.path().string());
}

void EzAutoClicker::Input::HotkeyListenerEvdev::stop_reading() {
	for (auto &watch : watches)
		watch.disconnect();
	watches.clear();

	for (libevdev *device: devices) {
		int fd = libevdev_get_fd(device);

		if (fd > -1)
			close(fd);
		libevdev_free(device);
	}
	devices.clear();
}

bool EzAutoClicker::Input::HotkeyListenerEvdev::register_hotkey(const Hotkey &p_hotkey) {
	auto lock = std::unique_lock(hotkey_mutex);
	if (p_hotkey.key == 0)
		return false;

	hotkey = p_hotkey;
	return true;
}

void EzAutoClicker::Input::HotkeyListenerEvdev::unregister_hotkey() {
	auto lock = std::unique_lock(hotkey_mutex);
	hotkey.key = 0;
}

void EzAutoClicker::Input::HotkeyListenerEvdev::handle_event(input_event &event) {
	if (event.type != EV_KEY)
		return;

	bool key_pressed = event.value == 1;
	if (event.code == KEY_LEFTCTRL || event.code == KEY_RIGHTCTRL) {
		if (key_pressed)
			modifier_pressed = modifier_pressed | Hotkey::Modifier::CTRL;
		else
			modifier_pressed = modifier_pressed ^ Hotkey::Modifier::CTRL;
		return;
	}

	if (event.code == KEY_LEFTSHIFT || event.code == KEY_RIGHTSHIFT) {
		if (key_pressed)
			modifier_pressed = modifier_pressed | Hotkey::Modifier::SHIFT;
		else
			modifier_pressed = modifier_pressed ^ Hotkey::Modifier::SHIFT;
		return;
	}

	if (event.code == KEY_LEFTALT || event.code == KEY_RIGHTALT) {
		if (key_pressed)
			modifier_pressed = modifier_pressed | Hotkey::Modifier::ALT;
		else
			modifier_pressed = modifier_pressed ^ Hotkey::Modifier::ALT;
		return;
	}

	if (event.code == KEY_LEFTMETA || event.code == KEY_RIGHTMETA) {
		if (key_pressed)
			modifier_pressed = modifier_pressed | Hotkey::Modifier::SUPER;
		else
			modifier_pressed = modifier_pressed ^ Hotkey::Modifier::SUPER;
		return;
	}

	auto lock = std::unique_lock(hotkey_mutex);
	bool is_hotkey_pressed = event.code == hotkey.key && key_pressed;
	bool modifiers_match = modifier_pressed == hotkey.modifiers
	                    || hotkey.modifiers == Hotkey::Modifier::ALL;

	if (is_hotkey_pressed && modifiers_match)
		signal_pressed.emit();
}

bool EzAutoClicker::Input::HotkeyListenerEvdev::listen_for_device(libevdev *device) {
	int status;

	while (true) {
		input_event event;
		status = libevdev_next_event(device, LIBEVDEV_READ_FLAG_NORMAL, &event);

		if (status == -EAGAIN)
			return errno == EAGAIN;

		if (status != LIBEVDEV_READ_STATUS_SUCCESS)
			continue;

		handle_event(event);
	}
}