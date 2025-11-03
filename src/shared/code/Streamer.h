//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <optional>
#include <tuple>

template<typename... Args>
class Streamer
{
public:
	using EventCallback = std::function<void(Args...)>;

	int Subscribe(EventCallback callback);
	void Unsubscribe(int event_id);
	void UnsubscribeAll();
	void Invoke(Args... args);
	void ClearValues();
	bool IsPopulated() const;

private:
	std::vector<std::pair<int, EventCallback>> callbacks;
	std::optional<std::tuple<Args...>> cached_args;
	int next_event_id = 0;
};

// Implementation in same header
template<typename... Args>
int Streamer<Args...>::Subscribe(EventCallback callback)
{
	int event_id = next_event_id++;

	// If we have cached values, immediately invoke the callback
	if (cached_args.has_value())
	{
		std::apply(callback, cached_args.value());
	}

	callbacks.push_back({event_id, callback});
	return event_id;
}

template<typename... Args>
void Streamer<Args...>::Unsubscribe(int event_id)
{
	callbacks.erase(
		std::remove_if(callbacks.begin(), callbacks.end(),
					   [event_id](const auto& pair) { return pair.first == event_id; }),
		callbacks.end()
	);
}

template<typename... Args>
void Streamer<Args...>::UnsubscribeAll()
{
	callbacks.clear();
	next_event_id = 0;
}

template<typename... Args>
void Streamer<Args...>::Invoke(Args... args)
{
	// Cache the arguments
	cached_args = std::make_tuple(args...);

	// Invoke all callbacks
	for (auto& [id, callback] : callbacks)
		callback(args...);
}

template<typename... Args>
void Streamer<Args...>::ClearValues()
{
	cached_args.reset();
}

template<typename... Args>
bool Streamer<Args...>::IsPopulated() const
{
	return cached_args.has_value();
}