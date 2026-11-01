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

namespace Input {

struct Click {
	/**
	 * @brief Mouse buttons that can be clicked.
	 */
	enum class MouseButton {
		LEFT, ///< Left mouse button.
		MIDDLE, ///< Middle mouse button (wheel click).
		RIGHT, ///< Right mouse button.
		UNKNOWN ///< Unspecified or unsupported button.
	};

	MouseButton button;
	bool set_mouse_location = false;

	struct {
		int x;
		int y;
	} position = {0, 0};

	struct {
		int width;
		int height;
	} screen_size = {1000, 1000};
};

}

}