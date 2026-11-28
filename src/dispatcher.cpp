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
#include <src/dispatcher.hpp>

EzAutoClicker::Dispatcher::Dispatcher(GMainContext* context) :state(std::make_shared<State>()), context(g_main_context_ref(context ? context : g_main_context_default())) {
}

EzAutoClicker::Dispatcher::~Dispatcher() {
	{
		auto lock = std::scoped_lock(state->mutex);
		state->alive = false;
		state->slot = nullptr;
	}

	g_main_context_unref(context);
}

gboolean EzAutoClicker::Dispatcher::dispatch(gpointer data) noexcept {
	auto state = *static_cast<std::shared_ptr<State>*>(data);

	Slot callback;
	{
		auto lock = std::unique_lock(state->mutex);
	
		if (!state->alive)
			return G_SOURCE_REMOVE;
		callback = state->slot;
	}

	try {
		if (callback)
			callback();
	} catch (...) {
		// GLib can't handle exceptions so we catch them here.
	}

	return G_SOURCE_REMOVE;
}

void EzAutoClicker::Dispatcher::destroy_data(gpointer data) {
	delete static_cast<std::shared_ptr<State>*>(data);
}

void EzAutoClicker::Dispatcher::connect(const Slot &slot) {
	auto lock = std::scoped_lock(state->mutex);
	state->slot = slot;
}

void EzAutoClicker::Dispatcher::emit() {
	GSource *source = g_idle_source_new();

	g_source_set_callback(
		source,
		&Dispatcher::dispatch,
		&state,
		nullptr
	);

	g_source_attach(source, context);
	g_source_unref(source);
}
