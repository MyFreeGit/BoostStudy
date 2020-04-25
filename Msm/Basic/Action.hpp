#pragma once
#include "TracerMocker.hpp"

namespace Action
{
constexpr auto on_event_stop = "on_event_stop";
struct OnEventStop
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    void operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(on_event_stop);
    }
};
}