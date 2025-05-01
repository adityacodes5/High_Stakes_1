
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
    chassis.moveToPoint(-24, 42, 1500, {.forwards = false,.minSpeed = 30});
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
    chassis.moveToPoint(-50, 20, 2000, {.forwards = false, .minSpeed = 60});



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
    chassis.moveToPose(50, -5, 90, 2500, {.maxSpeed = 60});
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
    pros::delay(700);
    intake.move(0);
    chassis.moveToPoint(-54, 0, 1500);
    chassis.turnToHeading(0, 900);
    chassis.moveToPose(-48, -32, 0, 2000, {.forwards = false, .maxSpeed = 60});

    chassis.waitUntil(30);
    clampPiston.toggle();
    intake.move(128);
    chassis.moveToPoint(-24, -24, 1500, {.maxSpeed = 70, .minSpeed = 40});
    chassis.turnToPoint(-2, -51, 1000);
    chassis.moveToPoint(-2, -51, 2000, {.maxSpeed = 65, .minSpeed = 40});
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
    intake.move(128);

    chassis.moveToPoint(48, 5, 4000, {.minSpeed = 40});
    chassis.turnToPoint(48, 0, 1200);
    chassis.moveToPoint(-60, -60, 3000, {.minSpeed = 80});


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


void trackerTest(){
    pros::Task armTask = pros::Task(armHandlerFunction);

    chassis.setPose(-63.5, 0, 90);
    intakeHandler.disable = true;
    intake.move(128);
    pros::delay(700);
    intake.move(0);
    chassis.moveToPoint(-54, 0, 1000);
    chassis.turnToHeading(0, 750);
    chassis.moveToPose(-48, -32, 0, 2000, {.forwards = false, .maxSpeed = 60});

    chassis.waitUntil(30);
    clampPiston.toggle();
    intakeHandler.disable = false;
    intake.move(128);
    chassis.moveToPoint(-24, -24, 1500, {.maxSpeed = 70});
    chassis.moveToPoint(-20, -34, 1500, {.maxSpeed = 80});
    chassis.moveToPoint(26, -49, 2000, {.maxSpeed = 60});
    chassis.moveToPoint(1.5, -42, 1000, {.forwards = false, .maxSpeed = 80});
    chassis.turnToHeading(180, 1000);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    armMotors.tare_position(); // Reset encoder position to 0
    armMotors.move_relative(-355, 100); // Move 320 degrees relative at 50 rpm
    chassis.moveToPoint(0, -60, 1250, {.minSpeed = 40});
    chassis.waitUntilDone();
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(0, -68, 1500, {.minSpeed = 40});
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
    pros::delay(150);
    armHandler.moveArm(-1400, 1250, false);
    chassis.moveToPoint(0, -50, 1500, {.forwards = false, .minSpeed = 30});
    chassis.waitUntilDone();
    intakeHandler.disable = false;
    intake.move(128);
    armHandler.moveArm(2000, 1250, false);


    chassis.moveToPoint(-24, -48, 2000, {.maxSpeed = 70});
    chassis.moveToPoint(-60, -48, 3000, {.maxSpeed = 70});
    chassis.moveToPoint(-44, -44, 2000, {.forwards = false, .minSpeed = 40});
    chassis.moveToPoint(-48, -64, 2000, {.maxSpeed = 60});
    chassis.moveToPoint(-44, -44, 2000, {.forwards = false, .minSpeed = 40});
    chassis.turnToHeading(45, 800);
    chassis.moveToPose(-60, -60, 45, 2000, {.forwards = false, .minSpeed = 80});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();
    pros::delay(200);



    intake.move(128);
    chassis.moveToPoint(-54, 0, 2000, {.maxSpeed = 80});
    chassis.moveToPose(-48, 32, 180, 2500, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();

    chassis.waitUntil(30);
    clampPiston.toggle();
    intakeHandler.disable = false;
    intake.move(128);
    chassis.moveToPoint(-24, 24, 1500, {.maxSpeed = 60});
    //chassis.moveToPoint(-20, 34, 1500, {.maxSpeed = 60});
    //chassis.moveToPoint(27, 49, 2000, {.maxSpeed = 60});
    chassis.moveToPoint(1.5, 42, 1000, {.maxSpeed = 80});// if ladybrown, backwards!
    chassis.turnToHeading(0, 700);
    //armHandler.moveArm(500, 500, true);
    //armMotors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    //armMotors.brake();
    //chassis.waitUntilDone();
    //intakeHandler.disable = true;
    //armMotors.tare_position(); // Reset encoder position to 0
    //armMotors.move_relative(-355, 100); // Move 320 degrees relative at 50 rpm
    //chassis.moveToPose(1.5, 60, 0, 1250, {.minSpeed = 40});
    //chassis.waitUntilDone();
    //pros::delay(1250);
    //intake.move(0);
    chassis.moveToPoint(1.5, 68, 900, {.minSpeed = 40});
    //intake.move(128);
    //pros::delay(150);
    //intake.move(0);
    //pros::delay(150);
    //intake.move(128);
    //pros::delay(150);
    //intake.move(0);
    //intake.move(128);
    //pros::delay(200);
    //intake.move(0);
    //pros::delay(150);
    //armHandler.moveArm(-1400, 1250, false);
    chassis.moveToPoint(1.5, 46, 1500, {.forwards = false, .minSpeed = 30});
    //chassis.waitUntilDone();
    //intake.move(128);
    //intakeHandler.disable = false;
    //armHandler.moveArm(2000, 1250, false);
    chassis.turnToHeading(270, 900);
    chassis.moveToPoint(-24, 49, 2000, {.maxSpeed = 60});
    chassis.moveToPoint(-60, 49, 3000, {.maxSpeed = 60});
    chassis.moveToPoint(-44, 44, 2000, {.forwards = false, .minSpeed = 40});
    chassis.moveToPoint(-48, 64, 1500, {.maxSpeed = 80});
    chassis.moveToPoint(-44, 44, 2000, {.forwards = false, .minSpeed = 40});
    chassis.turnToHeading(135, 800);
    chassis.moveToPose(-63, 61, 135, 3000, {.forwards = false, .minSpeed = 40});
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(-128);
    pros::delay(250);
    intake.move(0);
    clampPiston.toggle();
    pros::delay(200);

    gameInit(teamColour::red);
    intakeHandler.disable = false;
    roller.move(128);
    chassis.moveToPoint(0, 48, 2500, {.minSpeed = 80});
    chassis.moveToPoint(24, 24, 2500, {.minSpeed = 70});
    chassis.moveToPose(41, 0, 330, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.moveToPoint(44, -5, 2000, {.forwards = false, .maxSpeed = 60});
    chassis.waitUntilDone();
    clampPiston.toggle();
    roller.move(0);
    intake.move(128);
    chassis.moveToPoint(42, 24, 2000);
    chassis.moveToPoint(44, 48, 2000, {.maxSpeed = 60});
    chassis.moveToPoint(44, 60, 3000, {.maxSpeed = 60});
    chassis.moveToPoint(40, 48, 1000, {.forwards = false, .minSpeed = 60});
    chassis.moveToPoint(60, 48, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(44, 44, 2000, {.forwards = false, .minSpeed = 40});
    chassis.moveToPose(61, 61, 225, 3000, {.forwards = false, .minSpeed = 40});
    chassis.waitUntilDone();
    clampPiston.toggle();
    chassis.moveToPoint(48, 0, 1200, {.minSpeed = 90});
    chassis.moveToPoint(60, -60, 3000,  {.minSpeed = 90});
    chassis.waitUntilDone();
    armHandler.moveArm(-1400, 1250, false);
    chassis.moveToPose(0, 0, 135, 5000, {.forwards = false, .maxSpeed = 50});
    



}


void newSigBlueRightAllianceStake(){
    
    intake.move(0);
    chassis.moveToPose(2, -24, 330, 1250, {.forwards = false});
    chassis.moveToPoint(5, -27, 600, {.forwards = false});
    chassis.waitUntilDone();
    pros::delay(250);
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
    pros::delay(750);
    intakeHandler.disable = true;
    intake.move(0);
    roller.move(128);

    chassis.moveToPoint(30, 1, 2000);
    pros::delay(1000);
    clampPiston.toggle();
    chassis.turnToHeading(180, 1000);
    chassis.moveToPoint(30, 16, 1000, {.forwards = false});
    chassis.waitUntilDone();
    leftMotors.move(30);
    rightMotors.move(30);
    pros::delay(115);
    leftMotors.move(0);
    rightMotors.move(0);
    pros::delay(150);
    chassis.waitUntilDone();
    intakeHandler.disable = false;
    intake.move(128);
    pros::delay(750);
    intake.move(0);
    intakeHandler.disable = true;
    chassis.moveToPoint(30, -30, 5000, {.maxSpeed = 60});


}

void newSigRedLeftAllianceStake(){
    
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
    chassis.moveToPoint(29, -48, 1500, {.maxSpeed = 60});
    chassis.moveToPoint(17, -44, 1000, {.forwards = false, .minSpeed = 80});  
    chassis.turnToPoint(22, -28, 600);
    chassis.moveToPoint(22, -28, 1500, {.maxSpeed = 60});
    pros::delay(750);
    intake.move(0);
    roller.move(128);

    chassis.moveToPoint(-30, 1, 2000);
    pros::delay(1000);
    clampPiston.toggle();
    chassis.turnToHeading(180, 1000);
    chassis.moveToPoint(-30, 16, 1000, {.forwards = false});
    chassis.waitUntilDone();
    leftMotors.move(30);
    rightMotors.move(30);
    pros::delay(115);
    leftMotors.move(0);
    rightMotors.move(0);
    pros::delay(150);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(128);
    pros::delay(750);
    intake.move(0);
    chassis.moveToPoint(-30, -30, 5000, {.maxSpeed = 60});


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


void allianceStakeTest(){
    chassis.moveToPoint(0, -4, 2000, {.forwards = false});
    chassis.turnToHeading(90, 1000);

    chassis.moveToPoint(30, -4, 1000);
    chassis.turnToHeading(180, 800);
    chassis.moveToPoint(30, 16, 1000, {.forwards = false});
    chassis.waitUntilDone();
    leftMotors.move(30);
    rightMotors.move(30);
    pros::delay(115);
    leftMotors.move(0);
    rightMotors.move(0);
    pros::delay(150);
    chassis.waitUntilDone();
    intakeHandler.disable = true;
    intake.move(128);

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