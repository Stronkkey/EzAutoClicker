#pragma once

#include <gtkmm.h>

namespace EzAutoClicker {

class HotkeyListener;
class InputPlayer;
class MainWindow;

class EzAutoClickerApp : public Gtk::Application {
private:
	int on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &options) override;
	void on_toggle();

	MainWindow *main_window;
	InputPlayer *input_player;
	HotkeyListener *hotkey_listener;
protected:
	EzAutoClickerApp();

	void on_activate() override;
public:
	static Glib::RefPtr<EzAutoClickerApp> create(
		InputPlayer *input_player = nullptr,
		HotkeyListener *hotkey_listener = nullptr);
};

}