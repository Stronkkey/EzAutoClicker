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
#if EZAUTOCLICKER_ENABLE_WLR
 
#include <src/input/wlr/clicker_wlr.hpp>
 
#include <chrono>
#include <string_view>

#include <linux/input-event-codes.h>

#include <src/input/click.hpp>

void EzAutoClicker::Input::ClickerWlr::on_global(
		void *data,
		wl_registry *registry,
		uint32_t name,
		const char *interface,
		uint32_t)
{
	auto *clicker_wlr = static_cast<EzAutoClicker::Input::ClickerWlr*>(data);
	std::string_view interface_view = interface;

	if (interface_view == wl_seat_interface.name)
		clicker_wlr->seat = static_cast<wl_seat*>(wl_registry_bind(
			registry,
			name,
			&wl_seat_interface,
			1));

	if (interface_view == zwlr_virtual_pointer_manager_v1_interface.name)
		clicker_wlr->manager = static_cast<zwlr_virtual_pointer_manager_v1*>(wl_registry_bind(
			registry,
			name,
			&zwlr_virtual_pointer_manager_v1_interface,
			1));
}

bool EzAutoClicker::Input::ClickerWlr::initialize() {
	display = wl_display_connect(nullptr);

	if (!display)
		return false;

	static const wl_registry_listener listener = {
		.global = on_global,
		.global_remove = on_global_remove,
	};

	wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &listener, this);
	wl_display_roundtrip(display);

	if (!seat || !manager)
		return false;

	pointer = zwlr_virtual_pointer_manager_v1_create_virtual_pointer(manager, seat);
	connected = true;
	return connected;
}

static std::chrono::milliseconds get_time_ms() {
	auto now = std::chrono::high_resolution_clock::now();
	return duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
}

/**
 * @brief Converts a Clicker mouse button to its linux event code.
 * @return The linux event code or BTN_LEFT if the button is not supported (e.g. MOUSE_BUTTON::UNKNOWN).
 */
static int mouse_button_to_code(EzAutoClicker::Input::Click::MouseButton button) {
	using MouseButton = EzAutoClicker::Input::Click::MouseButton;

	switch (button) {
		case MouseButton::LEFT:
			return BTN_LEFT;
		case MouseButton::RIGHT:
			return BTN_RIGHT;
		case MouseButton::MIDDLE:
			return BTN_MIDDLE;
		default:
			return BTN_LEFT;
	}
}

void EzAutoClicker::Input::ClickerWlr::send_click(const Click &click) {
	int button = mouse_button_to_code(click.button);
	uint32_t now_ms = get_time_ms().count();

	if (click.set_mouse_location) {
		zwlr_virtual_pointer_v1_motion_absolute(
			pointer,
			now_ms,
			click.position.x,
			click.position.y,
			click.screen_size.width,
			click.screen_size.height);
		zwlr_virtual_pointer_v1_frame(pointer);
		wl_display_flush(display);
	}

	zwlr_virtual_pointer_v1_button(
		pointer,
		now_ms,
		button,
		WL_POINTER_BUTTON_STATE_PRESSED);

	zwlr_virtual_pointer_v1_frame(pointer);
	zwlr_virtual_pointer_v1_button(
		pointer,
		now_ms,
		button,
		WL_POINTER_BUTTON_STATE_RELEASED);

	zwlr_virtual_pointer_v1_frame(pointer);
	wl_display_flush(display);
}

#endif