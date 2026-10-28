#include <src/window.hpp>

#include <linux/input-event-codes.h>

#include <src/input/hotkey_listener_evdev.hpp>
#include <src/input/input_player_x11.hpp>
#include <src/input/input_player_uinput.hpp>

EzAutoClicker::MainWindow::MainWindow(BaseObjectType *gobject, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::ApplicationWindow(gobject) {
	hotkey_listener = nullptr;
	sending_inputs = false;

	start_button = builder->get_widget<Gtk::Button>("start_button");
	stop_button = builder->get_widget<Gtk::Button>("stop_button");

	milliseconds_entry = builder->get_widget<Gtk::SpinButton>("milliseconds_entry");
	seconds_entry = builder->get_widget<Gtk::SpinButton>("seconds_entry");
	minutes_entry = builder->get_widget<Gtk::SpinButton>("minutes_entry");
	hours_entry = builder->get_widget<Gtk::SpinButton>("hours_entry");

	mouse_button_dropdown = builder->get_widget<Gtk::DropDown>("mouse_button_dropdown");
	click_type_dropdown = builder->get_widget<Gtk::DropDown>("click_type_dropdown");

	current_location_toggle = builder->get_widget<Gtk::CheckButton>("current_location_toggle");
	pick_location_toggle = builder->get_widget<Gtk::CheckButton>("pick_location_toggle");
	pick_location_button = builder->get_widget<Gtk::Button>("pick_location_button");
	x_position_entry = builder->get_widget<Gtk::SpinButton>("x_position_entry");
	y_position_entry = builder->get_widget<Gtk::SpinButton>("y_position_entry");

	repeal_until_stopped_toggle = builder->get_widget<Gtk::CheckButton>("repeal_until_stopped_toggle");
	click_repeat_counter = builder->get_widget<Gtk::SpinButton>("click_repeat_counter");
	repeat_toggle = builder->get_widget<Gtk::CheckButton>("repeat_toggle");

	threaded_input.on_autostopping.connect([this]{
		set_sending_inputs(false);
	});

	update_options();
	connect_signals();
}

void EzAutoClicker::MainWindow::update_mouse_button_option() {
	guint pos = mouse_button_dropdown->get_selected();

	if (pos == 0)
		options.input.button = Input::MOUSE_BUTTON::LEFT;
	else if (pos == 1)
		options.input.button = Input::MOUSE_BUTTON::MIDDLE;
	else
		options.input.button = Input::MOUSE_BUTTON::RIGHT;
}

void EzAutoClicker::MainWindow::update_click_type_option() {
	guint pos = click_type_dropdown->get_selected();

	if (pos == 0)
		options.click_type = InputPlayerOptions::ClickType::SINGLE;
	else
		options.click_type = InputPlayerOptions::ClickType::DOUBLE;
}

void EzAutoClicker::MainWindow::on_pick_location_button_pressed() {
	// Intentionally left blank
}

void EzAutoClicker::MainWindow::update_repeating_mode() {
	if (repeal_until_stopped_toggle->get_active())
		options.repeat_mode = InputPlayerOptions::RepeatMode::UNTIL_STOPPED;
	else
		options.repeat_mode = InputPlayerOptions::RepeatMode::REPEAT_X_TIMES;
}

void EzAutoClicker::MainWindow::update_options() {
	options.interval_ms = milliseconds_entry->get_value_as_int();
	options.interval_s = seconds_entry->get_value_as_int();
	options.interval_m = minutes_entry->get_value_as_int();
	options.interval_h = hours_entry->get_value_as_int();
	
	update_mouse_button_option();
	update_click_type_option();
	update_repeating_mode();

	options.input.set_mouse_location = pick_location_toggle->get_active();
	options.input.x_position = x_position_entry->get_value_as_int();
	options.input.y_position = y_position_entry->get_value_as_int();
	options.repeat_count = click_repeat_counter->get_value_as_int();
}

void EzAutoClicker::MainWindow::connect_signals() {
	start_button->signal_clicked().connect([this]{
		start_clicker();
	});

	stop_button->signal_clicked().connect([this]{
		stop_clicker();
	});

	pick_location_button->signal_clicked().connect(
		sigc::mem_fun(*this,
		&MainWindow::on_pick_location_button_pressed));
}

void EzAutoClicker::MainWindow::start_clicker() {
	if (sending_inputs)
		return;

	set_sending_inputs(true);
	update_options();
	threaded_input.start(input_player, options);
}

void EzAutoClicker::MainWindow::stop_clicker() {
	if (!sending_inputs)
		return;

	set_sending_inputs(false);
	threaded_input.stop();
}

void EzAutoClicker::MainWindow::set_sending_inputs(bool p_sending_inputs) {
	sending_inputs = p_sending_inputs;
	start_button->set_sensitive(!p_sending_inputs);
	stop_button->set_sensitive(p_sending_inputs);
}

void EzAutoClicker::MainWindow::toggle_clicker() {
	if (threaded_input.is_running())
		stop_clicker();
	else
		start_clicker();
}

void EzAutoClicker::MainWindow::set_input_player(InputPlayer *p_input_player) {
	input_player = p_input_player;
}

void EzAutoClicker::MainWindow::set_hotkey_listener(HotkeyListener *p_hotkey_listener) {
	if (hotkey_listener) {
		hotkey_listener->stop_reading();
		hotkey_listener->signal_pressed.clear();
	}

	hotkey_listener = p_hotkey_listener;
	hotkey_listener->signal_pressed.connect([this] {
		toggle_clicker();
	});
	hotkey_listener->start_reading();
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

void EzAutoClicker::MainWindow::set_mouse_button(Input::MOUSE_BUTTON mouse_button) {
	if (mouse_button == Input::MOUSE_BUTTON::LEFT)
		mouse_button_dropdown->set_selected(0);
	else if (mouse_button == Input::MOUSE_BUTTON::MIDDLE)
		mouse_button_dropdown->set_selected(1);
	else
		mouse_button_dropdown->set_selected(2);
}

void EzAutoClicker::MainWindow::set_click_type(InputPlayerOptions::ClickType click_type) {
	if (click_type == InputPlayerOptions::ClickType::SINGLE)
		click_type_dropdown->set_selected(0);
	else
		click_type_dropdown->set_selected(1);
}

void EzAutoClicker::MainWindow::set_repeat_mode(InputPlayerOptions::RepeatMode repeat_mode) {
	repeal_until_stopped_toggle->set_active(repeat_mode == InputPlayerOptions::RepeatMode::UNTIL_STOPPED);
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