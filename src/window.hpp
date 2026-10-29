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

#include <src/input/hotkey_listener.hpp>
#include <src/input/threaded_clicker.hpp>

namespace EzAutoClicker {

class MainWindow : public Gtk::ApplicationWindow {
private:
	Input::ClickerOptions options;
	Input::ThreadedClicker threaded_clicker;
	std::unique_ptr<Input::Clicker> clicker;
	std::unique_ptr<Input::HotkeyListener> hotkey_listener;
	sigc::connection hotkey_listener_connection;

	Gtk::Button *start_button;
	Gtk::Button *stop_button;

	Gtk::SpinButton *milliseconds_entry;
	Gtk::SpinButton *seconds_entry;
	Gtk::SpinButton *minutes_entry;
	Gtk::SpinButton *hours_entry;

	Gtk::DropDown *mouse_button_dropdown;
	Gtk::DropDown *click_type_dropdown;

	std::chrono::microseconds input_interval;

	Gtk::CheckButton *current_location_toggle;
	Gtk::CheckButton *pick_location_toggle;
	Gtk::Button *pick_location_button;
	Gtk::SpinButton *x_position_entry;
	Gtk::SpinButton *y_position_entry;

	Gtk::CheckButton *repeat_toggle;
	Gtk::SpinButton *click_repeat_counter;
	Gtk::CheckButton *repeal_until_stopped_toggle;

	bool sending_inputs;

	void on_pick_location_button_pressed();

	void update_mouse_button_option();
	void update_click_type_option();
	void update_repeating_mode();
	void update_options();
	void connect_signals();

	void start_clicker();
	void stop_clicker();

	void set_sending_inputs(bool p_sending_inputs);
public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

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
};

}