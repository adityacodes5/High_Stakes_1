#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"


gameMode currentMode = gameMode::testing;

pros::Controller controller(pros::E_CONTROLLER_MASTER);

pros::MotorGroup leftMotors({-3, -2, -4},pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({8, 9, 10}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)
pros::MotorGroup armMotors({11, -20}, pros::MotorGearset::green);
pros::MotorGroup intake({19}, pros::MotorGearset::blue);

pros::adi::Pneumatics clampPiston = pros::adi::Pneumatics('A', false);
pros::adi::Pneumatics doinkerPiston = pros::adi::Pneumatics('G', false);

pros::Optical opticalSensor(18);
pros::Imu imu(6);

pros::adi::DigitalIn bumper = pros::adi::DigitalIn('H');


OpticsHandler opticsHandler(opticalSensor, intake, controller, teamColour::neutral);
ArmHandler armHandler(controller, armMotors);


lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              8 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings linearController(10, // proportional gain (kP)
                                            0, // integral gain (kI)
                                            3, // derivative gain (kD)
                                            3, // anti windup
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(2, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             10, // derivative gain (kD)
                                             3, // anti windup
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
);

// sensors for odometry
lemlib::OdomSensors sensors(nullptr,
							nullptr,
							nullptr,
							nullptr,
                            &imu // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */


void initialize() {
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(true); // calibrate sensors
    chassis.resetLocalPosition(); // reset odometry

    if(pros::competition::is_field_control()) {
        gameMode currentMode = gameMode::tournament;
    }

    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, "Arm Position: %f", armMotors.get_position());


            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());

            pros::delay(50);
        }
    });
}


void disabled() {}

void competition_initialize() {}


void recoverIntake(){
    intakeStallRecovery(intake);
}

void colourSort(){
    opticsHandler.colourFilter();
}

void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    blueLeft(); //pass function of selected autonomous

}




/**
 * Runs in driver control
 */
void opcontrol() {
    chassis.setPose(0, 0, 0);
    bool clampTriggered = false;
    bool intakeTriggered = false;
    bool doinkerTriggered = false;

    pros::Task colourSortTask =  pros::Task(colourSort); 

    while (true) {

        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

        chassis.arcade(leftX, leftY);

        armHandler.update();

        if(controller.get_digital(DIGITAL_Y) && !clampTriggered) {
            clampTriggered = true;
            clampPiston.toggle();
        }
        if(!controller.get_digital(DIGITAL_Y) && clampTriggered) {
            clampTriggered = false;
        }

        if(controller.get_digital(DIGITAL_A) && !doinkerTriggered) {
            doinkerTriggered = true;
            doinkerPiston.toggle();
        }

        if(!controller.get_digital(DIGITAL_A) && doinkerTriggered) {
            doinkerTriggered = false;
        }

        
        pros::delay(10);

    }
}


