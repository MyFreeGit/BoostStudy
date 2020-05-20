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

class OrthogonalRegionTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Set initial state and define two Orthogonal region
        typedef boost::mpl::vector<State::InitState, State::WorkingState> initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            //|Start                |Event           |Next
            Row<State::InitState,    Event::Event,    State::NextState>,
            Row<State::NextState,    Event::Stop,     State::EndState>,
            Row<State::WorkingState, Event::HandOver, State::SpareState>,
            Row<State::SpareState,   Event::HandOver, State::WorkingState>
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

TEST_F(OrthogonalRegionTest, NormalTest)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::working_state_on_entry));

        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace(State::next_state_on_entry));

        EXPECT_CALL(mock, trace(State::spare_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    EXPECT_EQ(sut.current_state()[1], 2);
    sut.process_event(Event::Event{});
    EXPECT_EQ(sut.current_state()[0], 1);
    EXPECT_EQ(sut.current_state()[1], 2);
    sut.process_event(Event::HandOver{});
    EXPECT_EQ(sut.current_state()[0], 1);
    EXPECT_EQ(sut.current_state()[1], 3);
}

TEST_F(OrthogonalRegionTest, OneRegionReachEndState)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::TracerProvider::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::working_state_on_entry));

        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace(State::next_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(sut.current_state()[0], 0);
    EXPECT_EQ(sut.current_state()[1], 2);
    sut.process_event(Event::Event{});
    EXPECT_EQ(sut.current_state()[0], 1);
    EXPECT_EQ(sut.current_state()[1], 2);
    sut.process_event(Event::Stop{});
    EXPECT_EQ(sut.current_state()[0], 4);
    EXPECT_EQ(sut.current_state()[1], 2);

    // When One region hits the end state, the StateMachine is stoped and
    // no message is processed.
    sut.process_event(Event::HandOver{});
    EXPECT_EQ(sut.current_state()[0], 4);
    EXPECT_EQ(sut.current_state()[1], 2);
}
}
