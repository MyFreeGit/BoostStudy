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

class MsmActionTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Action
        void handle_event(const Event&)
        {
            TRACE_FUNCTION_CALL("handle_event()");
        }

        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            // Use _row to simplify the code, _row allows omitting action and guard
            //              |Start            |Event     |Next             |Action
                       a_row<State::InitState, Event,     State::InitState, &StateMachine_::handle_event>, // Use a_row can left the guard empty, save the typing
            msm::front::Row <State::InitState, EventStop, State::EndState,   Action::OnEventStop> // Event will not trigger the state transition
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
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace("handle_event()"));
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event());
    EXPECT_EQ(*sut.current_state(), 0);
}

TEST_F(MsmActionTest, StateTransitWithFunctorHandler)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); // on_exit() is called when handling the event message.
        EXPECT_CALL(mock, trace(Action::on_event_stop)); // on_entry() is called when state is transitted back to InitState
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(EventStop());
    EXPECT_EQ(*sut.current_state(), 1);
}
}