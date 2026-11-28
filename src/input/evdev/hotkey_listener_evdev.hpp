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

#include <mutex>
#include <vector>

#include <libevdev/libevdev.h>

#include <src/input/hotkey_listener.hpp>

namespace EzAutoClicker {

namespace Input {

class HotkeyListenerEvdev final : public HotkeyListener {
	std::vector<libevdev*> devices = {};
	//std::vector<sigc::connection> watches = {};
	std::mutex hotkey_mutex;
	Hotkey::Modifier modifier_pressed = Hotkey::Modifier::NONE;

	void for_input_device(const std::string_view &input_device_path);
	bool device_has_key(libevdev *device) const;

	void handle_event(input_event &event);
	bool listen_for_device(libevdev *device);
public:
	~HotkeyListenerEvdev();

	bool is_reading() const override;
	void start_reading() override;
	void stop_reading() override;

	bool register_hotkey(const Hotkey &p_hotkey) override;
	void unregister_hotkey() override;
};

}

}