#include "main.h"

OpticsHandler::OpticsHandler(pros::Optical &opticalSensor, pros::MotorGroup &intake, pros::MotorGroup &roller, pros::Controller &controller, teamColour colour) : opticalSensor(opticalSensor), intake(intake), roller(roller),controller(controller), colour(colour) {};

void OpticsHandler::colourFilter() {


    while (true) {

        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            filterParameters();
        }
        
        else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            intake.move(-128);
        }
        else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)){
            roller.move(128);
        }
        else{
            if(!isAuton){
                intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                intake.brake();
            }
            else if(isAuton){
                filterParameters();
            }
        }

        notifier = false;

        pros::delay(10); 
    }
}

void OpticsHandler::filterParameters(){
    const int STALL_TIME_MS = 150; // stall time (ms)
    opticalSensor.set_led_pwm(100);
    int hue = opticalSensor.get_hue();
    double intakeVelocity = intake.get_actual_velocity();
    if (((colour == teamColour::red && hue >= 200 && hue <= 230) || (colour == teamColour::blue && (hue >= 350 || hue <= 20))) && !disable){
        notifier = true;
        pros::delay(80);
        intake.move(0); // stop the intake
        pros::delay(150); // stall for 250 ms
        intake.move(128); // resume intake
        pros::delay(200);
    }
    else if(!isAuton){
        intake.move(128);
    }
}
