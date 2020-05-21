#include "Tracer.hpp"

namespace Tracer
{
//Tracer TracerProvider::tracer = std::make_unique<::testing::StrictMock<TracerMocker>>();
Tracer TracerProvider::tracer = std::make_unique<LogTracer>();
} 