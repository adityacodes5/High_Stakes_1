class OpticsHandler {
    private:
        pros::ADIAnalogIn &opticalSensor;
        bool triggered;
    public:
        OpticsHandler(pros::ADIAnalogIn &opticalSensor);
}
