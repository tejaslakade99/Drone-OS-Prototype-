#ifndef SURVEY_H
#define SURVEY_H

#include "DroneOperation.h"
#include <iostream>

class Survey : public DroneOperation
{

public:
  void execute(const CommandContext &context) override
  {
    // Check weather condition
    if (context.weather != "Clear")
    {
      std::cout << "Survey failed: Weather is not clear. Weather: " << context.weather << std::endl;
      return;
    }

    // Check if it's nighttime (surveys might not be possible at night)
    if (context.timeOfDay == "Night")
    {
      std::cout << "Survey failed: It's nighttime. Cannot perform survey." << std::endl;
      return;
    }

    // Check battery level (we need enough battery to perform the survey)
    if (context.batteryPower < 20)
    {
      std::cout << "Survey failed: Low battery. Battery: " << context.batteryPower << "%" << std::endl;
      return;
    }

    // Proceed with the survey operation
    std::cout << "Starting survey operation at altitude: " << context.altitude << " meters." << std::endl;
  }

    void executeSurvey(std::vector<int>& path) {
        std::cout << "Drone Surveying the Path:\n";
        for (int node : path) {
            std::cout << "Surveying node: " << node << "\n";
        }
    }

};

class Failure : public DroneOperation
{
public:
  void execute(const CommandContext &context) override
  {
    // Handle failure scenario
    std::cout << "Drone operation failed. Current battery: " << context.batteryPower << "%" << std::endl;

    // Check battery level (if too low, return home or stop)
    if (context.batteryPower < 10)
    {
      std::cout << "Battery critically low. Returning to home." << std::endl;
      // Trigger ReturnToHome logic or similar
    }
    else
    {
      std::cout << "Drone failure handled. Battery level is acceptable." << std::endl;
    }

    // Additional failure handling logic can be added here
  }
};

class ReturnToHome : public DroneOperation
{
public:
  void execute(const CommandContext &context) override
  {
    // Check battery level (if too low, return home immediately)
    if (context.batteryPower < 15)
    {
      std::cout << "Battery low (" << context.batteryPower << "%). Returning to home." << std::endl;
    }

    // Check if the weather conditions are suitable for returning home
    if (context.weather != "Clear")
    {
      std::cout << "Weather conditions are not suitable for return. Weather: " << context.weather << std::endl;
      return;
    }

    // Print message about returning to home based on altitude
    std::cout << "Returning to home from altitude: " << context.altitude << " meters." << std::endl;
    // Logic for returning home would go here (e.g., navigation, communication with base)
  }
};

#endif // SURVEY_H
