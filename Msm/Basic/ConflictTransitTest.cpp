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

namespace Action
{
auto constexpr action1 = "action1";
auto constexpr action2 = "action2";
struct Action1
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    void operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(action1);
    }
};
struct Action2
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    void operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(action2);
    }
};
}; // namespace Action

namespace Guard
{
auto constexpr condition1 = "condition1";
auto constexpr condition2 = "condition2";
struct Condition1
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    bool operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(condition1);
        return fsm.condition1;
    }
};
struct Condition2
{
    template <class Event, class Fsm, class SourceState, class TargetState>
    bool operator()(Event const &, Fsm &fsm, SourceState &, TargetState &) const
    {
        TRACE_FUNCTION_CALL(condition2);
        return fsm.condition2;
    }
};
}; // namespace Guard
class ConflictTransitTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            // Use _row to simplify the code, _row allows omitting action and guard
            //|Start             |Event            |Next                 |Action          |Guard
            Row<State::InitState, Event::Event,     State::NextState,     Action::Action1, Guard::Condition1>, 
            Row<State::InitState, Event::Event,     State::AnotherState,  Action::Action2, Guard::Condition2>
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

TEST_F(ConflictTransitTest, Condition2ReturnTrue)
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
        EXPECT_CALL(mock, trace(State::anther_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    sut.condition2 = true;
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event::Event{});
    EXPECT_EQ(*sut.current_state(), 2);
}

TEST_F(ConflictTransitTest, Condition2ReturnFalse)
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
    sut.start();
    sut.condition2 = false;
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event::Event{});
    EXPECT_EQ(*sut.current_state(), 1);
}
}