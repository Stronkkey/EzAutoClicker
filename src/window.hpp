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

#include <chrono>

#include <gtk/gtk.h>

#include <src/input/hotkey_listener.hpp>
#include <src/input/threaded_clicker.hpp>

namespace EzAutoClicker {

class MainWindow {
private:
	friend class MainWindowSignalHandler;

	GtkApplicationWindow *application_window;
	Input::ClickerOptions options;
	Input::ThreadedClicker threaded_clicker;
	std::unique_ptr<Input::Clicker> clicker;
	std::unique_ptr<Input::HotkeyListener> hotkey_listener;

	GtkAboutDialog *about_dialog;

	GtkButton *start_button;
	GtkButton *stop_button;

	GtkButton *hotkey_button;

	struct GDeleter {
		inline void operator()(char *string) {
			if (string)
				g_free(string);
		}
	};

	std::unique_ptr<char, GDeleter> hotkey_name = nullptr;

	GtkSpinButton *milliseconds_entry;
	GtkSpinButton *seconds_entry;
	GtkSpinButton *minutes_entry;
	GtkSpinButton *hours_entry;

	GtkDropDown *mouse_button_dropdown;
	GtkDropDown *click_type_dropdown;

	std::chrono::microseconds input_interval;

	GtkCheckButton *current_location_toggle;
	GtkCheckButton *pick_location_toggle;
	GtkSpinButton *x_position_entry;
	GtkSpinButton *y_position_entry;

	GtkCheckButton *repeat_toggle;
	GtkSpinButton *click_repeat_counter;
	GtkCheckButton *repeal_until_stopped_toggle;

	bool sending_inputs;
	bool capturing_key_presses;

	void update_mouse_button_option();
	void update_click_type_option();
	void update_repeating_mode();
	void update_options();
	void connect_signals();

	bool on_key_pressed(guint keyval, guint keycode, GdkModifierType state);

	void start_clicker();
	void stop_clicker();

	void set_sending_inputs(bool p_sending_inputs);
	void set_capturing_keypresses(bool p_capturing_key_presses);
public:
	MainWindow(GtkBuilder *builder);
	~MainWindow();

	void toggle_clicker();

	void set_clicker(std::unique_ptr<Input::Clicker> &&p_clicker);
	void set_hotkey_listener(std::unique_ptr<Input::HotkeyListener> &&p_hotkey_listener);

	void set_interval_ms(std::chrono::milliseconds interval);
	void set_interval_s(std::chrono::seconds interval);
	void set_interval_m(std::chrono::minutes interval);
	void set_interval_h(std::chrono::hours interval);

	void set_mouse_button(Input::Click::MouseButton mouse_button);
	void set_click_type(Input::ClickerOptions::ClickType click_type);
	void set_repeat_mode(Input::ClickerOptions::RepeatMode repeat_mode);
	void set_use_mouse_offset(bool use_mouse_offset);
	void set_mouse_x_position(int x);
	void set_mouse_y_position(int y);
	void set_repeat_count(long long repeat_count);
	void set_random_offset(std::chrono::milliseconds random_offset);

	inline bool is_clicker_running() const {
		return threaded_clicker.is_running();
	}

	inline GtkApplicationWindow *get_window() {
		return application_window;
	}

	inline void present() {
		gtk_window_present(GTK_WINDOW(get_window()));
	}
};

}