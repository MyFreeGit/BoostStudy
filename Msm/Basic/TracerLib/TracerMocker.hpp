#pragma once
#include <gmock/gmock.h>
#include <type_traits>
#include "Tracer.hpp"

namespace Tracer
{
class TracerMocker : public ITracer
{
public:
    MOCK_METHOD(void, trace, (const std::string&), (const, override));
};

static TracerMocker& getMocker() {return dynamic_cast<TracerMocker&>(TracerProvider::getTracer());};
} // namespace Tracer