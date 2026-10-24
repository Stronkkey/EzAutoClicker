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

#include <src/input/hotkey_listener_x11.hpp>

#include <X11/keysym.h>

#include <thread>

EzAutoClicker::HotkeyListenerX11::HotkeyListenerX11() : worker_thread(), mutex(), key_code(0), running(false) {
	display = XOpenDisplay(nullptr);

	if (!display)
		return;

	Window root = DefaultRootWindow(display);
	key_code = XKeysymToKeycode(display, XK_Escape);
	unsigned int modifiers = ControlMask;

	// NumLock (Mod2) and CapsLock (Lock) change the modifier mask,
	// so grab every combination, otherwise the hotkey fails when they're on.
	unsigned int ignored[] = {0, LockMask, Mod2Mask, LockMask | Mod2Mask};

	for (unsigned int extra : ignored) {
		XGrabKey(display, key_code, modifiers | extra, root,
			False, GrabModeAsync, GrabModeAsync);
	}

	XSelectInput(display, root, KeyPressMask);
}

EzAutoClicker::HotkeyListenerX11::~HotkeyListenerX11() {
	if (display)
		XCloseDisplay(display);
}

void EzAutoClicker::HotkeyListenerX11::read_inputs(std::stop_token stop_token) {
	XEvent event;
	while (!stop_token.stop_requested()) {
		XNextEvent(display, &event);
		if (event.type == KeyPress && event.xkey.keycode == key_code)
			signal_pressed.emit();
	}

}

int EzAutoClicker::HotkeyListenerX11::start_reading() {
	if (running)
		return 1;

	worker_thread = std::jthread([this](std::stop_token stop_token) { read_inputs(stop_token); });
	running = true;
	return 1;
}

void EzAutoClicker::HotkeyListenerX11::stop_reading() {
	if (!running)
		return;

	running = false;
	worker_thread.request_stop();
}


#endif