#ifndef FUNCTIONS_H
#define FUNCTIONS_H


// Set console window size
void SetWindowSize(int width, int height);

// Get current date/time, format is YYYY-MM-DD HH:mm:ss
std::string CurrentDateTime();

// Show command line usage
void ShowCommandlineUsage(std::string name);

// Print the error status if an error occurs.
// See the SCU Programmer's Guide for a list of status/error codes.
void ExitIfError(SA_STATUS st);

// Check if command is a number
bool CheckIfNumber(std::string command);

// Write current position to currentposition file
void WritePositionToFile(double position);

// Read current position from currentposition file
double ReadPositionFromFile();

// Move positioner
void MovePositioner(std::string command, double &position);

// Print device ID, DLL version and Firmware version
void PrintDeviceInfo();

// Print internal temperature and voltage
void PrintSystemProperty();

// Print header
void PrintHeader();

#endif  // FUNCTIONS_H
