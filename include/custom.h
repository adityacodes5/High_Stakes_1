#ifndef _CUSTOM_H_
#define _CUSTOM_H_

class IntakeHandler{
    private:
        pros::v5::MotorGroup &intake;
    public:

        bool disable = false;
        
        IntakeHandler(pros::v5::MotorGroup &intake);
        void intakeStallRecovery();
};


#endif // _CUSTOM_H_
