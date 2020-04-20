#include <gtest/gtest.h>
#include "Tracer.hpp"
#include "Event.hpp"
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
/* State transition with action is triggered */
namespace
{
namespace msm = boost::msm;

class MsmActionTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // States with entries
        struct InitState : public msm::front::state<>
        {
            // Entry action
            template <class Event, class Fsm>
            void on_entry(Event const &, Fsm &) const
            {
                tracer->init_state_on_entry();
            }
            // Exit action
            template <class Event, class Fsm>
            void on_exit(Event const &, Fsm &) const
            {
                tracer->init_state_on_exit();
            }
        };
        //End State
        struct EndState : public msm::front::terminate_state<>
        {
        };

        // Set initial state
        typedef InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            // Use _row to simplify the code, _row allows omitting action and guard
            //  |Start      |Event     |Next     |Action
            _row<InitState, EventStop, EndState>, // EventStop Trigger the state is transitted to EndState
            _row<InitState, Event,     InitState> // Event will not trigger the state transition
        >{};

       static std::unique_ptr<ITracer> tracer;
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;

protected:
};
std::unique_ptr<ITracer> MsmActionTest::StateMachine_::tracer = std::make_unique<OutputTracer>();

TEST_F(MsmActionTest, StateTransit)
{
    {
        ::testing::InSequence s;
        using ::testing::StrictMock;
        auto tracer = std::make_unique<StrictMock<MockTracer>>();
        EXPECT_CALL(*tracer, state_machine_on_entry());
        EXPECT_CALL(*tracer, init_state_on_entry());
        EXPECT_CALL(*tracer, init_state_on_exit());
        EXPECT_CALL(*tracer, state_machine_on_exit()); // Process the EventStop make state_machine hit end state and exit.
        MsmActionTest::StateMachine_::tracer = std::move(tracer);
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(EventStop());
    EXPECT_EQ(*sut.current_state(), 1);
}

TEST_F(MsmActionTest, WithoutStateTransit)
{
    {
        ::testing::InSequence s;
        using ::testing::StrictMock;
        auto tracer = std::make_unique<StrictMock<MockTracer>>();
        EXPECT_CALL(*tracer, state_machine_on_entry());
        EXPECT_CALL(*tracer, init_state_on_entry());
        EXPECT_CALL(*tracer, init_state_on_exit());
        EXPECT_CALL(*tracer, state_machine_on_exit()); // Process the EventStop make state_machine hit end state and exit.
        MsmActionTest::StateMachine_::tracer = std::move(tracer);
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event());
    EXPECT_EQ(*sut.current_state(), 0);
}
}