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

#include <src/input/input_player.hpp>

#include <memory>

struct libevdev;
struct libevdev_uinput;

namespace EzAutoClicker {

/**
 * @brief uinput backend for InputPlayer interface.
 *
 * Sends inputs through virtual input devices created with libevdev
 * and uinput. Works on both X11 and Wayland, but needs write access to the
 * uinput device file (usually /dev/uinput). Call initialize() before using
 * other methods.
 */
class InputPlayerUInput final : public InputPlayer {
private:
	libevdev *mouse_device = nullptr; ///< libevdev handle describing the virtual mouse.
	libevdev *keyboard_device = nullptr; ///< libevdev handle describing the virtual keyboard.
	libevdev_uinput *mouse_uinput_device = nullptr; ///< uinput device created from mouse_device.
	libevdev_uinput *keyboard_uinput_device = nullptr; ///< uinput device created from keyboard_device.

	bool connected = false; ///< true once initialize() has succeeded.
	int mouse_uinput_fd = -1; ///< File descriptor for the mouse uinput device file.
	int keyboard_uinput_fd = -1; ///< File descriptor for the keyboard uinput device file.
	
	/**
	 * @brief Sends a single input event to the virtual mouse device.
	 * @param type The event type (e.g. EV_KEY).
	 * @param code The event code (e.g. BTN_LEFT).
	 * @param value The event value (e.g. 1 = press, 0 = release).
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool send_mouse_input(int type, int code, int value);

	/**
	 * @brief Sends a single input event to the virtual keyboard device.
	 * @param type The event type (e.g. EV_KEY).
	 * @param code The event code (e.g. BTN_LEFT).
	 * @param value The event value (e.g. 1 = press, 0 = release).
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool send_keyboard_input(int type, int code, int value);

	/**
	 * @brief Flushes pending mouse events by sending a synchronisation event.
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool synchronise_mouse_inputs();
	
	/**
	 * @brief Flushes pending keyboard events by sending a synchronisation event.
	 * @return true if the event was written successfully, false otherwise.
	 */
	bool synchronise_keyboard_inputs();
public:
	/**
	 * @brief Destructor, destroys the virtual devices, frees the libevdev handles
	 * and closes the file descriptors used by the virtual mouse and virtual keyboard.
	 */
	~InputPlayerUInput();

	/**
	 * @brief Sets the file descriptor for the uinput device file used by the virtual mouse.
	 * @param uinput_fd The file descriptor returned by open().
	 * @note Has to be called before initialize() to take effect.
	 */
	constexpr void set_mouse_file_descriptor(int uinput_fd) {
		mouse_uinput_fd = uinput_fd;
	}

	/**
	 * @brief Sets the file descriptor for the uinput device file used by the virtual keyboard.
	 * @param uinput_fd The file descriptor returned by open().
	 * @note Has to be called before initialize() to take effect.
	 */
	constexpr void set_keyboard_file_descriptor(int uinput_fd) {
		keyboard_uinput_fd = uinput_fd;
	}

	/**
	 * @brief Creates the virtual mouse and keyboard devices. Attempts to open /dev/uinput
	 * if the file descriptors for the virtual mouse or virtual keyboard haven't been set.
	 * @return true if the devices were created successfully, false otherwise.
	 */
	bool initialize() override;

	/**
	 * @brief Checks whether the virtual devices have been created.
	 * @return true if inputs can currently be sent, false otherwise.
	 */
	bool is_connected() const override;

	/**
	 * @brief Sends a single click of the given mouse button through uinput.
	 * @param button The button to click. Defaults to MOUSE_BUTTON::LEFT.
	 * @note Requires initialize() to be called beforehand, does nothing otherwise.
	 */
	void send_input(const Input &input) override;
};

std::unique_ptr<EzAutoClicker::InputPlayerUInput> create_input_player_uinput();

}