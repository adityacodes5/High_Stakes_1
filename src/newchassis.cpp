#include "main.h"

// Define the moveLinear function
// Moves the chassis forward or backward in a straight line relative to its current heading.
void moveLinear(lemlib::Chassis& chassis, double dist, int timeout, lemlib::MoveToPointParams p, bool async) {
    // Wait for any previous asynchronous movements to complete unless this call is also async
    if (!async) {
        chassis.waitUntilDone();
    }

    // Get the current pose (position and heading)
    // Assuming getPose() returns heading in degrees
    lemlib::Pose pose = chassis.getPose(true); // Use true for global pose if needed, false for local

    // Set movement direction based on the sign of dist
    p.forwards = (dist >= 0);
    //double absDist = std::fabs(dist); // Use absolute distance for calculation
    double absDist = dist; // Use the distance directly, as it is already signed   
    // Calculate target coordinates based on current pose and distance
    // Convert heading (pose.theta) to radians for std::sin/cos
    double headingRad = pose.theta * M_PI / 180.0;
    double targetX = pose.x + std::sin(headingRad) * absDist;
    double targetY = pose.y + std::cos(headingRad) * absDist;

    // Use the passed-in chassis object to call its member functions
    chassis.moveToPoint(targetX, targetY, timeout, p, async);
}

// Define the movePolar function
// Moves the chassis towards a point defined by a distance and angle relative to the current position.
// The angle is measured from the positive Y-axis (field-relative) by default.
void movePolar(lemlib::Chassis& chassis, double dist, double angle, int timeout, lemlib::MoveToPointParams p, bool radians, bool async) {
    // Wait for any previous asynchronous movements to complete unless this call is also async
    if (!async) {
        chassis.waitUntilDone();
    }

    // Get the current pose
    lemlib::Pose pose = chassis.getPose(true); // Use true for global pose

    // Set movement direction based on the sign of dist
    p.forwards = (dist >= 0);
    double absDist = std::fabs(dist); // Use absolute distance for calculation

    // Convert angle to radians if it was provided in degrees
    double angleRad = radians ? angle : angle * M_PI / 180.0;

    // Calculate target coordinates based on current pose, distance, and angle
    double targetX = pose.x + std::sin(angleRad) * absDist;
    double targetY = pose.y + std::cos(angleRad) * absDist;

    // Use the passed-in chassis object to call its member functions
    chassis.moveToPoint(targetX, targetY, timeout, p, async);
}

// Define the shimmy function
// Attempts to rock the chassis back and forth rapidly.
// NOTE: This implementation depends on accessing the drivetrain motors.
// If lemlib::Chassis does not provide public access to its drivetrain or motors,
// this function might not work as intended or might need access to the global motor pointers.
void shimmy(lemlib::Chassis& chassis, int timeout, int delay) {
    // Cancel any ongoing chassis movements
    chassis.cancelAllMotions();
    chassis.cancelMotion(); // LemLib might have separate motion cancellation

    auto start = pros::millis();

    // Attempt to access drivetrain motors - THIS IS SPECULATIVE
    // Replace 'chassis.drivetrain.leftMotors' and 'chassis.drivetrain.rightMotors'
    // with the actual way to access motors if different or possible.
    // If direct access isn't possible, this function needs rethinking.

    // Example using hypothetical direct access (MAY NOT WORK):
    /*
    auto* leftMotors = chassis.drivetrain.leftMotors; // Hypothetical access
    auto* rightMotors = chassis.drivetrain.rightMotors; // Hypothetical access

    if (leftMotors && rightMotors) { // Check if pointers are valid
         while (pros::millis() < start + timeout) {
             leftMotors->move_voltage(-12000);
             rightMotors->move_voltage(12000);
             pros::delay(delay);
             leftMotors->move_voltage(12000);
             rightMotors->move_voltage(-12000);
             pros::delay(delay);
         }
         // Stop motors after timeout
         leftMotors->move_voltage(0);
         rightMotors->move_voltage(0);
    } else {
         // Log a warning if motors couldn't be accessed
         lemlib::logger::warn("shimmy(): Could not access drivetrain motors via lemlib::Chassis object.");
    }
    */

    // If direct access is not possible, log a warning.
    // You might need to use the global motor group pointers directly if they exist.
    // Placeholder delay to simulate the action time if motors can't be controlled
    pros::delay(timeout);


}

// Add definitions for any other custom chassis functions here
