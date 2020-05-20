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

class MsmGuardTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            //|Start             |Event            |Next             |Action             |Guard
            Row<State::InitState, Event::Loopback,  State::InitState, msm::front::none>, 
            Row<State::InitState, Event::Stop,      State::EndState,  Action::OnEventStop, Guard::EventStopGuard>
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

TEST_F(MsmGuardTest, GuardReturnTrue)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Guard::event_stop_guard));
        // Guard Check passed, call state exit and action handling correspondingly
        EXPECT_CALL(mock, trace(State::init_state_on_exit)); 
        EXPECT_CALL(mock, trace(Action::on_event_stop));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event::Stop(true));
    EXPECT_EQ(*sut.current_state(), 1);
}

TEST_F(MsmGuardTest, GuardReturnFalse)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(Guard::event_stop_guard));
        // Guard Check fails, nothing happened.
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(Event::Stop(false));
    EXPECT_EQ(*sut.current_state(), 0);
}
}