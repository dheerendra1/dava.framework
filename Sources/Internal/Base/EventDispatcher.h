/*==================================================================================
    Copyright (c) 2008, DAVA Consulting, LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the DAVA Consulting, LLC nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE DAVA CONSULTING, LLC AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL DAVA CONSULTING, LLC BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Revision History:
        * Created by Vitaliy Borodovsky 
=====================================================================================*/
#ifndef __DAVAENGINE_EVENTDISPATCHER_H__
#define __DAVAENGINE_EVENTDISPATCHER_H__

#include "Base/BaseTypes.h"
#include "Base/BaseObject.h"
#include "Base/Message.h"

namespace DAVA
{
/**
	\ingroup baseobjects
	\brief this class is a list of messages mapped to event ids
 */
class EventDispatcher : public BaseObject
{
public:
	EventDispatcher(); 
	virtual ~EventDispatcher();

	/**
		\brief Function to add event to event dispatcher.
		\param[in] eventId	event id we will use to perform messages
		\param[in] msg		message we assign for this eventId
	 */
	void AddEvent(int32 eventId, const Message &msg);
	/**
		\brief Function to remove event from event dispatcher.
		\param[in] eventId	event id 
		\param[in] msg		message we want to delete for given eventId
		\returns true if we removed something, false if not
	 */
	bool RemoveEvent(int32 eventId, const Message &msg);
	
	/**
	 \brief Function to remove all events from event dispatcher.
	 \returns true if we removed something, false if not
	 */
	bool RemoveAllEvents();
	

	/**
		\brief Function to perform all events with given id from this event dispatcher.
	 
		When this function called, it perform all messages linked to given eventId.
		It use this pointer for caller parameter of Message. caller variable is used when you want to 
		perform event but with different caller.

		\param[in] eventId	event id 
		\param[in] eventParam this param is used when we want to replace caller in message with another class 
	 */
	void PerformEvent(int32 eventId);	
	void PerformEvent(int32 eventId, BaseObject *caller);

	
	/**
	 \brief Function to perform all events with given id from this event dispatcher.
	 
	 When this function called, it perform all messages linked to given eventId.
	 It use this pointer for caller parameter of Message. caller variable is used when you want to 
	 perform event but with different caller.
	 
	 \param[in] eventId	event id 
	 \param[in] eventParam this param is used when we want to replace caller in message with another class 
	 \param[in] callerData this param is used when we want to send some data from the caller
	 */
	void PerformEventWithData(int32 eventId, void *callerData);
	void PerformEventWithData(int32 eventId, BaseObject *caller, void *callerData);

	/**
		\brief Clone dispatcher make 100% copy of this dispatcher with reference count equal to 1
		\returns new EventDispatcher that contains the same data as first one
	 */
	EventDispatcher *CloneDispatcher();
	
	/**
		\brief This function copy all date from another dispatcher to this dispatcher
	 */
	void CopyDataFrom(EventDispatcher *srcDispatcher);

protected:

	class Event 
	{
	public:
		int32	eventType;
		Message msg;
		Event()
		{
		}
	};
	
	List<Event> events;
};
	
/**
	\ingroup baseobjects
	\brief	Helper to implement event dispatchers.
 
	In some cases we want to avoid inheritance from multiple objects.
	But some classes require to be both singletons & handle events.
	For these cases we've added this define to avoid multiple copying of 
	the same code.
 */
#define IMPLEMENT_EVENT_DISPATCHER(eventDispatcherName)	\
public:\
	void AddEvent(int32 eventType, const Message &msg){eventDispatcherName.AddEvent(eventType, msg); }; \
	bool RemoveEvent(int32 eventType, const Message &msg){return eventDispatcherName.RemoveEvent(eventType, msg); };\
	bool RemoveAllEvents(){return eventDispatcherName.RemoveAllEvents(); };\
protected:\
	EventDispatcher eventDispatcherName;
	
	
	
	
};

#endif