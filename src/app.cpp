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
#include <src/input/clicker_factory.hpp>
#include <src/input/evdev/clicker_uinput.hpp>
#include <src/input/evdev/hotkey_listener_evdev.hpp>
#include <src/input/hotkey_listener_factory.hpp>
#include <src/input/wlr/clicker_wlr.hpp>
#include <src/input/x11/clicker_x11.hpp>
#include <src/input/x11/hotkey_listener_x11.hpp>

EzAutoClicker::EzAutoClickerApp::EzAutoClickerApp() : Gtk::Application(Constants::APP_ID) {
	set_id(Constants::APP_ID);
	set_version(Constants::APP_VERSION);
	set_option_context_summary(Constants::CONTEXT_SUMMARY);
	set_option_context_description(Constants::OPTION_CONTEXT_DESCRIPTION);
	add_option_entries();

	// Exported action, callable from other processes
	add_action(Constants::TOGGLE_LONG_NAME,
		sigc::mem_fun(*this,
		&EzAutoClickerApp::on_toggle));
}

void EzAutoClicker::EzAutoClickerApp::add_option_entries() {
	add_main_option_entry(Gio::Application::OptionType::BOOL,
		Constants::TOGGLE_LONG_NAME,
		Constants::TOGGLE_SHORT_NAME,
		Constants::TOGGLE_OPTION_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		Constants::INTERVAL_MS_LONG_NAME,
		Constants::INTERVAL_MS_SHORT_NAME,
		Constants::INTERVAL_MS_OPTION_DESCRIPTION,
		Constants::INTERVAL_MS_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		Constants::INTERVAL_S_LONG_NAME,
		Constants::INTERVAL_S_SHORT_NAME,
		Constants::INTERVAL_S_OPTION_DESCRIPTION,
		Constants::INTERVAL_S_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		Constants::INTERVAL_M_LONG_NAME,
		Constants::INTERVAL_M_SHORT_NAME,
		Constants::INTERVAL_M_OPTION_DESCRIPTION,
		Constants::INTERVAL_M_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		Constants::INTERVAL_H_LONG_NAME,
		Constants::INTERVAL_H_SHORT_NAME,
		Constants::INTERVAL_H_OPTION_DESCRIPTION,
		Constants::INTERVAL_H_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		Constants::MOUSE_BUTTON_LONG_NAME,
		Constants::MOUSE_BUTTON_SHORT_NAME,
		Constants::MOUSE_BUTTON_OPTION_DESCRIPTION,
		Constants::MOUSE_BUTTON_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		Constants::CLICK_TYPE_LONG_NAME,
		Constants::CLICK_TYPE_SHORT_NAME,
		Constants::CLICK_TYPE_OPTION_DESCRIPTION,
		Constants::CLICK_TYPE_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		Constants::PICK_LOCATION_LONG_NAME,
		Constants::PICK_LOCATION_SHORT_NAME,
		Constants::PICK_LOCATION_OPTION_DESCRIPTION,
		Constants::PICK_LOCATION_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::BOOL,
		Constants::REPEAT_UNTIL_STOPPED_LONG_NAME,
		Constants::REPEAT_UNTIL_STOPPED_SHORT_NAME,
		Constants::REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::INT,
		Constants::SET_REPEAT_COUNT_LONG_NAME,
		Constants::SET_REPEAT_COUNT_SHORT_NAME,
		Constants::SET_REPEAT_COUNT_OPTION_DESCRIPTION,
		Constants::SET_REPEAT_COUNT_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		Constants::CLICKER_LONG_NAME,
		Constants::CLICKER_SHORT_NAME,
		Constants::CLICKER_OPTION_DESCRIPTION,
		Constants::CLICKER_ARG_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING,
		Constants::HOTKEY_LISTENER_LONG_NAME,
		Constants::HOTKEY_LISTENER_SHORT_NAME,
		Constants::HOTKEY_LISTENER_OPTION_DESCRIPTION,
		Constants::HOTKEY_LISTENER_ARG_DESCRIPTION);

	// This option is not handled here, but instead in the main function.
	add_main_option_entry(Gio::Application::OptionType::BOOL,
		Constants::ALLOW_ROOT_LONG_NAME,
		Constants::ALLOW_ROOT_SHORT_NAME,
		Constants::ALLOW_ROOT_OPTION_DESCRIPTION,
		{},
		Glib::OptionEntry::Flags::HIDDEN);
}

Glib::RefPtr<EzAutoClicker::EzAutoClickerApp> EzAutoClicker::EzAutoClickerApp::create(
		std::unique_ptr<Input::Clicker> clicker, std::unique_ptr<Input::HotkeyListener> hotkey_listener)
{
	auto app = Glib::make_refptr_for_instance(new EzAutoClickerApp());
	app->clicker = std::move(clicker);
	app->hotkey_listener = std::move(hotkey_listener);

	return app;
}

int EzAutoClicker::EzAutoClickerApp::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) {
	if (options->contains(Constants::TOGGLE_LONG_NAME)) {
		handle_toggle_option();
		return 1;
	}

	gint32 int_value;
	Glib::ustring string_value;
	bool bool_value;

	if (options->lookup_value(Constants::INTERVAL_MS_LONG_NAME, int_value))
		cli_options.interval_ms = int_value;

	if (options->lookup_value(Constants::INTERVAL_S_LONG_NAME, int_value))
		cli_options.interval_s = int_value;

	if (options->lookup_value(Constants::INTERVAL_M_LONG_NAME, int_value))
		cli_options.interval_m = int_value;

	if (options->lookup_value(Constants::INTERVAL_H_LONG_NAME, int_value))
		cli_options.interval_h = int_value;

	if (options->lookup_value(Constants::MOUSE_BUTTON_LONG_NAME, string_value))
		handle_mouse_button_option(string_value);

	if (options->lookup_value(Constants::CLICK_TYPE_LONG_NAME, string_value))
		handle_click_type_option(string_value);

	if (options->lookup_value(Constants::PICK_LOCATION_LONG_NAME, string_value))
		handle_pick_location_option(string_value);

	if (options->lookup_value(Constants::REPEAT_UNTIL_STOPPED_LONG_NAME, bool_value))
		cli_options.repeat_mode = bool_value ? Input::ClickerOptions::RepeatMode::UNTIL_STOPPED : Input::ClickerOptions::RepeatMode::REPEAT_X_TIMES;

	if (options->lookup_value(Constants::SET_REPEAT_COUNT_LONG_NAME, int_value))
		cli_options.repeat_count = int_value;

	if (options->lookup_value(Constants::CLICKER_LONG_NAME, string_value))
		handle_clicker_option(string_value);

	if (options->lookup_value(Constants::HOTKEY_LISTENER_LONG_NAME, string_value))
		handle_hotkey_listener_option(string_value);

	return -1;
}

void EzAutoClicker::EzAutoClickerApp::handle_toggle_option() {
	// Connect to the session bus and find out if an instance is already running
	if (!register_application()) {
		g_printerr("Could not register on the session bus.\n");
		return;
	}

	if (!is_remote()) {
		g_printerr("%s is not running. Start it first.\n", Constants::APP_NAME);
		return;
	}

	activate_action(Constants::TOGGLE_LONG_NAME);
}

void EzAutoClicker::EzAutoClickerApp::handle_mouse_button_option(const Glib::ustring &value) {
	if (value.lowercase() == "left")
		cli_options.button = Input::Click::MouseButton::LEFT;
	if (value.lowercase() == "middle")
		cli_options.button = Input::Click::MouseButton::MIDDLE;
	if (value.lowercase() == "right")
		cli_options.button = Input::Click::MouseButton::RIGHT;
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

void EzAutoClicker::EzAutoClickerApp::handle_clicker_option(const Glib::ustring &value) {
	if (value.lowercase() == "uinput")
		clicker = Input::ClickerFactory::create_clicker_uinput();
	#if EZAUTOCLICKER_ENABLE_WLR
	if (value.lowercase() == "wlr")
		clicker = Input::ClickerFactory::create_clicker_wlr();
	#endif
	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	if (value.lowercase() == "x11")
		clicker = Input::ClickerFactory::create_clicker_x11();
	#endif
}

void EzAutoClicker::EzAutoClickerApp::handle_hotkey_listener_option(const Glib::ustring &value) {
	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	if (value.lowercase() == "x11")
		hotkey_listener = Input::HotkeyListenerFactory::create_hotkey_listener_x11();
	#endif
	if (value.lowercase() == "evdev")
		hotkey_listener = Input::HotkeyListenerFactory::create_hotkey_listener_evdev();
}

void EzAutoClicker::EzAutoClickerApp::on_toggle() {
	main_window->toggle_clicker();
}

std::unique_ptr<EzAutoClicker::Input::Clicker> EzAutoClicker::EzAutoClickerApp::get_clicker() {
	if (clicker)
		return std::move(clicker);
	return Input::ClickerFactory::create_clicker();
}

std::unique_ptr<EzAutoClicker::Input::HotkeyListener> EzAutoClicker::EzAutoClickerApp::get_hotkey_listener() {
	if (hotkey_listener)
		return std::move(hotkey_listener);
	return Input::HotkeyListenerFactory::create_hotkey_listener();
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
	main_window->set_clicker(get_clicker());
	main_window->set_hotkey_listener(get_hotkey_listener());
	main_window->set_title(Constants::APP_NAME);
	add_window(*main_window);

	set_cli_options_on_main_window();

	main_window->signal_hide().connect([this] {
		delete main_window;
	});

	main_window->present();
}