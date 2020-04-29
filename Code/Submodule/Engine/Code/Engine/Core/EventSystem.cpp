#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
//#include "Engine/Core/NamedProperties.hpp"

EventSystem* g_theEventSystem = nullptr;


EventSubscription::EventSubscription( EventCallbackFunction callbackFunctionPtr )
{
	m_callbackFunctionPtr = callbackFunctionPtr;
}

EventSystem::EventSystem()
{

}

EventSystem::~EventSystem()
{

}

void EventSystem::Startup()
{

}

void EventSystem::Shutdown()
{

}

void EventSystem::BeginFrame()
{

}

void EventSystem::EndFrame()
{

}

void EventSystem::SubscriptionEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback )
{
	EventSubscription* subscriber = new EventSubscription(callback);
	m_allSubscriptions[eventName].push_back(subscriber);
}

void EventSystem::UnsubscriptionEventCallbackFunction( const std::string& eventName, EventCallbackFunction callback )
{
	std::map<std::string, SubscribersList>::iterator subscribersIterator;
	for (subscribersIterator = m_allSubscriptions.begin(); subscribersIterator != m_allSubscriptions.end(); subscribersIterator++)
	{
		if(eventName.compare(subscribersIterator->first) == 0)
		{
			for(int subscribeIndex = 0; subscribeIndex < static_cast<int>(subscribersIterator->second.size()); subscribeIndex++)
			{
				if(subscribersIterator->second[subscribeIndex]->m_callbackFunctionPtr == callback)
				{
					subscribersIterator->second[subscribeIndex]->m_callbackFunctionPtr = nullptr;
					subscribersIterator->second.erase(subscribersIterator->second.begin() + subscribeIndex);
					break;
				}
			}
			break;
		}
	}
}

int EventSystem::FireEvent( const std::string& eventName )
{
	EventArgs args;
	return FireEvent(eventName, args);
}

int EventSystem::FireEvent( const std::string& eventName, EventArgs& args )
{
	int eventsFired = 0;
	std::map<std::string, SubscribersList>::iterator subscribersIterator;

	subscribersIterator = m_allSubscriptions.find(eventName);

	if(subscribersIterator == m_allSubscriptions.end())
	{
		return 0;
	}

	bool eventConsumption;
	for(int subscriberIndex = 0; subscriberIndex < static_cast<int>(subscribersIterator->second.size()); subscriberIndex++)
	{
		eventConsumption = subscribersIterator->second[subscriberIndex]->m_callbackFunctionPtr(args);
		eventsFired++;
		if(eventConsumption)
		{
			break;
		}
	}

	return eventsFired;
}

int EventSystem::GetNumSubscribersForCommand( const std::string& eventName ) const
{
	std::map<std::string, SubscribersList>::const_iterator subscribersIterator;
	subscribersIterator = m_allSubscriptions.find(eventName);

	int eventCount = (int)(subscribersIterator->second.size());
	return eventCount;
}

void EventSystem::GetSubscribedEventsList( std::vector<std::string>& eventNamesWithSubscribers ) const
{
	std::map<std::string, SubscribersList>::const_iterator subscribersIterator;
	for (subscribersIterator = m_allSubscriptions.begin(); subscribersIterator != m_allSubscriptions.end(); subscribersIterator++)
	{
		if(subscribersIterator->second.size() != 0)
		{
			eventNamesWithSubscribers.push_back(subscribersIterator->first);
		}
	}
}


