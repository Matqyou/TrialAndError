//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include "shared/string/Strings.h"

template<typename... Args>
class EventChannel
{
private:
	using EventCallback = std::function<void(Args...)>;
	std::vector<std::pair<int, EventCallback>> callbacks;
	std::unordered_map<int, size_t> id_to_index;
	int next_event_id = 0;
	bool is_invoking = false;
	std::vector<int> pending_unsubscribes;

public:
	int Subscribe(EventCallback callback) {
		int event_id = next_event_id++;
		id_to_index[event_id] = callbacks.size();
		callbacks.push_back({event_id, std::move(callback)});
		return event_id;
	}

	void Unsubscribe(int event_id) {
		if (is_invoking) {
			// Defer until after Invoke() completes
			pending_unsubscribes.push_back(event_id);
			return;
		}

		auto it = id_to_index.find(event_id);
		if (it == id_to_index.end()) return;

		size_t index = it->second;

		// Swap with last element
		if (index != callbacks.size() - 1) {
			callbacks[index] = std::move(callbacks.back());
			id_to_index[callbacks[index].first] = index;
		}

		callbacks.pop_back();
		id_to_index.erase(it);
	}

	void Invoke(Args... args) {
		is_invoking = true;

		for (auto& [id, callback] : callbacks)
			callback(args...);

		is_invoking = false;

		// Process deferred unsubscribes
		for (int event_id : pending_unsubscribes) {
			Unsubscribe(event_id);
		}
		pending_unsubscribes.clear();
	}
};