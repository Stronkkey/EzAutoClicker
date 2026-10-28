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
#include <src/app.hpp>

#include <src/app_constants.hpp>

EzAutoClicker::EzAutoClickerApp::EzAutoClickerApp() : Gtk::Application(APP_ID) {
	set_id(APP_ID);
	set_version(APP_VERSION);
	add_option_entries();

	// Exported action, callable from other processes
	add_action(TOGGLE_LONG_NAME,
		sigc::mem_fun(*this,
		&EzAutoClickerApp::on_toggle));
}

void EzAutoClicker::EzAutoClickerApp::add_option_entries() {
	add_main_option_entry(Gio::Application::OptionType::BOOL,
		TOGGLE_LONG_NAME,
		TOGGLE_SHORT_NAME,
		TOGGLE_OPTION_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		INTERVAL_MS_LONG_NAME,
		INTERVAL_MS_SHORT_NAME,
		INTERVAL_MS_OPTION_DESCRIPTION,
		INTERVAL_MS_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		INTERVAL_S_LONG_NAME,
		INTERVAL_S_SHORT_NAME,
		INTERVAL_S_OPTION_DESCRIPTION,
		INTERVAL_S_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		INTERVAL_M_LONG_NAME,
		INTERVAL_M_SHORT_NAME,
		INTERVAL_M_OPTION_DESCRIPTION,
		INTERVAL_M_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		INTERVAL_H_LONG_NAME,
		INTERVAL_H_SHORT_NAME,
		INTERVAL_H_OPTION_DESCRIPTION,
		INTERVAL_H_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		MOUSE_BUTTON_LONG_NAME,
		MOUSE_BUTTON_SHORT_NAME,
		MOUSE_BUTTON_OPTION_DESCRIPTION,
		MOUSE_BUTTON_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		CLICK_TYPE_LONG_NAME,
		CLICK_TYPE_SHORT_NAME,
		CLICK_TYPE_OPTION_DESCRIPTION,
		CLICK_TYPE_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		PICK_LOCATION_LONG_NAME,
		PICK_LOCATION_SHORT_NAME,
		PICK_LOCATION_OPTION_DESCRIPTION,
		PICK_LOCATION_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::BOOL,
		REPEAT_UNTIL_STOPPED_LONG_NAME,
		REPEAT_UNTIL_STOPPED_SHORT_NAME,
		REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		SET_REPEAT_COUNT_LONG_NAME,
		SET_REPEAT_COUNT_SHORT_NAME,
		SET_REPEAT_COUNT_OPTION_DESCRIPTION,
		SET_REPEAT_COUNT_ARG_DESCRIPTION);
}

Glib::RefPtr<EzAutoClicker::EzAutoClickerApp> EzAutoClicker::EzAutoClickerApp::create(
		Input::Clicker *clicker, Input::HotkeyListener *hotkey_listener)
{
	auto app = Glib::make_refptr_for_instance(new EzAutoClickerApp());
	app->clicker = clicker;
	app->hotkey_listener = hotkey_listener;

	return app;
}

int EzAutoClicker::EzAutoClickerApp::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) {
	if (options->contains(TOGGLE_LONG_NAME)) {
		handle_toggle_option();
		return 1;
	}

	gint32 int_value;
	Glib::ustring string_value;
	bool bool_value;

	if (options->lookup_value(INTERVAL_MS_LONG_NAME, int_value))
		cli_options.interval_ms = int_value;

	if (options->lookup_value(INTERVAL_S_LONG_NAME, int_value))
		cli_options.interval_s = int_value;

	if (options->lookup_value(INTERVAL_M_LONG_NAME, int_value))
		cli_options.interval_m = int_value;

	if (options->lookup_value(INTERVAL_H_LONG_NAME, int_value))
		cli_options.interval_h = int_value;

	if (options->lookup_value(MOUSE_BUTTON_LONG_NAME, string_value))
		handle_mouse_button_option(string_value);

	if (options->lookup_value(CLICK_TYPE_LONG_NAME, string_value))
		handle_click_type_option(string_value);

	if (options->lookup_value(PICK_LOCATION_LONG_NAME, string_value))
		handle_pick_location_option(string_value);

	if (options->lookup_value(REPEAT_UNTIL_STOPPED_LONG_NAME, bool_value))
		cli_options.repeat_mode = bool_value ? Input::ClickerOptions::RepeatMode::UNTIL_STOPPED : Input::ClickerOptions::RepeatMode::REPEAT_X_TIMES;

	if (options->lookup_value(SET_REPEAT_COUNT_LONG_NAME, int_value))
		cli_options.repeat_count = int_value;

	return -1;
}

void EzAutoClicker::EzAutoClickerApp::handle_toggle_option() {
	// Connect to the session bus and find out if an instance is already running
	if (!register_application()) {
		g_printerr("Could not register on the session bus.\n");
		return;
	}

	if (!is_remote()) {
		g_printerr("%s is not running. Start it first.\n", APP_NAME);
		return;
	}

	activate_action(TOGGLE_LONG_NAME);
}

void EzAutoClicker::EzAutoClickerApp::handle_mouse_button_option(const Glib::ustring &value) {
	if (value.lowercase() == "left")
		cli_options.button = Input::Click::MOUSE_BUTTON::LEFT;
	if (value.lowercase() == "middle")
		cli_options.button = Input::Click::MOUSE_BUTTON::MIDDLE;
	if (value.lowercase() == "right")
		cli_options.button = Input::Click::MOUSE_BUTTON::RIGHT;
}

void EzAutoClicker::EzAutoClickerApp::handle_click_type_option(const Glib::ustring &value) {
	if (value.lowercase() == "single")
		cli_options.click_type = Input::ClickerOptions::ClickType::SINGLE;
	if (value.lowercase() == "double")
		cli_options.click_type = Input::ClickerOptions::ClickType::DOUBLE;
}

void EzAutoClicker::EzAutoClickerApp::handle_pick_location_option(const Glib::ustring &value) {
	size_t pos = value.find(',');

	if (pos == Glib::ustring::npos)
		return;

	try {
		cli_options.mouse_x_position = std::stoi(value.substr(0, pos));
		cli_options.mouse_y_position = std::stoi(value.substr(pos + 1, Glib::ustring::npos));
	} catch (const std::exception&) {
		return;
	}

	cli_options.set_mouse_location = true;
}

void EzAutoClicker::EzAutoClickerApp::on_toggle() {
	main_window->toggle_clicker();
}

void EzAutoClicker::EzAutoClickerApp::set_cli_options_on_main_window() {
	if (cli_options.button)
		main_window->set_mouse_button(*cli_options.button);

	if (cli_options.interval_ms)
		main_window->set_interval_ms(std::chrono::milliseconds(*cli_options.interval_ms));

	if (cli_options.interval_s)
		main_window->set_interval_s(std::chrono::seconds(*cli_options.interval_s));

	if (cli_options.interval_m)
		main_window->set_interval_m(std::chrono::minutes(*cli_options.interval_m));

	if (cli_options.interval_h)
		main_window->set_interval_h(std::chrono::hours(*cli_options.interval_h));

	if (cli_options.random_offset_ms)
		main_window->set_random_offset(std::chrono::milliseconds(*cli_options.random_offset_ms));

	if (cli_options.click_type)
		main_window->set_click_type(*cli_options.click_type);

	if (cli_options.repeat_mode)
		main_window->set_repeat_mode(*cli_options.repeat_mode);

	if (cli_options.repeat_count)
		main_window->set_repeat_count(*cli_options.repeat_count);

	if (cli_options.set_mouse_location) {
		main_window->set_use_mouse_offset(*cli_options.set_mouse_location);
		if (*cli_options.set_mouse_location) {
			main_window->set_mouse_x_position(cli_options.mouse_x_position);
			main_window->set_mouse_y_position(cli_options.mouse_y_position);
		}
	}
}

void EzAutoClicker::EzAutoClickerApp::on_activate() {
	// Called for the first start, and when a second plain launch happens
	auto windows = get_windows();
	if (!windows.empty()) {
		windows.front()->present();
		return;
	}

	auto builder = Gtk::Builder::create_from_resource("/org/Stronkkey/EzAutoClicker/main_window.ui");

	main_window = Gtk::Builder::get_widget_derived<MainWindow>(builder, "main_window");
	main_window->set_clicker(clicker);
	main_window->set_hotkey_listener(hotkey_listener);
	main_window->set_title(APP_NAME);
	add_window(*main_window);

	set_cli_options_on_main_window();

	main_window->signal_hide().connect([this] {
		delete main_window;
	});

	main_window->present();
}