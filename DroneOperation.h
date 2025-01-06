#ifndef DRONEOPERATION_H
#define DRONEOPERATION_H
#include <iostream>

struct CommandContext {
  std::string weather;   // Weather conditions - Clear, Stormy
  std::string timeOfDay; // Day or Night
  int batteryPower;      // Battery percentage
  int altitude;          // Altitude of the drone
};

class DroneOperation
{
public:
  virtual void execute(const CommandContext& context) = 0;
  virtual ~DroneOperation() = default;
};

class TakeOffCommand: public DroneOperation{
public:
  void execute(const CommandContext &context) override
  {
    if(context.batteryPower < 20){
      std::cout << "Takeoff aborted: Battery power is too low (" << context.batteryPower << "%)." << std::endl;
    }
    else if (context.weather == "Stormy")
    {
      std::cout << "Takeoff aborted: Weather conditions are stormy." << std::endl;
    }
    else
    {
      std::cout << "Drone is taking off during " << context.timeOfDay << "." << std::endl;
    }
  }
};

class Land: public DroneOperation{
public:
  void execute(const CommandContext &context) override
  {
    std::cout << "Drone is landing safely during " << context.timeOfDay << "." << std::endl;
  }
};

#endif // DRONEOPERATION_H