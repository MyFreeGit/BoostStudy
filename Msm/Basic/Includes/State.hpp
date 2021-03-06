#pragma once
#include "TracerMocker.hpp"
#include <boost/msm/front/state_machine_def.hpp>
namespace msm = boost::msm;

namespace State
{
constexpr auto init_state_on_entry = "init_state_on_entry";
constexpr auto init_state_on_exit = "init_state_on_exit";
constexpr auto next_state_on_entry = "next_state_on_entry";
constexpr auto another_state_on_entry = "another_state_on_entry";
constexpr auto working_state_on_entry = "working_state_on_entry";
constexpr auto spare_state_on_entry = "spare_state_on_entry";
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

struct NextState : public msm::front::state<>
{
    // Entry action
    template <class Event, class Fsm>
    void on_entry(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(next_state_on_entry);
    }
};

struct AnotherState : public msm::front::state<>
{
    // Entry action
    template <class Event, class Fsm>
    void on_entry(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(another_state_on_entry);
    }
};

struct WorkingState : public msm::front::state<>
{
    // Entry action
    template <class Event, class Fsm>
    void on_entry(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(working_state_on_entry);
    }
};

struct SpareState : public msm::front::state<>
{
    // Entry action
    template <class Event, class Fsm>
    void on_entry(Event const &, Fsm &) const
    {
        TRACE_FUNCTION_CALL(spare_state_on_entry);
    }
};
} // namespace State