#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <gtest/gtest.h>
#include <memory>
#include "Event.hpp"
#include "Tracer.hpp"
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
            tracer->state_machine_on_entry();
        }
        // Exit action
        template <class Event, class Fsm>
        void on_exit(Event const&, Fsm&) const
        {
            tracer->state_machine_on_exit();
        }

        // States with entries
        struct InitState : public msm::front::state<>
        {
            // Entry action
            template <class Event, class Fsm>
            void on_entry(Event const&, Fsm&) const
            {
                tracer->init_state_on_entry();
            }
            // Exit action
            template <class Event, class Fsm>
            void on_exit(Event const&, Fsm&) const
            {
                tracer->init_state_on_exit();
            }
        };
        // End State
        struct EndState : public msm::front::terminate_state<>
        {
        };

        // Set initial state
        typedef InitState initial_state;
        // Transition table
        struct transition_table
            : public boost::mpl::vector<
                  // Use _row to simplify the code, _row allows omitting action and guard
                  //  |Start     |Event     |Next
                  _row<InitState, EventStop, EndState>, // EventStop Trigger the state is transitted to EndState
                  _row<InitState, Event, InitState> // Event will not trigger the state transition
                  >
        {
        };

        static std::unique_ptr<ITracer> tracer;
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;

protected:
};
std::unique_ptr<ITracer> MsmStateEntriesTest::StateMachine_::tracer = std::make_unique<OutputTracer>();

TEST_F(MsmStateEntriesTest, StateTransit)
{
    {
        ::testing::InSequence s;
        using ::testing::StrictMock;
        auto tracer = std::make_unique<StrictMock<MockTracer>>();
        EXPECT_CALL(*tracer, state_machine_on_entry());
        EXPECT_CALL(*tracer, init_state_on_entry());
        EXPECT_CALL(*tracer, init_state_on_exit());
        EXPECT_CALL(*tracer, state_machine_on_exit()); // Process the EventStop make state_machine hit end state and exit.
        MsmStateEntriesTest::StateMachine_::tracer = std::move(tracer);
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(EventStop());
    EXPECT_EQ(*sut.current_state(), 1);
}

TEST_F(MsmStateEntriesTest, WithoutStateTransit)
{
    {
        ::testing::InSequence s;
        using ::testing::StrictMock;
        auto tracer = std::make_unique<StrictMock<MockTracer>>();
        EXPECT_CALL(*tracer, state_machine_on_entry());
        EXPECT_CALL(*tracer, init_state_on_entry());
        EXPECT_CALL(*tracer, init_state_on_exit()); // on_exit() is called when handling the event message.
        EXPECT_CALL(*tracer, init_state_on_entry()); // on_entry() is called when state is transitted back to InitState
        // Due to the state machine still in initial state, the state_machine isn't stopped and not exit, 
        // so the state_machine's on_exit isn't called.
        MsmStateEntriesTest::StateMachine_::tracer = std::move(tracer);
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event());
    EXPECT_EQ(*sut.current_state(), 0);
}
} // namespace