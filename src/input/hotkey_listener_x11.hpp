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

#if EZAUTOCLICKER_X11_INPUT_ENABLED

#include <mutex>
#include <thread>

#include <X11/Xlib.h>

#include <src/input/hotkey_listener.hpp>

namespace EzAutoClicker {

class HotkeyListenerX11 final : public HotkeyListener {
private:
	Display *display;

	std::jthread worker_thread;
	std::mutex mutex;
	KeyCode key_code;
	bool running;

	void read_inputs(std::stop_token stop_token);
public:
	HotkeyListenerX11();
	~HotkeyListenerX11();

	int start_reading() override;
	void stop_reading() override;
};

}

#endif