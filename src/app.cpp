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

#include <config.hpp>
#include <src/app_cli_constants.hpp>
#include <src/input/clicker_factory.hpp>
#include <src/input/evdev/clicker_uinput.hpp>
#include <src/input/evdev/hotkey_listener_evdev.hpp>
#include <src/input/hotkey_listener_factory.hpp>
#include <src/input/wlr/clicker_wlr.hpp>
#include <src/input/x11/clicker_x11.hpp>
#include <src/input/x11/hotkey_listener_x11.hpp>

EzAutoClicker::EzAutoClickerApp::EzAutoClickerApp() {
	application = gtk_application_new(Constants::APP_ID, G_APPLICATION_DEFAULT_FLAGS);

	#if GLIB_MAJOR_VERSION >= 2 && GLIB_MINOR_VERSION >= 80
	g_application_set_version(G_APPLICATION(application), Constants::APP_VERSION);
	#endif

	g_application_set_option_context_summary(G_APPLICATION(application), EZAUTOCLICKER_CONTEXT_SUMMARY);
	g_application_set_option_context_description(G_APPLICATION(application), EZAUTOCLICKER_OPTION_CONTEXT_DESCRIPTION);
	add_option_entries();

	g_signal_connect(application, "activate", G_CALLBACK(EzAutoClickerApp::on_activate), this);
	g_signal_connect(application, "handle_local_options", G_CALLBACK(EzAutoClickerApp::on_handle_local_options), this);

	auto open_about_accel = gtk_accelerator_get_label(GDK_KEY_F1, static_cast<GdkModifierType>(0));
	const char *accels[] = {open_about_accel, ""};
	gtk_application_set_accels_for_action(application, "win.about", accels);

	GSimpleAction *action = g_simple_action_new(Constants::TOGGLE_LONG_NAME, nullptr);
	g_signal_connect(action, "activate", G_CALLBACK(EzAutoClickerApp::on_toggle), this);
	g_action_map_add_action(G_ACTION_MAP(application), G_ACTION(action));
	g_object_unref(action);
}

EzAutoClicker::EzAutoClickerApp::~EzAutoClickerApp() {
	if (application)
		g_object_unref(application);
}

void EzAutoClicker::EzAutoClickerApp::add_option_entries() {
	g_application_add_main_option(G_APPLICATION(application),
		Constants::TOGGLE_LONG_NAME,
		Constants::TOGGLE_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_NONE,
		EZAUTOCLICKER_TOGGLE_OPTION_DESCRIPTION,
		nullptr);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::INTERVAL_MS_LONG_NAME,
		Constants::INTERVAL_MS_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_INTERVAL_MS_OPTION_DESCRIPTION,
		EZAUTOCLICKER_INTERVAL_MS_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::INTERVAL_S_LONG_NAME,
		Constants::INTERVAL_S_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_INTERVAL_S_OPTION_DESCRIPTION,
		EZAUTOCLICKER_INTERVAL_S_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::INTERVAL_M_LONG_NAME,
		Constants::INTERVAL_M_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_INTERVAL_M_OPTION_DESCRIPTION,
		EZAUTOCLICKER_INTERVAL_M_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::INTERVAL_H_LONG_NAME,
		Constants::INTERVAL_H_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_INTERVAL_H_OPTION_DESCRIPTION,
		EZAUTOCLICKER_INTERVAL_H_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::MOUSE_BUTTON_LONG_NAME,
		Constants::MOUSE_BUTTON_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_STRING,
		EZAUTOCLICKER_MOUSE_BUTTON_OPTION_DESCRIPTION,
		Constants::MOUSE_BUTTON_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::CLICK_TYPE_LONG_NAME,
		Constants::CLICK_TYPE_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_STRING,
		EZAUTOCLICKER_CLICK_TYPE_OPTION_DESCRIPTION,
		Constants::CLICK_TYPE_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::PICK_LOCATION_LONG_NAME,
		Constants::PICK_LOCATION_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_STRING,
		EZAUTOCLICKER_PICK_LOCATION_OPTION_DESCRIPTION,
		EZAUTOCLICKER_PICK_LOCATION_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::REPEAT_UNTIL_STOPPED_LONG_NAME,
		Constants::REPEAT_UNTIL_STOPPED_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION,
		nullptr);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::SET_REPEAT_COUNT_LONG_NAME,
		Constants::SET_REPEAT_COUNT_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_INT64,
		EZAUTOCLICKER_SET_REPEAT_COUNT_OPTION_DESCRIPTION,
		EZAUTOCLICKER_SET_REPEAT_COUNT_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::CLICKER_LONG_NAME,
		Constants::CLICKER_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_STRING,
		EZAUTOCLICKER_CLICKER_OPTION_DESCRIPTION,
		Constants::CLICKER_ARG_DESCRIPTION);

	g_application_add_main_option(G_APPLICATION(application),
		Constants::HOTKEY_LISTENER_LONG_NAME,
		Constants::HOTKEY_LISTENER_SHORT_NAME,
		G_OPTION_FLAG_NONE,
		G_OPTION_ARG_STRING,
		EZAUTOCLICKER_HOTKEY_LISTENER_OPTION_DESCRIPTION,
		Constants::HOTKEY_LISTENER_ARG_DESCRIPTION);

	// This option is not handled here, but instead in the main function.
	g_application_add_main_option(G_APPLICATION(application),
		Constants::ALLOW_ROOT_LONG_NAME,
		Constants::ALLOW_ROOT_SHORT_NAME,
		G_OPTION_FLAG_HIDDEN,
		G_OPTION_ARG_NONE,
		EZAUTOCLICKER_ALLOW_ROOT_OPTION_DESCRIPTION,
		nullptr);
}

int EzAutoClicker::EzAutoClickerApp::on_handle_local_options(GApplication*, GVariantDict *options, gpointer data) {
	auto *app = static_cast<EzAutoClickerApp*>(data);
	if (g_variant_dict_contains(options, Constants::TOGGLE_LONG_NAME)) {
		app->handle_toggle_option();
		return 1;
	}

	gint64 int_value = 100;
	gchar *string_value;
	bool bool_value;

	if (g_variant_dict_lookup(options, Constants::INTERVAL_MS_LONG_NAME, "x", &int_value))
		app->cli_options.interval_ms = int_value;

	if (g_variant_dict_lookup(options, Constants::INTERVAL_S_LONG_NAME, "x", &int_value))
		app->cli_options.interval_s = int_value;

	if (g_variant_dict_lookup(options, Constants::INTERVAL_M_LONG_NAME, "x", &int_value))
		app->cli_options.interval_m = int_value;

	if (g_variant_dict_lookup(options, Constants::INTERVAL_H_LONG_NAME, "x", &int_value))
		app->cli_options.interval_h = int_value;

	if (g_variant_dict_lookup(options, Constants::MOUSE_BUTTON_LONG_NAME, "s", &string_value)) {
		app->handle_mouse_button_option(string_value);
		g_free(string_value);
	}

	if (g_variant_dict_lookup(options, Constants::CLICK_TYPE_LONG_NAME, "s", &string_value)) {
		app->handle_click_type_option(string_value);
		g_free(string_value);
	}

	if (g_variant_dict_lookup(options, Constants::PICK_LOCATION_LONG_NAME, "s", &string_value)) {
		app->handle_pick_location_option(string_value);
		g_free(string_value);
	}

	if (g_variant_dict_lookup(options, Constants::REPEAT_UNTIL_STOPPED_LONG_NAME, "b", &bool_value))
		app->cli_options.repeat_mode = bool_value ? Input::ClickerOptions::RepeatMode::UNTIL_STOPPED : Input::ClickerOptions::RepeatMode::REPEAT_X_TIMES;

	if (g_variant_dict_lookup(options, Constants::SET_REPEAT_COUNT_LONG_NAME, "x", &int_value))
		app->cli_options.repeat_count = int_value;

	if (g_variant_dict_lookup(options, Constants::CLICKER_LONG_NAME, "s", &string_value)) {
		app->handle_clicker_option(string_value);
		g_free(string_value);
	}

	if (g_variant_dict_lookup(options, Constants::HOTKEY_LISTENER_LONG_NAME, "s", &string_value)) {
		app->handle_hotkey_listener_option(string_value);
		g_free(string_value);
	}

	return -1;
}

void EzAutoClicker::EzAutoClickerApp::handle_toggle_option() {
	// Connect to the session bus and find out if an instance is already running
	if (!g_application_register(G_APPLICATION(application), nullptr, nullptr)) {
		g_printerr("Could not register on the session bus.\n");
		return;
	}

	if (!g_application_get_is_remote(G_APPLICATION(application))) {
		g_printerr("%s is not running. Start it first.\n", Constants::APP_NAME);
		return;
	}

	g_action_group_activate_action(G_ACTION_GROUP(application), Constants::TOGGLE_LONG_NAME, nullptr);
}

static std::string string_to_lower(const std::string &str) {
	std::string lowercase = std::string();
	lowercase.resize(str.size());

	for (size_t i = 0; i < str.size(); i++)
		lowercase[i] = std::tolower(static_cast<unsigned char>(str[i]));

	return lowercase;
}

void EzAutoClicker::EzAutoClickerApp::handle_mouse_button_option(const std::string &value) {
	std::string lowercase = string_to_lower(value);

	if (lowercase == "left")
		cli_options.button = Input::Click::MouseButton::LEFT;
	if (lowercase == "middle")
		cli_options.button = Input::Click::MouseButton::MIDDLE;
	if (lowercase == "right")
		cli_options.button = Input::Click::MouseButton::RIGHT;
}

void EzAutoClicker::EzAutoClickerApp::handle_click_type_option(const std::string &value) {
	std::string lowercase = string_to_lower(value);

	if (lowercase == "single")
		cli_options.click_type = Input::ClickerOptions::ClickType::SINGLE;
	if (lowercase == "double")
		cli_options.click_type = Input::ClickerOptions::ClickType::DOUBLE;
}

void EzAutoClicker::EzAutoClickerApp::handle_pick_location_option(const std::string &value) {
	size_t pos = value.find(',');

	if (pos == std::string::npos)
		return;

	try {
		cli_options.mouse_x_position = std::stoi(value.substr(0, pos));
		cli_options.mouse_y_position = std::stoi(value.substr(pos + 1, std::string::npos));
	} catch (const std::exception&) {
		return;
	}

	cli_options.set_mouse_location = true;
}

void EzAutoClicker::EzAutoClickerApp::handle_clicker_option(const std::string &value) {
	std::string lowercase = string_to_lower(value);

	if (lowercase == "uinput") {
		clicker = Input::ClickerFactory::create_clicker_uinput();
		cli_options.clicker_set = true;
	}

	#if EZAUTOCLICKER_ENABLE_WLR
	if (lowercase == "wlr") {
		clicker = Input::ClickerFactory::create_clicker_wlr();
		cli_options.clicker_set = true;
	}
	#endif

	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	if (lowercase == "x11") {
		clicker = Input::ClickerFactory::create_clicker_x11();
		cli_options.clicker_set = true;
	}
	#endif

	if (!cli_options.clicker_set && !cli_options.fail_initialization) {
		g_printerr("%s \"%s\" is not a valid clicker backend.\n", Constants::ERROR_MESSAGE_TEMPLATE, value.c_str());
		cli_options.fail_initialization = true;
	}
}

void EzAutoClicker::EzAutoClickerApp::handle_hotkey_listener_option(const std::string &value) {
	std::string lowercase = string_to_lower(value);

	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	if (lowercase == "x11") {
		cli_options.hotkey_listener_set = true;
		hotkey_listener = Input::HotkeyListenerFactory::create_hotkey_listener_x11();
	}
	#endif

	if (lowercase == "evdev") {
		cli_options.hotkey_listener_set = true;
		hotkey_listener = Input::HotkeyListenerFactory::create_hotkey_listener_evdev();
	}

	if (!cli_options.hotkey_listener_set && !cli_options.fail_initialization) {
		g_printerr("%s \"%s\" is not a valid hotkey listener backend.\n", Constants::ERROR_MESSAGE_TEMPLATE, value.c_str());
		cli_options.fail_initialization = true;
	}
}

void EzAutoClicker::EzAutoClickerApp::on_toggle(GSimpleAction*, GVariant*, gpointer data) {
	auto *app = static_cast<EzAutoClickerApp*>(data);
	app->main_window->toggle_clicker();
}

std::unique_ptr<EzAutoClicker::Input::Clicker> EzAutoClicker::EzAutoClickerApp::get_clicker() {
	if (cli_options.clicker_set)
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

static void on_main_window_hide(GtkWidget*, gpointer data) {
	EzAutoClicker::MainWindow *main_window = static_cast<EzAutoClicker::MainWindow*>(data);
	delete main_window;
}

void EzAutoClicker::EzAutoClickerApp::on_activate(GtkApplication*, gpointer data) {
	// Called for the first start, and when a second plain launch happens
	auto *app = static_cast<EzAutoClickerApp*>(data);
	GList *windows = gtk_application_get_windows(app->application);
	gpointer front_window = windows ? windows->data : nullptr;

	if (front_window) {
		gtk_window_present(GTK_WINDOW(front_window));
		return;
	}

	if (app->cli_options.fail_initialization) {
		g_application_quit(G_APPLICATION(app->application));
		return;
	}

	GtkBuilder *builder = gtk_builder_new_from_resource("/io/github/Stronkkey/EzAutoClicker/main_window.ui");
	app->main_window = new MainWindow(builder);
	g_object_unref(builder);

	app->main_window->set_clicker(app->get_clicker());
	app->main_window->set_hotkey_listener(app->get_hotkey_listener());
	gtk_application_add_window(app->application, GTK_WINDOW(app->main_window->get_window()));

	app->set_cli_options_on_main_window();

	g_signal_connect(app->main_window->get_window(), "hide", G_CALLBACK(on_main_window_hide), app->main_window);
	app->main_window->present();
}

int EzAutoClicker::EzAutoClickerApp::run(int argc, char *argv[]) {
	return g_application_run(G_APPLICATION(application), argc, argv);
}

void EzAutoClicker::EzAutoClickerApp::set_clicker(std::unique_ptr<Input::Clicker> &&p_clicker) {
	clicker = std::move(p_clicker);
}

void EzAutoClicker::EzAutoClickerApp::set_hotkey_listener(std::unique_ptr<Input::HotkeyListener> &&p_hotkey_listener) {
	hotkey_listener = std::move(p_hotkey_listener);
}
