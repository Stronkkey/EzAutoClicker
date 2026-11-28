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

#include <gtk/gtk.h>

#include <src/input/hotkey_listener.hpp>
#include <src/input/clicker.hpp>
#include <src/window.hpp>

namespace EzAutoClicker {

class EzAutoClickerApp {
private:
	void add_option_entries();
	static gint on_handle_local_options(GApplication*, GVariantDict *options, gpointer data);

	void handle_toggle_option();
	void handle_mouse_button_option(const std::string &value);
	void handle_click_type_option(const std::string &value);
	void handle_pick_location_option(const std::string &value);
	void handle_clicker_option(const std::string &value);
	void handle_hotkey_listener_option(const std::string &value);
	static void on_toggle(GSimpleAction*, GVariant*, gpointer data);

	GtkApplication *application;
	MainWindow *main_window;
	std::unique_ptr<Input::Clicker> clicker;
	std::unique_ptr<Input::HotkeyListener> hotkey_listener;

	std::unique_ptr<Input::Clicker> get_clicker();
	std::unique_ptr<Input::HotkeyListener> get_hotkey_listener();

	struct {
		std::optional<Input::Click::MouseButton> button = std::nullopt;
		unsigned short mouse_x_position = 0;
		unsigned short mouse_y_position = 0;
		bool clicker_set = false;
		bool hotkey_listener_set = false;
		bool fail_initialization = false;
		std::optional<gint32> interval_ms = std::nullopt;
		std::optional<gint32> interval_s = std::nullopt;
		std::optional<gint32> interval_m = std::nullopt;
		std::optional<gint32> interval_h = std::nullopt;
		std::optional<gint32> random_offset_ms = std::nullopt;
		std::optional<gint32> repeat_count = std::nullopt;
		std::optional<Input::ClickerOptions::ClickType> click_type = std::nullopt;
		std::optional<Input::ClickerOptions::RepeatMode> repeat_mode = std::nullopt;
		std::optional<bool> set_mouse_location = std::nullopt;
	} cli_options;

	void set_cli_options_on_main_window();
	static void on_activate(GtkApplication*, gpointer user_data);
public:
	EzAutoClickerApp();
	~EzAutoClickerApp();
	int run(int argc, char *argv[]);

	void set_clicker(std::unique_ptr<Input::Clicker> &&p_clicker);
	void set_hotkey_listener(std::unique_ptr<Input::HotkeyListener> &&p_hotkey_listener);
};

}