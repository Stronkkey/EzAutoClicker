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

#define APP_ID "org.Stronkkey.EzAutoClicker"
#define APP_NAME "EzAutoClicker"
#define APP_VERSION "1.0"

#define NO_SHORT_NAME '\0'

#define TOGGLE_SHORT_NAME 't'
#define TOGGLE_LONG_NAME "toggle"
#define TOGGLE_OPTION_DESCRIPTION "Start or stop the autoclicker in a running instance."

#define INTERVAL_MS_SHORT_NAME NO_SHORT_NAME
#define INTERVAL_MS_LONG_NAME "milliseconds"
#define INTERVAL_MS_OPTION_DESCRIPTION "Sets the interval for milliseconds."
#define INTERVAL_MS_ARG_DESCRIPTION "NUMBER"

#define INTERVAL_S_SHORT_NAME NO_SHORT_NAME
#define INTERVAL_S_LONG_NAME "seconds"
#define INTERVAL_S_OPTION_DESCRIPTION "Sets the interval for seconds."
#define INTERVAL_S_ARG_DESCRIPTION "NUMBER"

#define INTERVAL_M_SHORT_NAME NO_SHORT_NAME
#define INTERVAL_M_LONG_NAME "minutes"
#define INTERVAL_M_OPTION_DESCRIPTION "Sets the interval for minutes."
#define INTERVAL_M_ARG_DESCRIPTION "NUMBER"

#define INTERVAL_H_SHORT_NAME NO_SHORT_NAME
#define INTERVAL_H_LONG_NAME "hours"
#define INTERVAL_H_OPTION_DESCRIPTION "Sets the interval for hours."
#define INTERVAL_H_ARG_DESCRIPTION "NUMBER"

#define MOUSE_BUTTON_SHORT_NAME NO_SHORT_NAME
#define MOUSE_BUTTON_LONG_NAME "mouse-button"
#define MOUSE_BUTTON_OPTION_DESCRIPTION "Set the mouse button to use."
#define MOUSE_BUTTON_ARG_DESCRIPTION "left|middle|right"

#define CLICK_TYPE_SHORT_NAME NO_SHORT_NAME
#define CLICK_TYPE_LONG_NAME "click-type"
#define CLICK_TYPE_OPTION_DESCRIPTION "Set the click type."
#define CLICK_TYPE_ARG_DESCRIPTION "single|double"

#define CURRENT_LOCATION_SHORT_NAME NO_SHORT_NAME
#define CURRENT_LOCATION_LONG_NAME "use-current-location"
#define CURRENT_LOCATION_OPTION_DESCRIPTION "Use current mouse location when sending clicks."

#define PICK_LOCATION_SHORT_NAME NO_SHORT_NAME
#define PICK_LOCATION_LONG_NAME "pick-location"
#define PICK_LOCATION_OPTION_DESCRIPTION "Use a set mouse location when sending clicks."
#define PICK_LOCATION_ARG_DESCRIPTION "X,Y"

#define REPEAT_UNTIL_STOPPED_SHORT_NAME NO_SHORT_NAME
#define REPEAT_UNTIL_STOPPED_LONG_NAME "repeat-until-stopped"
#define REPEAT_UNTIL_STOPPED_OPTION_DESCRIPTION "Repeat click until stopped."

#define SET_REPEAT_COUNT_SHORT_NAME NO_SHORT_NAME
#define SET_REPEAT_COUNT_LONG_NAME "repeat-count"
#define SET_REPEAT_COUNT_OPTION_DESCRIPTION "Sets the repeat count."
#define SET_REPEAT_COUNT_ARG_DESCRIPTION "NUMBER"