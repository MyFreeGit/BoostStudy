#pragma once
#include <string>
#include <memory>
#include <iostream>
namespace Tracer
{
#define TRACE_FUNCTION_CALL(func_name)\
    Tracer::TracerProvider::getTracer().trace(func_name)

class ITracer
{
public:
    virtual void trace(const std::string&) const = 0;
    virtual ~ITracer() {};
};

class LogTracer : public ITracer
{
public:
    void trace(const std::string& str) const override
    {
        std::cout << str << std::endl;
    }
};

using Tracer = std::unique_ptr<ITracer>;
struct TracerProvider
{
public:
    static ITracer& getTracer() {return *tracer;};
    template<typename T, typename = std::enable_if_t<std::is_base_of<ITracer, T>::value>>
    static void setTracer(std::unique_ptr<T> tracer) { TracerProvider::tracer = std::move(tracer);}
private:
    TracerProvider() {};
    static Tracer tracer;
};

} // namespace Tracer