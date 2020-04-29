#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <map>

using EventCallbackFunction = bool (*)(EventArgs& args);
//typedef bool (*EventCallbackFunction)(EventArgs& args);

// ------------------------------------------------------------------
// Event Subscription
// ------------------------------------------------------------------
class EventSubscription
{
	friend class EventSystem;

private:

	EventSubscription (EventCallbackFunction callbackFunctionPtr);

private:

	EventCallbackFunction m_callbackFunctionPtr = nullptr;

};

// ------------------------------------------------------------------
// Event System;
// ------------------------------------------------------------------
class EventSystem
{
	typedef std::vector<EventSubscription*> SubscribersList;

public:

	EventSystem();
	~EventSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscriptionEventCallbackFunction(const std::string& eventName, EventCallbackFunction callback);
	void UnsubscriptionEventCallbackFunction(const std::string& eventName, EventCallbackFunction callback);
	int FireEvent(const std::string& eventName);
	int FireEvent(const std::string& eventName, EventArgs& args);
	int GetNumSubscribersForCommand(const std::string& eventName) const;
	void GetSubscribedEventsList(std::vector<std::string>& eventNamesWithSubscribers) const;

private:

	std::map<std::string, SubscribersList> m_allSubscriptions;

};

extern EventSystem* g_theEventSystem;




