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

#include <cstdint>

namespace EzAutoClicker {

namespace Input {

struct Hotkey {
        enum class Modifier : uint32_t {
	        NONE = 0,
	        CTRL = 1 << 0,
                SHIFT = 1 << 1,
	        ALT = 1 << 2,
	        SUPER = 1 << 3,
                ALL = CTRL | SHIFT | ALT | SUPER,
        };

        /**
         * @brief Evdev keycode. 0 = unset.
         */
	uint16_t key = 0;

        /**
         * @brief Bitmask of modifier.
         */
	Modifier modifiers = Modifier::NONE;

        constexpr bool has_modifiers_flag(Modifier flag) const {
	        return (static_cast<uint32_t>(modifiers) & static_cast<uint32_t>(flag)) != 0;
        }
};

constexpr Hotkey::Modifier operator|(Hotkey::Modifier left, Hotkey::Modifier right) {
	return static_cast<Hotkey::Modifier>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
}

constexpr Hotkey::Modifier operator^(Hotkey::Modifier left, Hotkey::Modifier right) {
	return static_cast<Hotkey::Modifier>(static_cast<uint32_t>(left) ^ static_cast<uint32_t>(right));
}

constexpr Hotkey::Modifier operator&(Hotkey::Modifier left, Hotkey::Modifier right) {
	return static_cast<Hotkey::Modifier>(static_cast<uint32_t>(left) & static_cast<uint32_t>(right));
}

constexpr bool operator==(Hotkey::Modifier left, Hotkey::Modifier right) {
	return static_cast<uint32_t>(left) == static_cast<uint32_t>(right);
}

}

}