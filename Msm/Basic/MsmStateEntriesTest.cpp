#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <gtest/gtest.h>
#include <memory>
#include "Event.hpp"
#include "State.hpp"
#include "TracerMocker.hpp"
/* Demonstrate on_entry and on_exit in a state. When enter an state, the on_entry() is called when it is defined.
   When the state machine process an event, according to the transition table, if the event trigger the state
   transition, state's on_exit() is called. */
namespace
{
namespace msm = boost::msm;

class MsmStateEntriesTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Entry action For StateMachine
        template <class Event, class Fsm>
        void on_entry(Event const&, Fsm&) const
        {
            TRACE_FUNCTION_CALL("state_machine_on_entry()");
        }
        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const&, Fsm&) const
        {
            TRACE_FUNCTION_CALL("state_machine_on_exit()");
        }

        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table
            : public boost::mpl::vector<
                  // Use _row to simplify the code, _row allows omitting action and guard
                  //  |Start     |Event     |Next
                  _row<State::InitState, EventStop, State::EndState>, // EventStop Trigger the state is transitted to EndState
                  _row<State::InitState, Event,     State::InitState> // Event will not trigger the state transition
                  >
        {
        };
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;

protected:
    void SetUp() override
    {
        Tracer::TracerProvider::setTracer(std::make_unique<::testing::StrictMock<Tracer::TracerMocker>>());
    }

};

TEST_F(MsmStateEntriesTest, StateTransit)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace("state_machine_on_entry()"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace("state_machine_on_exit()")); // Stop the StateMachine will trigger the StaetMachine's on_exit.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(EventStop());
    EXPECT_EQ(*sut.current_state(), 1);
    sut.stop(); // Trigger the StateMachine's on_exit function
}

TEST_F(MsmStateEntriesTest, WithoutStateTransit)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace("state_machine_on_entry()"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); // on_exit() is called when handling the event message.
        EXPECT_CALL(mock, trace(State::init_state_on_entry)); // on_entry() is called when state is transitted back to InitState
        // Due to the state machine still in initial state, the state_machine isn't stopped and not exit, 
        // so the state_machine's on_exit isn't called.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event());
    EXPECT_EQ(*sut.current_state(), 0);
}
} // namespace