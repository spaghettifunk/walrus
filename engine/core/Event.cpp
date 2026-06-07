#include <engine/containers/DArray.h>
#include <engine/core/Event.h>
#include <engine/core/Memory.h>

// This should be more than enough codes...
#define MAX_MESSAGE_CODES 16384

namespace Walrus
{
    struct RegisteredEvent
    {
        void* listener;
        Event::OnEvent callback;
    };

    struct EventCodeEntry
    {
        DArray<RegisteredEvent> events;
    };

    // State structure.
    struct EventSystemState
    {
        // Lookup table for event codes.
        EventCodeEntry registered[MAX_MESSAGE_CODES];
    };

    Event::Event()
    {
        if (is_Initialized == true)
        {
            return;
        }
        is_Initialized = false;
        state = New<EventSystemState>(MemoryTag::MEMORY_TAG_APPLICATION);

        is_Initialized = true;
    }

    Event::~Event()
    {
        if (!state)
        {
            return;
        }

        Delete(state, MemoryTag::MEMORY_TAG_APPLICATION);
        state = nullptr;
        is_Initialized = false;
    }

    bool Event::Register(u16 code, void* listener, Event::OnEvent on_event)
    {
        if (is_Initialized == false)
        {
            return false;
        }

        DArray<RegisteredEvent>& events = state->registered[code].events;
        u64 registered_count = events.Length();
        for (u64 i = 0; i < registered_count; ++i)
        {
            if (events[i].listener == listener)
            {
                // TODO: warn
                return false;
            }
        }

        // If at this point, no duplicate was found. Proceed with registration.
        RegisteredEvent event;
        event.listener = listener;
        event.callback = on_event;
        events.PushBack(event);

        return true;
    }

    bool Event::Unregister(u16 code, void* listener, Event::OnEvent on_event)
    {
        if (is_Initialized == false)
        {
            return false;
        }

        // On nothing is registered for the code, boot out.
        DArray<RegisteredEvent>& events = state->registered[code].events;
        if (events.IsEmpty())
        {
            // TODO: warn
            return false;
        }

        u64 registered_count = events.Length();
        for (u64 i = 0; i < registered_count; ++i)
        {
            RegisteredEvent e = events[i];
            if (e.listener == listener && e.callback == on_event)
            {
                // Found one, remove it
                RegisteredEvent popped_event;
                events.RemoveAt(i, &popped_event);
                return true;
            }
        }

        // Not found.
        return false;
    }

    bool Event::Fire(u16 code, void* sender, EventContext context)
    {
        if (is_Initialized == false)
        {
            return false;
        }

        // If nothing is registered for the code, boot out.
        DArray<RegisteredEvent>& events = state->registered[code].events;
        if (events.IsEmpty())
        {
            return false;
        }

        u64 registered_count = events.Length();
        for (u64 i = 0; i < registered_count; ++i)
        {
            RegisteredEvent e = events[i];
            if (e.callback(code, sender, e.listener, context))
            {
                // Message has been handled, do not send to other listeners.
                return true;
            }
        }

        // Not found.
        return false;
    }
} // namespace Walrus
