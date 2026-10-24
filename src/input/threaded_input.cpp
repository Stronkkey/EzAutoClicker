#include <src/input/input_player.hpp>
#include <src/input/threaded_input.hpp>

#include <random>

EzAutoClicker::ThreadedInput::ThreadedInput() : mutex(), condition_variable() {
	worker_thread = {};
	running = false;
	current_repeat_count = 0;

	input_player = nullptr;
	input = {.button = Input::MOUSE_BUTTON::LEFT};

	options.interval = std::chrono::milliseconds(100);
	options.random_offset_span = std::chrono::milliseconds(0);
	options.repeat_until = 1;
	current_repeat_count = 0;
	options.double_inputs = false;
	options.enable_random_offset = false;
	options.infinitly_repeating = true;
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

void EzAutoClicker::ThreadedInput::run(std::stop_token stop_token) {
	auto next = std::chrono::steady_clock::now();
	while (!stop_token.stop_requested()) {
		bool finished_repeating = ++current_repeat_count > options.repeat_until;
		if (!options.infinitly_repeating && finished_repeating)
			break;

		send_click();
		next += options.interval;
		if (options.enable_random_offset)
			next += get_random_offset();

		auto lock = std::unique_lock(mutex);
		condition_variable.wait_until(lock, stop_token, next, [] {
			return false;
		});
	}

	stop();
}

void EzAutoClicker::ThreadedInput::send_click() {
	if (!input_player)
		return;

	if (options.double_inputs)
		input_player->send_input(input);
	input_player->send_input(input);
}

void EzAutoClicker::ThreadedInput::start() {
	if (running)
		return;

	running = true;

	worker_thread = std::jthread([this](std::stop_token stop_token) {
		run(stop_token);
	});
}

void EzAutoClicker::ThreadedInput::stop() {
	if (!running)
		return;

	running = false;
	current_repeat_count = 0;
	worker_thread.request_stop();
}