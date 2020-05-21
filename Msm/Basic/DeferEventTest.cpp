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
            //|Start                |Event            |Next                |Action            |Guard
            Row<State::InitState,    Event::Event,     State::WorkingState, Action::Action1,   Guard::Condition1>,
            Row<State::InitState,    Event::HandOver,  msm::front::none,    msm::front::Defer, Guard::Condition2>, 
            Row<State::WorkingState, Event::HandOver,  State::SpareState>,
            Row<State::SpareState,   Event::HandOver,  State::WorkingState>
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
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Guard::condition2)).Times(2);
        EXPECT_CALL(mock, trace(Guard::condition1));

        // The Event::Event is received, and state transition is triggered.
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace(Action::action1));
        EXPECT_CALL(mock, trace(State::working_state_on_entry));

        // The first Deferred Handle Event is processed
        EXPECT_CALL(mock, trace(State::spare_state_on_entry));
        // The second Deferred Handle Event is processed
        EXPECT_CALL(mock, trace(State::working_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    sut.process_event(Event::HandOver{});
    sut.process_event(Event::HandOver{});
    EXPECT_EQ(sut.current_state()[0], 0);

    sut.process_event(Event::Event{});
    EXPECT_EQ(sut.current_state()[0], 1);
}

TEST_F(DeferEventTest, Condition2ReturnFalse)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Guard::condition2));
        EXPECT_CALL(mock, trace(Guard::condition1));

        // The Event::Event is received, and state transition is triggered.
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace(Action::action1));
        EXPECT_CALL(mock, trace(State::working_state_on_entry));
        // Due to Guard::Condition2 return false, the Event::HandOver isn't 
        // saved, so there is State Transit from WorkingState to SpareState
    }
    StateMachine sut{};
    sut.condition2 = false;
    sut.start();
    sut.process_event(Event::HandOver{});
    EXPECT_EQ(sut.current_state()[0], 0);

    sut.process_event(Event::Event{});
    EXPECT_EQ(sut.current_state()[0], 1);
}
}