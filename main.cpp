/**********************************************************************
* SMARACT POSITIONER CONTROL
* 
* Simple console program for controlling a SmarAct linear positioning 
* stage without position sensor. The positioning stage is connected to 
* a Control Unit (CU) with USB interface.
* 
* Copyright 2019 Tofwerk AG, Thun, Switzerland
* Author: Patrick Sturm <sturm@tofwerk.com>
**********************************************************************/

#include <iostream>
#include <sstream>
#include <windows.h>
#include <SCU3DControl.h>
#include "functions.h"
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[]) {
  
  string command;  // input command
  double position;  // current position (mm)
  
  // read current position from currentposition file
  position = ReadPositionFromFile();
  
  if (argc > 2) {  // too many command line arguments
    
    ShowCommandlineUsage(argv[0]);
    return -1;
    
  } else if (argc == 2) {  // command line argument mode
    
    command = argv[1];
    
    // Check if command is a number
    bool isNumber = CheckIfNumber(command);
    
    // move positioner
    if (isNumber) {

      ExitIfError(SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION));
      
      MovePositioner(command, position);
      
      SA_ReleaseDevices();
      
      WritePositionToFile(position);
      
      cout << position << " mm";
    }
    
    // Set current position to zero
    if (command == "z") {
      position = 0;
      cout << position << " mm";
      WritePositionToFile(position);
    }
    
    // Print some diagnostic information
    if (command == "i") {
      
      // Open the control unit with USB interface in synchronous communication mode
      ExitIfError(SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION));
      
      PrintDeviceInfo();
      printf("\n");
      PrintSystemProperty();
      cout << "Current position: " << position << " mm";
      
      ExitIfError(SA_ReleaseDevices());
    }
    
    // Print help message
    if (command == "h") {
      printf("\nSMARACT POSITIONER CONTROL \n\n");
      printf("Version 0.1.1 \n");
      printf("Copyright 2019 Tofwerk AG \n\n");
      ShowCommandlineUsage(argv[0]);
      printf("\n");
    }
    
    if (!isNumber && command != "i" && command != "h" && command != "z") {
      ShowCommandlineUsage(argv[0]);
    }
    
    return 0;
    
  } else {  // console mode
    
    SetConsoleTitle("SmarAct Positioner Control");
    SetWindowSize(100, 30);
    
    PrintHeader();
    
    // Open the control unit with USB interface in synchronous communication mode
    SA_STATUS st = SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION);
    if (st != SA_OK) {
      printf("SCU error code %u.", st);
      Sleep(5000);
      exit(1);
    }
    
    PrintDeviceInfo();
    printf("\n\n");
    
    // command prompt
    cout << position << " mm $ ";
    
    do {
      
      cin >> command;
      
      // Check if command is a number
      bool isNumber = CheckIfNumber(command);
      
      // move positioner
      if (isNumber) {
        
        MovePositioner(command, position);
        
        WritePositionToFile(position);
        
        cout << position << " mm $ ";
      }
      
      // Set current position to zero
      if (command == "z") {
        position = 0;
        cout << position << " mm $ ";
        WritePositionToFile(position);
      }
      
      // Print some diagnostic information
      if (command == "i") {
        
        cout << CurrentDateTime() << " | ";
        PrintSystemProperty();
        
        cout << position << " mm $ ";
      }
      
      // Clear screen
      if (command == "c") {
        system("cls");
        PrintHeader();
        PrintDeviceInfo();
        printf("\n\n");
        // command prompt
        cout << position << " mm $ ";
      }
      
    } while (command != "q");
    
    SA_ReleaseDevices();
    return 0;
  }
}
