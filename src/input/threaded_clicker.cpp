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
#include <src/input/threaded_clicker.hpp>

#include <random>

#include <src/input/clicker.hpp>

EzAutoClicker::Input::ThreadedClicker::ThreadedClicker() {
	worker_thread = {};
	running = false;
}

EzAutoClicker::Input::ThreadedClicker::~ThreadedClicker() {
	stop();
}

std::chrono::milliseconds EzAutoClicker::Input::ThreadedClicker::get_random_offset(long random_offset_ms) const {
	static std::random_device device;
	static std::mt19937_64 rng(device());

	if (random_offset_ms < 1)
		return std::chrono::milliseconds(0);

	auto distribution = std::uniform_int_distribution<int>(
		-random_offset_ms,
		random_offset_ms);
	return std::chrono::milliseconds(distribution(rng));
}

void EzAutoClicker::Input::ThreadedClicker::run(std::stop_token stop_token, Clicker *clicker, const ClickerOptions &options) {
	long long current_repeat_count = 0;
	long long repeat_count = options.repeat_count;
	ClickerOptions::RepeatMode repeat_mode = options.repeat_mode;
	ClickerOptions::ClickType click_type = options.click_type;

	while (!stop_token.stop_requested()) {
		bool finished_repeating = ++current_repeat_count > repeat_count;
		if (repeat_mode != ClickerOptions::RepeatMode::UNTIL_STOPPED && finished_repeating) {
			on_autostopping.emit();
			stop();
			break;
		}

		send_click(clicker, options.click);
		if (click_type == ClickerOptions::ClickType::DOUBLE)
			send_click(clicker, options.click);

		auto wait_time = std::chrono::milliseconds(
			get_clicker_options_total_interval_ms(options));
		if (options.add_random_offset)
			wait_time += get_random_offset(options.random_offset_ms);

		auto lock = std::unique_lock(mutex);
		condition_variable.wait_for(lock, stop_token, wait_time, [] {
			return false;
		});
	}
}

void EzAutoClicker::Input::ThreadedClicker::send_click(Clicker *clicker, const Click &click) {
	if (!clicker)
		return;
	clicker->send_click(click);
}

void EzAutoClicker::Input::ThreadedClicker::start(Clicker *clicker, const ClickerOptions &options) {
	if (running)
		return;

	running = true;
	worker_thread = std::jthread([this, clicker, options](std::stop_token stop_token) {
		run(stop_token, clicker, options);
	});
}

void EzAutoClicker::Input::ThreadedClicker::stop() {
	if (!running)
		return;

	running = false;
	worker_thread.request_stop();
}