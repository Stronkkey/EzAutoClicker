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
#include <src/window.hpp>

#include <linux/input-event-codes.h>

#include <glib/gi18n.h>

#include <config.hpp>

namespace EzAutoClicker {

class MainWindowSignalHandler {
public:
	MainWindowSignalHandler() = delete;
	~MainWindowSignalHandler() = delete;

	void operator=(MainWindowSignalHandler&) = delete;
	void operator=(const MainWindowSignalHandler&) = delete;

	static void on_start_button_clicked(GtkButton*, gpointer data) {
		MainWindow *main_window = static_cast<MainWindow*>(data);
		main_window->start_clicker();
	}

	static void on_stop_button_clicked(GtkButton*, gpointer data) {
		MainWindow *main_window = static_cast<MainWindow*>(data);
		main_window->stop_clicker();
	}

	static void on_hotkey_button_clicked(GtkButton*, gpointer data) {
		MainWindow *main_window = static_cast<MainWindow*>(data);
		main_window->set_capturing_keypresses(true);
	}

	static gboolean on_key_pressed(GtkEventController*, guint keyval, guint keycode, GdkModifierType modifier, gpointer data) {
		MainWindow *main_window = static_cast<MainWindow*>(data);
		return main_window->on_key_pressed(keyval, keycode, modifier);
	}
};

}

static gboolean on_about_dialog_close_request(GtkWindow *about_dialog, gpointer) {
	gtk_widget_set_visible(GTK_WIDGET(about_dialog), false);
	return true;
}

static void on_about_dialog_action_activate(GSimpleAction*, GVariant*, gpointer data) {
	GtkAboutDialog *about_dialog = static_cast<GtkAboutDialog*>(data);
	gtk_window_present(GTK_WINDOW(about_dialog));
} 

EzAutoClicker::MainWindow::MainWindow(GtkBuilder *builder) {//BaseObjectType *gobject, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::ApplicationWindow(gobject) {
	clicker = nullptr;
	hotkey_listener = nullptr;
	sending_inputs = false;
	capturing_key_presses = false;

	application_window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "main_window"));
	gtk_window_set_title(GTK_WINDOW(application_window), Constants::APP_NAME);

	about_dialog = GTK_ABOUT_DIALOG(gtk_builder_get_object(builder, "about_dialog"));
	gtk_about_dialog_set_version(about_dialog, Constants::APP_VERSION);
	gtk_about_dialog_set_program_name(about_dialog, Constants::APP_NAME);

	GtkHeaderBar *header_bar = GTK_HEADER_BAR(gtk_builder_get_object(builder, "header_bar"));
	gtk_header_bar_set_show_title_buttons(header_bar, TRUE);
	gtk_window_set_titlebar(GTK_WINDOW(application_window), GTK_WIDGET(header_bar));

	start_button = GTK_BUTTON(gtk_builder_get_object(builder, "start_button"));
	stop_button = GTK_BUTTON(gtk_builder_get_object(builder, "stop_button"));
	hotkey_button = GTK_BUTTON(gtk_builder_get_object(builder, "hotkey_button"));

	milliseconds_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "milliseconds_entry"));
	seconds_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "seconds_entry"));
	minutes_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "minutes_entry"));
	hours_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "hours_entry"));

	mouse_button_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "mouse_button_dropdown"));
	click_type_dropdown = GTK_DROP_DOWN(gtk_builder_get_object(builder, "click_type_dropdown"));

	current_location_toggle = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "current_location_toggle"));
	pick_location_toggle = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "pick_location_toggle"));
	x_position_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "x_position_entry"));
	y_position_entry = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "y_position_entry"));

	repeal_until_stopped_toggle = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "repeal_until_stopped_toggle"));
	click_repeat_counter = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "click_repeat_counter"));
	repeat_toggle = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "repeat_toggle"));

	threaded_clicker.on_autostopping.connect([this]{
		set_sending_inputs(false);
	});

	update_options();
	connect_signals();
}

EzAutoClicker::MainWindow::~MainWindow() {
	gtk_window_destroy(GTK_WINDOW(application_window));
}

void EzAutoClicker::MainWindow::update_mouse_button_option() {
	guint pos = gtk_drop_down_get_selected(mouse_button_dropdown);

	if (pos == 0)
		options.click.button = Input::Click::MouseButton::LEFT;
	else if (pos == 1)
		options.click.button = Input::Click::MouseButton::MIDDLE;
	else
		options.click.button = Input::Click::MouseButton::RIGHT;
}

void EzAutoClicker::MainWindow::update_click_type_option() {
	guint pos = gtk_drop_down_get_selected(click_type_dropdown);

	if (pos == 0)
		options.click_type = Input::ClickerOptions::ClickType::SINGLE;
	else
		options.click_type = Input::ClickerOptions::ClickType::DOUBLE;
}

void EzAutoClicker::MainWindow::update_repeating_mode() {
	if (gtk_check_button_get_active(repeal_until_stopped_toggle))
		options.repeat_mode = Input::ClickerOptions::RepeatMode::UNTIL_STOPPED;
	else
		options.repeat_mode = Input::ClickerOptions::RepeatMode::REPEAT_X_TIMES;
}

void EzAutoClicker::MainWindow::update_options() {
	options.interval_ms = gtk_spin_button_get_value_as_int(milliseconds_entry);
	options.interval_s = gtk_spin_button_get_value_as_int(seconds_entry);
	options.interval_m = gtk_spin_button_get_value_as_int(minutes_entry);
	options.interval_h = gtk_spin_button_get_value_as_int(hours_entry);
	
	update_mouse_button_option();
	update_click_type_option();
	update_repeating_mode();

	options.click.set_mouse_location = gtk_check_button_get_active(pick_location_toggle);
	options.click.position.x = gtk_spin_button_get_value_as_int(x_position_entry);
	options.click.position.y = gtk_spin_button_get_value_as_int(y_position_entry);
	options.repeat_count = gtk_spin_button_get_value_as_int(click_repeat_counter);
	options.add_random_offset = false;
	options.random_offset_ms = 0;

	GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(application_window));
	GdkDisplay *display = surface ? gdk_surface_get_display(surface) : nullptr;
	
	if (!display)
		return;

	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display, surface);

	if (!monitor)
		return;

	GdkRectangle geometry;
	gdk_monitor_get_geometry(monitor, &geometry);

	options.click.screen_size.width = geometry.width;
	options.click.screen_size.height = geometry.height;
}

void EzAutoClicker::MainWindow::connect_signals() {
	GtkEventController *controller = gtk_event_controller_key_new();
	GSimpleAction *about_action = g_simple_action_new("about", nullptr);

	g_signal_connect(start_button, "clicked", G_CALLBACK(&MainWindowSignalHandler::on_start_button_clicked), this);
	g_signal_connect(about_action, "activate", G_CALLBACK(on_about_dialog_action_activate), about_dialog);
	g_signal_connect(stop_button, "clicked", G_CALLBACK(&MainWindowSignalHandler::on_stop_button_clicked), this);
	g_signal_connect(hotkey_button, "clicked", G_CALLBACK(&MainWindowSignalHandler::on_hotkey_button_clicked), this);
	g_signal_connect(controller, "key_pressed", G_CALLBACK(&MainWindowSignalHandler::on_key_pressed), this);
	g_signal_connect(about_dialog, "close_request", G_CALLBACK(on_about_dialog_close_request), nullptr);

	g_action_map_add_action(G_ACTION_MAP(application_window), G_ACTION(about_action));
	gtk_widget_add_controller(GTK_WIDGET(application_window), controller);
}

/**
 * @brief Checks if the modifier is equal to GDK_NO_MODIFIER_MASK.
 * @details Use this function over directly comparing (==) the modifier with
 * GDK_NO_MODIFIER_MASK as that enum was only added in gtkmm 4.14.
 * @return true if the modifier equals to
 * GDK_NO_MODIFIER_MASK (>= gtkmm 4.14)
 * or 0 (< gtkmm 4.14), false otherwise.
 */
static constexpr bool is_gdk_modifier_no_modifier(GdkModifierType modifier) {
	#if GDK_MAJOR_VERSION >= 4 && GDK_MINOR_VERSION >= 14
	return modifier == GDK_NO_MODIFIER_MASK;
	#else
	return static_cast<int>(modifier) == 0;
	#endif
}

/**
 * @brief Converts a Gdk::ModifierType into a hotkey modifier.
 * @note Unsupported Modifiers will be lost after conversion.
 */
static constexpr EzAutoClicker::Input::Hotkey::Modifier gdk_modifier_to_hotkey_modifier(GdkModifierType modifier) {
	using Modifier = EzAutoClicker::Input::Hotkey::Modifier;
	if (is_gdk_modifier_no_modifier(modifier))
		return Modifier::NONE;

	Modifier mod = Modifier::NONE;

	if (static_cast<int>(modifier & GDK_CONTROL_MASK) != 0)
		mod = mod | Modifier::CTRL;

	if (static_cast<int>(modifier & GDK_SHIFT_MASK) != 0)
		mod = mod | Modifier::SHIFT;
	
	if (static_cast<int>(modifier & GDK_ALT_MASK) != 0)
		mod = mod | Modifier::ALT;

	if (static_cast<int>(modifier & GDK_SUPER_MASK) != 0)
		mod = mod | Modifier::SUPER;

	return mod;
}

/**
 * @brief Enum used to determine if a keyval is a modifier
 * and should be shown to the user while setting a hotkey.
 */
enum class KeyValType {
	KEY, ///< This valid key for the hotkey. Proceed to setting the hotkey.
	MODIFIER, ///< Show modifier when setting hotkey. Wait until the user presses another key.
	MODIFIER_NO_SHOW, ///< Don't show modifier when setting hotkey. Don't use a modifier.
};

/**
 * @brief Determines what type a keyval is.
 * @see @e KeyValType
 */
static constexpr KeyValType get_keyval_into_from_keyval(guint keyval) {
	switch (keyval) {
		case GDK_KEY_Control_L:
			return KeyValType::MODIFIER;
		case GDK_KEY_Control_R:
			return KeyValType::MODIFIER;
		case GDK_KEY_Shift_L:
			return KeyValType::MODIFIER;
		case GDK_KEY_Shift_R:
			return KeyValType::MODIFIER;
		case GDK_KEY_Caps_Lock:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Shift_Lock:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Meta_L:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Meta_R:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Alt_L:
			return KeyValType::MODIFIER;
		case GDK_KEY_Alt_R:
			return KeyValType::MODIFIER;
		case GDK_KEY_Super_L:
			return KeyValType::MODIFIER;
		case GDK_KEY_Super_R:
			return KeyValType::MODIFIER;
		case GDK_KEY_Hyper_L:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Hyper_R:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_Tab:
			return KeyValType::MODIFIER_NO_SHOW;
		// Fn key on laptops.
		case GDK_KEY_WakeUp:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_ISO_Lock:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_ISO_Level2_Latch:
			return KeyValType::MODIFIER_NO_SHOW;
		case GDK_KEY_ISO_Level3_Shift:
			return KeyValType::MODIFIER_NO_SHOW;
	}

	return KeyValType::KEY;
}

/**
 * @note Converts a Gdk keycode into a hotkey key.
 */
static constexpr unsigned gdk_keycode_to_hotkey_key(guint keycode) {
	return keycode - 8;
}

bool EzAutoClicker::MainWindow::on_key_pressed(guint keyval, guint keycode, GdkModifierType state) {	
	if (!capturing_key_presses)
		return false;

	if (keyval == GDK_KEY_Escape) {
		set_capturing_keypresses(false);
		return true;
	}

	char *c_key_description = gtk_accelerator_get_label(keyval, state);
	auto key_description = std::unique_ptr<char, GDeleter>(c_key_description);
	KeyValType type = get_keyval_into_from_keyval(keyval);
	if (type != KeyValType::KEY) {
		if (type == KeyValType::MODIFIER)
			gtk_button_set_label(hotkey_button, key_description.get());
		return true;
	}

	Input::Hotkey hotkey;
	hotkey.key = gdk_keycode_to_hotkey_key(keycode);
	hotkey.modifiers = gdk_modifier_to_hotkey_modifier(state);

	if (hotkey_listener)
		hotkey_listener->register_hotkey(hotkey);

	hotkey_name = std::move(key_description);
	set_capturing_keypresses(false);
	return true;
}

void EzAutoClicker::MainWindow::start_clicker() {
	if (sending_inputs || !clicker)
		return;

	set_sending_inputs(true);
	update_options();
	threaded_clicker.start(clicker.get(), options);
}

void EzAutoClicker::MainWindow::stop_clicker() {
	if (!sending_inputs)
		return;

	set_sending_inputs(false);
	threaded_clicker.stop();
}

void EzAutoClicker::MainWindow::set_sending_inputs(bool p_sending_inputs) {
	sending_inputs = p_sending_inputs;
	gtk_widget_set_sensitive(GTK_WIDGET(start_button), !p_sending_inputs);
	gtk_widget_set_sensitive(GTK_WIDGET(stop_button), p_sending_inputs);
}

void EzAutoClicker::MainWindow::set_capturing_keypresses(bool p_capturing_key_presses) {
	capturing_key_presses = p_capturing_key_presses;
	if (p_capturing_key_presses)
		gtk_button_set_label(hotkey_button, _("Press a key..."));
	else
		gtk_button_set_label(hotkey_button, hotkey_name.get());
}

void EzAutoClicker::MainWindow::toggle_clicker() {
	if (threaded_clicker.is_running())
		stop_clicker();
	else
		start_clicker();
}

void EzAutoClicker::MainWindow::set_clicker(std::unique_ptr<Input::Clicker> &&p_clicker) {
	if (p_clicker)
		clicker = std::move(p_clicker);
}

void EzAutoClicker::MainWindow::set_hotkey_listener(std::unique_ptr<Input::HotkeyListener> &&p_hotkey_listener) {
	if (!p_hotkey_listener)
		return;

	if (hotkey_listener)
		hotkey_listener->stop_reading();

	hotkey_listener = std::move(p_hotkey_listener);
	hotkey_listener->signal_pressed.connect([this] {
		if (!capturing_key_presses)
			toggle_clicker();
	});

	auto hotkey = Input::Hotkey{.key = KEY_F5, .modifiers = Input::Hotkey::Modifier::NONE};
	char *key_description = gtk_accelerator_get_label(GDK_KEY_F5, static_cast<GdkModifierType>(0));

	hotkey_name = std::unique_ptr<char, GDeleter>(key_description);
	gtk_button_set_label(hotkey_button, hotkey_name.get());
	hotkey_listener->register_hotkey(hotkey);
}

void EzAutoClicker::MainWindow::set_interval_ms(std::chrono::milliseconds interval) {
	gtk_spin_button_set_value(milliseconds_entry, interval.count());
}

void EzAutoClicker::MainWindow::set_interval_s(std::chrono::seconds interval) {
	gtk_spin_button_set_value(seconds_entry, interval.count());
}

void EzAutoClicker::MainWindow::set_interval_m(std::chrono::minutes interval) {
	gtk_spin_button_set_value(minutes_entry, interval.count());
}

void EzAutoClicker::MainWindow::set_interval_h(std::chrono::hours interval) {
	gtk_spin_button_set_value(hours_entry, interval.count());
}

void EzAutoClicker::MainWindow::set_mouse_button(Input::Click::MouseButton mouse_button) {
	if (mouse_button == Input::Click::MouseButton::LEFT)
		gtk_drop_down_set_selected(mouse_button_dropdown, 0);
	else if (mouse_button == Input::Click::MouseButton::MIDDLE)
		gtk_drop_down_set_selected(mouse_button_dropdown, 1);
	else
		gtk_drop_down_set_selected(mouse_button_dropdown, 2);
}

void EzAutoClicker::MainWindow::set_click_type(Input::ClickerOptions::ClickType click_type) {
	if (click_type == Input::ClickerOptions::ClickType::SINGLE)
		gtk_drop_down_set_selected(click_type_dropdown, 0);
	else
		gtk_drop_down_set_selected(click_type_dropdown, 1);
}

void EzAutoClicker::MainWindow::set_repeat_mode(Input::ClickerOptions::RepeatMode repeat_mode) {
	gtk_check_button_set_active(repeal_until_stopped_toggle, repeat_mode == Input::ClickerOptions::RepeatMode::UNTIL_STOPPED);
}

void EzAutoClicker::MainWindow::set_use_mouse_offset(bool use_mouse_offset) {
	gtk_check_button_set_active(pick_location_toggle, use_mouse_offset);
}

void EzAutoClicker::MainWindow::set_mouse_x_position(int x) {
	gtk_spin_button_set_value(x_position_entry, x);
}

void EzAutoClicker::MainWindow::set_mouse_y_position(int y) {
	gtk_spin_button_set_value(y_position_entry, y);
}

void EzAutoClicker::MainWindow::set_repeat_count(long long repeat_count) {
	gtk_spin_button_set_value(click_repeat_counter, repeat_count);
}

void EzAutoClicker::MainWindow::set_random_offset(std::chrono::milliseconds) {
}