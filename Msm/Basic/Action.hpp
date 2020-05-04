#pragma once
#include "TracerMocker.hpp"

namespace Action
{
constexpr auto on_event_stop = "on_event_stop";
constexpr auto on_event_inner = "on_event_inner";
struct OnEventStop
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    void operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(on_event_stop);
    }
};

struct OnEventInner
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    void operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(on_event_inner);
    }
};
}