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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <glibmm/dispatcher.h>

#include <src/input/clicker.hpp>
#include <src/input/clicker_options.hpp>

namespace EzAutoClicker {

namespace Input {

class ThreadedClicker {
private:
	std::mutex mutex;
	std::condition_variable_any condition_variable;
	std::jthread worker_thread;
	bool running;

	std::chrono::milliseconds get_random_offset(long random_offset_ms) const;

	void run(std::stop_token stop_token, Clicker *clicker, const ClickerOptions &options);
	void send_click(Clicker *clicker, const Click &click);
public:
	Glib::Dispatcher on_autostopping;

	ThreadedClicker();
	~ThreadedClicker();

	void start(Clicker *clicker, const ClickerOptions &options);
	void stop();

	constexpr bool is_running() const {
		return running;
	}
};

}

}