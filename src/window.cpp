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

#include <glibmm/i18n.h>

#include <config.hpp>

EzAutoClicker::MainWindow::MainWindow(BaseObjectType *gobject, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::ApplicationWindow(gobject) {
	clicker = nullptr;
	hotkey_listener = nullptr;
	sending_inputs = false;
	capturing_key_presses = false;
	hotkey_listener_connection = {};

	about_dialog = builder->get_widget<Gtk::AboutDialog>("about_dialog");
	about_dialog->set_version(Constants::APP_VERSION);
	about_dialog->set_program_name(Constants::APP_NAME);
	about_dialog->signal_close_request().connect([this]{
		about_dialog->hide();
		return true;
	}, false);

	auto header_bar = builder->get_widget<Gtk::HeaderBar>("header_bar");
	set_titlebar(*header_bar);

	auto about_action = Gio::SimpleAction::create("about");
	about_action->signal_activate().connect([this](const Glib::VariantBase&){
		about_dialog->present();
	});

	add_action(about_action);

	start_button = builder->get_widget<Gtk::Button>("start_button");
	stop_button = builder->get_widget<Gtk::Button>("stop_button");
	hotkey_button = builder->get_widget<Gtk::Button>("hotkey_button");

	milliseconds_entry = builder->get_widget<Gtk::SpinButton>("milliseconds_entry");
	seconds_entry = builder->get_widget<Gtk::SpinButton>("seconds_entry");
	minutes_entry = builder->get_widget<Gtk::SpinButton>("minutes_entry");
	hours_entry = builder->get_widget<Gtk::SpinButton>("hours_entry");

	mouse_button_dropdown = builder->get_widget<Gtk::DropDown>("mouse_button_dropdown");
	click_type_dropdown = builder->get_widget<Gtk::DropDown>("click_type_dropdown");

	current_location_toggle = builder->get_widget<Gtk::CheckButton>("current_location_toggle");
	pick_location_toggle = builder->get_widget<Gtk::CheckButton>("pick_location_toggle");
	x_position_entry = builder->get_widget<Gtk::SpinButton>("x_position_entry");
	y_position_entry = builder->get_widget<Gtk::SpinButton>("y_position_entry");

	repeal_until_stopped_toggle = builder->get_widget<Gtk::CheckButton>("repeal_until_stopped_toggle");
	click_repeat_counter = builder->get_widget<Gtk::SpinButton>("click_repeat_counter");
	repeat_toggle = builder->get_widget<Gtk::CheckButton>("repeat_toggle");

	threaded_clicker.on_autostopping.connect([this]{
		set_sending_inputs(false);
	});

	update_options();
	connect_signals();
}

void EzAutoClicker::MainWindow::update_mouse_button_option() {
	guint pos = mouse_button_dropdown->get_selected();

	if (pos == 0)
		options.click.button = Input::Click::MouseButton::LEFT;
	else if (pos == 1)
		options.click.button = Input::Click::MouseButton::MIDDLE;
	else
		options.click.button = Input::Click::MouseButton::RIGHT;
}

void EzAutoClicker::MainWindow::update_click_type_option() {
	guint pos = click_type_dropdown->get_selected();

	if (pos == 0)
		options.click_type = Input::ClickerOptions::ClickType::SINGLE;
	else
		options.click_type = Input::ClickerOptions::ClickType::DOUBLE;
}

void EzAutoClicker::MainWindow::update_repeating_mode() {
	if (repeal_until_stopped_toggle->get_active())
		options.repeat_mode = Input::ClickerOptions::RepeatMode::UNTIL_STOPPED;
	else
		options.repeat_mode = Input::ClickerOptions::RepeatMode::REPEAT_X_TIMES;
}

void EzAutoClicker::MainWindow::update_options() {
	options.interval_ms = milliseconds_entry->get_value_as_int();
	options.interval_s = seconds_entry->get_value_as_int();
	options.interval_m = minutes_entry->get_value_as_int();
	options.interval_h = hours_entry->get_value_as_int();
	
	update_mouse_button_option();
	update_click_type_option();
	update_repeating_mode();

	options.click.set_mouse_location = pick_location_toggle->get_active();
	options.click.position.x = x_position_entry->get_value_as_int();
	options.click.position.y = y_position_entry->get_value_as_int();
	options.repeat_count = click_repeat_counter->get_value_as_int();
	options.add_random_offset = false;
	options.random_offset_ms = 0;

	auto surface = get_surface();
	auto display = get_display();

	if (!surface || !display)
		return;

	auto monitor = display->get_monitor_at_surface(surface);
	
	if (!monitor)
		return;

	Gdk::Rectangle geometry;
	monitor->get_geometry(geometry);

	options.click.screen_size.width = geometry.get_width();
	options.click.screen_size.height = geometry.get_height();
}

void EzAutoClicker::MainWindow::connect_signals() {
	start_button->signal_clicked().connect([this]{
		start_clicker();
	});

	stop_button->signal_clicked().connect([this]{
		stop_clicker();
	});

	hotkey_button->signal_clicked().connect([this] {
		set_capturing_keypresses(true);
	});

	auto controller = Gtk::EventControllerKey::create();
	controller->signal_key_pressed().connect(sigc::mem_fun(
			*this,
			&MainWindow::on_key_pressed), false);
	add_controller(controller);
}

/**
 * @brief Checks if the modifier is equal to Gdk::ModifierType::NO_MODIFIER_MASK.
 * @details Use this function over directly comparing (==) the modifier with
 * Gdk::ModifierType::NO_MODIFIER_MASK as that enum was only added in gtkmm 4.14.
 * @return true if the modifier equals to
 * Gdk::ModifierType::NO_MODIFIER_MASK (>= gtkmm 4.14)
 * or 0 (< gtkmm 4.14), false otherwise.
 */
static constexpr bool is_gdk_modifier_no_modifier(Gdk::ModifierType modifier) {
	#if GTKMM_MAJOR_VERSION >= 4 && GTKMM_MINOR_VERSION >= 14
	return modifier == Gdk::ModifierType::NO_MODIFIER_MASK;
	#else
	return static_cast<int>(modifier) == 0;
	#endif
}

/**
 * @brief Converts a Gdk::ModifierType into a hotkey modifier.
 * @note Unsupported Modifiers will be lost after conversion.
 */
static constexpr EzAutoClicker::Input::Hotkey::Modifier gdk_modifier_to_hotkey_modifier(Gdk::ModifierType modifier) {
	using Modifier = EzAutoClicker::Input::Hotkey::Modifier;
	if (is_gdk_modifier_no_modifier(modifier))
		return Modifier::NONE;

	Modifier mod = Modifier::NONE;

	if (static_cast<int>(modifier & Gdk::ModifierType::CONTROL_MASK) != 0)
		mod = mod | Modifier::CTRL;

	if (static_cast<int>(modifier & Gdk::ModifierType::SHIFT_MASK) != 0)
		mod = mod | Modifier::SHIFT;
	
	if (static_cast<int>(modifier & Gdk::ModifierType::ALT_MASK) != 0)
		mod = mod | Modifier::ALT;

	if (static_cast<int>(modifier & Gdk::ModifierType::SUPER_MASK) != 0)
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

bool EzAutoClicker::MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {	
	if (!capturing_key_presses)
		return false;

	if (keyval == GDK_KEY_Escape) {
		set_capturing_keypresses(false);
		return true;
	}

	Glib::ustring key_description = Gtk::Accelerator::get_label(keyval, state);
	KeyValType type = get_keyval_into_from_keyval(keyval);
	if (type != KeyValType::KEY) {
		if (type == KeyValType::MODIFIER)
			hotkey_button->set_label(key_description);
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
	start_button->set_sensitive(!p_sending_inputs);
	stop_button->set_sensitive(p_sending_inputs);
}

void EzAutoClicker::MainWindow::set_capturing_keypresses(bool p_capturing_key_presses) {
	capturing_key_presses = p_capturing_key_presses;
	if (p_capturing_key_presses)
		hotkey_button->set_label(_("Press a key..."));
	else
		hotkey_button->set_label(hotkey_name);
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

	if (hotkey_listener_connection.connected())
		hotkey_listener_connection.disconnect();

	hotkey_listener = std::move(p_hotkey_listener);
	auto slot = hotkey_listener->signal_pressed.connect([this] {
		if (!capturing_key_presses)
			toggle_clicker();
	});

	auto hotkey = Input::Hotkey{.key = KEY_F5,
	                            .modifiers = Input::Hotkey::Modifier::NONE};
	hotkey_name = Gtk::Accelerator::get_label(GDK_KEY_F5,
	                            static_cast<Gdk::ModifierType>(0));
	hotkey_button->set_label(hotkey_name);
	hotkey_listener->register_hotkey(hotkey);
}

void EzAutoClicker::MainWindow::set_interval_ms(std::chrono::milliseconds interval) {
	milliseconds_entry->set_value(interval.count());
}

void EzAutoClicker::MainWindow::set_interval_s(std::chrono::seconds interval) {
	seconds_entry->set_value(interval.count());
}

void EzAutoClicker::MainWindow::set_interval_m(std::chrono::minutes interval) {
	minutes_entry->set_value(interval.count());
}

void EzAutoClicker::MainWindow::set_interval_h(std::chrono::hours interval) {
	hours_entry->set_value(interval.count());
}

void EzAutoClicker::MainWindow::set_mouse_button(Input::Click::MouseButton mouse_button) {
	if (mouse_button == Input::Click::MouseButton::LEFT)
		mouse_button_dropdown->set_selected(0);
	else if (mouse_button == Input::Click::MouseButton::MIDDLE)
		mouse_button_dropdown->set_selected(1);
	else
		mouse_button_dropdown->set_selected(2);
}

void EzAutoClicker::MainWindow::set_click_type(Input::ClickerOptions::ClickType click_type) {
	if (click_type == Input::ClickerOptions::ClickType::SINGLE)
		click_type_dropdown->set_selected(0);
	else
		click_type_dropdown->set_selected(1);
}

void EzAutoClicker::MainWindow::set_repeat_mode(Input::ClickerOptions::RepeatMode repeat_mode) {
	repeal_until_stopped_toggle->set_active(repeat_mode == Input::ClickerOptions::RepeatMode::UNTIL_STOPPED);
}

void EzAutoClicker::MainWindow::set_use_mouse_offset(bool use_mouse_offset) {
	pick_location_toggle->set_active(use_mouse_offset);
}

void EzAutoClicker::MainWindow::set_mouse_x_position(int x) {
	x_position_entry->set_value(x);
}

void EzAutoClicker::MainWindow::set_mouse_y_position(int y) {
	y_position_entry->set_value(y);
}

void EzAutoClicker::MainWindow::set_repeat_count(long long repeat_count) {
	click_repeat_counter->set_value(repeat_count);
}

void EzAutoClicker::MainWindow::set_random_offset(std::chrono::milliseconds) {
}