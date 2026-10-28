#include "src/input/input_player_options.hpp"
#include <src/input/input_player.hpp>
#include <src/input/threaded_input.hpp>

#include <random>

EzAutoClicker::ThreadedInput::ThreadedInput() {
	worker_thread = {};
	running = false;
}

EzAutoClicker::ThreadedInput::~ThreadedInput() {
	stop();
}

std::chrono::milliseconds EzAutoClicker::ThreadedInput::get_random_offset(unsigned long random_offset_ms) const {
	static std::random_device device;
	static std::mt19937_64 rng(device());

	auto distribution = std::uniform_int_distribution<int>(
		-random_offset_ms,
		random_offset_ms);
	return std::chrono::milliseconds(distribution(rng));
}

void EzAutoClicker::ThreadedInput::run(std::stop_token stop_token, InputPlayer *input_player, const InputPlayerOptions &options) {
	long long current_repeat_count = 0;
	long long repeat_count = options.repeat_count;
	InputPlayerOptions::RepeatMode repeat_mode = options.repeat_mode;
	InputPlayerOptions::ClickType click_type = options.click_type;

	while (!stop_token.stop_requested()) {
		bool finished_repeating = ++current_repeat_count > repeat_count;
		if (repeat_mode != InputPlayerOptions::RepeatMode::UNTIL_STOPPED && finished_repeating) {
			on_autostopping.emit();
			stop();
			break;
		}

		send_input(input_player, options.input);
		if (click_type == InputPlayerOptions::ClickType::DOUBLE)
			send_input(input_player, options.input);

		auto wait_time = std::chrono::milliseconds(
			get_input_player_options_total_interval_ms(options));
		if (options.add_random_offset)
			wait_time += get_random_offset(options.random_offset_ms);

		auto lock = std::unique_lock(mutex);
		condition_variable.wait_for(lock, stop_token, wait_time, [] {
			return false;
		});
	}
}

void EzAutoClicker::ThreadedInput::send_input(InputPlayer *input_player, const Input &input) {
	if (!input_player)
		return;
	input_player->send_input(input);
}

void EzAutoClicker::ThreadedInput::start(InputPlayer *input_player, const InputPlayerOptions &options) {
	if (running)
		return;

	running = true;
	worker_thread = std::jthread([this, input_player, options](std::stop_token stop_token) {
		run(stop_token, input_player, options);
	});
}

void EzAutoClicker::ThreadedInput::stop() {
	if (!running)
		return;

	running = false;
	worker_thread.request_stop();
}