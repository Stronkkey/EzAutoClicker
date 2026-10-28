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

/**
 * @brief Abstract interface for sending clicks.
 *
 * Platform-specific backends (currently uinput with ClickerUInput and X11 with ClickerX11)
 * implement this interface. Call initialize() before using any other method.
 */
class Clicker {
public:
	virtual ~Clicker() = default;

	/**
	 * @brief Prepares the backend for sending inputs.
	 * @return true if initialization succeeded, false otherwise.
	 */
	virtual bool initialize() =0;

	/**
	 * @brief Sends a single click to the clicker.
	 * @param click The click to send.
	 * @note Requires initialize to be called beforehand, does nothing otherwise.
	 */
	virtual void send_click(const Click &click)=0;

	/**
	 * @brief Checks whether the backend is connected and inputs can be sent.
	 * @return true if inputs can currently be sent, false otherwise. Always returns
	 * false if initialize hasn't been called.
	 */
	virtual bool is_connected() const=0;
};

}

}