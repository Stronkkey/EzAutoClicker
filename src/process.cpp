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
#include <ranges>
#include <src/process.hpp>

#include <grp.h>
#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>

std::optional<pid_t> EzAutoClicker::Process::get_ppid_of_process(pid_t pid) {
	std::filesystem::path path = std::format("/proc/{0}/stat", std::to_string(pid));
	auto file = std::ifstream(path);

	if (!file)
		return std::nullopt;

	std::string file_content = {std::istreambuf_iterator<char>(file), {}};
	size_t pos = file_content.rfind(')');
	auto fields = std::istringstream(file_content.substr(pos + 1));
	std::string state = {};
	pid_t ppid = 0;

	if (!(fields >> state >> ppid))
		return std::nullopt;

	return ppid;
}

std::optional<uid_t> EzAutoClicker::Process::get_euid_of_process(pid_t pid) {
	struct stat attr;
	std::filesystem::path path = std::format("/proc/{0}", pid);

	if (stat(path.c_str(), &attr) == -1)
		return std::nullopt;
	return attr.st_uid;
}

std::optional<std::unordered_map<std::string, std::string>> EzAutoClicker::Process::get_environ_for_process(pid_t pid) {
	auto file = std::ifstream(std::format("/proc/{0}/environ", pid));

	if (!file)
		return std::nullopt;

	std::string file_content = {std::istreambuf_iterator<char>(file), {}};
	std::unordered_map<std::string, std::string> process_environ = {};
	constexpr const char delimiter[1] = {'\0'};

	for (const auto &entry : std::views::split(file_content, delimiter)) {
		auto view = std::string_view(entry);
		size_t pos = view.find('=');
		if (pos != std::string_view::npos)
			process_environ.insert_or_assign(
				std::string(view.substr(0, pos)),
				std::string(view.substr(pos + 1)));
	}

	return process_environ;
}

std::optional<pid_t> EzAutoClicker::Process::get_process_running_as_uid(uid_t target_uid) {
	uid_t uid = 0;
	pid_t ppid = getpid();

	while (uid != target_uid && ppid > 1) {
		ppid = get_ppid_of_process(ppid).value_or(0);
		uid = get_euid_of_process(ppid).value_or(0);
	} 

	if (uid != target_uid)
		return std::nullopt;
	return ppid;
}

void EzAutoClicker::Process::set_env_from_user_env(const std::unordered_set<std::string> &env) {
	std::optional<pid_t> user_process = get_process_running_as_uid(get_running_user_id());

	if (!user_process)
		return;

	std::unordered_map<std::string, std::string> process_environ = get_environ_for_process(*user_process).value_or({});
	for (const auto &var : env) {
		auto it = process_environ.find(var);
		if (it != process_environ.end())
			setenv(it->first.c_str(), it->second.c_str(), 1);
	}	
}

static void for_env_var(char *env_variable, const std::unordered_set<std::string> &whitelist) {
	std::string_view current = env_variable;
	size_t pos = current.find('=');

	if (pos == std::string_view::npos)
		return;

	auto env_name = std::string(current.substr(0, pos));
	if (whitelist.contains(env_name)) {
		printf("skipped %s\n", env_name.c_str());
		return;
	}

	printf("unset %s\n", env_name.c_str());
	unsetenv(env_name.c_str());
}

void EzAutoClicker::Process::clear_environ_with_whitelist(const std::unordered_set<std::string> &whitelist) {
	if (!environ || (*environ) == nullptr)
		return;

	for(char **env_variable = environ; *env_variable; env_variable++)
		for_env_var(*env_variable, whitelist);
}

bool EzAutoClicker::Process::is_running_as_root() {
	return geteuid() == 0;
}

uid_t EzAutoClicker::Process::get_running_user_id() {
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

void EzAutoClicker::Process::drop_root_privileges() {
	if (!is_running_as_root())
		return;

	uid_t running_user_id = get_running_user_id();
	const bool found_running_user = running_user_id != 0;
	if (!found_running_user)
		throw std::runtime_error("Couldn't find suitable user to drop privileges to. Try running the program with SUDO_UID=$(id -u).");

	const passwd *pw = getpwuid(running_user_id);
	if (!pw)
		throw std::runtime_error(std::format("Failed to find user with the uid {0}.", running_user_id));

	const bool set_initgroups_failed = initgroups(pw->pw_name, pw->pw_gid) != 0;
	if (set_initgroups_failed)
		throw std::runtime_error("Failed to remove supplementary groups.");

	const bool setresgid_failed = setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) != 0;
	if (setresgid_failed)
		throw std::runtime_error("Failed to set gid.");

	const bool setresuid_failed = setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid) != 0;
	if (setresuid_failed)
		throw std::runtime_error("Failed to set uid.");

	const bool privileges_escalated = setuid(0) != -1 || setgid(0) != -1;
	if (privileges_escalated)
		throw std::runtime_error("Was able to regain root.");

	const bool uid_gid_mismatch = getuid() != pw->pw_uid || geteuid() != pw->pw_uid ||
		getgid() != pw->pw_gid || getegid() != pw->pw_gid;
	if (uid_gid_mismatch)
		throw std::runtime_error("uid/gid mismatch after privilege drop");
}