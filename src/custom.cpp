#include "main.h"

void intakeStallRecovery(pros::v5::MotorGroup &intake) {
    const int STALL_THRESHOLD = 5; // Speed (rpm) considered "stalled"
    const int STALL_TIME_MS = 250; // How long (in ms) of near-zero speed to be considered "stalled"
    const int JIGGLE_DURATION = 250; // How long to reverse the intake (in ms)
    const int JIGGLE_POWER = -127; // Power to use for jiggle back-and-forth
    const int NORMAL_POWER = 127; // Default intake power

    while (true) {
        // Check motor velocity to see if it's stalled
        double intakeVelocity = intake.get_actual_velocity();

        // If velocity is low but motor is supposed to be running
        if (fabs(intakeVelocity) < STALL_THRESHOLD && fabs(intake.get_voltage()) > 2000) {
            pros::delay(STALL_TIME_MS); // Wait to ensure it's truly stalled

            // Double-check after delay to confirm stall
            intakeVelocity = intake.get_actual_velocity();
            if (fabs(intakeVelocity) < STALL_THRESHOLD) {
                // Perform back-and-forth motion to free up the blockage
                intake.move(JIGGLE_POWER); // Move backward
                pros::delay(JIGGLE_DURATION);
                intake.move(NORMAL_POWER); // Move forward
                pros::delay(JIGGLE_DURATION);
            }
        }
        
        pros::delay(50); // Reduce CPU load
    }
}
