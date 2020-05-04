#pragma once
#include <gmock/gmock.h>
#include <type_traits>

#define TRACE_FUNCTION_CALL(func_name)\
    Tracer::TracerProvider::getTracer().trace(func_name)

namespace Tracer
{
class ITracer
{
public:
    virtual void trace(std::string) const = 0;
    virtual ~ITracer() {};
};

class TracerMocker : public ITracer
{
public:
    MOCK_METHOD(void, trace, (std::string), (const, override));
};

using Tracer = std::unique_ptr<ITracer>;
struct TracerProvider
{
public:
    static ITracer& getTracer() {return *tracer;};
    static TracerMocker& getMocker() {return dynamic_cast<TracerMocker&>(*tracer);};
    template<typename T, typename = std::enable_if_t<std::is_base_of<ITracer, T>::value>>
    static void setTracer(std::unique_ptr<T> tracer) { TracerProvider::tracer = std::move(tracer);}
private:
    TracerProvider() {};
    static Tracer tracer;
};
} // namespace Tracer