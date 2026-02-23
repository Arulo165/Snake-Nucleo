#include "mbed.h"
#include <LEDMatrix.h>

AnalogIn    stickX(PC_2);
AnalogIn    stickY(PC_3);
DigitalIn   stickSW(PC_15);

DigitalIn btn(PA_0);

int xPos = 0;
int yPos = 0;
int counter = 0;

// der schlaue Leser fragt sich evtl warum da stick mapping so kimisch ist nun ja dies liegt 
// daran das es verschiedene perspektiven gibt...
void MoveRight()
{
    deactivateLED(xPos, yPos);
    yPos++;
    if(yPos > 7) yPos = 0;
    activateLED(xPos, yPos);
    sleep(300);
}

void MoveLeft()
{
    deactivateLED(xPos, yPos);
    yPos--;
    if(yPos < 0) yPos = 7;
    activateLED(xPos, yPos);
    sleep(300);
}

void MoveUp()
{
    deactivateLED(xPos, yPos);
    xPos++;
    if(xPos > 7) xPos = 0;
    activateLED(xPos, yPos);
    sleep(300);
}

void MoveDown()
{
    deactivateLED(xPos, yPos);
    xPos--;
    if(xPos < 0) xPos = 7;
    activateLED(xPos, yPos);
    sleep(300);
}

int main() {
    Cs = 1;
    Clk = 0;
    initMatrix();
    activateLED(xPos, yPos);

    while(true)
    {
        if(stickX.read() > 0.7)
        {
            MoveDown();
        }
        else if (stickX.read() < 0.3) 
        {
            MoveUp();
        }

        if(stickY.read() > 0.7) 
        {
            MoveLeft();
        }   
        else if(stickY.read() < 0.3) 
        {
            MoveRight();
        }  
        
    }
}