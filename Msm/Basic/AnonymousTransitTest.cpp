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

class AnonymousTransitTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            //|Start             |Event            |Next                 |Action          |Guard
            Row<State::InitState, msm::front::none, State::NextState,     Action::Action1, Guard::Condition1>, 
            Row<State::InitState, msm::front::none, State::AnotherState,  Action::Action2, Guard::Condition2>
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

TEST_F(AnonymousTransitTest, Condition2ReturnTrue)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        // Guard::Condition2 is select first
        EXPECT_CALL(mock, trace(Guard::condition2));

        // Guard::Condition2 return true, trigger exiting InitState
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        // Action2 is performed
        EXPECT_CALL(mock, trace(Action::action2));
        // State is transit to AnotherState
        EXPECT_CALL(mock, trace(State::another_state_on_entry));
    }
    StateMachine sut{};
    sut.condition2 = true;
    sut.start();
    EXPECT_EQ(*sut.current_state(), 2);
}

TEST_F(AnonymousTransitTest, Condition2ReturnFalse)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        // Guard::Condition2 is selected first
        EXPECT_CALL(mock, trace(Guard::condition2));
        // Guard::Condition2 return false, then evaluate Guard::Condition1
        EXPECT_CALL(mock, trace(Guard::condition1));
    
        // Guard::Condition1 return true, trigger exiting InitState
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        // Action1 is performed
        EXPECT_CALL(mock, trace(Action::action1));
        // State is transit to NextState
        EXPECT_CALL(mock, trace(State::next_state_on_entry));
    }
    StateMachine sut{};
    sut.condition2 = false;
    sut.start();
    EXPECT_EQ(*sut.current_state(), 1);
}

TEST_F(AnonymousTransitTest, BothConditionReturnFalse)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        // Guard::Condition2 is selected first
        EXPECT_CALL(mock, trace(Guard::condition2));
        // Guard::Condition2 return false, then evaluate Guard::Condition1
        EXPECT_CALL(mock, trace(Guard::condition1));
        // Both Guard Check fail, no action and state transit performed.
    }
    StateMachine sut{};
    sut.condition1 = false;
    sut.condition2 = false;
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
}

struct StateMachineWithoutActionAndGuard_ : public msm::front::state_machine_def<StateMachineWithoutActionAndGuard_>
{
    // Set initial state
    typedef State::InitState initial_state;
    // Transition table
    struct transition_table : public boost::mpl::vector<
        //|Start             |Event            |Next
        Row<State::InitState, msm::front::none, State::NextState>, // This transit will never be happened. Only for demonstration.
        Row<State::InitState, msm::front::none, State::AnotherState>
    >{};
};
// Pick a back-end
typedef msm::back::state_machine<StateMachineWithoutActionAndGuard_> StateMachineWithoutActionAndGuard;

TEST_F(AnonymousTransitTest, StateMachineWithoutActionAndGuard)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        // State is transit to AnotherState
        EXPECT_CALL(mock, trace(State::another_state_on_entry));
    }
    StateMachineWithoutActionAndGuard sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 2);
}
}