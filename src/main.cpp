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

#include <src/app.hpp>
#include <src/input/clicker_factory.hpp>
#include <src/input/hotkey_listener_evdev.hpp>
#include <src/input/hotkey_listener_factory.hpp>

/**
 * @brief Gets the uid of the user who is actually running the program.
 * @details Checks for the environment variables set by pkexec or sudo which contain
 * the uid of the running user, uses getuid() if the environment variables aren't set
 * or can't be parsed.
 * @return the uid of the running user, or the real uid of the program.
 */
uid_t get_running_user_id() {
	const char *pkexec_env = getenv("PKEXEC_UID");
	const char *sudo_env = getenv("SUDO_UID");

	if (pkexec_env) {
		try {
			return std::stoul(pkexec_env);
		} catch (const std::exception&) {
		}
	}

	if (sudo_env) {
		try {
			return std::stoul(sudo_env);
		} catch (const std::exception&) {
		}
	}

	return getuid();
}

/**
 * @brief Checks if the program is running as root.
 * @details This function gets the effective uid of the program
 * and checks if it is equal to 0.
 * @returns true if the program is running as root, false otherwise.
 */
bool is_running_as_root() {
	return geteuid() == 0;
}

/**
 * @brief Permanently drops all root privileges so they can't be restored again.
 * @throw std::runtime_error if able to restore root privileges after dropping them.
 * @see https://web.archive.org/web/20241226230125/https://unix.stackexchange.com/a/21290
 */
void drop_root_privileges() {
	if (geteuid() != 0)
		return;

	uid_t running_user_id = get_running_user_id();
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
	#if EZAUTOCLICKER_X11_INPUT_ENABLED
	XInitThreads();
	#endif

	std::unique_ptr<EzAutoClicker::Input::Clicker> clicker = nullptr;
	std::unique_ptr<EzAutoClicker::Input::HotkeyListener> hotkey_listener = nullptr;

	if (is_running_as_root()) {
		// We intentionally only try to create a uinput clicker with root privileges
		// as that is the only clicker that can make use of them. Other clickers
		// shouldn't be created with root privileges as it can create security vulnerabilities
		// like privilege escalation attacks. Also applies to hotkey listeners as well.
		// Note: running any gui application as root is not safe, this doesn't change this.
		auto clicker_uinput= EzAutoClicker::Input::ClickerFactory::create_clicker_uinput();
		if (clicker_uinput && clicker_uinput->is_connected())
			clicker = std::move(clicker_uinput);
		
		auto hotkey_listener_evdev = EzAutoClicker::Input::HotkeyListenerFactory::create_hotkey_listener_evdev();
		if (hotkey_listener_evdev && hotkey_listener_evdev->is_reading())
			hotkey_listener = std::move(hotkey_listener_evdev);

		drop_root_privileges();
	}

	auto app = EzAutoClicker::EzAutoClickerApp::create(
		std::move(clicker),
		std::move(hotkey_listener));
	return app->run(argc, argv);
}