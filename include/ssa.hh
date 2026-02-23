#include "mbed.h"

DigitalOut led_1(PA_5);
DigitalIn button_1(PA_1);
DigitalOut led_2(PA_6);
DigitalIn button_2(PA_4);
DigitalOut led_3(PA_7);
DigitalIn button_3(PB_0);
DigitalOut led_4(PB_6);

DigitalOut data(PA_9);
DigitalOut uhr(PA_8);
DigitalOut store(PB_5);

#define sleep ThisThread::sleep_for

namespace ssa {

  const int siebenSegWerte[] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
  };

  const int siebenSegBuchstaben[] = {
    0b10001000, // A
    0b10000011, // b
    0b11000110, // C
    0b10100001, // d
    0b10000110, // E
    0b10001110, // F
    0b10000010, // G
    0b10001001, // H
    0b11111001, // I (wie 1)
    0b11110001, // J
    0b10001010, // K (ähnlich H)
    0b11000111, // L
    0b10101011, // M (sehr abstrakt)
    0b10101011, // N (ähnlich M)
    0b11000000, // O
    0b10001100, // P
    0b10011000, // Q
    0b10101111, // R
    0b10010010, // S (wie 5)
    0b10000111, // T
    0b11000001, // U
    0b11000001, // V (wie U)
    0b11000001, // W (wie U)
    0b10001001, // X (wie H)
    0b10010001, // Y
    0b10100100  // Z (wie 2)
  };

  const int enableWerte[] = {
    0b00000001, //linkes 
    0b00000010,
    0b00000100,
    0b00001000 //rechts
  };

  void latch() {
    store = 0;
    store = 1;
  }

  void chooseDigit(int value) { 
    if (value >= 0 && value <= 9) { //wenn value 0-9
      int shiften = siebenSegWerte[value]; //passende stelle des array auslesen
      for (int i = 7; i >= 0; i--) { //durch jedes bit (8 stellen) loopen
        int bit = (shiften >> i) & 1;  //um i (i zählt runter) verschieben und mit der maske verunden
        data = bit; //extrahiertes bit in data schreiben
        uhr = 0; //1 clock tick
        uhr = 1; 
      } 
    } 
  }

  void chooseDigitWithDot(int value) {
    int pattern = 0xFF;
    if (value >= 0 && value <= 9) {
      pattern = ssa::siebenSegWerte[value];
    }
    pattern &= ~(1 << 7); //MSB auf 0 (an)
    for (int i = 7; i >= 0; i--) {
      int bit = (pattern >> i) & 1;
      data = bit;
      uhr = 0;
      uhr = 1;
    }
  }

  void chooseDisplay(int value) {
    if (value >= 1 && value <= 4) { //wenn value 1-4
      int display = enableWerte[value-1];
      for (int i = 7; i >= 0; i--) { //durch jedes bit (8 stellen) loopen
        int bit = (display >> i) & 1; //um i (i zählt runter) verschieben und mit der maske verunden
        data = bit; //extrahiertes bit in data schreiben
        uhr = 0; //1 clock tick
        uhr = 1;
      }
    }
    latch(); //ausgeben
  }

  void showNumber(int number) {
    int digits[4] = {0, 0, 0, 0};

    // Einzelstellen extrahieren
    digits[3] = number % 10;        // rechts
    digits[2] = (number / 10) % 10;
    digits[1] = (number / 100) % 10;
    digits[0] = (number / 1000) % 10; // links

    for (int i = 0; i < 4; i++) {
      ssa::chooseDigit(digits[i]);
      ssa::chooseDisplay(i+1);
      sleep(2);
    }
  }

  void chooseLetter(char letter) { 
    // Nur Großbuchstaben A-Z behandeln
    if (letter >= 'A' && letter <= 'Z') { 
      int index = letter - 'A';             // A=0, B=1, ... Z=25
      int shiften = siebenSegBuchstaben[index]; // passender 7-Segment-Code

      for (int i = 7; i >= 0; i--) {       // durch jedes Bit (8 Stellen) loopen
        int bit = (shiften >> i) & 1;      // Bit extrahieren
        data = bit;                         // Bit an die Datenleitung
        uhr = 0;                            // Clock tick
        uhr = 1;
      } 
    } 
  }

  void showWord(const char* word) {
    char letters[4] = {' ', ' ', ' ', ' '};

    for (int i = 0; i < 4 && word[i] != '\0'; i++) {
      letters[i] = word[i];
    }

    for (int i = 0; i < 4; i++) {
      ssa::chooseLetter(letters[i]);
      ssa::chooseDisplay(i + 1); 
    }
  }
  
};


//BEISPIEL
/*

#if 1

int main() {
  while(true) {
    ssa::showNumber(1870);
  }
}

#endif

*/
