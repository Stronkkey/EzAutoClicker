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

namespace EzAutoClicker {

struct Input {
	/**
	 * @brief Mouse buttons that can be clicked.
	 */
	enum class MOUSE_BUTTON {
		LEFT, ///< Left mouse button.
		MIDDLE, ///< Middle mouse button (wheel click).
		RIGHT, ///< Right mouse button.
		UNKNOWN ///< Unspecified or unsupported button.
	};

	MOUSE_BUTTON button;
	bool use_global_mouse_position = true;
	int mouse_x_offset = 100;
	int mouse_y_offset = 100;
};

}