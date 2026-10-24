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
#include <src/window.hpp>

#define APP_ID "org.Stronkkey.EzAutoClicker"
#define APP_NAME "EzAutoClicker"

#define TOGGLE_SHORT_NAME 't'
#define TOGGLE_LONG_NAME "toggle"
#define TOGGLE_OPTION_DESCRIPTION "Start or stop the autoclicker in the running instance"

#define SEND_CLICK_SHORT_NAME 's'
#define SEND_CLICK_LONG_NAME "send-click"
#define SEND_CLICK_OPTION_DESCRIPTION "Send a click"

EzAutoClicker::EzAutoClickerApp::EzAutoClickerApp() : Gtk::Application(APP_ID) {
	add_main_option_entry(Gio::Application::OptionType::BOOL,
		TOGGLE_LONG_NAME,
		TOGGLE_SHORT_NAME,
		TOGGLE_OPTION_DESCRIPTION);

	add_main_option_entry(Gio::Application::OptionType::STRING_VECTOR,
		SEND_CLICK_LONG_NAME,
		SEND_CLICK_SHORT_NAME,
		SEND_CLICK_OPTION_DESCRIPTION);

	// Runs in the process that was launched, before anything is sent to the primary instance
	signal_handle_local_options().connect(
		sigc::mem_fun(*this,
			&EzAutoClickerApp::on_handle_local_options), false);

	// Exported action, callable from other processes
	add_action("toggle",
		sigc::mem_fun(*this,
			&EzAutoClickerApp::on_toggle));
}

Glib::RefPtr<EzAutoClicker::EzAutoClickerApp> EzAutoClicker::EzAutoClickerApp::create(
		InputPlayer *p_input_player, HotkeyListener *p_hotkey_listener)
{
	auto app = Glib::make_refptr_for_instance(new EzAutoClickerApp());
	app->input_player = p_input_player;
	app->hotkey_listener = p_hotkey_listener;

	return app;
}

int EzAutoClicker::EzAutoClickerApp::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) {
	if (!options->contains("toggle"))
		return -1;

	// Connect to the session bus and find out if an instance is already running
	if (!register_application()) {
		g_printerr("Could not register on the session bus.\n");
		return 1;
	}

	if (!is_remote()) {
		g_printerr("Autoclicker is not running. Start it first.\n");
		return 1;
	}

	activate_action("toggle");
	return 0; // exit this process, the primary instance does the work
}

void EzAutoClicker::EzAutoClickerApp::on_toggle() {
	main_window->toggle_clicker();
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
	main_window->set_input_player(input_player);
	main_window->set_hotkey_listener(hotkey_listener);
	main_window->set_title(APP_NAME);
	add_window(*main_window);

	main_window->signal_hide().connect([this] {
		delete main_window;
	});

	main_window->present();
}