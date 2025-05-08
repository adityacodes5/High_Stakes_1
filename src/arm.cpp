#include "main.h"

ArmHandler::ArmHandler(pros::Controller& controller, pros::MotorGroup& armMotors, pros::Rotation &armRotation,  double kP, double settleError) :
    controller(controller),
    armMotors(armMotors),
    armRotation(armRotation),
    kP(kP),
    settleError(settleError)
{}

bool armUp = false;
void ArmHandler::update() {
    armRotation.set_position(0);
    initialPosition = armRotation.get_position();
    while (true) {
        int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

        if (fabs(armY) > 5) {
            // Allow free movement based on joystick input
            armMotors.move(armY);
            armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            isBraked = false; 
            isMovingToPosition = false;
        } else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
            // Reset the motor's position to 0 and move 320 degrees relative to the new zero
            armMotors.tare_position(); // Reset encoder position to 0
            //armMotors.move_relative(-355, 70); // Move 320 degrees relative at 50 rpm
            //armMotors.move_relative(-300, 80);
            moveArm(-205, 1000, true);

            isMovingToPosition = true;
            isBraked = false;
            //armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        } else if (fabs(armY) <= 5 && !isBraked && !isMovingToPosition) {
            // Engage brake hold when arm is idle
            armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            armMotors.brake();
            isBraked = true;
        }
        pros::delay(10);
    }
}
double ArmHandler::compute(double error) {
    double output = kP * error;

    if (fabs(error) < settleError) { 
        timeSpentSettled += 10;
    } 
    else {
        timeSpentSettled = 0;
    }

    

    timeSpentRunning += 10; // ✅ Ensure timeSpentRunning is increased inside compute()

    // ✅ Ensure output is strong enough to move the arm
    if (fabs(output) < 10) { 
        output = (output > 0) ? 10 : -10; // Minimum motor power threshold
    }

    // ✅ Limit motor power to valid range
    if (output > 127) output = 127;
    if (output < -127) output = -127;

    return output;
}
bool ArmHandler::isSettled(){

    if(timeSpentSettled > settleTime){
        return true;
    }

    else if(timeSpentRunning >= timeout && timeout != 0){ // If timeout does equal 0, the move will never actually time out. Setting timeout to 0 is the same as setting it to infinity
        return true;
    }


    else{
        return false;
    }
}

void ArmHandler::resetTimers(){
    this-> settleError = 0;
    this-> settleTime = 0;
    this-> timeout = 0;
    this-> accumulatedError = 0;
    this-> previousError = 0;
    this-> output = 0;
    this-> timeSpentSettled = 0;
    this-> timeSpentRunning = 0;

    this->startedMoving = false;
}
void ArmHandler::moveArm(double targetPos, double timeout, bool relative){
    resetTimers();

    this->timeout = timeout;
    int loopTime = 10;  // Each loop runs every 10ms
    targetPos = 38.3*targetPos;
    targetPos = targetPos + initialPosition;

    while (!isSettled() || !startedMoving) {
        double error = targetPos - armRotation.get_position();
        double output = compute(error);
        armMotors.move(output);

        if(fabs(output) > 10){
            startedMoving = true;
        }

        pros::delay(loopTime);
        timeSpentRunning += loopTime;  

        if (timeSpentRunning >= timeout) { 
            armMotors.brake();
            return;
        }
    }

    armMotors.brake();
}