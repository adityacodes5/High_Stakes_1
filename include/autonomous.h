#ifndef AUTONOMOUS_H
#define AUTONOMOUS_H

extern lemlib::Chassis chassis;
extern pros::MotorGroup intake;
extern pros::adi::Pneumatics clampPiston;
extern pros::adi::DigitalIn bumper;

extern void blueLeft();
extern void redRight();
extern void blueRight();
extern void redLeft();
extern void progSkills();

#endif // AUTONOMOUS_H