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

#include <sigc++/sigc++.h>

namespace EzAutoClicker {

class HotkeyListener {
public:
	virtual ~HotkeyListener() = default;
	sigc::signal<void()> signal_pressed = {};

	// Returns the number of devices being watched (0 = nothing usable)
	virtual int start_reading() =0;
	virtual void stop_reading() =0;
};

}