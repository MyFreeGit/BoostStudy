#include "TracerMocker.hpp"

namespace Tracer
{
Tracer TracerProvider::tracer = std::make_unique<::testing::StrictMock<TracerMocker>>();
} 