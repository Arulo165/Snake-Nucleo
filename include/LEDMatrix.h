#pragma once

#include "mbed.h"
#include <cstdint>

enum DirectionType
{
    cDirType_Up = 0,
    cDirType_Down,
    cDirType_Left,
    cDirType_Right,
};

// Globals
DigitalOut Din(PC_0);
DigitalOut Cs(PB_0);
DigitalOut Clk(PC_1);

// Koordinaten system als array erstellen
uint8_t coordinateSystem[8] =
{
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

void sendByte(uint8_t data) {
    for (int i = 7; i >= 0; i--) {
        Din = (data >> i) & 1;
        Clk = 1;
        Clk = 0;
    }
}

void sendData(uint8_t reg, uint8_t data) {
    Cs = 0;
    sendByte(reg);
    sendByte(data);
    Cs = 1;
}

void refresh()
{
    for(int i = 0; i < 8; i++)
    {
        Cs = 0;
        sendByte(i + 1);       
        sendByte(coordinateSystem[i]); 
        Cs = 1;
    }
}

void initMatrix() {
    sendData(0x09, 0x00); // Kein BCD Decode
    sendData(0x0A, 0x08); // Helligkeit (0-15)
    sendData(0x0B, 0x07); // Alle 8 Reihen aktiv
    sendData(0x0C, 0x01); // Normal-Betrieb (kein Shutdown)
    sendData(0x0F, 0x00); // Display-Test aus
    refresh();
}

void activateLED(uint8_t x, uint8_t y)
{
    coordinateSystem[7-y] |= (1 << x);
    refresh();
}

void deactivateLED(uint8_t x, uint8_t y)
{
    coordinateSystem[7-y] &= ~(1 << (x));
    refresh();
}

void clearMatrix()
{
    for(int i = 0; i < 8; i++) coordinateSystem[i] = 0;
    refresh();
}

void lightUp()
{
    for(int i = 0; i < 8; i++) coordinateSystem[i] = 0xFF;
    refresh();
}
