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
#pragma once

#include <fcntl.h>
#include <pwd.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace EzAutoClicker {

namespace Process {

/**
 * @brief Gets the ppid of a running process.
 * @param pid The process for which to find the ppid.
 * @returns The ppid if successful or std::nullopt if
 * the ppid couldn't be found.
 */
std::optional<pid_t> get_ppid_of_process(pid_t pid);

/**
 * @brief Gets the euid that a running process is using.
 * @param pid The process for which to find the euid.
 * @returns The euid if successful or std::nullopt if
 * the euid couldn't be found.
 */
std::optional<uid_t> get_euid_of_process(pid_t pid);

/**
 * @brief Gets the environ that a program is running with.
 * @param pid The process for which to find the environ.
 * @returns A hash table of the process environment variables or std::nullopt
 * if there was error reading the environ from the process.
 * The key corresponds to the the name of the environment variable and the value
 * corresponds to the value of said environment variable.
 */
std::optional<std::unordered_map<std::string, std::string>> get_environ_for_process(pid_t pid);

/**
 * @brief Looks for the first parent process that is ran as the target uid.
 * @param target_uid the uid to find.
 * @returns the pid of the first parent process that is ran as the
 * target uid or if std::nullopt if no suitable process couldn't be found.
 */
std::optional<pid_t> get_process_running_as_uid(uid_t target_uid);

/**
 * @brief Sets the specified environment variables from the user's environment.
 * @param env The environment variables to read from the user.
 */
void set_env_from_user_env(const std::unordered_set<std::string> &env);

/**
 * @brief Unsets all environment variables except the ones specified by a whitelist.
 * @param whitelist The whitelist of environment variables to not unset.
 * @note This method may also remove some environment variables required by other
 * methods in this namespace. Namely SUDO_UID and PKEXEC_UID. It's recommended to
 * include these these variable names inside the whitelist to avoid bugs.
 */
void clear_environ_with_whitelist(const std::unordered_set<std::string> &whitelist);

/**
 * @brief Checks if the program is running as root.
 * @details Gets the effective uid of the program and checks if it is equal to 0.
 * @returns true if the program is running as root, false otherwise.
 */
bool is_running_as_root();

/**
 * @brief Gets the uid of the user who is actually running the program.
 * @details Checks for the environment variables set by pkexec or sudo which contain
 * the uid of the running user, uses getuid() if the environment variables aren't set
 * or can't be parsed.
 * @return The uid of the running user, or the real uid of the program.
 */
uid_t get_running_user_id();

/**
 * @brief Permanently drops all root privileges so they can't be restored again.
 * @throw std::runtime_error if able to restore root privileges after dropping them.
 * @see https://web.archive.org/web/20241226230125/https://unix.stackexchange.com/a/21290
 */
void drop_root_privileges();

}

}