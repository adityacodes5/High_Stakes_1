#include "main.h"

void blueLeft(){
    chassis.moveToPose(-2, -24, 30, 4000, {.forwards = false});
    chassis.moveToPoint(-5, -27, 4000, {.forwards = false});
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
    chassis.moveToPoint(-34, -32, 4000, {.maxSpeed = 50});
}
void redRight(){
    chassis.moveToPose(2, -24, 330, 4000, {.forwards = false});
    chassis.moveToPoint(5, -27, 4000, {.forwards = false});
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
    chassis.moveToPoint(34, -32, 4000, {.maxSpeed = 50});

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