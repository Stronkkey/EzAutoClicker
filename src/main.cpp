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
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>

#include <print>

#include <src/app.hpp>
#include <src/input/hotkey_listener_factory.hpp>
#include <src/input/input_player_factory.hpp>

void throw_error_and_quit(const std::string_view &error_message) {
	std::print("{0}", error_message);
	exit(EXIT_FAILURE);
}

int get_running_user_id() {
	const char *sudo_env = getenv("SUDO_UID");
	const char *pkexec_env = getenv("PKEXEC_UID");

	if (pkexec_env != NULL) {
		try {
			return std::stoi(pkexec_env);
		} catch (const std::invalid_argument&) {
			return getuid();
		}
	}

	if (sudo_env != NULL) {
		try {
			return std::stoi(sudo_env);
		} catch (const std::invalid_argument&) {
			return getuid();
		}
	}

	return getuid();
}

bool is_running_as_root() {
	return geteuid() == 0;
}

void drop_root_privileges() {
	if (geteuid() != 0)
		return;

	int running_user_id = get_running_user_id();
	if (running_user_id == 0)
		throw std::runtime_error("Couldn't find suitable user to drop privileges to. Try running the program with SUDO_UID=$(id -u).");

	const passwd *pw = getpwuid(running_user_id);
	if (pw == nullptr) {
		throw std::runtime_error(std::format("Failed to find user with the uid {0}.", running_user_id));
	}

	if (initgroups(pw->pw_name, pw->pw_gid) != 0)
		throw std::runtime_error("Failed to remove supplementary groups.");

	if (setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) != 0)
		throw std::runtime_error("Failed to set gid.");

	if (setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid) != 0)
		throw std::runtime_error("Failed to set uid.");

	if (setuid(0) != -1 || setgid(0) != -1)
		throw std::runtime_error("Was able to regain root.");

	if (getuid() != pw->pw_uid || geteuid() != pw->pw_uid ||
		getgid() != pw->pw_gid || getegid() != pw->pw_gid)
		throw std::runtime_error("uid/gid mismatch after privilege drop");
}


int main(int argc, char *argv[]) {
	std::unique_ptr<EzAutoClicker::InputPlayer> input_player = nullptr;
	std::unique_ptr<EzAutoClicker::HotkeyListener> hotkey_listener = nullptr;

	if (is_running_as_root()) {
		auto input_player_uinput= EzAutoClicker::InputPlayerFactory::create_input_player_uinput();

		if (input_player_uinput)
			input_player = std::move(input_player_uinput);
		hotkey_listener = EzAutoClicker::HotkeyListenerFactory::create_hotkey_listener_evdev();
		drop_root_privileges();
	}

	if (!input_player)
		input_player = EzAutoClicker::InputPlayerFactory::create_compatible_input_player();

	if (!hotkey_listener)
		hotkey_listener = EzAutoClicker::HotkeyListenerFactory::create_compatible_hotkey_listener();

	auto app = EzAutoClicker::EzAutoClickerApp::create(
		input_player.get(),
		hotkey_listener.get());
	return app->run(argc, argv);
}