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
pros::adi::Pneumatics doinkerPiston = pros::adi::Pneumatics('F', false);

pros::Optical opticalSensor(18);
pros::Imu imu(6);

pros::Rotation linearWheel = pros::Rotation(7);
pros::Rotation angularWheel = pros::Rotation(-12);

pros::MotorGroup roller({11}, pros::MotorGearset::green);
pros::MotorGroup conveyer({19}, pros::MotorGearset::blue);

pros::adi::DigitalIn bumper = pros::adi::DigitalIn('H');


OpticsHandler opticsHandler(opticalSensor, intake, roller, controller, currentColour);
ArmHandler armHandler(controller, armMotors, 0.7, 10);
IntakeHandler intakeHandler(conveyer);   


lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
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
lemlib::ControllerSettings angularController(1.8, // proportional gain (kP)
                                             0, // integral gain (kI)
                                             10, // derivative gain (kD)
                                             3, // anti windup
                                             1, // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3, // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0 // maximum acceleration (slew)
);

lemlib::TrackingWheel veritical_tracking_wheel(&linearWheel, lemlib::Omniwheel::NEW_2, 0.6);
lemlib::TrackingWheel horizontal_tracking_wheel(&angularWheel, lemlib::Omniwheel::NEW_2, -2.7);
// sensors for odometry
lemlib::OdomSensors sensors(&veritical_tracking_wheel,
							nullptr,
							&horizontal_tracking_wheel,
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



double calibrateTracker(pros::v5::Rotation wheel, double startValue){
    double currentValue = startValue - wheel.get_position();
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
    opticalSensor.set_integration_time(20);
    double startValueLinear = linearWheel.get_angle();
    double startValueAngular = angularWheel.get_angle();


    
    
    pros::Task screenTask([&]() {
        std::string autonNames[7] = {"Blue 3+1 Right", "Red 3+1 Left", "Blue 4 Right", "Red 4 Left", "Blue 2 Left", "Red 2 Right", "Prog Skills"};  
        while (true) {

            if(bumper.get_new_press()){
                selection++;
                if(selection >= 7){
                    selection = 0;
                }
            }
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            pros::lcd::print(3, "Arm Position: %f, Eff: %f", armMotors.get_position(), armMotors.get_efficiency());
            pros::lcd::print(4, "Auto: %s", autonNames[selection].c_str());
            pros::lcd::print(5, "Linear Calc: %f", calibrateTracker(linearWheel, startValueLinear));
            pros::lcd::print(6, "Angular Calc: %f", calibrateTracker(angularWheel, startValueAngular));


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
    intakeHandler.intakeStallRecovery();
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

void armHandlerFunction(){
    armHandler.update();
}

void gameInit(teamColour colour, bool isAuton = true){
    opticsHandler.isAuton = isAuton;
    opticsHandler.disable = false;
    opticsHandler.colour = colour;
    pros::Task colourSortTask =  pros::Task(colourSort);
    isGameInit = true;
}


void newOmniAuto(){
    pros::Task armTask = pros::Task(armHandlerFunction);

    chassis.setPose(-63.5, 0, 90);
    intakeHandler.disable = true;
    intake.move(128);
    pros::delay(500);
    intake.move(0);
    chassis.moveToPoint(-49, 0, 750);
    chassis.turnToHeading(0, 650);
    chassis.moveToPoint(-49, -28, 1750, {.forwards = false, .maxSpeed = 70});

    chassis.waitUntil(26);
    clampPiston.toggle();
    intakeHandler.disable = false;
    intake.move(128);
    chassis.turnToPoint(-24, -24.5, 600);
    chassis.moveToPoint(-24, -24.5, 1500, {.maxSpeed = 70});
    chassis.turnToPoint(-20, -34, 500);
    chassis.moveToPoint(-20, -34, 700, {.maxSpeed = 70});
    chassis.turnToPoint(22, -51, 700);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(128);
    armMotors.tare_position(); // Reset encoder position to 0
    armMotors.move_relative(-270, 100); // Move 320 degrees relative at 50 rpm
    chassis.moveToPoint(22, -51, 1750, {.maxSpeed = 65});
    chassis.moveToPoint(1.5, -48, 1250, {.forwards = false});
    chassis.turnToHeading(180, 700);
    conveyer.move(128);
    pros::delay(150);
    conveyer.move(0);
    pros::delay(150);
    conveyer.move(128);
    pros::delay(150);
    conveyer.move(0);
    pros::delay(150);
    conveyer.move(128);
    pros::delay(200);
    conveyer.move(0);
    roller.move(0);
    conveyer.move(128);
    chassis.moveToPoint(1.5, -68, 1200, {.minSpeed = 40});
    chassis.waitUntilDone();

    //chassis.moveToPoint(1, -68, 1250, {.minSpeed = 40});
    //pros::delay(300);
    //pros::delay(1000);

    conveyer.move(0);
    armHandler.moveArm(-1400, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(0, 150);
    roller.move(128);
    pros::delay(200);
    chassis.moveToPoint(1.5, -52, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = false;
    intake.move(128);
    armHandler.moveArm(2000, 1000, false);

    chassis.turnToHeading(270, 650);
    chassis.moveToPoint(-60, -53, 3000, {.maxSpeed = 70});
    armHandler.moveArm(2000, 1000, false);

    //chassis.moveToPoint(-48, -53, 1000, {.forwards = false});
    //chassis.turnToHeading(180, 600);
    chassis.moveToPoint(-46, -66, 1300);
    //chassis.moveToPoint(-48, -62, 400, {.forwards = false});
    //chassis.turnToHeading(45, 800);
    //chassis.moveToPoint(-56, -58, 600, {.forwards = false});
    chassis.moveToPoint(-70, -70, 1200, {.forwards = false});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();

    chassis.swingToHeading(30, DriveSide::LEFT, 800);




    chassis.moveToPoint(-50, 0, 1750, {.maxSpeed = 85});
    chassis.turnToHeading(180, 800);
    chassis.moveToPoint(-50, 30, 1750, {.forwards = false, .maxSpeed = 70});

    chassis.waitUntil(26);
    clampPiston.toggle();
    intakeHandler.disable = false;
    intake.move(128);
    chassis.turnToPoint(-24, 24.5, 650);
    chassis.moveToPoint(-24, 24.5, 1250, {.maxSpeed = 70});
    chassis.turnToPoint(-20, 34, 500);
    chassis.moveToPoint(-20, 34, 600);
    chassis.turnToPoint(23, 48, 600);
    chassis.moveToPoint(23, 48, 1750, {.maxSpeed = 70});
    chassis.moveToPoint(5, 48, 1500, {.forwards = false});
    chassis.turnToHeading(0, 800);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    armMotors.tare_position(); // Reset encoder position to 0
    armMotors.move_relative(-330, 100); // Move 320 degrees relative at 50 rpm
    chassis.moveToPoint(4, 60, 1250);

    chassis.waitUntilDone();
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(4, 68, 1500);
    intake.move(128);
    pros::delay(150);
    intake.move(0);
    pros::delay(150);
    intake.move(128);
    pros::delay(150);
    intake.move(0);
    intake.move(128);
    pros::delay(200);
    intake.move(0);
    armHandler.moveArm(-1400, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(180, 150);
    chassis.moveToPoint(4, 50, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = false;
    intake.move(128);
    armHandler.moveArm(2000, 1000, false);

    chassis.turnToHeading(270, 650);
    chassis.moveToPoint(-60, 51, 1750, {.maxSpeed = 70});

    //chassis.moveToPoint(-48, 48, 800, {.forwards = false});
    //chassis.turnToHeading(0, 650);
    chassis.moveToPoint(-48, 64, 800);
    //chassis.moveToPoint(-44, 54, 750, {.forwards = false});
    //chassis.turnToHeading(135, 800);
    chassis.moveToPoint(-70, 70, 1000, {.forwards = false});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();

    gameInit(teamColour::red);
    intakeHandler.disable = false;
    roller.move(128);
    chassis.moveToPoint(6, 50, 1000, {.minSpeed = 80});
    chassis.turnToPoint(27, 20, 700);
    chassis.moveToPoint(27, 20, 2500, {.minSpeed = 70});
    chassis.moveToPose(45, 2, 330, 1500, {.forwards = false, .maxSpeed = 60});
    chassis.moveToPoint(49, -2, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    roller.move(0);
    intake.move(128);
    
    chassis.turnToPoint(22, -27, 600);
    chassis.moveToPoint(22, -27, 1300, {.maxSpeed = 60});
    chassis.turnToPoint(24, -51, 600);
    chassis.moveToPoint(24, -51, 800);
    chassis.turnToPoint(60, -51, 600);
    chassis.moveToPoint(60, -51, 1500, {.maxSpeed = 60});
    chassis.turnToHeading(45, 700);
    chassis.moveToPoint(60, -68, 1500, {.forwards = false, .minSpeed = 50});
    chassis.waitUntilDone();
    intake.move(-128);
    pros::delay(200);
    intake.move(128);
    clampPiston.toggle();
    chassis.moveToPoint(60, 0, 1200, {.minSpeed = 90});
    chassis.moveToPoint(70, 70, 2000,  {.minSpeed = 75});
    intake.move(0);
    chassis.moveToPose(0, 0, 45, 2500, {.forwards = false,.maxSpeed = 75, .minSpeed = 60});
    armHandler.moveArm(-1400, 1250, false);
    chassis.moveToPoint(60, 60, 500, {.maxSpeed = 40});

}


void newSigRedLeft4Ring(){
    
    intake.move(0);
    chassis.moveToPose(-2, -24, 30, 1250, {.forwards = false});
    chassis.moveToPoint(-5, -27, 600, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();  
    gameInit(teamColour::red);
    intake.move(128);
    chassis.turnToPoint(9.5, -45.5, 1000);
    chassis.moveToPoint(9.5, -45.5, 1500, {.maxSpeed = 60});
    chassis.turnToPoint(20, -48, 600);
    chassis.moveToPoint(18, -48, 700, {.maxSpeed = 60});
    chassis.turnToHeading(90, 750);
    chassis.moveToPoint(26, -48, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(17, -44, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(22, -28, 600);
    chassis.moveToPoint(22, -28, 1500, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);

    chassis.moveToPoint(-28, -30, 5000, {.maxSpeed = 60});


}

void newSigBlueRight4Ring(){
        
    intake.move(0);
    chassis.moveToPose(2, -24, 330, 1250, {.forwards = false});
    chassis.moveToPoint(5, -27, 600, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();  
    gameInit(teamColour::blue);
    intake.move(128);
    chassis.turnToPoint(-9.5, -45.5, 1000);
    chassis.moveToPoint(-9.5, -45.5, 1500, {.maxSpeed = 60});
    chassis.turnToPoint(-20, -48, 600);
    chassis.moveToPoint(-18, -48, 700, {.maxSpeed = 60});
    chassis.turnToHeading(270, 750);
    chassis.moveToPoint(-29, -48, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(-17, -44, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(-22, -28, 600);
    chassis.moveToPoint(-22, -28, 1500, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);

    chassis.moveToPoint(30, -30, 5000, {.maxSpeed = 50});

}

void goofyAhhRedAllianceStake(){
    chassis.setPose(-12, 3, 270);
    chassis.moveToPoint(-30.5, 3, 1250);
    chassis.turnToHeading(0, 800);
    chassis.moveToPoint(-30.5, 16.5, 1000);
    chassis.waitUntilDone();
    armHandler.moveArm(-1000, 1000, true);
    armHandler.moveArm(1500, 700, true);

    intake.move(0);
    chassis.moveToPose(-6, -26,  330, 1000, {.forwards = false});
    chassis.waitUntilDone();
    chassis.moveToPoint(1, -32, 1250, {.forwards = false,.maxSpeed = 40});
    chassis.waitUntilDone();
    clampPiston.toggle();  
    gameInit(teamColour::red);
    intake.move(128);
    chassis.turnToPoint(9.5, -42, 1000);
    chassis.moveToPoint(9.5, -42, 1500, {.maxSpeed = 60});
    chassis.turnToPoint(20, -45.5, 500);
    chassis.moveToPoint(18, -45.5, 700, {.maxSpeed = 60});
    chassis.turnToHeading(90, 750);
    chassis.moveToPoint(26, -45.5, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(17, -42, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(22, -28, 600);
    chassis.moveToPoint(22, -28, 1500, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);

    //chassis.moveToPoint(-28, -30, 5000, {.maxSpeed = 60}); // GAME: TOUCH WALL
    chassis.moveToPoint(-60, -10, 5000, {.maxSpeed = 80}); // ELIMS

}

void goofyAhhBlueAllianceStake(){
    chassis.setPose(12, 3, 90);
    chassis.moveToPoint(30.5, 3, 1250);
    chassis.turnToHeading(0, 800);
    chassis.moveToPoint(30.5, 16.5, 1000);
    chassis.waitUntilDone();
    armHandler.moveArm(-1000, 1000, true);
    armHandler.moveArm(1500, 700, true);

    intake.move(0);
    chassis.moveToPose(6, -26,  30, 1000, {.forwards = false});
    chassis.waitUntilDone();
    chassis.moveToPoint(-1, -32, 1250, {.forwards = false,.maxSpeed = 40});
    chassis.waitUntilDone();
    clampPiston.toggle();  
    gameInit(teamColour::blue);
    intake.move(128);
    chassis.turnToPoint(-9.5, -42, 1000);
    chassis.moveToPoint(-9.5, -42, 1500, {.maxSpeed = 60});
    chassis.turnToPoint(-20, -45.5, 500);
    chassis.moveToPoint(-18, -45.5, 700, {.maxSpeed = 60});
    chassis.turnToHeading(270, 750);
    chassis.moveToPoint(-26, -45.5, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(-17, -42, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(-22, -28, 600);
    chassis.moveToPoint(-22, -28, 1500, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);

    //chassis.moveToPoint(28, -30, 5000, {.maxSpeed = 60}); //GAME
    chassis.moveToPoint(60, -10, 5000, {.maxSpeed = 80}); //ELIMS
}


void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    switch(selection){
        case 0:
            newOmniAuto(); //TEMPORARY
            break;
        case 1:
            goofyAhhRedAllianceStake();
            break;
        case 2:
            newSigBlueRight4Ring();
            break;
        case 3:
            newSigRedLeft4Ring();
            break;
        case 4:
            gameInit(teamColour::blue);
            blueLeft();
            break;
        case 5:
            gameInit(teamColour::red);
            redRight();
            break;
        case 6:
            newOmniAuto();
            break;
    }

}


void opcontrol() {

    pros::Task controllerFeedbackTask =  pros::Task(controllerFeedback);
    pros::Task armTask = pros::Task(armHandlerFunction);

    
    gameInit(teamColour::neutral, false);
    

    intake.move(0);

    while (true) {

        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        chassis.arcade(leftX, leftY);

        controller.get_digital_new_press(DIGITAL_Y) ? clampPiston.toggle() : 0; 
        controller.get_digital_new_press(DIGITAL_A) ? doinkerPiston.toggle() : 0;
        
        pros::delay(10);

    }
}


