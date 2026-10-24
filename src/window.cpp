#include "src/input/threaded_input.hpp"
#include <src/window.hpp>

#include <src/input/hotkey_listener_evdev.hpp>
#include <src/input/input_x11.hpp>
#include <src/input/input_uinput.hpp>

#include <linux/input-event-codes.h>

EzAutoClicker::MainWindow::MainWindow(BaseObjectType *gobject, const Glib::RefPtr<Gtk::Builder> &builder) : Gtk::ApplicationWindow(gobject), threaded_input() {
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

	repeal_until_stopped_toggle = builder->get_widget<Gtk::CheckButton>("repeal_until_stopped_toggle");
	click_repeat_counter = builder->get_widget<Gtk::SpinButton>("click_repeat_counter");
	repeat_toggle = builder->get_widget<Gtk::CheckButton>("repeat_toggle");

	create_gui();
}

void EzAutoClicker::MainWindow::on_mouse_button_selection_changed() {
	guint pos = mouse_button_dropdown->get_selected();

	if (pos == 0)
		threaded_input.input.button = Input::MOUSE_BUTTON::LEFT;
	else if (pos == 1)
		threaded_input.input.button = Input::MOUSE_BUTTON::MIDDLE;
	else
		threaded_input.input.button = Input::MOUSE_BUTTON::RIGHT;
}

void EzAutoClicker::MainWindow::on_click_type_selection_changed() {
	guint pos = click_type_dropdown->get_selected();
	
	if (pos == 0)
		threaded_input.options.double_inputs = false;
	else
		threaded_input.options.double_inputs = true;
}

void EzAutoClicker::MainWindow::on_repeat_button_pressed() {
	if (repeal_until_stopped_toggle->get_active())
		threaded_input.options.infinitly_repeating = true;
	else
		threaded_input.options.infinitly_repeating = false;
}

void EzAutoClicker::MainWindow::update_interval_from_entries() {
	input_interval = std::chrono::milliseconds(milliseconds_entry->get_value_as_int());
	input_interval += std::chrono::seconds(seconds_entry->get_value_as_int());
	input_interval += std::chrono::minutes(minutes_entry->get_value_as_int());
	input_interval += std::chrono::hours(hours_entry->get_value_as_int());

	if (input_interval < std::chrono::microseconds(500))
		input_interval = std::chrono::microseconds(500);
	threaded_input.options.interval = input_interval;
}

void EzAutoClicker::MainWindow::create_gui() {
	start_button->signal_clicked().connect([this]{
		start_clicker();
	});

	stop_button->signal_clicked().connect([this]{
		stop_clicker();
	});

	milliseconds_entry->signal_value_changed().connect(
		sigc::mem_fun(*this,
		&MainWindow::update_interval_from_entries));

	seconds_entry->signal_value_changed().connect(
		sigc::mem_fun(*this,
		&MainWindow::update_interval_from_entries));

	minutes_entry->signal_value_changed().connect(
		sigc::mem_fun(*this,
		&MainWindow::update_interval_from_entries));

	hours_entry->signal_value_changed().connect(
		sigc::mem_fun(*this,
		&MainWindow::update_interval_from_entries));

	mouse_button_dropdown->property_selected().signal_changed().connect(
	sigc::mem_fun(*this,
			&MainWindow::on_mouse_button_selection_changed));

	click_type_dropdown->property_selected().signal_changed().connect(
		sigc::mem_fun(*this,
		&MainWindow::on_click_type_selection_changed));

	repeat_toggle->signal_toggled().connect(
		sigc::mem_fun(*this,
		&MainWindow::on_repeat_button_pressed));

	click_repeat_counter->signal_value_changed().connect([this]{
		threaded_input.options.repeat_until = click_repeat_counter->get_value_as_int();
	});

	repeal_until_stopped_toggle->signal_toggled().connect(
		sigc::mem_fun(*this,
		&MainWindow::on_repeat_button_pressed));

	update_interval_from_entries();
}

void EzAutoClicker::MainWindow::start_clicker() {
	if (sending_inputs)
		return;

	start_button->set_sensitive(false);
	stop_button->set_sensitive(true);

	sending_inputs = true;
	threaded_input.options.interval = input_interval;
	threaded_input.start();
}

void EzAutoClicker::MainWindow::stop_clicker() {
	if (!sending_inputs)
		return;

	start_button->set_sensitive(true);
	stop_button->set_sensitive(false);

	sending_inputs = false;
	threaded_input.stop();
}

void EzAutoClicker::MainWindow::toggle_clicker() {
	if (threaded_input.is_running())
		stop_clicker();
	else
		start_clicker();
}

void EzAutoClicker::MainWindow::set_input_player(InputPlayer *p_input_player) {
	threaded_input.input_player = p_input_player;
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