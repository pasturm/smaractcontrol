/**********************************************************************
* SMARACT POSITIONER CONTROL
*
* Simple console program for controlling a SmarAct linear positioning 
* stage without position sensor. The positioning stage is connected to 
* a Control Unit (CU) with USB interface.
*
* Copyright 2019 Tofwerk AG, Thun, Switzerland
* Author: Patrick Sturm <sturm@tofwerk.com>
* 
* Usage: 
* smaract_cmdline <number or command>
*
* <number> Move the positioner by <number> mm.  
* i        Show some positioner diagnostic information.
* h        Show this help message.
* z        Set current position to zero.
*
**********************************************************************/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <SCU3DControl.h>
#include <sstream>
#include <windows.h>  // for Sleep()

using namespace std;

void show_usage(string name)
{
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

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        show_usage(argv[0]);
        return -1;
    } else if (argc > 2) {
        show_usage(argv[0]);
        return -1;
    }

    SA_STATUS st;
    string command;  // input command
    double position;  // current position (mm)
    double step_mm;  // step to move (mm)
    ifstream infile;  // currentposition file
    ofstream outfile;  // currentposition file
    SA_INDEX deviceIndex = 0;
    SA_INDEX channelIndex = 0;
    const unsigned int amplitude = 1000;  // in tenths of Volts
    const int frequency = 1000;  // in Hz
    const double mm_per_step = 0.00257;  // mm to step conversion, see labnote from 2019-05-01
    int steps;  // number of steps
    unsigned int info;
    int value;  // system property value
    int key;  // system property key

    // read current position from currentposition file
    infile.open("currentposition");
    infile >> position;
    infile.close();

	command = argv[1];
	
    // Check if command is a number
    bool isNumber = true;
    for (unsigned int i=0; i < command.length(); i++) {
        if (!isdigit(command[i]) & !ispunct(command[i]) )
        {
            isNumber = false;
            break;
        }
    }
    // move positioner
    if (isNumber) {
        // convert string to number
        stringstream strStream;
        strStream << command;
        strStream >> step_mm;
        // limit step size to reasonable value
        if (abs(step_mm) >= 10) {
            cerr << "Error: Step size must be < 10 mm.";
            exit(-1);
        }
        steps = (int)(step_mm/mm_per_step);
        // Open the control unit with USB interface in synchronous communication mode
        ExitIfError(SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION));
        // Move steps
        ExitIfError(SA_MoveStep_S(deviceIndex, channelIndex, steps, amplitude, frequency));
        // Wait until movement has finished.
        unsigned int status;
        do {
            ExitIfError(SA_GetStatus_S(deviceIndex, channelIndex, &status));
            Sleep(50);
        } while (status == SA_MOVING_STATUS);
        // Release the device
        ExitIfError(SA_ReleaseDevices());

        position += step_mm;
        
        // write current position to currentposition file
        outfile.open("currentposition");
        outfile << position << endl;
        outfile.close();

        cout << position << " mm";
    }

    // Resetting the position to 0 mm
    if (command == "z") {
        position = 0;
        cout << position << " mm";
        // write current position to currentposition file
        outfile.open("currentposition");
        outfile << position << endl;
        outfile.close();
    }

    // Print some diagnostic information
    if (command == "i") {

        // Open the control unit with USB interface in synchronous communication mode
        ExitIfError(SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION));

        ExitIfError(SA_GetDeviceID(deviceIndex, &info));
        printf("Device ID: %u | ", info);

        // Read the version code of the DLL.
        ExitIfError(SA_GetDLLVersion(&info));
        printf("DLL version: %d.%d.%d | ",
            ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));

        // Read the device firmware version
        ExitIfError(SA_GetDeviceFirmwareVersion(deviceIndex, &info));
        printf("Firmware Version: %d.%d.%d | ",
            ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));

        // get current step amplitude
        ExitIfError(SA_GetAmplitude_S(deviceIndex, channelIndex, &info));
        double amplitude_d = static_cast<double>(info)/10.0;
        printf("Step amplitude: %.1f V\n", amplitude_d);

        key = SA_INTERNAL_TEMPERATURE_PROP;
        ExitIfError(SA_GetSystemProperty_S(deviceIndex, key, &value));
        printf("Internal temperature: %u degC | ", value);

        key = SA_INTERNAL_VOLTAGE_PROP;
        ExitIfError(SA_GetSystemProperty_S(deviceIndex, key, &value));
        double volt = static_cast<double>(value)/10.0;
        printf("Internal voltage: %.1f V | ", volt);

        key = SA_HARDWARE_VERSION_CODE_PROP;
        ExitIfError(SA_GetSystemProperty_S(deviceIndex, key, &value));
        printf("Hardware version code: %u \n", value);
        cout << "Current position: " << position << " mm";

        // Release the device
        ExitIfError(SA_ReleaseDevices());
    }

    // Print help message
    if (command == "h") {
        printf("-------------------------------------------------------------------\n");
        printf("SMARACT POSITIONER CONTROL \n\n");
        printf("Version 0.1.0 \n");
        printf("Copyright 2019 Tofwerk AG \n");
        printf("sturm@tofwerk.com \n");
        printf("-------------------------------------------------------------------\n\n");
        show_usage(argv[0]);
    }

    if (!isNumber && command != "i" && command != "h" && command != "z") {
        show_usage(argv[0]);
    }

    return 0;
}
