#include "main.h"

ArmHandler::ArmHandler(pros::Controller& controller, pros::MotorGroup& armMotors) :
    controller(controller),
    armMotors(armMotors) {}

void ArmHandler::update() {
    int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

    if (fabs(armY) > 5) {
        // Allow free movement based on joystick input
        armMotors.move(armY);
        isBraked = false; 
        isMovingToPosition = false;
    } else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        // Reset the motor's position to 0 and move 320 degrees relative to the new zero
        armMotors.tare_position(); // Reset encoder position to 0
        armMotors.move_relative(-320, 50); // Move 320 degrees relative at 50 rpm
        isMovingToPosition = true;
        isBraked = false;
    } else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        // Set brake mode to coast when button L1 is pressed
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        isBraked = false;
        isMovingToPosition = false;
    } else if (fabs(armY) <= 5 && !isBraked && !isMovingToPosition) {
        // Engage brake hold when arm is idle
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
        isBraked = true;
    }
}