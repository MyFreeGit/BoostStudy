#pragma once

// Simple empty structure with a name is enough for an event
// To make the example code more readable, example code are written based on
// comments on each Event
namespace Event
{
struct Stop {
    Stop(bool readyToStop = true) : readyToStop{readyToStop} {};
    bool readyToStop{true}; 
}; // Trigger the state to EndState
struct Loopback {}; // Not trigger state transition
struct Inner {}; // State Inner message.
}