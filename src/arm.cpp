#include "main.h"

ArmHandler::ArmHandler(pros::Controller& controller, pros::MotorGroup& armMotors) :
    controller(controller),
    armMotors(armMotors) {};

void ArmHandler::update() {

    int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
    int armPosition = armMotors.get_position();

    if (fabs(armY) > 5) {
        if (armY > 0 || armPosition > HARD_STOP_POSITION) { 
            armMotors.move(armY);
            isBraked = false; 
            isMovingToPosition = false;
        } else {
            armMotors.brake();
            armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            isBraked = true;
        }
    } else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        if (armPosition < -50) {
            armMotors.move_absolute(0, 50);
            if(armMotors.get_voltage() > 5000) {
                armMotors.move_velocity(0);
            }
        } else {
            int targetPosition = armPosition - 320;
            if (targetPosition < HARD_STOP_POSITION) targetPosition = HARD_STOP_POSITION; 
            armMotors.move_absolute(targetPosition, 50);
        }
        isMovingToPosition = true;
        isBraked = false;
    } else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        isBraked = false;
        isMovingToPosition = false;
    } else if (fabs(armY) <= 5 && !isBraked && !isMovingToPosition) {
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
        isBraked = true;
    }

    if (armPosition < HARD_STOP_POSITION && armY <= 0) {
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
        if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
            armMotors.move_absolute(0, 80); 
        }
    }
}

