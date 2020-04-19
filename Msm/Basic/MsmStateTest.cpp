#include <gtest/gtest.h>
#include "Tracer.hpp"
#include <memory>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace
{
namespace msm = boost::msm;

class Tracer
{
public:
    MOCK_METHOD(void, on_entry, ());
    MOCK_METHOD(void, on_exit, ());
};

class MsmStateTest : public ::testing::Test
{
public:
    // Simple empty structure with a name is enough for an event
    struct event
    {
    };
    struct StateMachine_ : public msm::front::state_machine_def<StateMachine_>
    {
        // States with entries
        struct InitState : public msm::front::state<>
        {
            // Entry action
            template <class Event, class Fsm>
            void on_entry(Event const &, Fsm &) const
            {
                tracer->on_entry();
            }
            // Exit action
            template <class Event, class Fsm>
            void on_exit(Event const &, Fsm &) const
            {
                tracer->on_exit();
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
            //              Start      Event   Next         Action            Guard
            msm::front::Row<InitState, event,  EndState,    msm::front::none, msm::front::none>
        >{};

       static std::unique_ptr<ITracer> tracer;
    };

    // Pick a back-end
    typedef msm::back::state_machine<StateMachine_> StateMachine;

protected:
};
std::unique_ptr<ITracer> MsmStateTest::StateMachine_::tracer = std::make_unique<OutputTracer>();

TEST_F(MsmStateTest, StateElement)
{
    {
        ::testing::InSequence s;
        using ::testing::StrictMock;
        auto tracer = std::make_unique<StrictMock<MockTracer>>();
        EXPECT_CALL(*tracer, on_entry());
        EXPECT_CALL(*tracer, on_exit());
        MsmStateTest::StateMachine_::tracer = std::move(tracer);
    }
    StateMachine sut{};
    sut.start();
    EXPECT_EQ(*sut.current_state(), 0);
    sut.process_event(event());
    EXPECT_EQ(*sut.current_state(), 1);
}
}