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
#if EZAUTOCLICKER_X11_INPUT_ENABLED

#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <src/input/input_player.hpp>

namespace EzAutoClicker {

/**
 * @brief X11 backend for the InputPlayer interface.
 *
 * Sends inputs through an X server connection using XTest. Call initialize()
 * to open the display before using other methods.
 */
class InputPlayerX11 final : public InputPlayer {
private:
	Display *display = nullptr; ///< Connection to the X server, nullptr if not connected.
public:
	/**
	 * @brief Destructor, closes the X display connection if it is open.
	 */
	~InputPlayerX11();

	/**
	 * @brief Opens the connection to the X server.
	 * @return true if the display was opened successfully, false otherwise.
	 */
	bool initialize() override;

	/**
	 * @brief Checks whether the X display connection is open.
	 * @return true if connected to the X server, false otherwise.
	 */
	constexpr bool is_connected() const override {
		return display != nullptr;
	}

	/**
	 * @brief Sends a single input to the X server.
	 * @param input The input to send.
	 * @note Requires initialize to be called beforehand, does nothing otherwise.
	 */
	void send_input(const Input &input) override;
};

}

#endif