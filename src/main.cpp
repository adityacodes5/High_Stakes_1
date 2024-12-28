#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({-3, -2, -4},
                            pros::MotorGearset::blue); // left motor group - ports 3 (reversed), 4, 5 (reversed)
pros::MotorGroup rightMotors({8, 9, 10}, pros::MotorGearset::blue); // right motor group - ports 6, 7, 9 (reversed)
pros::MotorGroup armMotors({11, -20}, pros::MotorGearset::green);
pros::MotorGroup intake({19}, pros::MotorGearset::blue);

pros::adi::Pneumatics clampPiston = pros::adi::Pneumatics('A', false);
pros::adi::Pneumatics doinkerPiston = pros::adi::Pneumatics('G', false);

pros::Optical opticalSensor(18);



// Inertial Sensor on port 10
pros::Imu imu(6);

OpticsHandler opticsHandler(opticalSensor, intake, controller, teamColour::blue);

//// tracking wheels
//// horizontal tracking wheel encoder. Rotation sensor, port 20, not reversed
//pros::Rotation horizontalEnc(20);
//// vertical tracking wheel encoder. Rotation sensor, port 11, reversed
//pros::Rotation verticalEnc(-11);
//// horizontal tracking wheel. 2.75" diameter, 5.75" offset, back of the robot (negative)
//lemlib::TrackingWheel horizontal(&horizontalEnc, lemlib::Omniwheel::NEW_275, -5.75);
//// vertical tracking wheel. 2.75" diameter, 2.5" offset, left of the robot (negative)
//lemlib::TrackingWheel vertical(&verticalEnc, lemlib::Omniwheel::NEW_275, -2.5);

// drivetrain settings
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


    // the default rate is 50. however, if you need to change the rate, you
    // can do the following.
    // lemlib::bufferedStdout().setRate(...);
    // If you use bluetooth or a wired connection, you will want to have a rate of 10ms

    // for more information on how the formatting for the loggers
    // works, refer to the fmtlib docs

    // thread to for brain screen and position logging
    pros::Task screenTask([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, "%f", armMotors.get_position());
            // log position telemetry
            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());

            //switch (autonomous_mode) {
            //    case R_RED_RR:
            //        pros::lcd::print(3, "Mode: Auto");
            //        break;
            //    case lemlib::Chassis::Mode::DRIVER:
            //        pros::lcd::print(3, "Mode: Driver");
            //        break;
            //    case lemlib::Chassis::Mode::TEST:
            // delay to save resources
            pros::delay(50);
        }
    });
}

/**
 * Runs while the robot is disabled
 */
void disabled() {}

/**
 * runs after initialize if the robot is connected to field control
 */
void competition_initialize() {}

// get a path used for pure pursuit
// this needs to be put outside a function
/**
 * Runs during auto
 *
 * This is an example autonomous routine which demonstrates a lot of the features LemLib has to offer
 */

void blueLeft(){
    chassis.moveToPose(-2, -24, 30, 4000, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(500);
    clampPiston.toggle();   
    pros::delay(250);
    intake.move(128);
    pros::delay(250);
    chassis.moveToPose(24, -31.5, 90, 4000, {.maxSpeed=50});
    chassis.waitUntilDone();
    chassis.turnToHeading(270, 2000);
    chassis.moveToPoint(-32, -32, 4000, {});
    chassis.waitUntilDone();
    chassis.moveToPoint(-34, -32, 4000);
}
void redRight(){
    chassis.moveToPose(2, -24, 330, 4000, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(500);
    clampPiston.toggle();   
    pros::delay(250);
    intake.move(128);
    pros::delay(250);
    chassis.moveToPose(-24, -31.5, 270, 4000, {.maxSpeed=50});
    chassis.waitUntilDone();
    chassis.turnToHeading(90, 2000);
    chassis.moveToPoint(32, -32, 4000, {});
    chassis.waitUntilDone();
    chassis.moveToPoint(34, -32, 4000);

}
void blueRight(){
    chassis.moveToPose(2, -24, 330, 4000, {.forwards = false});
    chassis.moveToPoint(5, -27, 4000, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(500);
    clampPiston.toggle();   
    pros::delay(250);
    intake.move(128);
    pros::delay(250);
    chassis.moveToPoint(-22, -28, 4000, {.maxSpeed = 60});
    chassis.moveToPose(-22, -45.5, 180, 4000, {.maxSpeed = 55});

}
void redLeft(){
    chassis.moveToPose(-2, -24, 30, 4000, {.forwards = false});
    chassis.moveToPoint(-5, -27, 4000, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(500);
    clampPiston.toggle();   
    pros::delay(250);
    intake.move(128);
    pros::delay(250);
    chassis.moveToPoint(22, -28, 4000, {.maxSpeed = 60});
    chassis.moveToPose(22, -45.5, 180, 4000, {.maxSpeed = 55});

}
void progSkills(){
    chassis.moveToPose(6.5, -20, 0, 4000, {.forwards = false});
    chassis.moveToPoint(6.5, -25, 4000, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    pros::delay(250);
    intake.move(128);
    pros::delay(250);
    chassis.turnToHeading(90, 2000);
    chassis.moveToPoint(30, -30, 4000, {.maxSpeed = 100, .minSpeed = 40});
    chassis.moveToPose(30, -54, 180, 4000, {.maxSpeed = 100, .minSpeed = 40});
    chassis.moveToPoint(8, -54, 4000, {.maxSpeed = 100, .minSpeed = 40});
    chassis.moveToPose(-4, -54, 270, 4000, {.maxSpeed = 100, .minSpeed = 40});
    chassis.moveToPoint(9, -63, 4000, {.maxSpeed = 100, .minSpeed = 40});
    chassis.moveToPose(-10, -72, 45, 4000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntil(16);
    clampPiston.toggle();
    //chassis.moveToPose(6.5, -24, 0, 4000, {.forwards = true});

    chassis.moveToPose(6.5, 6, 180, 4000, {.forwards = false});
    chassis.moveToPoint(6.5, 18, 4000, {.forwards = false, .maxSpeed = 50});
    chassis.waitUntilDone();
    clampPiston.toggle();
    //intake.move(128);
    //chassis.turnToHeading(90, 2000);
    //chassis.moveToPoint(30, 18, 4000, {.maxSpeed = 60});
    //chassis.moveToPose(30, 42, 180, 4000, {.maxSpeed = 60});
    //chassis.moveToPoint(8, 42, 4000, {.maxSpeed = 60});
    //chassis.moveToPose(-4, 42, 270, 4000, {.maxSpeed = 60});
    //chassis.moveToPoint(9, 51, 4000, {.maxSpeed = 60});
    chassis.moveToPose(-10, 60, 135, 4000, {.forwards = false, .maxSpeed = 100, .minSpeed = 50});
    //chassis.waitUntil(16);
    chassis.waitUntilDone();
    clampPiston.toggle();

    chassis.moveToPoint(0, 40, 4000);
    chassis.moveToPoint(85, 50, 4000);
    chassis.moveToPoint(100, -8, 4000, {.forwards = false});
    chassis.moveToPose(128, 60, 315, 4000, {.forwards = false, .maxSpeed = 60});

    chassis.moveToPoint(116, -48, 4000);
    chassis.turnToHeading(0, 2000);
    chassis.moveToPose(128, -72, 225, 4000, {.forwards = false, .maxSpeed = 60});
}

void recoverIntake(){
    intakeStallRecovery(intake);
}

void colourSort(){
    opticsHandler.colourFilter();
}

void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    //chassis.resetLocalPosition();  // Reset odometry
    //imu.reset(); // Reset the IMU
    //pros::delay(2000);
    //chassis.setPose(0, 0, 0);

    //leftMotors.move(100);
    //rightMotors.move(-100);
    //pros::delay(300);
    //leftMotors.brake();
    //rightMotors.brake();
//
    // Move to the target position with a heading of 90 degrees
    //RED RIGHT
    //redRight();
    //blueLeft();
    blueRight();
    //redLeft();
    //chassis.setPose({-55, 7.5, 270});
    



}




/**
 * Runs in driver control
 */
void opcontrol() {
    chassis.setPose(0, 0, 0);
    bool clampTriggered = false;
    bool intakeTriggered = false;
    bool doinkerTriggered = false;
    bool armTriggered = false;

    bool isBraked = false; // Flag to track if the brake is set
    bool isMovingToPosition = false; // Flag to track if the arm is moving to a set position
    const int HARD_STOP_POSITION = -2000; // Hard stop limit

    pros::Task colourSortTask =  pros::Task(colourSort); 

    // controller
    // loop to continuously update motors
    while (true) {
        // get joystick positions

		printf("Hello World!");
        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
        // move the chassis with curvature drive
        chassis.arcade(leftX, leftY);

        int armPosition = armMotors.get_position(); // Get the current motor position
        
        // 🟢 Joystick Movement Logic (Allow movement up and restrict movement down past -2000)
    // 🟢 Joystick Movement Logic (Allow movement up, restrict movement down at -2000)
    if (fabs(armY) > 5) {
        if (armY > 0 || armPosition > HARD_STOP_POSITION) { 
            // If moving up (armY > 0) or position is above -2000, allow movement
            armMotors.move(armY);
            isBraked = false; // Reset the brake flag since we're actively moving
            isMovingToPosition = false; // Cancel any active movement triggered by X
        } else {
            // If position is at the hard stop and joystick is moving down, stop the motor
            armMotors.brake();
            armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            isBraked = true; // Set the brake to prevent future calls
        }
    } 
    // 🟢 X Button Logic (Move the arm up or down but respect hard stop)
    else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
        if (armTriggered || armPosition < -50) {
            armMotors.move_absolute(0, 50); // Move to 0 absolute position
            if(armMotors.get_voltage() > 5000) {
                armMotors.move_velocity(0);
            }
        } else {
            // Calculate the target position for move_relative
            int targetPosition = armPosition - 320;
            if (targetPosition < HARD_STOP_POSITION) targetPosition = HARD_STOP_POSITION; // Clamp to -2000
            armMotors.move_absolute(targetPosition, 50); // Move to the clamped position
        }
        armTriggered = !armTriggered; // Toggle the state
        isMovingToPosition = true; // Indicate that we are actively moving to a position
        isBraked = false; // Reset the brake since the arm is moving
    } 
    // 🟢 L1 Button Logic (Switch brake mode to coast)
    else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        isBraked = false; // No brake, so we reset the brake flag
        isMovingToPosition = false; // No movement is currently happening
    } 
    // 🟢 Brake Logic (If joystick is at zero and arm is not actively moving)
    else if (fabs(armY) <= 5 && !isBraked && !isMovingToPosition) {
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
        isBraked = true; // Mark that the brake is set so it doesn't get set again
    }

    // 🟢 Emergency Stop Logic (If position goes below -2000)
    if (armPosition < HARD_STOP_POSITION && armY <= 0) {
        // If the arm is below -2000 and joystick is not moving up, brake
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
        if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
            armMotors.move_absolute(0, 80); // Move to 0 absolute position
        }
    }

        
        


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

        

        // delay to save resources
        pros::delay(10);

		//if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {
		//	chassis.turnToHeading(90, 1000, {.direction = AngularDirection::CW_CLOCKWISE, .minSpeed = 20});
		//}
//
		//if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
		//	chassis.moveToPose(20, 15, 90, 4000, {.minSpeed = 20});
		//	chassis.moveToPose(0, 0, 270, 4000, {.minSpeed = 20});
		//}
//
		//if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
		//	chassis.follow(test_1_txt, 5, 7000);
		//}
//
		//if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
		//	chassis.resetLocalPosition();
		//}
    }
}


