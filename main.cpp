#include <iostream>
#include <vector>
#include "DroneOperation.h"
#include "Survey.h"
#include "MissionPlanning.h"

int main()
{
    srand(time(0)); // Seed the random number generator
    // Initialize the context for the drone operation
    CommandContext context;
    context.weather = "Clear"; // Weather condition - Clear, Stormy
    context.timeOfDay = "Day"; // Time of day: "Day" or "Night"
    context.batteryPower = 50; // Battery power percentage
    context.altitude = 100;    // Altitude of the drone

    // Create instances of drone operations
    TakeOffCommand takeOff;
    Land land;
    Survey survey;
    Failure failure;
    ReturnToHome returnToHome;

    // Command to take off
    std::cout << "Executing TakeOffCommand...\n";
    takeOff.execute(context);

    // Command to perform survey operation
    std::cout << "\nExecuting Survey...\n";
    survey.execute(context); // First time survey execution

    MissionPlanning missionPlanning; // Only one instance needed

    // If survey passes, execute the survey path
    if (context.weather == "Clear" && context.timeOfDay == "Day" && context.batteryPower > 20)
    {
        // Example path for survey
        std::vector<int> surveyPath = missionPlanning.getShortestPath(0, 98);
        survey.executeSurvey(surveyPath); // Execute survey after finding the path
    }

    // Simulate a failure scenario
    context.batteryPower = 5; // Low battery
    std::cout << "\nExecuting Failure Handling...\n";
    failure.execute(context);

    // Command to return home if battery is low
    std::cout << "\nExecuting ReturnToHome...\n";
    returnToHome.execute(context);

    // Display the shortest path from MissionPlanning
    std::cout << "\nExecuting MissionPlanning to find the shortest path to home...\n";
    std::vector<int> shortestPath = missionPlanning.getShortestPath(98, 0);

    if (!shortestPath.empty())
    {
        std::cout << "Shortest path to return home from 98 to 0 is: ";
        for (int node : shortestPath)
        {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }

    // Command to land the drone
    std::cout << "\nExecuting Land...\n";
    land.execute(context);

    return 0;
}
