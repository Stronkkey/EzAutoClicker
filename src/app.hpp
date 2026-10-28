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

#include <gtkmm.h>

#include <src/window.hpp>
#include <src/input/hotkey_listener.hpp>
#include <src/input/input_player.hpp>

namespace EzAutoClicker {

class EzAutoClickerApp : public Gtk::Application {
private:
	void add_option_entries();
	int on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) override;

	void handle_toggle_option();
	void handle_mouse_button_option(const Glib::ustring &value);
	void handle_click_type_option(const Glib::ustring &value);
	void handle_pick_location_option(const Glib::ustring &value);
	void on_toggle();

	MainWindow *main_window;
	InputPlayer *input_player;
	HotkeyListener *hotkey_listener;

	struct {
		std::optional<Input::MOUSE_BUTTON> button = std::nullopt;
		unsigned short mouse_x_position = 0;
		unsigned short mouse_y_position = 0;
		std::optional<gint32> interval_ms = std::nullopt;
		std::optional<gint32> interval_s = std::nullopt;
		std::optional<gint32> interval_m = std::nullopt;
		std::optional<gint32> interval_h = std::nullopt;
		std::optional<gint32> random_offset_ms = std::nullopt;
		std::optional<gint32> repeat_count = std::nullopt;
		std::optional<InputPlayerOptions::ClickType> click_type = std::nullopt;
		std::optional<InputPlayerOptions::RepeatMode> repeat_mode = std::nullopt;
		std::optional<bool> set_mouse_location = std::nullopt;
	} cli_options;

	void set_cli_options_on_main_window();
protected:
	EzAutoClickerApp();

	void on_activate() override;
public:
	static Glib::RefPtr<EzAutoClickerApp> create(
		InputPlayer *input_player = nullptr,
		HotkeyListener *hotkey_listener = nullptr);
};

}