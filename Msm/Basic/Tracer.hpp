#pragma once
#include <iostream>
#include <gmock/gmock.h>

class ITracer
{
public:
    virtual void on_entry() const = 0;
    virtual void on_exit() const = 0;
};

class OutputTracer : public ITracer
{
public:
    void on_entry() const override
    {
        std::cout << "on_entry() is called" << std::endl;
    }

    void on_exit() const override
    {
        std::cout << "on_exit() is called" << std::endl;
    }
};

class MockTracer : public ITracer
{
public:
    MOCK_METHOD(void, on_entry, (), (const, override));
    MOCK_METHOD(void, on_exit, (), (const, override));
};