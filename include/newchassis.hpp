#pragma once

#include "lemlib/api.hpp" // Include lemlib for Pose, Chassis, etc.
#include <cmath>          // For std::sin, std::cos

// Declare the function prototype, taking the chassis object as an argument
void moveLinear(lemlib::Chassis& chassis, // Pass the chassis object
                double dist,
                int timeout,
                lemlib::MoveToPointParams p = {}, // Keep optional params
                bool async = true);

void movePolar(lemlib::Chassis& chassis, // Pass the chassis object
               double dist,
               double angle,
               int timeout,
               lemlib::MoveToPointParams p = {},
               bool radians = false,
               bool async = true);

void shimmy(lemlib::Chassis& chassis, // Pass the chassis object
            int timeout,
            int delay = 100);

// Add declarations for any other custom chassis functions here
