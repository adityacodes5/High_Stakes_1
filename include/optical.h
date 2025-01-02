class OpticsHandler {
    private:
        pros::Optical &opticalSensor;
        pros::MotorGroup &intake;
        pros::Controller &controller;

    public:
        teamColour colour;
        bool disable = false;
        bool isAuton = false;
        OpticsHandler(pros::Optical &opticalSensor, pros::MotorGroup &intake, pros:: Controller &controller, teamColour colour);

        void colourFilter();
        void filterParameters();
};
 