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
pros::Imu imu(5);

pros::Rotation linearWheel = pros::Rotation(7);
pros::Rotation angularWheel = pros::Rotation(-12);
pros::Rotation armRotation = pros::Rotation(-14);

pros::MotorGroup roller({11}, pros::MotorGearset::green);
pros::MotorGroup conveyer({19}, pros::MotorGearset::blue);

pros::adi::DigitalIn bumper = pros::adi::DigitalIn('H');


OpticsHandler opticsHandler(opticalSensor, intake, roller, controller, currentColour);
ArmHandler armHandler(controller, armMotors, armRotation, 0.005, 10);
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
                                            3.5, // derivative gain (kD)
                                            3, // anti windup
                                            1, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            3, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            20 // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(1.9, // proportional gain (kP)
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
            pros::lcd::print(7, "ArmCalc: %f", armRotation.get_position());


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
    chassis.moveToPoint(22, -51, 1750, {.maxSpeed = 60});
    chassis.moveToPoint(2, -48, 1250, {.forwards = false});
    chassis.turnToHeading(180, 700);
    chassis.moveToPoint(2, -60, 800);
    pros::delay(250);
    conveyer.move(128);
    pros::delay(100);
    conveyer.move(0);
    pros::delay(100);
    conveyer.move(128);
    pros::delay(150);
    conveyer.move(0);
    roller.move(0);
    conveyer.move(128);
    chassis.moveToPoint(2, -60, 800);

    chassis.moveToPoint(2, -68, 1250, {.minSpeed = 40});
    //pros::delay(300);
    //pros::delay(1000);
    chassis.waitUntilDone();

    conveyer.move(0);
    armHandler.moveArm(-1400, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(0, 150);
    roller.move(128);
    pros::delay(200);
    chassis.moveToPoint(2, -52, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = false;
    intake.move(128);
    armHandler.moveArm(2000, 1000, false);

    chassis.turnToHeading(270, 650);
    chassis.moveToPoint(-60, -53, 3000, {.maxSpeed = 70});
    armHandler.moveArm(2000, 1000, false);

    //chassis.moveToPoint(-48, -53, 1000, {.forwards = false});
    //chassis.turnToHeading(180, 600);
    chassis.moveToPoint(-45, -66, 1300);
    //chassis.moveToPoint(-48, -62, 400, {.forwards = false});
    //chassis.turnToHeading(45, 800);
    //chassis.moveToPoint(-56, -58, 600, {.forwards = false});
    chassis.moveToPoint(-65, -70, 1200, {.forwards = false});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();

    chassis.swingToHeading(30, DriveSide::LEFT, 800);




    chassis.moveToPoint(-48, -4, 1750, {.maxSpeed = 85});
    chassis.turnToHeading(180, 800);
    chassis.moveToPoint(-48, 30, 1750, {.forwards = false, .maxSpeed = 70});

    chassis.waitUntil(26);
    clampPiston.toggle();
    intakeHandler.disable = false;
    intake.move(128);
    chassis.turnToPoint(-24, 24.5, 650);
    chassis.moveToPoint(-24, 24.5, 1250, {.maxSpeed = 70});
    chassis.turnToPoint(-20, 34, 500);
    chassis.moveToPoint(-20, 34, 600);
    chassis.turnToPoint(25, 47, 600);
    chassis.moveToPoint(25, 47, 1750, {.maxSpeed = 65});
    chassis.moveToPoint(5, 48, 1500, {.forwards = false});
    chassis.turnToHeading(0, 1000);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    armMotors.tare_position(); // Reset encoder position to 0
    armMotors.move_relative(-330, 100); // Move 320 degrees relative at 50 rpm
    chassis.moveToPoint(5, 60, 1250);

    chassis.waitUntilDone();
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(5, 68, 1500);
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
    chassis.moveToPoint(5, 50, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = false;
    intake.move(128);
    armHandler.moveArm(2000, 1000, false);

    chassis.turnToHeading(270, 650);
    chassis.moveToPoint(-60, 52, 2000, {.maxSpeed = 70});

    //chassis.moveToPoint(-48, 48, 800, {.forwards = false});
    //chassis.turnToHeading(0, 650);
    chassis.moveToPoint(-48, 62, 1000);
    //chassis.moveToPoint(-44, 54, 750, {.forwards = false});
    //chassis.turnToHeading(135, 800);
    chassis.turnToHeading(chassis.getPose().theta + 90, 800);
    chassis.moveToPoint(-65, 70, 1000, {.forwards = false});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();

    roller.move(128);
    chassis.moveToPoint(6, 50, 1000, {.minSpeed = 80});
    chassis.turnToPoint(27, 20, 700);
    chassis.moveToPoint(27, 20, 2500, {.minSpeed = 70});
    chassis.moveToPose(44, 2, 330, 1500, {.forwards = false, .maxSpeed = 60});
    chassis.moveToPoint(50, -2, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    gameInit(teamColour::red);
    intakeHandler.disable = false;
    clampPiston.toggle();
    roller.move(0);
    intake.move(128);
    
    chassis.turnToPoint(22, -29, 600);
    chassis.moveToPoint(22, -29, 1300, {.maxSpeed = 60});
    chassis.turnToPoint(24, -52, 600);
    chassis.moveToPoint(24, -52, 800);
    chassis.turnToPoint(60, -52, 600);
    chassis.moveToPoint(60, -52, 1500, {.maxSpeed = 60});
    //chassis.swingToHeading(45, DriveSide::LEFT, 1000);
    chassis.moveToPoint(60, -47, 800, {.forwards = false, .minSpeed = 50});
    chassis.moveToPoint(60, -68, 1500, {.forwards = false, .minSpeed = 50});
    chassis.waitUntilDone();
    intake.move(-128);
    pros::delay(200);
    intake.move(128);
    clampPiston.toggle();
    chassis.moveToPoint(58, 0, 1200, {.minSpeed = 90});
    chassis.turnToPoint(75, 70, 600);
    chassis.moveToPoint(75, 70, 2000,  {.minSpeed = 75});
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
    chassis.turnToPoint(9.5, -46.5, 700);
    chassis.moveToPoint(9.5, -46.5, 900, {.maxSpeed = 60});
    chassis.turnToPoint(20, -49, 400);
    chassis.moveToPoint(18, -49, 700, {.maxSpeed = 60});
    chassis.turnToHeading(90, 400);
    chassis.moveToPoint(35, -49, 1250, {.maxSpeed = 60});
    chassis.moveToPoint(17, -44, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(22, -28, 600);
    chassis.moveToPoint(22, -28, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();

    chassis.moveToPoint(10, -13, 1000);
    chassis.turnToPoint(-60, -13, 600);
    chassis.waitUntilDone();
    intake.move(0);
    chassis.moveToPoint(-60, -13, 1800, {.maxSpeed = 55});
    chassis.waitUntilDone();
    doinkerPiston.toggle();
    chassis.moveToPose(-104, 16, 300, 2000);
    chassis.turnToHeading(30, 1000);



}

void newSigBlueRight4Ring(){
        
    intake.move(0);
    chassis.moveToPose(2, -24, 330, 1250, {.forwards = false});
    chassis.moveToPoint(5, -27, 600, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();  
    gameInit(teamColour::blue);
    intake.move(128);
    chassis.turnToPoint(-9.5, -46, 700);
    chassis.moveToPoint(-9.5, -46, 900, {.maxSpeed = 60});
    chassis.turnToPoint(-20, -48, 400);
    chassis.moveToPoint(-18, -48, 700, {.maxSpeed = 60});
    chassis.turnToHeading(270, 400);
    chassis.moveToPoint(-35, -48, 1250, {.maxSpeed = 60});
    chassis.moveToPoint(-17, -44, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(-22, -28, 600);
    chassis.moveToPoint(-22, -28, 1000, {.maxSpeed = 60});
    chassis.waitUntilDone();
   // pros::delay(500);

   chassis.moveToPoint(-10, -13, 1000);
   chassis.turnToPoint(60, -13, 600);
   chassis.waitUntilDone();
   intake.move(0);
   chassis.moveToPoint(60, -13, 1800, {.maxSpeed = 55});
   chassis.waitUntilDone();
   doinkerPiston.toggle();
   chassis.moveToPose(104, 16, 60, 2000);
   chassis.turnToHeading(135, 1000);


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

    chassis.moveToPoint(-28, -30, 5000, {.maxSpeed = 60}); // GAME: TOUCH WALL
    //chassis.moveToPoint(-60, -10, 5000, {.maxSpeed = 80}); // ELIMS

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

    chassis.moveToPoint(28, -30, 5000, {.maxSpeed = 60}); //GAME
    //chassis.moveToPoint(60, -10, 5000, {.maxSpeed = 80}); //ELIMS
}

void newProvsAuto(){
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

    //ring 1
    chassis.turnToPoint(-24, -24.5, 600);
    chassis.moveToPoint(-24, -24.5, 1500, {.maxSpeed = 70});

    //ring 2
    chassis.turnToPoint(-24, -51, 600);
    chassis.moveToPoint(-24, -51, 1500, {.maxSpeed = 70});

    //ring 3 + 4
    chassis.turnToPoint(-64, -51, 600);
    chassis.moveToPoint(-64, -51, 2500, {.maxSpeed = 70});
    chassis.turnToPoint(-53, -61, 600);
    chassis.moveToPoint(-53, -61, 1500, {.maxSpeed = 70});
    chassis.moveToPoint(-65, -67, 1250, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(-128);
    pros::delay(150);
    intake.move(0);

    //wall stake 1
    chassis.moveToPoint(-24, -50, 1000);
    chassis.moveToPoint(2, -50, 1500);
    chassis.turnToHeading(180, 700);

    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(128);
    armMotors.tare_position(); // Reset encoder position to 0
    armHandler.moveArm(-270, 1000, true); // Move 320 degrees relative at 50 rpm
    //chassis.moveToPoint(2, -60, 800);
    chassis.moveToPoint(2, -74, 2500, {.minSpeed = 40});
    pros::delay(1600);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);


    conveyer.move(0);
    armHandler.moveArm(-1500, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(0, 150);
    roller.move(128);
    pros::delay(200);

    chassis.moveToPoint(2, -56, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = true;
    armHandler.moveArm(2000, 1000, false);
    intake.move(0);

    //blue stake
    chassis.turnToHeading(250, 700);
    chassis.moveToPoint(62, -26, 2000, {.forwards = false, .maxSpeed = 80});
    clampPiston.toggle();
    chassis.moveToPoint(67, -67, 1700, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    chassis.moveToPoint(62, -40, 1500);
    roller.move(128);
    chassis.moveToPoint(25, -29, 1500, {.maxSpeed = 70});
    chassis.turnToHeading(225, 800);
    chassis.moveToPoint(51, -6, 1500, {.forwards = false, .maxSpeed = 60});
    chassis.moveToPoint(55, -2, 600, {.forwards = false});
    pros::delay(50);
    clampPiston.toggle();
    roller.move(0);
    intake.move(128);
    intakeHandler.disable = false;
    gameInit(teamColour::red);
    chassis.turnToPoint(25, 22, 600);
    chassis.moveToPoint(25, 22, 1200, {.maxSpeed = 70});
    chassis.turnToPoint(25, 46, 600);
    chassis.moveToPoint(25, 46, 1250, {.maxSpeed = 70});
    chassis.turnToHeading(90, 700);
    chassis.moveToPoint(50, 45, 1500, {.maxSpeed = 60});
    chassis.turnToHeading(0, 700);
    chassis.moveToPoint(50, 65, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(70, 70, 1500, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(-128);
    pros::delay(100);
    intake.move(0);

    gameInit(teamColour::neutral);
    //wall stake 2
    intakeHandler.disable = true;
    chassis.moveToPoint(-1, 48, 1750);
    chassis.waitUntilDone();
    intake.move(128);
    chassis.turnToHeading(0, 800);
    armMotors.tare_position(); // Reset encoder position to 0
    chassis.waitUntilDone();
    chassis.moveToPoint(-1, 62, 1000);
    armHandler.moveArm(-270, 1000, true); // Move 320 degrees relative at 50 rpm
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(-1, 72, 2500);
    pros::delay(1000);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    armHandler.moveArm(-1400, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(180, 150);
    chassis.moveToPoint(-1, 55, 1000, {.forwards = false});
    chassis.waitUntil(5);
    intakeHandler.disable = false;
    intake.move(0);
    roller.move(128);


    chassis.turnToPoint(-27, 24.5, 600);
    armHandler.moveArm(2000, 1000, false);
    chassis.moveToPoint(-27, 24.5, 1500, {.maxSpeed = 70});
    chassis.turnToHeading(90, 700);
    chassis.moveToPoint(-56, 24.5, 2500, {.forwards = false, .maxSpeed = 70});
    chassis.waitUntil(30);
    clampPiston.toggle();
    roller.move(0);
    intake.move(128);
    chassis.moveToPoint(-23, 46.5, 1500, {.maxSpeed = 70});
    chassis.turnToHeading(270, 700);
    chassis.moveToPoint(-64, 46.5, 2500, {.maxSpeed = 50});
    chassis.turnToPoint(-55, 60, 600);
    chassis.moveToPoint(-55, 60, 1500, {.maxSpeed = 70});
    chassis.moveToPoint(-70, 70, 1500, {.forwards = false});
    chassis.waitUntilDone();
    clampPiston.toggle();
    intake.move(-128);
    pros::delay(100);
    intake.move(0);
    chassis.moveToPoint(-40, 40, 700, {.minSpeed = 70});
    chassis.turnToHeading(330, 600);
    chassis.moveToPoint(0, 0, 2500, {.forwards = false, .maxSpeed = 55});
    armHandler.moveArm(-1000, 1000, false);
    chassis.moveToPoint(-70, -70, 1000, {.maxSpeed = 50});
    armHandler.moveArm(2000, 1000, false);







}

void provs4plusRed(){
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
    chassis.waitUntilDone();
    intake.move(0);
    chassis.turnToPoint(-22, -48, 700);
    chassis.moveToPoint(-17, -40, 1500);
    doinkerPiston.extend();
    chassis.moveToPoint(17, -44, 1000);
    chassis.turnToPoint(22, -28, 600);
    chassis.waitUntilDone();
    doinkerPiston.retract(); //retract doinker piston
    intake.move(128);
    chassis.moveToPoint(22, -28, 1500, {.maxSpeed = 60});
    chassis.waitUntilDone();
    pros::delay(500);

    chassis.moveToPoint(-28, -30, 5000, {.maxSpeed = 60});

}

void wallStakeTest(){
    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    intakeHandler.disable = true;
    intake.move(128);
    armHandler.moveArm(-280, 1000, true); // Move 320 degrees relative at 50 rpm
    armMotors.brake();
    chassis.moveToPoint(0, 15, 1000);
    chassis.waitUntilDone();
    pros::delay(250);
    chassis.moveToPoint(0, 35, 2500);
    pros::delay(700);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    pros::delay(80);
    intake.move(128);
    pros::delay(80);
    intake.move(0);
    armHandler.moveArm(-1400, 1250, false);
    chassis.turnToHeading(90, 150);
    chassis.turnToHeading(270, 150);
    chassis.turnToHeading(180, 150);
    chassis.moveToPoint(0, 0, 1000, {.forwards = false});
}


void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    switch(selection){
        case 0:
            newSigRedLeft4Ring(); //TEMPORARY
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
            newProvsAuto();
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


