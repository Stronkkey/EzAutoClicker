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

namespace EzAutoClicker {

namespace Constants {

constexpr const char *APP_ID = "io.github.Stronkkey.EzAutoClicker";

constexpr const char *APP_NAME = "EzAutoClicker";
constexpr const char *APP_VERSION = "1.0";

constexpr const char *ERROR_MESSAGE_TEMPLATE = "EzAutoClicker: error:";

constexpr const char *ALLOW_ROOT_ERROR =
        "this program cannot be ran as root. Use the --allow-root option to launch as root user anyways. exiting.";
constexpr const char *UNALLOWED_ROOT_ERROR =
        "this program cannot be ran as root. exiting.";

}

}
