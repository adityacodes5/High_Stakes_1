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
pros::adi::Pneumatics leftDoinkerPiston = pros::adi::Pneumatics('F', false);
pros::adi::Pneumatics rightDoinkerPiston = pros::adi::Pneumatics('E', false);
pros::adi::Pneumatics intakeRamp = pros::adi::Pneumatics('G', false);

pros::Optical opticalSensor(18);
pros::Imu imu(5);

pros::Rotation linearWheel = pros::Rotation(7);
pros::Rotation angularWheel = pros::Rotation(-12);
pros::Rotation armRotation = pros::Rotation(-14);

pros::MotorGroup roller({11}, pros::MotorGearset::green);
pros::MotorGroup conveyer({19}, pros::MotorGearset::blue);

pros::adi::DigitalIn bumper = pros::adi::DigitalIn('H');


OpticsHandler opticsHandler(opticalSensor, intake, roller, controller, currentColour);
ArmHandler armHandler(controller, armMotors, armRotation, 0.007, 10);
IntakeHandler intakeHandler(conveyer);   


lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
                              &rightMotors, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::NEW_325, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2 // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings linearController(7.8, // proportional gain (kP)
                                            0, // integral gain (kI)
                                            5, // derivative gain (kD)
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

lemlib::TrackingWheel veritical_tracking_wheel(&linearWheel, lemlib::Omniwheel::NEW_2, 0.9);
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

//// input curve for steer input during driver control
//lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
//                                  10, // minimum output where drivetrain will move out of 127
//                                  1.019 // expo curve gain
//);

lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  8, // minimum output where drivetrain will move out of 127
                                  1.03// expo curve gain
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

void armHandlerFunction(){
    armHandler.update();
}

void initialize() {

    
    pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(true); // calibrate sensors
    chassis.resetLocalPosition(); // reset odometry
    opticalSensor.set_integration_time(20);
    double startValueLinear = linearWheel.get_angle();
    double startValueAngular = angularWheel.get_angle();

    pros::Task armTask = pros::Task(armHandlerFunction);


    
    
    pros::Task screenTask([&]() {
        std::string autonNames[7] = {"Blue 4 Right", "Red 4 Left", "Blue 3+1 Under Ladder", "Red 3+1 Under Ladder", "Blue 2 Left", "Red 2 Right", "Prog Skills"};  
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



void gameInit(teamColour colour, bool isAuton = true){
    opticsHandler.isAuton = isAuton;
    opticsHandler.disable = false;
    opticsHandler.colour = colour;
    pros::Task colourSortTask =  pros::Task(colourSort);
    isGameInit = true;
}



void newSigRedLeft4Ring(){

    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    armMotors.brake();
    
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
    //chassis.moveToPoint(-28, -30, 5000, {.maxSpeed = 60});// Touch Wall


    chassis.waitUntilDone();
    intake.move(0);
    chassis.moveToPoint(-60, -13, 1800, {.maxSpeed = 70});
    chassis.waitUntilDone();
    leftDoinkerPiston.toggle();
    chassis.moveToPose(-104, 16, 300, 2000);
    chassis.turnToHeading(30, 1000);



}

void newSigBlueRight4Ring(){

    
    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    armMotors.brake();
        
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

   //chassis.moveToPoint(28, -30, 5000, {.maxSpeed = 60}); GAME
   intake.move(0);
   chassis.moveToPoint(60, -13, 1800, {.maxSpeed = 70});
   chassis.waitUntilDone();
   leftDoinkerPiston.toggle();
   chassis.moveToPose(104, 16, 60, 2000);
   chassis.turnToHeading(135, 1000);


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
    leftDoinkerPiston.extend();
    chassis.moveToPoint(17, -44, 1000);
    chassis.turnToPoint(22, -28, 600);
    chassis.waitUntilDone();
    leftDoinkerPiston.retract(); //retract doinker piston
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


//void worldsBluePosUnderLadder(){ //Alliance Stake. Middle ring, under ladder rings
//    chassis.setPose(0, 0, 120);
//    armHandler.moveArm(-160, 375, true);
//    intake.move(128);
//    pros::delay(350);
//    intake.move(0);
//    armHandler.moveArm(-1400, 1000, true);
//    chassis.moveToPoint(-4, 10, 700, {.forwards = false});
//    intake.move(-128);
//    pros::delay(200);
//    intake.move(0);
//    //moveLinear(chassis, -5, 1000, {.forwards = false});
//    chassis.turnToPoint(8, 13, 500);
//    chassis.waitUntilDone();
//    intakeRamp.extend();
//    roller.move(128);
//    chassis.moveToPoint(8, 13, 1200, {.maxSpeed = 75});
//    pros::Task t1([&](){
//        armHandler.moveArm(0, 2000, true);
//        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
//        armMotors.brake();
//    });
//    chassis.waitUntil(12);
//    intakeRamp.retract();
//    //chassis.turnToHeading(140, 400);
//    chassis.waitUntilDone();
//    roller.move(0);
//    chassis.moveToPose(-11.5, 40, 150,1500, {.forwards = false, .maxSpeed = 70});
//    chassis.waitUntil(31);
//    clampPiston.extend();
//
//    chassis.turnToPoint(6, 51, 500);
//    chassis.waitUntilDone();
//    intake.move(0);
//    chassis.moveToPoint(6, 51, 800);
//    chassis.turnToHeading(50, 350); 
//    chassis.waitUntilDone();
//    leftDoinkerPiston.extend();
//    pros::delay(200);
//    moveLinear(chassis, -2, 200, {.minSpeed = 40});  
//    chassis.turnToHeading(20, 350);
//    moveLinear(chassis, -38, 800, {.minSpeed = 40});
//    chassis.waitUntil(20);
//    gameInit(teamColour::blue);
//    intake.move(128);
//    chassis.turnToPoint(-8, 37, 500);
//    chassis.waitUntilDone();
//    leftDoinkerPiston.retract();
//    chassis.turnToPoint(-30, 42, 300);
//    //chassis.moveToPoint(-14, 33, 800, {.maxSpeed = 60});
//    chassis.moveToPoint(-30, 42, 1000);
//    chassis.moveToPoint(-42, -14, 1100);
//    chassis.turnToHeading(225, 400);
//
//    //chassis.turnToHeading(180, 400);
//    //rightDoinkerPiston.retract();
//    //chassis.moveToPoint(-42, -18, 1000);
//    pros::Task t2([&](){
//        armHandler.moveArm(-1400, 2000, true);
//        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
//        armMotors.brake();
//    });
//    chassis.waitUntilDone();
//    chassis.setPose(0, 0, 0);
//    moveLinear(chassis, 35, 1500, {.forwards = true, .minSpeed = 50});
//    moveLinear(chassis, -10, 400, {.forwards = false, .minSpeed = 40});
//    moveLinear(chassis, 20, 1000, {.forwards = true, .minSpeed = 40});
//    moveLinear(chassis, -5, 200, {.forwards = false, .minSpeed = 40});
//
//
//
//
//
//}

void worldsBluePosUnderLadder(bool elims){ //Alliance Stake. Middle ring, under ladder rings
    chassis.setPose(0, 0, 120);//
    armHandler.moveArm(-160, 375, true);
    intake.move(128);
    pros::delay(350);
    intake.move(0);
    armHandler.moveArm(-1400, 1000, true);
    chassis.moveToPoint(-4, 10, 700, {.forwards = false});//
    intake.move(-128);
    pros::delay(100);
    intake.move(0);
    //moveLinear(chassis, -5, 1000, {.forwards = false});
    chassis.turnToPoint(8, 13, 500);//
    chassis.waitUntilDone();
    intakeRamp.extend();
    roller.move(128);
    chassis.moveToPoint(8, 13, 1200, {.maxSpeed = 75});//
    pros::Task t1([&](){
        armHandler.moveArm(100, 2000, true);
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
    });
    chassis.waitUntil(12);
    intakeRamp.retract();
    //chassis.turnToHeading(140, 400);
    chassis.waitUntilDone();
    //roller.move(0);
    chassis.moveToPose(-12.5, 40, 150,1500, {.forwards = false, .maxSpeed = 70});//
    chassis.waitUntil(31);
    clampPiston.extend();
    roller.move(0);

    chassis.turnToPoint(6.5, 51, 700);//
    chassis.waitUntilDone();
    intake.move(0);
    chassis.moveToPoint(6.5, 51, 900);//
    chassis.turnToHeading(40, 350);// 
    chassis.waitUntilDone();
    leftDoinkerPiston.extend();//
    pros::delay(200);
    moveLinear(chassis, -2, 200, {.minSpeed = 40});  
    chassis.turnToHeading(20, 350);//
    moveLinear(chassis, -38, 800, {.minSpeed = 40});
    chassis.waitUntil(20);
    gameInit(teamColour::blue);
    intake.move(128);
    chassis.turnToPoint(-8, 37, 500);//
    chassis.waitUntilDone();
    leftDoinkerPiston.retract();//
    chassis.turnToPoint(-29, 41, 300);//
    //chassis.moveToPoint(-14, 33, 800, {.maxSpeed = 60});
    chassis.moveToPoint(-29, 41, 1000);//

    
    if (!elims){
        chassis.moveToPoint(-41, 30, 1000, {.forwards = false});
        chassis.waitUntilDone();
        pros::delay(750);
        clampPiston.retract();
        //pros::Task t3([&](){
        //    pros::delay(250);
        //    armHandler.moveArm(-1200, 2000, true);
        //    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        //    armMotors.brake();
        //});
        chassis.moveToPose(-4, 56, 110, 2000, {.maxSpeed = 75});//
        chassis.waitUntilDone();
        armHandler.moveArm(-1400, 2000, true);
    }
    if (elims){
        chassis.moveToPoint(-41, 5, 1100);//
        chassis.turnToHeading(225, 400);//

        //chassis.turnToHeading(180, 400);
        //rightDoinkerPiston.retract();
        //chassis.moveToPoint(-42, -18, 1000);
        pros::Task t2([&](){
            pros::delay(250);
            armHandler.moveArm(-1400, 2000, true);
            armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
            armMotors.brake();
        });
        chassis.waitUntilDone();
        chassis.setPose(0, 0, 0);
        moveLinear(chassis, 35, 1300, {.forwards = true, .minSpeed = 50});
        moveLinear(chassis, -10, 400, {.forwards = false, .minSpeed = 40});
        moveLinear(chassis, 20, 1000, {.forwards = true, .minSpeed = 40});
        moveLinear(chassis, -5, 200, {.forwards = false, .minSpeed = 40});
    }
    




}

void worldsRedPosUnderLadder(){ //Alliance Stake. Middle ring, under ladder rings
    chassis.setPose(0, 0, 240);//
    armHandler.moveArm(-160, 375, true);
    intake.move(128);
    pros::delay(350);
    intake.move(0);
    armHandler.moveArm(-1400, 1000, true);
    chassis.moveToPoint(4, 10, 700, {.forwards = false});//
    intake.move(-128);
    pros::delay(100);
    intake.move(0);
    //moveLinear(chassis, -5, 1000, {.forwards = false});
    chassis.turnToPoint(-8, 13, 500);//
    chassis.waitUntilDone();
    intakeRamp.extend();
    roller.move(128);
    chassis.moveToPoint(-8, 13, 1200, {.maxSpeed = 75});//
    pros::Task t1([&](){
        armHandler.moveArm(0, 2000, true);
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
    });
    chassis.waitUntil(12);
    intakeRamp.retract();
    //chassis.turnToHeading(140, 400);
    chassis.waitUntilDone();
    //roller.move(0);
    chassis.moveToPose(12.5, 40, 210,1500, {.forwards = false, .maxSpeed = 70});//
    chassis.waitUntil(31);
    clampPiston.extend();
    roller.move(0);

    chassis.turnToPoint(-5, 51, 700);//
    chassis.waitUntilDone();
    intake.move(0);
    chassis.moveToPoint(-5, 51, 900);//
    chassis.turnToHeading(320, 350);// 
    chassis.waitUntilDone();
    rightDoinkerPiston.extend();//
    pros::delay(200);
    moveLinear(chassis, -2, 200, {.minSpeed = 40});  
    chassis.turnToHeading(340, 350);//
    moveLinear(chassis, -38, 800, {.minSpeed = 40});
    chassis.waitUntil(20);
    gameInit(teamColour::red);
    intake.move(128);
    chassis.turnToPoint(8, 37, 500);//
    chassis.waitUntilDone();
    rightDoinkerPiston.retract();//
    chassis.turnToPoint(32, 40, 300);//
    //chassis.moveToPoint(-14, 33, 800, {.maxSpeed = 60});
    chassis.moveToPoint(32, 40, 1000);//
    chassis.moveToPoint(43, 5, 1100);//
    chassis.turnToHeading(135, 400);//

    //chassis.turnToHeading(180, 400);
    //rightDoinkerPiston.retract();
    //chassis.moveToPoint(-42, -18, 1000);
    pros::Task t2([&](){
        pros::delay(250);
        armHandler.moveArm(-1400, 2000, true);
        armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        armMotors.brake();
    });
    chassis.waitUntilDone();
    chassis.setPose(0, 0, 0);
    moveLinear(chassis, 35, 1500, {.forwards = true, .minSpeed = 50});
    moveLinear(chassis, -10, 400, {.forwards = false, .minSpeed = 40});
    moveLinear(chassis, 20, 1000, {.forwards = true, .minSpeed = 40});
    moveLinear(chassis, -5, 200, {.forwards = false, .minSpeed = 40});




}

void redRingRush(){
    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    armMotors.brake();
    chassis.setPose(0, 0, 340);
    leftDoinkerPiston.extend();
    roller.move(128);
    chassis.moveToPoint(-15, 40.5, 1000);
    chassis.moveToPoint(4, 24, 1000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntil(26);
    clampPiston.extend();
    chassis.turnToHeading(290, 500);
    chassis.waitUntilDone();
    leftDoinkerPiston.retract();
    chassis.turnToHeading(270, 200);
    roller.move(0);
    intake.move(128);
    gameInit(teamColour::red);
    chassis.moveToPoint(-24, 24, 1200, {.maxSpeed = 70});
    chassis.moveToPoint(-22, 0, 1000);
    chassis.turnToHeading(225, 500);
    chassis.moveToPoint(-50, -28, 1700, {.maxSpeed = 50, .minSpeed = 40});
    chassis.moveToPoint(-20, 2, 1200, {.forwards = false, .maxSpeed = 30});
    chassis.moveToPoint(-46, -24, 900, {.maxSpeed = 50,.minSpeed = 40});
    chassis.moveToPoint(-26, -4, 1000, {.forwards = false, .maxSpeed = 30});
    chassis.moveToPoint(-4, -6, 1000,  {.maxSpeed = 70});
    chassis.moveToPoint(38, 38, 2000, {.maxSpeed = 60});
    //moveLinear(chassis, 46, 1000, {.minSpeed = 50});


}

void blueRingRush(){
    armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    armMotors.brake();
    chassis.setPose(0, 0, 20); //
    rightDoinkerPiston.extend();//
    roller.move(128);
    chassis.moveToPoint(15, 40.5, 1000);//
    chassis.moveToPoint(-4, 24, 1000, {.forwards = false, .maxSpeed = 60});//
    chassis.waitUntil(26);
    clampPiston.extend();
    chassis.turnToHeading(70, 500);//
    chassis.waitUntilDone();
    rightDoinkerPiston.retract();//
    chassis.turnToHeading(90, 200);//
    roller.move(0);
    intake.move(128);
    gameInit(teamColour::blue);
    chassis.moveToPoint(24, 24, 1200, {.maxSpeed = 70});//
    chassis.moveToPoint(22, 0, 1000);//
    chassis.turnToHeading(135, 500);//
    chassis.moveToPoint(50, -28, 1700, {.maxSpeed = 50, .minSpeed = 40});//
    chassis.moveToPoint(20, 2, 1200, {.forwards = false, .maxSpeed = 30});//
    chassis.moveToPoint(46, -24, 900, {.maxSpeed = 50,.minSpeed = 40});//
    chassis.moveToPoint(26, -4, 1000, {.forwards = false, .maxSpeed = 30});//
    chassis.moveToPoint(4, -6, 1000,  {.maxSpeed = 70});//
    //chassis.moveToPoint(-38, 38, 2000, {.maxSpeed = 60});//
    //moveLinear(chassis, 46, 1000, {.minSpeed = 50});


}




void autonomous() {
    pros::Task intakeStallTask =  pros::Task(recoverIntake); 

    switch(selection){
        case 0:
            worldsBluePosUnderLadder(false); //TEMPORARY
            break;
        case 1:
            newSigRedLeft4Ring();
            break;
        case 2:
            worldsBluePosUnderLadder(true);
            break;
        case 3:
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
            //newProvsAuto();
            break;
    }

}


void opcontrol() {

    pros::Task controllerFeedbackTask =  pros::Task(controllerFeedback);

    
    gameInit(teamColour::neutral, false);
    

    intake.move(0);

    while (true) {

        int leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);

        chassis.arcade(leftX, leftY);

        controller.get_digital_new_press(DIGITAL_Y) ? clampPiston.toggle() : 0; 
        controller.get_digital_new_press(DIGITAL_A) ? leftDoinkerPiston.toggle() : 0;
        controller.get_digital_new_press(DIGITAL_B) ? rightDoinkerPiston.toggle() : 0;

        
        pros::delay(10);

    }
}


