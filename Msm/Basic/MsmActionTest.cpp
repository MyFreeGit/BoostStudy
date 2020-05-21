#include <gtest/gtest.h>
#include "TracerMocker.hpp"
#include "State.hpp"
#include "Event.hpp"
#include "Action.hpp"
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
/* State transition with action is triggered */
namespace
{
namespace msm = boost::msm;
using msm::front::Row;

class MsmActionTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Action
        void handle_loopback(const Event::Loopback&)
        {
            TRACE_FUNCTION_CALL("handle_loopback");
        }

        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            // Use _row to simplify the code, _row allows omitting action and guard
            //   |Start            |Event           |Next              |Action
            a_row<State::InitState, Event::Loopback, State::InitState,  &StateMachine_::handle_loopback>, // Use a_row to allow function pointer as action handler
            Row  <State::InitState, Event::Stop,     State::EndState,   Action::OnEventStop>, // EventStop triggers the state transition
            Row  <State::InitState, Event::Inner,    msm::front::none,  Action::OnEventInner>
        >{};
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;

protected:
    void SetUp() override
    {
        Tracer::TracerProvider::setTracer(std::make_unique<::testing::StrictMock<Tracer::TracerMocker>>());
    }
};

TEST_F(MsmActionTest, StateTransitWithFunctionEventHandler)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); // on_exit() is called before handling the event message.
        EXPECT_CALL(mock, trace("handle_loopback"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Loopback());
    EXPECT_EQ(sut.current_state()[0], 0);
}

TEST_F(MsmActionTest, StateTransitWithFunctorHandler)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); // on_exit() is called before handling the event message.
        EXPECT_CALL(mock, trace(Action::on_event_stop));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Stop());
    EXPECT_EQ(sut.current_state()[0], 1);
}

TEST_F(MsmActionTest, InnerStateTransitWithFunctorHandler)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Action::on_event_inner));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    sut.process_event(Event::Inner());
    EXPECT_EQ(sut.current_state()[0], 0);
}
}