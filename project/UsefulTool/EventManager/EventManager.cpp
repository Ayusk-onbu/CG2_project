#include "EventManager.h"

void EventManager::BindEventToTag(GAMEEVENTID eventId, EVENTCATEGORY category, int detailId)
{
	DirectionTag tag = std::make_pair(static_cast<int>(category), detailId);
	bindings_[eventId].push_back(tag);
}