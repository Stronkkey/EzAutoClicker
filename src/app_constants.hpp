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

const Glib::ustring APP_ID = "org.Stronkkey.EzAutoClicker";

const Glib::ustring APP_NAME = "EzAutoClicker";
const Glib::ustring APP_VERSION = "1.0";

constexpr const char NO_SHORT_NAME = '\0';

constexpr const char TOGGLE_SHORT_NAME = 't';
const Glib::ustring TOGGLE_LONG_NAME = "toggle";
const Glib::ustring TOGGLE_OPTION_DESCRIPTION = "Start or stop the autoclicker in a running instance.";

constexpr const char INTERVAL_MS_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring INTERVAL_MS_LONG_NAME = "milliseconds";
const Glib::ustring INTERVAL_MS_OPTION_DESCRIPTION = "Sets the interval for milliseconds.";
const Glib::ustring INTERVAL_MS_ARG_DESCRIPTION = "NUMBER";

constexpr const char INTERVAL_S_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring INTERVAL_S_LONG_NAME = "seconds";
const Glib::ustring INTERVAL_S_OPTION_DESCRIPTION = "Sets the interval for seconds.";
const Glib::ustring INTERVAL_S_ARG_DESCRIPTION = "NUMBER";

constexpr const char INTERVAL_M_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring INTERVAL_M_LONG_NAME = "minutes";
const Glib::ustring INTERVAL_M_OPTION_DESCRIPTION = "Sets the interval for minutes.";
const Glib::ustring INTERVAL_M_ARG_DESCRIPTION = "NUMBER";

constexpr const char INTERVAL_H_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring INTERVAL_H_LONG_NAME = "hours";
const Glib::ustring INTERVAL_H_OPTION_DESCRIPTION = "Sets the interval for hours.";
const Glib::ustring INTERVAL_H_ARG_DESCRIPTION = "NUMBER";

constexpr const char MOUSE_BUTTON_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring MOUSE_BUTTON_LONG_NAME = "mouse-button";
const Glib::ustring MOUSE_BUTTON_OPTION_DESCRIPTION = "Set the mouse button to use.";
const Glib::ustring MOUSE_BUTTON_ARG_DESCRIPTION = "left|middle|right";

constexpr const char CLICK_TYPE_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring CLICK_TYPE_LONG_NAME = "click-type";
const Glib::ustring CLICK_TYPE_OPTION_DESCRIPTION = "Set the click type.";
const Glib::ustring CLICK_TYPE_ARG_DESCRIPTION = "single|double";

constexpr const char CURRENT_LOCATION_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring CURRENT_LOCATION_LONG_NAME = "use-current-location";
const Glib::ustring CURRENT_LOCATION_OPTION_DESCRIPTION = "Use current mouse location when sending clicks.";

constexpr const char PICK_LOCATION_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring PICK_LOCATION_LONG_NAME = "pick-location";
const Glib::ustring PICK_LOCATION_OPTION_DESCRIPTION = "Use a set mouse location when sending clicks.";
const Glib::ustring PICK_LOCATION_ARG_DESCRIPTION = "X,Y";

constexpr const char REPEAT_UNTIL_STOPPED_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring REPEAT_UNTIL_STOPPED_LONG_NAME = "repeat-until-stopped";
const Glib::ustring REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION = "Repeat click until stopped.";

constexpr const char SET_REPEAT_COUNT_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring SET_REPEAT_COUNT_LONG_NAME = "repeat-count";
const Glib::ustring SET_REPEAT_COUNT_OPTION_DESCRIPTION = "Sets the repeat count.";
const Glib::ustring SET_REPEAT_COUNT_ARG_DESCRIPTION = "NUMBER";

constexpr const char CLICKER_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring CLICKER_LONG_NAME = "clicker";
const Glib::ustring CLICKER_OPTION_DESCRIPTION = "Specifies the clicker backend.";
const Glib::ustring CLICKER_ARG_DESCRIPTION = "x11|uinput|auto";

constexpr const char INPUT_LISTENER_SHORT_NAME = NO_SHORT_NAME;
const Glib::ustring INPUT_LISTENER_LONG_NAME = "input-listener";
const Glib::ustring INPUT_LISTENER_OPTION_DESCRIPTION = "Specifies the input listener backend.";
const Glib::ustring INPUT_LISTENER_ARG_DESCRIPTION = "x11|evdev|auto";

}