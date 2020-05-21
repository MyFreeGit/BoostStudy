#include "Tracer.hpp"
#include "State.hpp"
#include "Event.hpp"
#include "Action.hpp"
#include "Guard.hpp"
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace msm = boost::msm;
using msm::front::Row;
struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
{
    // Make the Current StateMachine support Defer Message
    typedef int activate_deferred_events;
    // Set initial state
    typedef State::InitState initial_state;
    // Transition table
    struct transition_table : public boost::mpl::vector<
        // |Start               |Event            |Next
        Row<State::InitState,    Event::Event,     State::NextState>,
        Row<State::InitState,    Event::Stop,      msm::front::none,    msm::front::Defer>,   // Defer Stop Event
        Row<State::NextState,    msm::front::none, State::WorkingState>, // Next State is Anonymous State
        Row<State::NextState,    Event::Stop,      State::EndState>,
        Row<State::WorkingState, Event::HandOver,  State::SpareState>,
        Row<State::SpareState,   Event::HandOver,  State::WorkingState>
    >{};
    // Control the return value of the Condition1 and Condition2
    bool condition1{true}, condition2{true};
};
// Pick a back-end
typedef msm::back::state_machine<StateMachine_> StateMachine;

int main()
{
    StateMachine sm;
    sm.start();
    sm.process_event(Event::Stop{});  // This will trigger the assertion failure due to no transistion.
    sm.process_event(Event::Event{});
    return 0;
}
