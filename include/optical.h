class OpticsHandler {
    private:
        pros::Optical &opticalSensor;
        pros::MotorGroup &intake;
        pros::Controller &controller;
        teamColour colour;
    public:
        OpticsHandler(pros::Optical &opticalSensor, pros::MotorGroup &intake, pros:: Controller &controller, teamColour colour);

        void colourFilter();
};
 