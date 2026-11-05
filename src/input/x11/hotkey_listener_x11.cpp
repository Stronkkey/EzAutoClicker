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

#include <src/input/x11/hotkey_listener_x11.hpp>

#include <poll.h>

#include <thread>

#include <X11/keysym.h>

static constexpr KeyCode get_hotkey_x11_key(const EzAutoClicker::Input::Hotkey &hotkey) {
	return hotkey.key + 8;
}

static constexpr Mask get_hotkey_x11_modifier(const EzAutoClicker::Input::Hotkey &hotkey) {
	using Modifier = EzAutoClicker::Input::Hotkey::Modifier;
	Mask mask = None;

	if (hotkey.modifiers == Modifier::ALL)
		return AnyModifier;

	if (hotkey.has_modifiers_flag(Modifier::SHIFT))
		mask = mask | ShiftMask;

	if (hotkey.has_modifiers_flag(Modifier::ALT))
		mask = mask | Mod1Mask;

	if (hotkey.has_modifiers_flag(Modifier::CTRL))
		mask = mask | ControlMask;

	if (hotkey.has_modifiers_flag(Modifier::SUPER))
		mask = mask | Mod4Mask;

	return mask;
}

/**
 * @brief Get an array of modifiers to ignore when grabbing a key.
 * @details NumLock (Mod2) and CapsLock (Lock) change the modifier mask,
 * so these mask should be ignored otherwise the hotkey fails when they're on.
 */
static constexpr std::array<Mask, 4> get_ignore_modifiers() {
	return {None, LockMask, Mod2Mask, LockMask | Mod2Mask};
}

EzAutoClicker::Input::HotkeyListenerX11::HotkeyListenerX11() : worker_thread(), running(false) {
	if (XInitThreads() == 0)
		return;

	display = XOpenDisplay(nullptr);
	if (!display)
		return;

	XSelectInput(display, DefaultRootWindow(display), KeyPressMask);
}

EzAutoClicker::Input::HotkeyListenerX11::~HotkeyListenerX11() {
	stop_reading();
	if (display) {
		unregister_hotkey();
		XCloseDisplay(display);
	}
}

void EzAutoClicker::Input::HotkeyListenerX11::read_inputs(std::stop_token stop_token) {
	int x11_fd = ConnectionNumber(display);
	XEvent event;
	pollfd poll_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;

	while (!stop_token.stop_requested()) {
		while (XPending(display) > 0) {
			XNextEvent(display, &event);
			auto lock = std::unique_lock(hotkey_mutex);
			if (event.type == KeyPress && event.xkey.keycode == get_hotkey_x11_key(hotkey))
				signal_pressed.emit();
		}

		poll_fd.fd = x11_fd;
		poll(&poll_fd, 1, 100); // wake up every 100 ms to re-check the stop token
	}
}

bool EzAutoClicker::Input::HotkeyListenerX11::is_reading() const {
	return running;
}

void EzAutoClicker::Input::HotkeyListenerX11::start_reading() {
	if (running || !display)
		return;

	worker_thread = std::jthread([this](std::stop_token stop_token) { read_inputs(stop_token); });
	running = true;
}

void EzAutoClicker::Input::HotkeyListenerX11::stop_reading() {
	if (!running || !display)
		return;

	running = false;
	worker_thread.request_stop();
}

bool EzAutoClicker::Input::HotkeyListenerX11::register_hotkey(const Hotkey &p_hotkey) {
	unregister_hotkey();
	if (p_hotkey.key == 0 || !display)
		return false;

	auto lock = std::unique_lock(hotkey_mutex);
	hotkey = p_hotkey;

	KeyCode key_code = get_hotkey_x11_key(p_hotkey);
	if (p_hotkey.modifiers == Hotkey::Modifier::ALL) {
		XGrabKey(display,
			key_code,
			AnyModifier,
			DefaultRootWindow(display),
			False,
			GrabModeAsync,
			GrabModeAsync);
		return true;
	}

	Mask modifiers = get_hotkey_x11_modifier(p_hotkey);
	for (Mask ignore : get_ignore_modifiers())
		XGrabKey(display,
			key_code,
			modifiers | ignore,
			DefaultRootWindow(display),
			False,
			GrabModeAsync,
			GrabModeAsync);

	return true;
}

void EzAutoClicker::Input::HotkeyListenerX11::unregister_hotkey() {
	if (hotkey.key == 0 || !display)
		return;

	KeyCode key_code = get_hotkey_x11_key(hotkey);
	if (hotkey.modifiers == Hotkey::Modifier::ALL) {
		XUngrabKey(display,
			key_code,
			AnyModifier,
			DefaultRootWindow(display));
		return;
	}

	Mask modifiers = get_hotkey_x11_modifier(hotkey);
	for (Mask ignore : get_ignore_modifiers())
		XUngrabKey(display,
			key_code,
			modifiers | ignore,
			DefaultRootWindow(display));
}


#endif