//
// Created by Matq on 06/11/2025.
//

#pragma once

#include <functional>
#include <algorithm>
#include <vector>

template<typename... Args>
class EventOnce
{
public:
	using EventCallback = std::function<void(Args...)>;

	int Subscribe(EventCallback callback);
	void Unsubscribe(int event_id);
	void UnsubscribeAll();
	void Invoke(Args... args);

private:
	std::vector<std::pair<int, EventCallback>> callbacks;
	int next_event_id = 0;
};

// Subscribe as before
template<typename... Args>
int EventOnce<Args...>::Subscribe(EventCallback callback)
{
	int event_id = next_event_id++;
	callbacks.push_back({event_id, callback});
	return event_id;
}

// Unsubscribe by id
template<typename... Args>
void EventOnce<Args...>::Unsubscribe(int event_id)
{
	callbacks.erase(
		std::remove_if(callbacks.begin(), callbacks.end(),
					   [event_id](const auto& pair) { return pair.first == event_id; }),
		callbacks.end()
	);
}

// Clear all subscribers
template<typename... Args>
void EventOnce<Args...>::UnsubscribeAll()
{
	callbacks.clear();
	next_event_id = 0;
}

// Invoke all callbacks, then clear all subscribers
template<typename... Args>
void EventOnce<Args...>::Invoke(Args... args)
{
	for (auto& [id, callback] : callbacks)
		callback(args...);

	// Clear after invocation
	UnsubscribeAll();
}
