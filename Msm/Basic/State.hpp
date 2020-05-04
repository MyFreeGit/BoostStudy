#pragma once
#include "TracerMocker.hpp"
#include <boost/msm/front/state_machine_def.hpp>
namespace msm = boost::msm;

namespace State
{
constexpr auto init_state_on_entry = "init_state_on_entry";
constexpr auto init_state_on_exit = "init_state_on_exit";
// States with entries
struct InitState : public msm::front::state<>
{
    // Entry action
    template <class Event, class Fsm>
    void on_entry(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(init_state_on_entry);
    }
    // Exit action
    template <class Event, class Fsm>
    void on_exit(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(init_state_on_exit);
    }
};

// End State
struct EndState : public msm::front::terminate_state<>
{
};

} // namespace State