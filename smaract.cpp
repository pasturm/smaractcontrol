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
#include <fstream>
#include <stdlib.h>
#include <SCU3DControl.h>
#include <sstream>
#include <windows.h>  // for Sleep()
#include <ctime>

using namespace std;

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
std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

int main(int argc, char* argv[])
{
    SetConsoleTitle("SmarAct Positioner Control");
    SetWindowSize(100, 30);

    SA_STATUS st;
    string command;  // input command
    double position;  // current position (mm)
    double step_mm;  // step to move (mm)
    ifstream infile;  // currentposition file
    ofstream outfile;  // currentposition file
    SA_INDEX deviceIndex = 0;
    SA_INDEX channelIndex = 0;
    const unsigned int amplitude = 1000;  // in tenths of Volts
    const unsigned int frequency = 1000;  // in Hz
    const double mm_per_step = 0.00257;  // mm to step conversion, see labnote from 2019-05-01
    int steps;  // number of steps
    unsigned int info;
    int value;  // system property value
    int key;  // system porperty key

  	printf("-------------------------------------------------------------------\n");
    printf("SMARACT POSITIONER CONTROL \n\n");
    printf("Version 0.1.0 \n");
    printf("Copyright 2019 Tofwerk AG \n");
    printf("-------------------------------------------------------------------\n\n");
    printf("Type a number or command and press enter.\n\n");
    printf("<number> Move the positioner by <number> mm.\n");
    printf("i        Show some diagnostic information.\n");
    printf("z        Set current position to zero.\n");
    printf("q        Quit program.\n\n");
    printf("The command prompt shows the current position.\n");
    printf("-------------------------------------------------------------------\n\n");
    
    // Open the control unit with USB interface in synchronous communication mode
    st = SA_InitDevices(SA_SYNCHRONOUS_COMMUNICATION);
    if (st != SA_OK) {
        printf("SCU error code %u.", st);
        Sleep(5000);
        exit(1);
    }

    SA_GetDeviceID(deviceIndex, &info);
    printf("Device ID: %u | ", info);
    SA_GetDLLVersion(&info);
    printf("DLL version: %d.%d.%d | ",
        ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));
    SA_GetDeviceFirmwareVersion(deviceIndex, &info);
    printf("Firmware Version: %d.%d.%d\n\n",
        ((info >> 24) & 0xff), ((info >> 16) & 0xff), (info & 0xffff));

    // read current position from currentposition file
    infile.open("currentposition");
    infile >> position;
    infile.close();

    // command prompt
    cout << position << " mm $ ";

    do {
    	
    	cin >> command;

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
                printf("Step size must be < 10 mm.\n");
                step_mm = 0;
            }
            steps = (int)(step_mm/mm_per_step);
            st = SA_MoveStep_S(deviceIndex, channelIndex, steps, amplitude, frequency);
            if (st == SA_OK) {
                // Wait until movement has finished.
                unsigned int status;
                do {
                    SA_GetStatus_S(deviceIndex, channelIndex, &status);
                    Sleep(50);
                } while (status == SA_MOVING_STATUS);

                position += step_mm;
                
                // write current position to currentposition file
                outfile.open("currentposition");
                outfile << position << endl;
                outfile.close();
            } else {
                printf("SCU error code %u.\n", st);
            }

            cout << position << " mm $ ";
        }

        // Set current position to zero
        if (command == "z") {
            position = 0;
            cout << position << " mm $ ";
            // write current position to currentposition
            outfile.open("currentposition");
            outfile << position << endl;
            outfile.close();
        }

        // Print some diagnostic information
        if (command == "i") {

            cout << currentDateTime() << " | ";

            key = SA_INTERNAL_TEMPERATURE_PROP;
            SA_GetSystemProperty_S(deviceIndex, key, &value);
            printf("Internal temperature: %u degC | ", value);

            key = SA_INTERNAL_VOLTAGE_PROP;
            SA_GetSystemProperty_S(deviceIndex, key, &value);
            double volt = static_cast<double>(value)/10.0;
            printf("Internal voltage: %.1f V\n", volt);

            cout << position << " mm $ ";
        }

    } while (command != "q");

    // Release the device
    SA_ReleaseDevices();
    return 0;
}
