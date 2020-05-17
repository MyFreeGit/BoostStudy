#include <gtest/gtest.h>
#include "TracerMocker.hpp"
#include "State.hpp"
#include "Event.hpp"
#include "Action.hpp"
#include "Guard.hpp"
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
/* State transition with action is triggered */
namespace
{
namespace msm = boost::msm;
using msm::front::Row;

class DeferEventTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Make the Current StateMachine support Defer Message
        typedef int activate_deferred_events;
        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            //|Start             |Event        |Next             |Action            |Guard
            Row<State::InitState, Event::Event, State::NextState, Action::Action1,   Guard::Condition1>,
            Row<State::InitState, Event::Stop,  msm::front::none, msm::front::Defer, Guard::Condition1>, 
            Row<State::NextState, Event::Stop,  State::EndState,  Action::Action2,   Guard::Condition2>
        >{};
        // Control the return value of the Condition1 and Condition2
        bool condition1{true}, condition2{true};
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;
protected:
    void SetUp() override
    {
        Tracer::TracerProvider::setTracer(std::make_unique<::testing::StrictMock<Tracer::TracerMocker>>());
    }
};

TEST_F(DeferEventTest, Condition2ReturnTrue)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Guard::condition1));
        EXPECT_CALL(mock, trace(Guard::condition1));

        // Guard::Condition1 return true, trigger exiting InitState
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        // Action1 is performed
        EXPECT_CALL(mock, trace(Action::action1));
        // State is transit to NextState
        EXPECT_CALL(mock, trace(State::next_state_on_entry));

        // Deferred Stop Event is processed
        EXPECT_CALL(mock, trace(Guard::condition2));
        // Guard2 return true, and transition to EndState is triggered
        EXPECT_CALL(mock, trace(Action::action2));
    }
    StateMachine sut{};
    sut.start();
    sut.process_event(Event::Stop{});
    sut.process_event(Event::Event{});
    EXPECT_EQ(*sut.current_state(), 2);
}
}