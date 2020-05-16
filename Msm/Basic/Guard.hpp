#pragma once
#include "TracerMocker.hpp"
#include "Event.hpp"
namespace Guard
{
auto constexpr event_stop_guard = "event_stop_guard";
auto constexpr event_stop_general_guard = "event_stop_general_guard";
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
} // namespace Guard