#include "main.h"

OpticsHandler::OpticsHandler(pros::Optical &opticalSensor, pros::MotorGroup &intake, pros::Controller &controller, teamColour colour) : opticalSensor(opticalSensor), intake(intake), controller(controller), colour(colour) {};

void OpticsHandler::colourFilter() {
    const int STALL_TIME_MS = 100; // stall time (ms)
    opticalSensor.set_led_pwm(100);

    while (true) {
        //pros::c::optical_rgb_s_t rgb = opticalSensor.get_rgb();
        int hue = opticalSensor.get_hue();
        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
            if ((colour == teamColour::red && hue >= 200 && hue <= 230) ||
                (colour == teamColour::blue && (hue >= 350 || hue <= 20))){
                pros::delay(30);
                intake.move(0); // stop the intake
                pros::delay(STALL_TIME_MS); // stall for 250 ms
                intake.move(128); // resume intake
                pros::delay(250);
            }
            else if(colour == teamColour::neutral) {
                intake.move(128); // resume intake
            }
            else{
                intake.move(128);
            }
        }
        else if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            intake.move(-128);
        }
        else{
            intake.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
            intake.brake();
        }

        pros::delay(10); 
    }
}