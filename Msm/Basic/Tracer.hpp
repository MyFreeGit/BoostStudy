#pragma once
#include <iostream>
#include <gmock/gmock.h>
#include <spdlog/spdlog.h>
#define TRACE_FUNCTION_CALL()\
    spdlog::info("{} is called.",__PRETTY_FUNCTION__)

class ITracer
{
public:
    virtual void state_machine_on_entry() const = 0;
    virtual void state_machine_on_exit() const = 0;
    virtual void init_state_on_entry() const = 0;
    virtual void init_state_on_exit() const = 0;
    virtual void on_event_stop() const = 0;
    virtual void on_event() const = 0;
    virtual ~ITracer() {};  // Need to reduce the mock object leak. Need for derived class to override it.
};

class MockTracer : public ITracer
{
public:
    MOCK_METHOD(void, state_machine_on_entry, (), (const, override));
    MOCK_METHOD(void, state_machine_on_exit, (), (const, override));
    MOCK_METHOD(void, init_state_on_entry, (), (const, override));
    MOCK_METHOD(void, init_state_on_exit, (), (const, override));
    MOCK_METHOD(void, on_event_stop, (), (const, override));
    MOCK_METHOD(void, on_event, (), (const, override));
};

class OutputTracer : public ITracer
{
public:
    void state_machine_on_entry() const override
    {
        TRACE_FUNCTION_CALL();
    }

    void state_machine_on_exit() const override
    {
        TRACE_FUNCTION_CALL();
    }

    void init_state_on_entry() const override
    {
        TRACE_FUNCTION_CALL();
    }

    void init_state_on_exit() const override
    {
        TRACE_FUNCTION_CALL();
    }

    void on_event_stop() const override
    {
        TRACE_FUNCTION_CALL();
    }

    void on_event() const override
    {
        TRACE_FUNCTION_CALL();
    }
};