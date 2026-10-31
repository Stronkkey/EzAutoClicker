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

#if EZAUTOCLICKER_ENABLE_WLR

#include <wayland-client.h>
#include "wlr-virtual-pointer-unstable-v1-client-protocol.h"

#include <src/input/clicker.hpp>

namespace EzAutoClicker {

namespace Input {

class ClickerWlr final : public Clicker {
private:
	wl_display *display = nullptr;
	wl_seat *seat = nullptr;
	zwlr_virtual_pointer_manager_v1 *manager = nullptr;
	zwlr_virtual_pointer_v1 *pointer = nullptr;
	bool connected = false;

	static void on_global(
		void *data,
		wl_registry *registry,
		uint32_t name,
		const char *interface,
		uint32_t version);
	static void on_global_remove(void*, wl_registry*, uint32_t) {}
public:
	bool initialize() override;
	void send_click(const Click &click) override;

	constexpr bool is_connected() const override {
		return connected;
	}
};

}

}

#endif