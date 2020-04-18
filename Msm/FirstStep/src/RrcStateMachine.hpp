#include <iostream>
#include <boost/msm/back/state_machine.hpp>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace {
    namespace msm = boost::msm;
    namespace msmf = boost::msm::front;
    namespace mpl = boost::mpl;

    // ----- Events
    struct RrcConnect {};
    struct RrcSuspend {};
    struct RrcResume {};
    struct ConnectionFailure {};
    struct RrcRelease {};

    // ----- State machine

}