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

#include <src/dispatcher.hpp>
#include <src/input/hotkey.hpp>

namespace EzAutoClicker {

namespace Input {

class HotkeyListener {
protected:
	Hotkey hotkey = {};
public:
	virtual ~HotkeyListener() = default;
	Dispatcher signal_pressed = {};

	virtual bool is_reading() const =0;
	virtual void start_reading() =0;
	virtual void stop_reading() =0;

	virtual bool register_hotkey(const Hotkey &p_hotkey) =0;
	virtual void unregister_hotkey() =0;
};

}

}