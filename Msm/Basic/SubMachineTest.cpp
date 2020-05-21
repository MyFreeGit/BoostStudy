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

class SubMachineTest : public ::testing::Test
{
public:
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // Define SubMachine
        struct SubMachine_ : public msm::front::state_machine_def<SubMachine_>
        {
            // Set initial state
            typedef State::AnotherState initial_state;
            // Transition table
            struct transition_table : public boost::mpl::vector<
                //|Start                |Event        |Next
                Row<State::AnotherState, Event::Inner, State::NextState>,
                Row<State::NextState,    Event::Stop,  State::EndState>
            >{};
        };
        typedef msm::back::state_machine<SubMachine_> SubMachine;

        // Set initial state
        typedef State::InitState initial_state;
        // Transition table
        struct transition_table : public boost::mpl::vector<
            //|Start             |Event        |Next
            Row<State::InitState, Event::Event, SubMachine>
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

TEST_F(SubMachineTest, Condition2ReturnTrue)
{
    {
        ::testing::InSequence s;
        auto& mock = Tracer::getMocker();
        EXPECT_CALL(mock, trace(State::init_state_on_entry));
        EXPECT_CALL(mock, trace(State::init_state_on_exit));
        EXPECT_CALL(mock, trace(State::another_state_on_entry));
        EXPECT_CALL(mock, trace(State::next_state_on_entry));
    }
    StateMachine sut{};
    sut.start();
    sut.process_event(Event::Event{});
    EXPECT_EQ(sut.current_state()[0], 1);

    sut.process_event(Event::Inner{});
    EXPECT_EQ(sut.current_state()[0], 1);
}
}