## SmarAct Positioner Control

Simple console program for controlling a [SmarAct](https://www.smaract.com) 
linear positioning stage without position sensor.

### Prerequisites

The positioning stage is connected to a SmarAct Simple Control Unit (SCU) with 
USB interface. The SCU software including the C programming libraries need to be
installed.


### GUI

smaract.exe

![](smaract.png)

### Command line usage

The program can also be used with command line arguments.

Usage:
```shell
smaract <number or command>
```

`<number>`  Move the positioner by `<number>` mm.  
`i`               Show internal temperature and voltage.  
`h`               Show this help message.  
`z`               Set current position to 0 mm.                      

### Author
Patrick Sturm, TOFWERK
