class OpticsHandler {
    private:
        pros::Optical &opticalSensor;
        pros::MotorGroup &intake;
        pros::MotorGroup &roller;
        pros::Controller &controller;

    public:
        teamColour colour;
        bool disable = false;
        bool isAuton = false;
        bool notifier = false;
        OpticsHandler(pros::Optical &opticalSensor, pros::MotorGroup &intake, pros::MotorGroup &roller, pros::Controller &controller, teamColour colour);

        void colourFilter();
        void filterParameters();
};
 