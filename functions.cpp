#include <windows.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <SCU3DControl.h>
#include "functions.h"

using namespace std;

// Set console window size
void SetWindowSize(int width, int height) {
  _COORD coord;
  coord.X = width;
  coord.Y = height;
  _SMALL_RECT rect;
  rect.Top = 0;
  rect.Left = 0;
  rect.Bottom = height - 1;
  rect.Right = width - 1;
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);      // Get handle
  SetConsoleScreenBufferSize(handle, coord);            // Set buffer size
  SetConsoleWindowInfo(handle, true, &rect);            // Set window size
}

// Get current date/time, format is YYYY-MM-DD HH:mm:ss
std::string CurrentDateTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
  return buf;
}

// Show command line usage
void ShowCommandlineUsage(string name) {
  cout << "Usage:\n" << name << " <number or command>\n" 
       << "<number> Move the positioner by <number> mm.\n"
       << "i        Show some positioner diagnostic information.\n"
       << "h        Show this help message.\n"
       << "z        Set current position to zero.\n";
}

// Print the error status if an error occurs.
// See the SCU Programmer's Guide for a list of status/error codes.
void ExitIfError(SA_STATUS st) {
  if (st != SA_OK) {
    cerr << "SCU error code " << st;
    exit(st);
  }
}

// Check if command is a number
bool CheckIfNumber(string command) {
  bool isNumber = true;
  for (unsigned int i=0; i < command.length(); i++) {
    if (!isdigit(command[i]) & !ispunct(command[i]) )
    {
      isNumber = false;
      break;
    }
  }
  return isNumber;
}

// Write current position to currentposition file
void WritePositionToFile(double position) {
  ofstream outfile;
  outfile.open("currentposition");
  outfile << position << endl;
  outfile.close();
}

// Read current position from currentposition file
double ReadPositionFromFile() {
  ifstream infile;
  double position;
  infile.open("currentposition");
  infile >> position;
  infile.close();
  return position;
}

// Move positioner
void MovePositioner(string command, double &position) {
  
  double step_mm;  // step to move (mm)
  const unsigned int amplitude = 1000;  // in tenths of Volts
  const unsigned int frequency = 1000;  // in Hz
  const double mm_per_step = 0.00257;  // mm to step conversion, see labnote from 2019-05-01
  SA_INDEX device_index = 0;
  SA_INDEX channel_index = 0;
  double newposition;
  
  // convert string to number
  stringstream strStream;
  strStream << command;
  strStream >> step_mm;
  
  // limit step size to reasonable value
  if (abs(step_mm) >= 10) {
    printf("Step size must be < 10 mm.\n");
    step_mm = 0;
  }
  
  // convert step_mm to steps
  int steps = (int)(step_mm/mm_per_step);
  
  // move
  SA_STATUS st = SA_MoveStep_S(device_index, channel_index, steps, amplitude, frequency);
  
  // Wait until movement has finished.
  if (st == SA_OK) {
    
    unsigned int status;
    do {
      SA_GetStatus_S(device_index, channel_index, &status);
      Sleep(50);
    } while (status == SA_MOVING_STATUS);
    
    position += step_mm;
    
  } else {
    printf("SCU error code %u.\n", st);
  }
}

// Print device ID, DLL version and Firmware version
void PrintDeviceInfo() {
  SA_INDEX device_index = 0;
  unsigned int info;
  
  SA_GetDeviceID(device_index, &info);
  printf("Device ID: %u | ", info);
  
  SA_GetDLLVersion(&info);
  printf("DLL version: %d.%d.%d | ",
         ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));
  
  SA_GetDeviceFirmwareVersion(device_index, &info);
  printf("Firmware Version: %d.%d.%d",
         ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));
}

// Print internal temperature and voltage
void PrintSystemProperty() {
  SA_INDEX device_index = 0;
  int value;  // system property value

  SA_GetSystemProperty_S(device_index, SA_INTERNAL_TEMPERATURE_PROP, &value);
  printf("Internal temperature: %u degC | ", value);
  
  SA_GetSystemProperty_S(device_index, SA_INTERNAL_VOLTAGE_PROP, &value);
  double volt = static_cast<double>(value)/10.0;
  printf("Internal voltage: %.1f V\n", volt);
}