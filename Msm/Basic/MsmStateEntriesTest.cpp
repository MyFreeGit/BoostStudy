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
using msm::front::Row;

class MsmStateEntriesTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Entry action For StateMachine
        template <class Event, class Fsm>
        void on_entry(Event const&, Fsm&) const
        {
            TRACE_FUNCTION_CALL("state_machine_on_entry");
        }
        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const&, Fsm&) const
        {
            TRACE_FUNCTION_CALL("state_machine_on_exit");
        }

        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table
            : public boost::mpl::vector<
                  // |Start            |Event           |Next
                  Row<State::InitState, Event::Stop,     State::EndState>,  // EventStop Trigger the state is transitted to EndState
                  Row<State::InitState, Event::Loopback, State::InitState>, // Event will not trigger the state transition
                  // Inner state transit doesn't trigger state's on_exit and on_entry function.
                  // Set next state to msm::front::none means there is no state transition.
                  Row<State::InitState, Event::Inner,    msm::front::none>  
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
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace("state_machine_on_entry"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace("state_machine_on_exit")); // Stop the StateMachine will trigger the StateMachine's on_exit.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Stop());
    EXPECT_EQ(sut.current_state()[0], 1);
    sut.stop(); // Trigger the StateMachine's on_exit function
}

TEST_F(MsmStateEntriesTest, StateTransitBack)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace("state_machine_on_entry"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); // on_exit() is called when handling the event message.
        EXPECT_CALL(mock, trace(State::init_state_on_entry)); // on_entry() is called when state is transitted back to InitState
        // Due to the state machine still in initial state, the state_machine isn't stopped and not exit, 
        // so the state_machine's on_exit isn't called.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Loopback());
    EXPECT_EQ(sut.current_state()[0], 0);
}

TEST_F(MsmStateEntriesTest, InnerStateTransit)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace("state_machine_on_entry"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry)); 
        // No state transist is triggersed, so there is no on_exit and next on_entry is called.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Inner());
    EXPECT_EQ(sut.current_state()[0], 0);
}
} // namespace