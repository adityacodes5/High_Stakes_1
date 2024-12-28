#include "main.h"

void intakeStallRecovery(pros::v5::MotorGroup &intake) {
    const int STALL_THRESHOLD = 5; // rpm
    const int STALL_TIME_MS = 250; // detection time (ms)
    const int JIGGLE_DURATION = 250; // time to reverse (ms)
    const int JIGGLE_POWER = -128; // strength of "jiggle"
    const int NORMAL_POWER = 128; // default intake power

    while (true) {
        double intakeVelocity = intake.get_actual_velocity();

        // If velocity < threshold, then start timing the stall
        if (fabs(intakeVelocity) < STALL_THRESHOLD && fabs(intake.get_voltage()) > 2000) {
            pros::delay(STALL_TIME_MS); 

            intakeVelocity = intake.get_actual_velocity();
            if (fabs(intakeVelocity) < STALL_THRESHOLD) {
                intake.move(JIGGLE_POWER);
                pros::delay(JIGGLE_DURATION);
                intake.move(NORMAL_POWER); 
                pros::delay(JIGGLE_DURATION);
            }
        }
        
        pros::delay(50); 
    }
}
