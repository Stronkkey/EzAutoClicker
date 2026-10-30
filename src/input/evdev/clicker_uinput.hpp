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

#include <libevdev/libevdev-uinput.h>

#include <src/input/clicker.hpp>

namespace EzAutoClicker {

namespace Input {

/**
 * @brief uinput backend for Clicker interface.
 *
 * Sends clicks through virtual input devices created with libevdev
 * and uinput. Works on both X11 and Wayland, but needs write access to the
 * uinput device file (usually /dev/uinput). Call initialize() before using
 * other methods.
 */
class ClickerUInput final : public Clicker {
private:
	libevdev *mouse_device = nullptr; ///< libevdev handle describing the virtual mouse.
	libevdev_uinput *mouse_uinput_device = nullptr; ///< uinput device created from mouse_device.

	bool connected = false; ///< true once initialize() has succeeded.
	int mouse_uinput_fd = -1; ///< File descriptor for the mouse uinput device file.

	/**
	 * @brief Sends a single input event to the virtual mouse device.
	 * @param type The event type (e.g. EV_KEY).
	 * @param code The event code (e.g. BTN_LEFT).
	 * @param value The event value (e.g. 1 = press, 0 = release).
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool send_mouse_input(int type, int code, int value);

	/**
	 * @brief Flushes pending mouse events by sending a synchronisation event.
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool synchronise_mouse_inputs();
public:
	/**
	 * @brief Destructor, destroys the virtual mouse, frees the libevdev handle
	 * and closes the file descriptor used by the virtual mouse.
	 */
	~ClickerUInput();

	/**
	 * @brief Sets the file descriptor for the uinput device file used by the virtual mouse.
	 * @param uinput_fd The file descriptor returned by open().
	 * @note Has to be called before initialize() to take effect.
	 */
	constexpr void set_mouse_file_descriptor(int uinput_fd) {
		mouse_uinput_fd = uinput_fd;
	}

	/**
	 * @brief Creates the virtual mouse devicec. Attempts to open /dev/uinput
	 * if the file descriptors for the virtual mouse haven't been set.
	 * @return true if the device was created successfully, false otherwise.
	 */
	bool initialize() override;

	/**
	 * @brief Checks whether the virtual device mouse has been created and clicks can be sent.
	 * @return true if clicks can currently be sent, false otherwise.
	 */
	bool is_connected() const override;

	/**
	 * @brief Sends a single click of the given mouse button through uinput.
	 * @param button The click to send.
	 * @note Requires initialize() to be called beforehand, does nothing otherwise.
	 */
	void send_click(const Click &click) override;
};

}

}