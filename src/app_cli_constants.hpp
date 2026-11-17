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

#include <glibmm/i18n.h>

namespace EzAutoClicker {

namespace Constants {

#define EZAUTOCLICKER_CONTEXT_SUMMARY _("EzAutoClicker - automate mouse clicks at fixed intervals.")
#define EZAUTOCLICKER_OPTION_CONTEXT_DESCRIPTION _("Press the configured hotkey (default F5) to start or stop clicking.")

constexpr const char NO_SHORT_NAME = '\0';

constexpr const char TOGGLE_SHORT_NAME = 't';
constexpr const char *TOGGLE_LONG_NAME = "toggle";
#define EZAUTOCLICKER_TOGGLE_OPTION_DESCRIPTION _("Start or stop the autoclicker in a running instance.")

constexpr const char INTERVAL_MS_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_MS_LONG_NAME = "milliseconds";
#define EZAUTOCLICKER_INTERVAL_MS_OPTION_DESCRIPTION _("Click interval in milliseconds.")
#define EZAUTOCLICKER_INTERVAL_MS_ARG_DESCRIPTION _("MILLISECONDS")

constexpr const char INTERVAL_S_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_S_LONG_NAME = "seconds";
#define EZAUTOCLICKER_INTERVAL_S_OPTION_DESCRIPTION _("Click interval in seconds.")
#define EZAUTOCLICKER_INTERVAL_S_ARG_DESCRIPTION _("SECONDS")

constexpr const char INTERVAL_M_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_M_LONG_NAME = "minutes";
#define EZAUTOCLICKER_INTERVAL_M_OPTION_DESCRIPTION _("Click interval in minutes.")
#define EZAUTOCLICKER_INTERVAL_M_ARG_DESCRIPTION _("MINUTES")

constexpr const char INTERVAL_H_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *INTERVAL_H_LONG_NAME = "hours";
#define EZAUTOCLICKER_INTERVAL_H_OPTION_DESCRIPTION _("Click interval in hours.")
#define EZAUTOCLICKER_INTERVAL_H_ARG_DESCRIPTION _("HOURS")

constexpr const char MOUSE_BUTTON_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *MOUSE_BUTTON_LONG_NAME = "mouse-button";
#define EZAUTOCLICKER_MOUSE_BUTTON_OPTION_DESCRIPTION _("Mouse button which is clicked.")
constexpr const char *MOUSE_BUTTON_ARG_DESCRIPTION = "left|middle|right";

constexpr const char CLICK_TYPE_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CLICK_TYPE_LONG_NAME = "click-type";
#define EZAUTOCLICKER_CLICK_TYPE_OPTION_DESCRIPTION _("Single (default) sends only one click. Double sends two clicks at the same times.")
constexpr const char *CLICK_TYPE_ARG_DESCRIPTION = "single|double";

constexpr const char CURRENT_LOCATION_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CURRENT_LOCATION_LONG_NAME = "use-current-location";
#define EZAUTOCLICKER_CURRENT_LOCATION_OPTION_DESCRIPTION _("Send clicks at the current mouse position.")

constexpr const char PICK_LOCATION_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *PICK_LOCATION_LONG_NAME = "pick-location";
#define EZAUTOCLICKER_PICK_LOCATION_OPTION_DESCRIPTION _("Send clicks at given screen position. Overrides the use-current-location option.")
#define EZAUTOCLICKER_PICK_LOCATION_ARG_DESCRIPTION _("X,Y")

constexpr const char REPEAT_UNTIL_STOPPED_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *REPEAT_UNTIL_STOPPED_LONG_NAME = "repeat-until-stopped";
#define EZAUTOCLICKER_REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION _("Sets Clicker to only stop when manually stopped. Overrides repeat-count argument.")

constexpr const char SET_REPEAT_COUNT_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *SET_REPEAT_COUNT_LONG_NAME = "repeat-count";
#define EZAUTOCLICKER_SET_REPEAT_COUNT_OPTION_DESCRIPTION _("Amount of times to repeat before automatically stopping clicker.")
#define EZAUTOCLICKER_SET_REPEAT_COUNT_ARG_DESCRIPTION _("REPEAT_COUNT")

constexpr const char CLICKER_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *CLICKER_LONG_NAME = "clicker";
#define EZAUTOCLICKER_CLICKER_OPTION_DESCRIPTION _("The clicker backend used to send clicks.")

#if EZAUTOCLICKER_X11_INPUT_ENABLED
#if EZAUTOCLICKER_ENABLE_WLR
constexpr const char *CLICKER_ARG_DESCRIPTION = "uinput|wlr|x11|auto";
#else
constexpr const char *CLICKER_ARG_DESCRIPTION = "uinput|x11|auto";
#endif
#else
#if EZAUTOCLICKER_ENABLE_WLR
constexpr const char *CLICKER_ARG_DESCRIPTION = "uinput|wlr|auto";
#else
constexpr const char *CLICKER_ARG_DESCRIPTION = "uinput|auto";
#endif
#endif

constexpr const char HOTKEY_LISTENER_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *HOTKEY_LISTENER_LONG_NAME = "hotkey-listener";
#define EZAUTOCLICKER_HOTKEY_LISTENER_OPTION_DESCRIPTION _("The hotkey listener backend used to listen for the hotkey.")

#if EZAUTOCLICKER_X11_INPUT_ENABLED
constexpr const char *HOTKEY_LISTENER_ARG_DESCRIPTION = "x11|evdev|auto";
#else
constexpr const char *HOTKEY_LISTENER_ARG_DESCRIPTION = "evdev|auto";
#endif

constexpr const char ALLOW_ROOT_SHORT_NAME = NO_SHORT_NAME;
constexpr const char *ALLOW_ROOT_LONG_NAME = "allow-root";
#define EZAUTOCLICKER_ALLOW_ROOT_OPTION_DESCRIPTION _("Allows running the program as root.")

/**
 * @note This is only used by main because GLib doesn't work as root.
 */
constexpr const char *ALLOW_ROOT_FLAG = "--allow-root"; 

}

}
