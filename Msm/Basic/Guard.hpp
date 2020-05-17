#pragma once
#include "TracerMocker.hpp"
#include "Event.hpp"
namespace Guard
{
constexpr auto event_stop_guard = "event_stop_guard";
constexpr auto event_stop_general_guard = "event_stop_general_guard";
constexpr auto condition1 = "condition1";
constexpr auto condition2 = "condition2";

struct EventStopGuard
{
    // General Operator
    template <class Event, class Fsm, class SourceState, class TargetState>
    bool operator()(Event &, Fsm &, SourceState &, TargetState &)
    {
        TRACE_FUNCTION_CALL(event_stop_general_guard);
        return true;
    }

    // The parameter is same as action functor, the different is the return type of Guard is bool
    // which is determine whether the state transition is triggered.
    template <class Fsm, class SourceState, class TargetState>
    bool operator()(Event::Stop const &e, Fsm &, SourceState &, TargetState &)
    {
        TRACE_FUNCTION_CALL(event_stop_guard);
        return e.readyToStop;
    }
};

struct Condition1
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    bool operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(condition1);
        return fsm.condition1;
    }
};

struct Condition2
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    bool operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(condition2);
        return fsm.condition2;
    }
};
} // namespace Guard