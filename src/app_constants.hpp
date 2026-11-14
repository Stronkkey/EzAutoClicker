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

#include <glibmm/stringutils.h>

namespace EzAutoClicker {

namespace Constants {

constexpr const char *APP_ID = "io.github.Stronkkey.EzAutoClicker";

constexpr const char *APP_NAME = "EzAutoClicker";
constexpr const char *APP_VERSION = "1.0";

constexpr const char *CONTEXT_SUMMARY = "EzAutoClicker - automate mouse clicks at fixed intervals.";
constexpr const char *OPTION_CONTEXT_DESCRIPTION = "Press the configured hotkey (default F5) to start or stop clicking.";

constexpr const char NO_SHORT_NAME = '\0';

constexpr const char TOGGLE_SHORT_NAME = 't';
constexpr const char *TOGGLE_LONG_NAME = "toggle";
constexpr const char *TOGGLE_OPTION_DESCRIPTION = "Start or stop the autoclicker in a running instance.";

constexpr const char INTERVAL_MS_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_MS_LONG_NAME = "milliseconds";
constexpr const char *INTERVAL_MS_OPTION_DESCRIPTION = "Click interval in milliseconds.";
constexpr const char *INTERVAL_MS_ARG_DESCRIPTION = "MILLISECONDS";

constexpr const char INTERVAL_S_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_S_LONG_NAME = "seconds";
constexpr const char *INTERVAL_S_OPTION_DESCRIPTION = "Click interval in seconds.";
constexpr const char *INTERVAL_S_ARG_DESCRIPTION = "SECONDS";

constexpr const char INTERVAL_M_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_M_LONG_NAME = "minutes";
constexpr const char *INTERVAL_M_OPTION_DESCRIPTION = "Click interval in minutes.";
constexpr const char *INTERVAL_M_ARG_DESCRIPTION = "MINUTES";

constexpr const char INTERVAL_H_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_H_LONG_NAME = "hours";
constexpr const char *INTERVAL_H_OPTION_DESCRIPTION = "Click interval in hours.";
constexpr const char *INTERVAL_H_ARG_DESCRIPTION = "HOURS";

constexpr const char MOUSE_BUTTON_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *MOUSE_BUTTON_LONG_NAME = "mouse-button";
constexpr const char *MOUSE_BUTTON_OPTION_DESCRIPTION = "Mouse button which is clicked.";
constexpr const char *MOUSE_BUTTON_ARG_DESCRIPTION = "left|middle|right";

constexpr const char CLICK_TYPE_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CLICK_TYPE_LONG_NAME = "click-type";
constexpr const char *CLICK_TYPE_OPTION_DESCRIPTION = "Single (default) sends only one click. Double sends two clicks at the same times.";
constexpr const char *CLICK_TYPE_ARG_DESCRIPTION = "single|double";

constexpr const char CURRENT_LOCATION_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CURRENT_LOCATION_LONG_NAME = "use-current-location";
constexpr const char *CURRENT_LOCATION_OPTION_DESCRIPTION = "Send clicks at the current mouse position.";

constexpr const char PICK_LOCATION_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *PICK_LOCATION_LONG_NAME = "pick-location";
constexpr const char *PICK_LOCATION_OPTION_DESCRIPTION = "Send clicks at given screen position. Overrides the use-current-location option.";
constexpr const char *PICK_LOCATION_ARG_DESCRIPTION = "X,Y";

constexpr const char REPEAT_UNTIL_STOPPED_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *REPEAT_UNTIL_STOPPED_LONG_NAME = "repeat-until-stopped";
constexpr const char *REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION = "Sets Clicker to only stop when manually stopped. Overrides repeat-count argument.";

constexpr const char SET_REPEAT_COUNT_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *SET_REPEAT_COUNT_LONG_NAME = "repeat-count";
constexpr const char *SET_REPEAT_COUNT_OPTION_DESCRIPTION = "Amount of times to repeat before automatically stopping clicker.";
constexpr const char *SET_REPEAT_COUNT_ARG_DESCRIPTION = "REPEAT_COUNT";

constexpr const char CLICKER_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CLICKER_LONG_NAME = "clicker";
constexpr const char *CLICKER_OPTION_DESCRIPTION = "The clicker backend used to send clicks.";
constexpr const char *CLICKER_ARG_DESCRIPTION = "uinput|wlr|x11|auto";

constexpr const char HOTKEY_LISTENER_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *HOTKEY_LISTENER_LONG_NAME = "hotkey-listener";
constexpr const char *HOTKEY_LISTENER_OPTION_DESCRIPTION = "The hotkey listener backend used to listen for the hotkey.";
constexpr const char *HOTKEY_LISTENER_ARG_DESCRIPTION = "x11|evdev|auto";

constexpr const char ALLOW_ROOT_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *ALLOW_ROOT_LONG_NAME = "allow-root";
constexpr const char *ALLOW_ROOT_OPTION_DESCRIPTION = "Allows running the program as root.";

constexpr const char *ERROR_MESSAGE_TEMPLATE = "EzAutoClicker: error:";

/**
 * @note This is only used by main because GLib doesn't work as root.
 */
constexpr const char *ALLOW_ROOT_FLAG = "--allow-root"; 
constexpr const char *ALLOW_ROOT_ERROR =
        "this program cannot be ran as root. Use the --allow-root option to launch as root user anyways. exiting.";
constexpr const char *UNALLOWED_ROOT_ERROR =
        "this program cannot be ran as root. exiting.";

}

}
