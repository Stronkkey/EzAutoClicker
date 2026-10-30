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
#include <src/input/input_listener_evdev.hpp>

#include <fcntl.h>
#include <unistd.h>

#include <filesystem>

#include <glibmm/main.h>

EzAutoClicker::Input::HotkeyListenerEvdev::~HotkeyListenerEvdev() {
	stop_reading_hotkey();
}

void EzAutoClicker::Input::HotkeyListenerEvdev::for_input_device(const std::string_view &input_device_path) {
	if (!input_device_path.contains("event"))
		return;

	int fd = open(input_device_path.data(), O_RDONLY | O_NONBLOCK);
	if (fd < 0)
		return;

	libevdev *device;
	libevdev_new_from_fd(fd, &device);
	if (!device_has_key(device)) {
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

bool EzAutoClicker::Input::HotkeyListenerEvdev::is_reading_hotkey() const {
	return !devices.empty();
}

void EzAutoClicker::Input::HotkeyListenerEvdev::start_reading_hotkey() {
	key_code = KEY_LEFTBRACE;
	stop_reading_hotkey();

	if (!std::filesystem::exists("/dev/input"))
		return;

	using directory_iterator = std::filesystem::directory_iterator;
	constexpr const auto option = std::filesystem::directory_options::follow_directory_symlink;

	for (const auto &entry: directory_iterator("/dev/input", option))
		for_input_device(entry.path().string());
}

void EzAutoClicker::Input::HotkeyListenerEvdev::stop_reading_hotkey() {
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

void EzAutoClicker::Input::HotkeyListenerEvdev::handle_event(input_event &event) {
	if (event.type == EV_KEY && event.code == key_code && event.value == 1)
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