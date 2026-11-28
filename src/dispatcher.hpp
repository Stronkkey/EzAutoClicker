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

#include <functional>
#include <memory>
#include <mutex>

#include <glib.h>

namespace EzAutoClicker {

class Dispatcher {
public:
	using Slot = std::function<void()>;
private:
	struct State {
		std::mutex mutex;
		Slot slot;
		bool alive = true;
	
		~State() {
			g_print("state delete\n");
		}
	};

	std::shared_ptr<State> state;
	GMainContext *context;

	static gboolean dispatch(gpointer data) noexcept;
	static void destroy_data(gpointer data);
public:
	Dispatcher(GMainContext* context = nullptr);
	~Dispatcher();

	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	void connect(const Slot &slot);
	void emit();
};

}
