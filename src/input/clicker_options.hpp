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

#include <src/input/click.hpp>

namespace EzAutoClicker {

namespace Input {

struct ClickerOptions {
        Click click;
	long interval_ms;
	long interval_s;
	long interval_m;
	long interval_h;
	long random_offset_ms;
	long repeat_count;
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

constexpr long long get_clicker_options_total_interval_ms(const ClickerOptions &clicker_options) {
	return clicker_options.interval_ms +
		(clicker_options.interval_s * 1000) +
		(clicker_options.interval_m * 1000 * 60) +
		(clicker_options.interval_h * 1000 * 60 * 60);
}

}

}