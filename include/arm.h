// arm.h
#ifndef ARM_H
#define ARM_H


class ArmHandler {
    private:
        pros::Controller &controller;
        pros::MotorGroup &armMotors;
        pros::Rotation &armRotation;
        double kP;
        double settleError;

        double accumulatedError = 0;
        double previousError = 0;
        double output = 0;
        double timeSpentSettled = 0;
        double timeSpentRunning = 0;
        
        double settleTime = 250;
        double timeout = 0;

        double initialPosition;

        bool startedMoving = false;
    
        bool isBraked = false;
        bool isMovingToPosition = false;
        const int HARD_STOP_POSITION = -5000;
    public:

        ArmHandler(pros::Controller& controller, pros::MotorGroup& armMotors, pros::Rotation &armRotation, double kP, double settleError);
    
        void update();
        double compute(double targetPosition);
        bool isSettled();
        void resetTimers();
        void moveArm(double targetPos, double timeout = 0, bool relative = true);


};

#endif // ARM_H
