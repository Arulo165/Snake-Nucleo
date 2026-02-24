#include "mbed.h"
#include <LEDMatrix.h>
#include <vector>

struct Snakepart
{
    int xPos = 0;
    int yPos = 0;
};

struct Apple
{
    int xPos;
    int yPos;
};

AnalogIn    stickX(PC_2);
AnalogIn    stickY(PC_3);
DigitalIn   stickSW(PC_15);

InterruptIn btn(PA_1);

Snakepart snakeHead;
Apple apple;
int counter = 0;
int appleSpawnTime;
int moves = 0;
bool appleSpawned = false;
bool appleCollected = false;
DirectionType lastDirection = cDirType_Up;
std::vector<Snakepart> snake;
int lastX;
int lastY;
bool grow = false;

bool gameRunning = false;
int lastScore = 0;
bool restartRequested = false;

void render()
{
    clearMatrix();
    
    // Schlange zeichnen
    for(int i = 0; i < snake.size(); i++)
    {
        activateLED(snake[i].xPos, snake[i].yPos);
    }
    
    // Apfel zeichnen
    if(appleSpawned)
    {
        activateLED(apple.xPos, apple.yPos);
    }
    
    refresh();
}

// der schlaue Leser fragt sich evtl warum da stick mapping so komisch ist nun ja dies liegt 
// daran das es verschiedene perspektiven gibt...
void MoveRight()
{
    printf("Move Right \n");
    for(int i = snake.size()-1; i > 0; i--)
    {
        snake[i].xPos = snake[i-1].xPos; 
        snake[i].yPos = snake[i-1].yPos;
    }
    
    snake[0].yPos++;
    if(snake[0].yPos > 7) snake[0].yPos = 0;

    if(grow)
    {
        Snakepart newPart;
        newPart.xPos = lastX;
        newPart.yPos = lastY;
        snake.push_back(newPart);
        grow = false;
    }
    render();
}

void MoveLeft()
{
    printf("Move Left \n");
    for(int i = snake.size()-1; i > 0; i--)
    {
        snake[i].xPos = snake[i-1].xPos; 
        snake[i].yPos = snake[i-1].yPos;
    }

    snake[0].yPos--;
    if(snake[0].yPos < 0) snake[0].yPos = 7;

    if(grow)
    {
        Snakepart newPart;
        newPart.xPos = lastX;
        newPart.yPos = lastY;
        snake.push_back(newPart);
        grow = false;
    }
    render();
}

void MoveUp()
{
    printf("Move Up \n");
    int lastX = snake.back().xPos;
    int lastY = snake.back().yPos;

    for(int i = snake.size()-1; i > 0; i--)
    {
        snake[i].xPos = snake[i-1].xPos; 
        snake[i].yPos = snake[i-1].yPos;
    }

    snake[0].xPos++;
    if(snake[0].xPos > 7) snake[0].xPos = 0;

    if(grow)
    {
        Snakepart newPart;
        newPart.xPos = lastX;
        newPart.yPos = lastY;
        snake.push_back(newPart);
        grow = false;
    }

    render(); // einmal alles neuzeichnen
}

void MoveDown()
{
    printf("Move Down \n");
    lastX = snake.back().xPos;
    lastY = snake.back().yPos;

    for(int i = snake.size()-1; i > 0; i--)
    {
        snake[i].xPos = snake[i-1].xPos; 
        snake[i].yPos = snake[i-1].yPos;
    }

    snake[0].xPos--;
    if(snake[0].xPos < 0) snake[0].xPos = 7;

    if(grow)
    {
        Snakepart newPart;
        newPart.xPos = lastX;
        newPart.yPos = lastY;
        snake.push_back(newPart);
        grow = false;
    }
    render();
}

void defineAppleSpawn()
{
    lastX = snake.back().xPos;
    lastY = snake.back().yPos;

    apple.xPos = rand() % 8;
    apple.yPos = rand() % 8;
    while(apple.xPos == snake[0].xPos && apple.yPos == snake[0].yPos)
    {
        apple.xPos = rand() % 8;
        apple.yPos = rand() % 8;
    }
    appleSpawned = true;
}

void defineAppleSpawnTime()
{
    srand(us_ticker_read()); 
    appleSpawnTime = (rand() % 9) + 7; 
}

void SpawnApple()
{
    defineAppleSpawn();
    activateLED(apple.xPos, apple.yPos);
    appleCollected = false;
}

void startGame()
{
    restartRequested = true;
    lastScore = 0;
}

bool checkCollision()
{
    for(int i=1; i < snake.size(); i++)
    {
        if(snake[0].xPos == snake[i].xPos && snake[0].yPos == snake[i].yPos)
        {
            return true;
        }
    }
    return false;
}

int main() {
    Cs = 1;
    Clk = 0;
    initMatrix();
    btn.rise(&startGame);

    snake.push_back(snakeHead); 
    activateLED(snake[0].xPos, snake[0].yPos);

    defineAppleSpawnTime();
    
    while(true)
    {
        while(!gameRunning)
        {
            lightUp();
            ssa::showNumber(lastScore);
            if(restartRequested)
            {
                restartRequested = false;
                snake.clear();
                Snakepart head;
                snake.push_back(head);
                lastDirection = cDirType_Up;
                appleSpawned = false;
                appleCollected = false;
                moves = 0;
                gameRunning = true;
                defineAppleSpawnTime();
                SpawnApple();
            }
        }
        while(gameRunning)
        {
            if(stickX.read() > 0.65 && lastDirection != cDirType_Up)
            {
                lastDirection = cDirType_Down;
            }
            else if (stickX.read() < 0.35 && lastDirection != cDirType_Down) 
            {
                lastDirection = cDirType_Up;
            }

            if(stickY.read() > 0.65 && lastDirection != cDirType_Right) 
            {
                lastDirection = cDirType_Left;
            }   
            else if(stickY.read() < 0.35 && lastDirection != cDirType_Left) 
            {
                lastDirection = cDirType_Right;
            }
            
            switch (lastDirection) 
            {
                case cDirType_Up: MoveUp();break;
                case cDirType_Down: MoveDown();break;
                case cDirType_Right: MoveRight();break;
                case cDirType_Left: MoveLeft();break;
            }

            if(apple.xPos == snake[0].xPos && apple.yPos == snake[0].yPos && appleSpawned) 
            {
                appleSpawned = false;
                appleCollected = true;
                moves = 0; 
                grow = true;
                lastScore++;
            }
            if(checkCollision())
            {
                gameRunning = false;
            }

            sleep(300);

            moves++;
            if((moves == appleSpawnTime && !appleSpawned) || appleCollected)
            {
                SpawnApple();
                moves = 0;
                defineAppleSpawnTime();
            }
        
        }
    }
}
