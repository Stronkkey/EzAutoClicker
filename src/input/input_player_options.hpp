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

namespace EzAutoClicker {

struct InputPlayerOptions {
        Input input;
	unsigned long interval_ms;
	unsigned long interval_s;
	unsigned long interval_m;
	unsigned long interval_h;
	unsigned long random_offset_ms;
	unsigned long repeat_count;
	bool add_random_offset;

	enum class ClickType {
		SINGLE,
		DOUBLE,
		UNKNOWN
	} click_type;

	enum class RepeatMode {
		UNTIL_STOPPED,
		REPEAT_X_TIMES,
		UNKNOWN
	} repeat_mode;

};

constexpr unsigned long long get_input_player_options_total_interval_ms(const InputPlayerOptions &input_player_options) {
	return input_player_options.interval_ms +
		(input_player_options.interval_s * 1000) +
		(input_player_options.interval_m * 1000 * 60) +
		(input_player_options.interval_h * 1000 * 60 * 60);
}

}