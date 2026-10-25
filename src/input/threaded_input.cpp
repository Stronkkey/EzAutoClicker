#include <src/input/input_player.hpp>
#include <src/input/threaded_input.hpp>

#include <random>

EzAutoClicker::ThreadedInput::ThreadedInput() : mutex(), condition_variable() {
	worker_thread = {};
	running = false;

	input_player = nullptr;
	input = {.button = Input::MOUSE_BUTTON::LEFT};

	options.interval = std::chrono::milliseconds(100);
	options.random_offset_span = std::chrono::milliseconds(0);
	options.repeat_until = 1;
	options.send_inputs_twice = false;
	options.add_random_offset = false;
	options.repeat_until_stopped = true;
}

EzAutoClicker::ThreadedInput::~ThreadedInput() {
	stop();
}

EzAutoClicker::ThreadedInput::duration_type EzAutoClicker::ThreadedInput::get_random_offset() const {
	static std::random_device device;
	static std::mt19937_64 rng(device());

	auto distribution = std::uniform_int_distribution<int64_t>(
		-options.random_offset_span.count(),
		options.random_offset_span.count());
	return duration_type(distribution(rng));
}

void EzAutoClicker::ThreadedInput::run(std::stop_token stop_token, const copy_thread &copy) {
	long long current_repeat_count = 0;
	long long repeat_until = copy.options.repeat_until;
	bool repeat_until_stopped = copy.options.repeat_until_stopped;
	bool send_inputs_twice = copy.options.send_inputs_twice;

	while (!stop_token.stop_requested()) {
		bool finished_repeating = ++current_repeat_count > repeat_until;
		if (!repeat_until_stopped && finished_repeating) {
			stop();
			on_autostopped.emit();
			break;
		}

		send_input(copy.input);
		if (send_inputs_twice)
			send_input(copy.input);

		auto wait_time = options.interval;
		if (options.add_random_offset)
			wait_time += get_random_offset();

		auto lock = std::unique_lock(mutex);
		condition_variable.wait_for(lock, stop_token, wait_time, [] {
			return false;
		});
	}
}

void EzAutoClicker::ThreadedInput::send_input(const Input &input) {
	if (!input_player)
		return;
	input_player->send_input(input);
}

void EzAutoClicker::ThreadedInput::start() {
	if (running)
		return;

	running = true;
	copy_thread copy;
	copy.input = input;
	copy.input_player = input_player;
	copy.options = options;
	worker_thread = std::jthread([this, copy](std::stop_token stop_token) {
		run(stop_token, copy);
	});
}

void EzAutoClicker::ThreadedInput::stop() {
	if (!running)
		return;

	running = false;
	worker_thread.request_stop();
}