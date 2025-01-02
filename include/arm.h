// arm.h
#ifndef ARM_H
#define ARM_H


class ArmHandler {
    private:
        pros::Controller &controller;
        pros::MotorGroup &armMotors;
    
        bool isBraked = false;
        bool isMovingToPosition = false;
        const int HARD_STOP_POSITION = -5000;
    public:
        ArmHandler(pros::Controller& controller, pros::MotorGroup& armMotors);
    
        void update();

};

#endif // ARM_H
