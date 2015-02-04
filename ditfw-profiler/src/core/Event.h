/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "HashMap.h"
#include "Hash.h"
#include "Log.h"

#include <tuple>
#include <functional>

#define EVENT_DYNAMIC_CAST

namespace dit {

    //-----------------------------------//

    template<int ...>
    struct seq { };

    template<int N, int ...S>
    struct genseq : genseq < N - 1, N - 1, S... > { };

    template<int ...S>
    struct genseq < 0, S... > {
        typedef seq<S...> type;
    };

    //-----------------------------------//

}

namespace dit {

    //-----------------------------------//

    class VoidEvent
    {
    public:
        typedef std::function<void(VoidEvent*)> Callable;

    public:
        VoidEvent() = default;
        VoidEvent(const VoidEvent&) = default;
        VoidEvent(uint64 key) : hash(key) {}

        virtual ~VoidEvent() = default;
        
    public:
        uint64 hash;
    };

    //-----------------------------------//

    template <typename... Args>
    class Event : public VoidEvent
    {
    public:
        typedef std::tuple<Args...> Params;
        typedef std::function<void (Params&)> Callable;

    public:
        Params params;

    public:

        //-----------------------------------//

        Event() = default;

        Event(uint64 key, Args... args)
            : VoidEvent(key)
            , params(args...)
        {}

        Event(uint64 key, const Params& rhs)
            : VoidEvent(key)
            , params(rhs)
        {}

        Event(uint64 key, Params&& rhs)
            : VoidEvent(key)
            , params(std::move(rhs))
        {}

    public:

        //-----------------------------------//

        static void Rebind(VoidEvent* event, const Callable& func)
        {
            Event* e = nullptr;

#ifdef EVENT_DYNAMIC_CAST
            try {
                e = dynamic_cast<Event*>(event);
            }
            catch (const std::bad_cast& exc) {
                log("EVENT CAST FAILURE: \"%0\"", exc.what());
                return;
            }
#else
            e = (Event*) event;
#endif
            func(e->params);
        }

        //-----------------------------------//

        static void Unpack(Params& params, const std::function<void (Args...)>& func)
        {
            typedef typename genseq<sizeof...(Args)>::type EventSignature;

            Call(params, func, EventSignature());
        }

        //-----------------------------------//

        template<int ...S>
        static void Call(Params& params, const std::function<void(Args...)>& func, seq<S...>)
        {
            func(std::move(std::get<S>(params))... );
        }
    };

    //-----------------------------------//

    class EventListener
    {
    public:
        EventListener() = default;
        EventListener(const EventListener& rhs) = default;
        EventListener(EventListener&& rhs)
            : hash(rhs.hash)
            , invoke(std::move(rhs.invoke))
        {
            rhs.hash = 0;
            rhs.invoke = nullptr;
        }

        EventListener& operator=(const EventListener& rhs) = default;
        EventListener& operator=(EventListener&& rhs)
        {
            hash = rhs.hash;
            invoke = std::move(rhs.invoke);

            rhs.hash = 0;
            rhs.invoke = nullptr;

            return *this;
        }

        template <typename... Args>
        EventListener(const String& key, const std::function<void(Args...)>& func)
        {
            typedef Event<Args...> Event;

            // hash the key string
            hash = MurmurHash64(key.c_str(), key.size(), 0);

            // adapt the function object to event pattern
            Event::Callable bind = [=](Event::Params& params) {
                Event::Unpack(params, func);
            };

            invoke = [=](VoidEvent* event) {
                Event::Rebind(event, bind);
            };
        }

        template <typename... Args>
        EventListener(uint64 key, const std::function<void(Args...)>& func)
        {
            typedef Event<Args...> Event;

            // hash the key string
            hash = key;

            // adapt the function object to event pattern
            Event::Callable bind = [=](Event::Params& params) {
                Event::Unpack(params, func);
            };

            invoke = [=](VoidEvent* event) {
                Event::Rebind(event, bind);
            };
        }

    public:
        void operator()(VoidEvent* event)
        {
            if (invoke != nullptr)
                invoke(event);
        }

    public:
        uint64 hash = 0;
        VoidEvent::Callable invoke = nullptr;
    };

    //-----------------------------------//

    class EventMap
    {
    public:
        HashMap<EventListener> table;

    public:
        EventMap() = default;
        EventMap(std::initializer_list<EventListener> listeners)
        {
            table.reserve(listeners.size());
            for (const auto& l : listeners)
                table.set(l.hash, l);
        }
        ~EventMap()
        {
            table.clear();
        }

    public:
        void Connect(EventListener listener)
        {
            table.set(listener.hash, listener);
        }

        void Disconnect(const String& key)
        {
            uint64 hash = MurmurHash64(key.c_str(), key.size(), 0);
            Disconnect(hash);
        }

        void Disconnect(uint64 key)
        {
            table.remove(key);
        }

        void Post(VoidEvent* e)
        {
            static EventListener _emptyListener{};
            auto& listener = const_cast<EventListener&>(table.get(e->hash, _emptyListener));
            listener(e);
        }
    };

    //-----------------------------------//

}