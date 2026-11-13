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
#include <src/process.hpp>

#if EZAUTOCLICKER_X11_INPUT_ENABLED

#include <X11/Xlib.h>

#endif

int main(int argc, char *argv[]) {
	std::unique_ptr<EzAutoClicker::Input::Clicker> clicker = nullptr;
	std::unique_ptr<EzAutoClicker::Input::HotkeyListener> hotkey_listener = nullptr;

	#if EZAUTOCLICKER_ALLOW_RUNNING_AS_ROOT
	bool allow_root = false;
	for (int i = 1; i < argc; i++) {
		std::string_view argument = argv[i];
		if (argument == EzAutoClicker::Constants::ALLOW_ROOT_FLAG)
			allow_root = true;
	}
	#endif

	if (EzAutoClicker::Process::is_running_as_root()) {
		#if EZAUTOCLICKER_ALLOW_RUNNING_AS_ROOT
		if (!allow_root) {
			g_printerr("%s\n", EzAutoClicker::Constants::ALLOW_ROOT_ERROR);
			return EXIT_FAILURE;
		}
		

		// We intentionally only try to create a uinput clicker with root privileges
		// as that is the only clicker that can make use of them. Other clickers
		// shouldn't be created with root privileges as it can create security vulnerabilities
		// like privilege escalation attacks. Also applies to hotkey listeners as well.
		// Note: running any gui application as root is not safe, this doesn't change this.		
		const std::unordered_set<std::string> required_variables = {
			"DBUS_SESSION_BUS_ADDRESS",
			"DISPLAY",
			"GTK_THEME",
			"HOME",
			"LANG",
			"PATH",
			"PKEXEC_UID",
			"SUDO_GID",
			"SUDO_HOME",
			"SUDO_UID",
			"WAYLAND_DISPLAY",
			"XAUTHORITY",
			"XDG_RUNTIME_DIR",
			"XDG_CURRENT_DESKTOP",
		};

		EzAutoClicker::Process::clear_environ_with_whitelist(required_variables);
		EzAutoClicker::Process::set_env_from_user_env(required_variables);

		auto clicker_uinput= EzAutoClicker::Input::ClickerFactory::create_clicker_uinput();
		if (clicker_uinput && clicker_uinput->is_connected())
			clicker = std::move(clicker_uinput);
		
		auto hotkey_listener_evdev = EzAutoClicker::Input::HotkeyListenerFactory::create_hotkey_listener_evdev();
		if (hotkey_listener_evdev && hotkey_listener_evdev->is_reading())
			hotkey_listener = std::move(hotkey_listener_evdev);

		EzAutoClicker::Process::drop_root_privileges();
		#else
		g_printerr("%s\n", EzAutoClicker::Constants::UNALLOWED_ROOT_ERROR.data());
		return EXIT_FAILURE;
		#endif
	}

	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	XInitThreads();
	#endif

	auto app = EzAutoClicker::EzAutoClickerApp::create(
		std::move(clicker),
		std::move(hotkey_listener));
	return app->run(argc, argv);
}