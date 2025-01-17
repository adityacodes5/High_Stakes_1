#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"


gameMode currentMode = gameMode::testing;
teamColour currentColour = teamColour::neutral;
int selection = 0;
bool isGameInit = false;


pros::Controller controller(pros::E_CONTROLLER_MASTER);

pros::MotorGroup leftMotors({-3, -2, -4},pros::MotorGearset::blue); 
pros::MotorGroup rightMotors({8, 9, 10}, pros::MotorGearset::blue); 
pros::MotorGroup armMotors({20}, pros::MotorGearset::green);
pros::MotorGroup intake({11, 19}, pros::MotorGearset::blue);

pros::adi::Pneumatics clampPiston = pros::adi::Pneumatics('A', false);
pros::adi::Pneumatics doinkerPiston = pros::adi::Pneumatics('G', false);

pros::Optical opticalSensor(18);
pros::Imu imu(6);

pros::Rotation angularWheel = pros::Rotation(17);
pros::MotorGroup roller({11}, pros::MotorGearset::green);

pros::adi::DigitalIn bumper = pros::adi::DigitalIn('H');


OpticsHandler opticsHandler(opticalSensor, intake, controller, currentColour);
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

lemlib::TrackingWheel horizontal_tracking_wheel(&angularWheel, lemlib::Omniwheel::NEW_2,-1.59);
// sensors for odometry
lemlib::OdomSensors sensors(nullptr,
							nullptr,
							nullptr,//&horizontal_tracking_wheel,
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


double startValue;

double calibrateTracker(){
    double currentValue = startValue - angularWheel.get_position();
    double distTraveled = currentValue * 2 * 3.14 / 36000;
    double radius = distTraveled / (chassis.getPose().theta * M_PI / 180);
    return radius;
}

void colourSort(){

    opticsHandler.colourFilter();
}

void initialize() {

    
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(true); // calibrate sensors
    chassis.resetLocalPosition(); // reset odometry
    angularWheel.reset(); 
    opticalSensor.set_integration_time(20);
    startValue = angularWheel.get_angle();


    
    
    pros::Task screenTask([&]() {
        std::string autonNames[5] = {"Blue Right (Negative)", "Blue Left (Positive)", "Red Right (Positive)", "Red Left (Negative)", "Prog Skills"};  
        while (true) {

            if(bumper.get_new_press()){
                selection++;
                if(selection >= 5){
                    selection = 0;
                }
            }
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, "Arm Position: %f", armMotors.get_position());
            pros::lcd::print(4, "Auto: %s", autonNames[selection].c_str());
            pros::lcd::print(5, "Tracking wheel: %f", startValue - angularWheel.get_angle());
            pros::lcd::print(6, "Calculated Radius: %f", calibrateTracker());


            lemlib::telemetrySink()->info("Chassis pose: {}", chassis.getPose());

            pros::delay(50);

        }
    });

    pros::Task controllerTask([&]() {
        std::string modeString;
        std::string colourString;

 	    while (true) {
            if(controller.get_digital_new_press(DIGITAL_DOWN)){
                opticsHandler.disable = !opticsHandler.disable;
                controller.clear_line(0);
            }
            if(controller.get_digital_new_press(DIGITAL_LEFT)){
                if(currentColour == teamColour::neutral || currentColour == teamColour::red){
                    currentColour = teamColour::blue;
                }
                else if(currentColour == teamColour::blue){
                    currentColour = teamColour::red;
                }
            }

            switch(currentColour){
                case teamColour::blue:
                    modeString = "T-B";
                    colourString = opticsHandler.disable ? "R-OF" : "R-ON";
                    opticsHandler.colour = teamColour::blue;
                    break;
                case teamColour::red:
                    modeString = "T-R";
                    colourString = opticsHandler.disable ? "B-OF" : "B-ON";
                    opticsHandler.colour = teamColour::red;
                    break;
            }

            controller.print(0, 0, std::string(modeString + " " + colourString).c_str());

         pros::delay(50);
 	}
    });
}


void disabled() {
}

void competition_initialize() {}


void recoverIntake(){
    intakeStallRecovery(intake);
}



void controllerFeedback(){
    auto startTime = pros::millis();
    bool protectedWarning = false;
    bool protectedNow = false;
    while(true){

        if((pros::millis() - startTime) >  85000 && !protectedWarning){
            protectedWarning = true;
            controller.rumble(".");
        }
        if((pros::millis() - startTime) >  90000 && !protectedNow){
            protectedNow = true;
            controller.rumble("..");
        }
        pros::delay(1000);
    }
}

void gameInit(teamColour colour, bool isAuton = true){
    opticsHandler.isAuton = isAuton;
    opticsHandler.disable = false;
    opticsHandler.colour = colour;
    pros::Task colourSortTask =  pros::Task(colourSort);
    isGameInit = true;
}

void redMogoRush(){
    gameInit(teamColour::red);
    chassis.moveToPoint(0, 36, 1250, {.minSpeed = 70});
    chassis.turnToPoint(-7, 55, 500);
    doinkerPiston.toggle();
    chassis.turnToHeading(270, 500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE, .minSpeed = 50});
    chassis.waitUntil(50);
    doinkerPiston.toggle();
    chassis.turnToHeading(290, 500);
    chassis.turnToHeading(90, 800);
    chassis.moveToPose(-20, 42, 105, 1750, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntil(18);
    clampPiston.toggle();
    pros::delay(250);
    intake.move(128);
    chassis.moveToPoint(-35, 42, 1500, {.forwards = false,.minSpeed = 30});
    chassis.turnToPoint(-12, 32, 500);
    chassis.moveToPoint(-12, 32, 1500, {.maxSpeed = 50});
    chassis.turnToHeading(180, 2000);
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(0);
    chassis.turnToHeading(90, 600);
    chassis.moveToPose(-30, 34, 90, 1000, {.forwards = false, .minSpeed = 50});
    chassis.moveToPoint(-40, 34, 1750, {.forwards = false,.maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(128);
    pros::delay(250);
    chassis.moveToPoint(-60, 8, 4000, {.forwards = true,.maxSpeed = 60});
    chassis.moveToPoint(-70, -4, 4000, {.forwards = true,.maxSpeed = 60});



}

//void redWallRush(){
//    gameInit(teamColour::red);
//    chassis.moveToPose(2, -24, 330, 4000, {.forwards = false});
//    chassis.waitUntilDone();
//    pros::delay(500);
//    clampPiston.toggle();   
//    pros::delay(250);
//    intake.move(128);
//    pros::delay(250);
//    chassis.moveToPose(-24, -31.5, 270, 4000, {.maxSpeed=50});
//    chassis.waitUntilDone();
//    chassis.moveToPose(-24, -3 , 0, 4000);
//    chassis.swingToHeading(305, DriveSide::LEFT, 2000);
//    chassis.waitUntilDone();
//    doinkerPiston.toggle();
//    pros::delay(250);
//    chassis.turnToHeading(0, 2000);
//    chassis.waitUntil(30);
//    doinkerPiston.toggle();
//    chassis.swingToHeading(315, DriveSide::LEFT, 2000);
//
//}

void newBlueRight(){
    gameInit(teamColour::blue);
    chassis.moveToPose(2, -24, 335, 1250, {.forwards = false});
    chassis.moveToPoint(6, -28, 400, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    pros::delay(150);
    intake.move(128);
    chassis.moveToPoint(-22, -28, 2000, {.maxSpeed = 60});
    chassis.moveToPose(-22, -44.5, 180, 4000, {.maxSpeed = 55});
    chassis.moveToPoint(-22, -35, 1500, {.forwards = false});
    chassis.moveToPose(-14, -44.5, 180, 1750, {.maxSpeed = 55});
    chassis.waitUntilDone();
    chassis.moveToPoint(-15, -35, 900, {.forwards = false, .minSpeed = 80});
    chassis.moveToPoint(25, -5, 750, {.minSpeed = 80});
    chassis.moveToPose(50, -5, 90, 2500, {.maxSpeed = 40});
    chassis.moveToPoint(25, -5, 750, {.forwards = false, .minSpeed = 70});
    chassis.moveToPoint(26, -32, 750, {.minSpeed = 70} );
    pros::delay(500);
}

void newRedLeft(){
    gameInit(teamColour::red);
    chassis.moveToPose(-2, -24, 25, 1250, {.forwards = false});
    chassis.moveToPoint(-6, -28, 400, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    pros::delay(150);
    intake.move(128);
    chassis.moveToPoint(22, -28, 2000, {.maxSpeed = 60});
    chassis.moveToPose(22, -44.5, 180, 4000, {.maxSpeed = 55});
    chassis.moveToPoint(22, -35, 1500, {.forwards = false});
    chassis.moveToPose(14, -44.5, 180, 1750, {.maxSpeed = 55});
    chassis.waitUntilDone();
    chassis.moveToPoint(15, -35, 900, {.forwards = false, .minSpeed = 80});
    chassis.moveToPoint(-25, -5, 750, {.minSpeed = 80});
    chassis.moveToPose(-50, -5, 270, 2500, {.maxSpeed = 40});
    chassis.moveToPoint(-25, -5, 750, {.forwards = false, .minSpeed = 70});
    chassis.moveToPoint(-26, -32, 750, {.minSpeed = 70} );
    pros::delay(500);
}

void newProgSkills(){
    chassis.setPose(0, -61.5, 0);
    intake.move(128);
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(0, -50, 2000);
    chassis.turnToHeading(270, 1200);
    chassis.moveToPose(32, -48, 270, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(128);
    chassis.moveToPoint(24, -24, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.turnToPoint(48, -24, 1000);
    chassis.moveToPoint(52, -24, 2500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPose(48, -36, 180, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPoint(48, -66, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPoint(48, -48, 1500, {.forwards = false, .minSpeed = 40});
    chassis.moveToPoint(60, -48, 1500, {.maxSpeed = 70, .minSpeed = 40});
    chassis.swingToHeading(315, DriveSide::RIGHT, 1500);
    chassis.moveToPoint(64, -62, 1000, {.forwards = false, .minSpeed = 40});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(0);
    
    chassis.moveToPose(12, -50, 90, 2000, {.forwards = false, .minSpeed = 60});
    chassis.moveToPose(-32, -48, 90, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(128);
    chassis.moveToPoint(-24, -24, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.turnToPoint(-48, -24, 1000);
    chassis.moveToPoint(-52, -24, 2500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPose(-48, -36, 180, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPoint(-48, -66, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.moveToPoint(-48, -48, 1500, {.forwards = false, .minSpeed = 40});
    chassis.moveToPoint(-60, -48, 1500, {.maxSpeed = 70, .minSpeed = 40});
    chassis.swingToHeading(45, DriveSide::LEFT, 1500);
    chassis.moveToPoint(-64, -62, 1000, {.forwards = false, .minSpeed = 40});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(0);

}

void evenNewerSkills(){
    chassis.setPose(-61.5, 0, 90);
    intake.move(128);
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(-54, 0, 2000);
    chassis.turnToHeading(0, 1200);
    chassis.moveToPose(-48, -32, 0, 2000, {.forwards = false, .maxSpeed = 60});

    chassis.waitUntil(30);
    clampPiston.toggle();
    intake.move(128);
    chassis.moveToPoint(-24, -24, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.turnToPoint(-2, -51, 1000);
    chassis.moveToPoint(-2, -51, 2000, {.maxSpeed = 60, .minSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.moveToPoint(0, -46, 2000, {.forwards = false});
    chassis.moveToPoint(-18, -48, 2000, {.maxSpeed = 65});
    chassis.moveToPose(-58, -48, 270, 2000, {.maxSpeed = 45});
    chassis.moveToPoint(-48, -48, 2250, {.forwards = false, .minSpeed = 40});
    chassis.turnToPoint(-48, -60, 1500);
    chassis.moveToPoint(-46, -61, 2250, {.minSpeed = 40});
    chassis.swingToHeading(45, DriveSide::RIGHT, 1500);
    chassis.moveToPoint(-58, -60, 1500, {.forwards = false, .minSpeed = 30});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(-128);
    pros::delay(250);
    intake.move(0);

    chassis.moveToPoint(-54, 0, 2000);
    chassis.moveToPose(-48, 26, 190, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(128);

    chassis.moveToPoint(-24, 24, 1500, {.maxSpeed = 60, .minSpeed = 40});
    chassis.turnToPoint(0, 52, 1000);
    chassis.moveToPoint(0, 52, 2000, {.maxSpeed = 60, .minSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(500);
    chassis.moveToPoint(0, 46, 2000, {.forwards = false});
    chassis.moveToPoint(-18, 48, 2000, {.maxSpeed = 65});
    chassis.moveToPose(-58, 48, 270, 2000, {.maxSpeed = 45});
    chassis.moveToPoint(-48, 48, 2250, {.forwards = false, .minSpeed = 40});
    chassis.turnToPoint(-48, 60, 1500);
    chassis.moveToPoint(-46, 61, 2250, {.minSpeed = 40});
    chassis.swingToHeading(135, DriveSide::RIGHT, 1500);
    chassis.moveToPoint(-58, 60, 1500, {.forwards = false, .minSpeed = 30});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    roller.move(128);

    //pros::delay(200);
//
    //chassis.moveToPoint(26, 50, 3500, {.maxSpeed = 80, .minSpeed = 40});
    //chassis.moveToPose(50, -3, 45, 3500, {.forwards = false, .maxSpeed = 50});
    //chassis.waitUntilDone();
    //clampPiston.toggle();
    //roller.move(0);
    //intake.move(128);
//
    //chassis.moveToPoint(48, 62, 2000, {.maxSpeed = 80, .minSpeed = 40});




}



void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    switch(selection){
        case 0:
            redMogoRush(); //TEMPORARY
            break;
        case 1:
            newBlueRight();
            break;
        case 2:
            newRedLeft();
            break;
        case 3:
            blueLeft();
            break;
        case 4:
            progSkills();
            break;
    }

}


void opcontrol() {

    pros::Task controllerFeedbackTask =  pros::Task(controllerFeedback);
    
    if(!isGameInit){
        gameInit(teamColour::neutral, false);
    }

    angularWheel.set_position(0);

    while (true) {




        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        int armY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

        chassis.arcade(leftX, leftY);

        armHandler.update();

        controller.get_digital_new_press(DIGITAL_Y) ? clampPiston.toggle() : 0; 
        controller.get_digital_new_press(DIGITAL_A) ? doinkerPiston.toggle() : 0;

        if(controller.get_digital_new_press(DIGITAL_B)){
            leftMotors.move_velocity(-64);
            rightMotors.move_velocity(64);
            pros::delay(5000);
            leftMotors.move_velocity(0);
            rightMotors.move_velocity(0);
        }
        
        pros::delay(10);

    }
}


