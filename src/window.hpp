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

#include <src/input/threaded_input.hpp>

namespace EzAutoClicker {

class HotkeyListener;

class MainWindow : public Gtk::ApplicationWindow {
private:
	ThreadedInput threaded_input;
	HotkeyListener *hotkey_listener;

	Gtk::Button *start_button;
	Gtk::Button *stop_button;

	Gtk::SpinButton *milliseconds_entry;
	Gtk::SpinButton *seconds_entry;
	Gtk::SpinButton *minutes_entry;
	Gtk::SpinButton *hours_entry;

	Gtk::DropDown *mouse_button_dropdown;
	Gtk::DropDown *click_type_dropdown;

	std::chrono::microseconds input_interval;

	Gtk::CheckButton *repeat_toggle;
	Gtk::SpinButton *click_repeat_counter;
	Gtk::CheckButton *repeal_until_stopped_toggle;

	bool sending_inputs;

	void on_mouse_button_selection_changed();
	void on_click_type_selection_changed();
	void on_repeat_button_pressed();

	void update_interval_from_entries();
	void create_gui();

	void start_clicker();
	void stop_clicker();
public:
	MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

	void toggle_clicker();

	void set_input_player(InputPlayer *p_input_player);
	void set_hotkey_listener(HotkeyListener *p_hotkey_listener);

	inline bool is_clicker_running() const {
		return threaded_input.is_running();
	}
};

}