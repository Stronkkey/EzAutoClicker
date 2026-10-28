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
#pragma once

#include <vector>

#include <libevdev/libevdev.h>

#include <src/input/hotkey_listener.hpp>

namespace EzAutoClicker {

namespace Input {

class HotkeyListenerEvdev final : public HotkeyListener {
	std::vector<libevdev*> devices = {};
	std::vector<sigc::connection> watches = {};
	unsigned int key_code = 0;

	void for_input_device(const std::string_view &input_device_path);
	bool device_has_key(libevdev *device) const;

	void handle_event(input_event &event);
	bool listen_for_device(libevdev *device);
public:
	~HotkeyListenerEvdev();

	constexpr void set_key_code(unsigned int new_key_code) {
		key_code = new_key_code;
	}

	constexpr unsigned int get_key_code() const {
		return key_code;
	}

	// Returns the number of devices being watched (0 = nothing usable)
	int start_reading() override;
	void stop_reading() override;
};

}

}