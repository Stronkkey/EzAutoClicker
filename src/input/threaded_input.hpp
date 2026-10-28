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

#include <glibmm.h>

#include <src/input/input_player_options.hpp>

namespace EzAutoClicker {

class InputPlayer;

class ThreadedInput {
private:
	std::mutex mutex;
	std::condition_variable_any condition_variable;
	std::jthread worker_thread;
	bool running;

	std::chrono::milliseconds get_random_offset(unsigned long random_offset_ms) const;

	void run(std::stop_token stop_token, InputPlayer *input_player, const InputPlayerOptions &options);
	void send_input(InputPlayer *input_player, const Input &p_input);
public:
	Glib::Dispatcher on_autostopping;

	ThreadedInput();
	~ThreadedInput();

	void start(InputPlayer *input_player, const InputPlayerOptions &options);
	void stop();

	constexpr bool is_running() const {
		return running;
	}
};

}