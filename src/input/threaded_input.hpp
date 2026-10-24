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

#include <src/input/input.hpp>

#include <sigc++/sigc++.h>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace EzAutoClicker {

class InputPlayer;

class ThreadedInput {
private:
	using duration_type = std::chrono::microseconds;

	std::mutex mutex;
	std::condition_variable_any condition_variable;
	std::jthread worker_thread;
	bool running;
	long long current_repeat_count;

	duration_type get_random_offset() const;

	void run(std::stop_token stop_token);
	void send_click();
public:
	InputPlayer *input_player;
	Input input;
	struct {
		duration_type interval;
		duration_type random_offset_span;
		long long repeat_until;
		bool double_inputs;
		bool enable_random_offset;
		bool infinitly_repeating;
	} options;

	ThreadedInput();
	~ThreadedInput();

	void start();
	void stop();

	constexpr bool is_running() const {
		return running;
	}
};

}