=== TEXT-CONTROLLED Security Alarm ===

This uses the at90usb1286 microcontroller to implement a text controlled secutiry alarm. It is designed for the LaFortuna, a board built at the University of Southampton. The project uses the SIM800L GSM module for sending and receiving text messages. It has a number of outputs to indicate system state.

=> To use

1- Enter number using up/down for change it and select to progress

2- After entering number the LaFortuna will send a text confirming system is on

3- Text 'ARM' to arm alarm or 'DISARM' to disarm alarm

Note the disarm command will stop the alarm if its currently going off.
If alarm goes off, you will be alerted via text. 

=> Pins used

    PE7 for select
    PE2 for down
    PE4 for up
    
    PF0 for when not armed LED
    PF1 for when armed LED but no alarm
    PF2 for when armed LED but alarm
    
    PD2 for receving UART with GSM module
    PD3 for send UART with GSM module
    
    PD1 for buzzer output
    PD0 for sensor input (uses interrupt)

    Note that GSM module requires 4.2V minimum to function when connecting the circuits - higher than the LaFortuna's voltage. 

=> Other requirements

    LCD screen with controller
    SIM800L GSM module connected to PD2 and PD3

=> How to Build

    Run 'make' in the bash command line

=> Code included from other sources

    MakeFile and colours file with thanks to Klaus-Peter Zauner
    LCD libraries with thanks to Steve Gunn

=> License 

My code is available using the MIT license.

Copyright 2017 Alexander Stonard

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

https://opensource.org/licenses/MIT
