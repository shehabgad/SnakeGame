#include <iostream>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <fcntl.h>
#include <list>
#include <stdlib.h>
#include <wchar.h>
#include <thread>
#include <cwchar>
using namespace std;

// setting up width and height of window
const int WIDTH = 120;
const int HEIGHT = 60;

// input and output handles
HANDLE wHND;
HANDLE rHND;


void ClearBuffer(CHAR_INFO buffer[],int bufferWidth = WIDTH, int bufferHeight = HEIGHT)
{
	for (int i = 0, size = WIDTH * HEIGHT; i < size; i++)
	{
	    buffer[i].Char.UnicodeChar = L' ';
		buffer[i].Attributes = 15;
	}
}

void DrawingBoundries(CHAR_INFO buffer[], wchar_t shapeX, wchar_t shapeY, int color) {
	for (int x = 5; x < WIDTH - 10; x++)
	{
		buffer[x + WIDTH * (HEIGHT - 57)].Char.UnicodeChar = shapeX;
		buffer[x + WIDTH * (HEIGHT - 57)].Attributes = color;

		buffer[x + WIDTH * (HEIGHT - 19)].Char.UnicodeChar = shapeX;
		buffer[x + WIDTH * (HEIGHT - 19)].Attributes = color;
	}
	
	for (int y = HEIGHT - 19, height = HEIGHT - 57; y >= height; y--)
	{

		buffer[110 + WIDTH * y].Char.UnicodeChar = shapeY;
		buffer[110 + WIDTH * y].Attributes = color;
		buffer[4 + WIDTH * y].Char.UnicodeChar = shapeY;
		buffer[4 + WIDTH * y].Attributes = color;
	}
}
void WriteText(CHAR_INFO buffer[], const wchar_t* text ,COORD pos, int color)
{

	for (int i = pos.X, writingSize= wcslen(text) + pos.X, j = 0; i < writingSize ; i++)
	{
		buffer[i + WIDTH * pos.Y].Char.UnicodeChar = text[j++];
		buffer[i + WIDTH * pos.Y].Attributes = color;

	}


}

int main()
{
	// set the mode to write unicode character
	_setmode(_fileno(stdout), _O_U16TEXT);

	// set the font famiily to arial unicode ms instead of consolos 
	/*
	 CONSOLE_FONT_INFOEX cfi;
	 cfi.cbSize = sizeof(cfi);
	 cfi.nFont = 0;
	 cfi.dwFontSize.X = 0;                   
	 cfi.dwFontSize.Y = 16;                  
	 cfi.FontFamily = FF_DONTCARE;
	 cfi.FontWeight = FW_NORMAL;
	 wcscpy_s(cfi.FaceName, L"Consolas"); 
	 SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	*/
	
	// setting up window
	wHND = GetStdHandle(STD_OUTPUT_HANDLE);
	rHND = GetStdHandle(STD_INPUT_HANDLE);
	SMALL_RECT windowSize;
	windowSize.Top = windowSize.Left = 0;
	windowSize.Right = WIDTH - 1;
	windowSize.Bottom = HEIGHT - 1;

	SetConsoleWindowInfo(wHND, TRUE, &windowSize);

	// setting up screen buffer size
	COORD screenBufferSize = { WIDTH, HEIGHT };
	SetConsoleScreenBufferSize(wHND, screenBufferSize);

	// setting up console buffer
	CHAR_INFO consoleBuffer[WIDTH * HEIGHT];
	COORD pos = { 0,0 };
	COORD bufferSize = { WIDTH, HEIGHT };
	SMALL_RECT writingArea = { 0 ,0, WIDTH - 1, HEIGHT - 1 };
	ClearBuffer(consoleBuffer);

	bool Menu = true;
	while (Menu)
	{
		for (int x = 5; x < WIDTH - 5; x++)
		{
			for (int y = 2; y < HEIGHT - 13; y++)
			{
			consoleBuffer[x + WIDTH * y].Char.UnicodeChar = L' ';
			consoleBuffer[x + WIDTH * y].Attributes = 205;
			}
		}

		WriteText(consoleBuffer, L"SNAKE GAME", COORD{ 50,20 }, 252);
		WriteText(consoleBuffer, L"MADE BY SHEHAB GAD", COORD{ 46,22 }, 252);
		WriteText(consoleBuffer, L"PRESS ENTER TO PLAY:", COORD{ 45,24 }, 252);


		DWORD numberOfEventsHappend = 0;
		GetNumberOfConsoleInputEvents(rHND, &numberOfEventsHappend);
		DWORD numberOfEventsRead = 0;
		if (numberOfEventsHappend != 0)
		{
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numberOfEventsHappend];
			ReadConsoleInput(rHND, eventBuffer, numberOfEventsHappend, &numberOfEventsRead);
			for (int i = 0; i < numberOfEventsRead; i++)
			{
				if (eventBuffer[i].EventType == KEY_EVENT)
				{
				    if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
					{
						Menu = false;
						break;
					}
				}
			}
		}
		WriteConsoleOutputW(wHND, consoleBuffer, bufferSize, pos, &writingArea);

	}
	ClearBuffer(consoleBuffer);
	bool Playing = true;
	while (Playing)
	{

		bool GameRunning = true;
		list<COORD> Snake = {
			COORD{50,20},
			COORD{51,20},
			COORD{52,20},
			COORD{53,20},
			COORD{54,20},
			COORD{55,20},
			COORD{56,20},
			COORD{57,20},
			COORD{58,20},
			COORD{59,20},
			COORD{60,20},
			COORD{61,20},
			COORD{62,20},
			COORD{63,20},
			COORD{64,20},
			COORD{65,20},
		};

		int SnakeDirection = 1;
		int FoodX = 0;
		int FoodY = 0;
		int FoodColor = 15;
		bool FoodEaten = false;
		int score = 0;
		while (GameRunning)
		{
			
			ClearBuffer(consoleBuffer);

			// responding to events
			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < 75ms)
			{
				DWORD numberOfEventsHappend = 0;
				GetNumberOfConsoleInputEvents(rHND, &numberOfEventsHappend);
				DWORD numberOfEventsRead = 0;
				if (numberOfEventsHappend != 0)
				{
					INPUT_RECORD* eventBuffer = new INPUT_RECORD[numberOfEventsHappend];
					ReadConsoleInput(rHND, eventBuffer, numberOfEventsHappend, &numberOfEventsRead);
					for (int i = 0; i < numberOfEventsRead; i++)
					{
						if (eventBuffer[i].Event.KeyEvent.bKeyDown)
						{
							if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_LEFT && SnakeDirection != 2)
							{
								SnakeDirection = 1;
							}
							else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RIGHT && SnakeDirection != 1)
							{
								SnakeDirection = 2;

							}
							else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP & SnakeDirection != 4)
							{
								SnakeDirection = 3;

							}
							else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN && SnakeDirection != 3)
							{
								SnakeDirection = 4;

							}
						}
					}
					delete[] eventBuffer;
				}

			}
			



			// Drawing the Boundraies 
			DrawingBoundries(consoleBuffer, L'\x2501', L'\x2503', 15);

			// drawing the snake
			if (SnakeDirection == 1)
			{
				Snake.push_front(COORD{ --Snake.front().X, Snake.front().Y });
			}
			else if (SnakeDirection == 2) {
				Snake.push_front(COORD{ ++Snake.front().X, Snake.front().Y });
			}
			else if (SnakeDirection == 3)
			{
				Snake.push_front(COORD{ Snake.front().X, --Snake.front().Y });
			}
			else if (SnakeDirection == 4)
			{
				Snake.push_front(COORD{ Snake.front().X, ++Snake.front().Y });
			}
			Snake.pop_back();
			for (auto i = Snake.begin(); i != Snake.end(); i++)
			{

				consoleBuffer[i->X + WIDTH * i->Y].Char.UnicodeChar = L'\x004f';
			}
			consoleBuffer[Snake.front().X + WIDTH * Snake.front().Y].Char.UnicodeChar = L'@';

			// Printing Score
			wchar_t scoreText[20];
			swprintf_s(scoreText, 20, L"SCORE: %d", score);
			WriteText(consoleBuffer, scoreText, COORD{ 55,2 }, 10);
			WriteText(consoleBuffer, L"MADE BY : SHEHAB GAD", COORD{ 5,1 }, 240);
			// Inserting Food
			
			if (!FoodEaten)
			{
				FoodX = 5 + (rand() % 105);
				FoodY = 3 + (rand() % 38);
				FoodColor = 1 + (rand() % 15);
				while (consoleBuffer[FoodX + WIDTH * FoodY].Char.UnicodeChar != L' ')
				{
					FoodX = 5 + (rand() % 105);
					FoodY = 3 + (rand() % 38);
				}
				FoodEaten = true;
			}
			consoleBuffer[FoodX + WIDTH * FoodY].Char.UnicodeChar = L'\x20dd';
			consoleBuffer[FoodX + WIDTH * FoodY].Attributes = FoodColor;

			//Snake and food collision detection 
			if (Snake.front().X == FoodX && Snake.front().Y == FoodY)
			{
				FoodEaten = false;
				score++;
				consoleBuffer[FoodX + WIDTH * FoodY].Char.UnicodeChar = L' ';
				for(int i = 0; i < 3; i++) Snake.push_back(COORD{ Snake.back().X, Snake.back().Y });
			
				
			}
			//collision detetction of the snake on boundaries
			if (Snake.front().Y == 3 || Snake.front().Y == 41 || Snake.front().X == 4 || Snake.front().X == 110)
			{
				GameRunning = false;
				DrawingBoundries(consoleBuffer, L'\x2501', L'\x2503', 12);
				for (auto i = Snake.begin(); i != Snake.end(); i++)
				{
					consoleBuffer[i->X + WIDTH * i->Y].Char.UnicodeChar = '-';
					consoleBuffer[i->X + WIDTH * i->Y].Attributes = 12;

				}
				consoleBuffer[Snake.front().X + WIDTH * Snake.front().Y].Char.UnicodeChar = 'X';
				consoleBuffer[FoodX + WIDTH * FoodY].Char.UnicodeChar = L' ';
				WriteText(consoleBuffer, L"GAME OVER", COORD{ 50,20 }, 207);
				WriteText(consoleBuffer, L"PRESS ENTER TO PLAY AGAIN OR ESC TO EXIT", COORD{ 35,21 }, 207);
				WriteText(consoleBuffer, L"MADE BY : SHEHAB GAD", COORD{ 5,1 }, 207);


			}
			// collision detection of the snake on itself
			auto it = Snake.begin();
			it++;
			for (auto i = ++it; i != Snake.end(); i++)
			{
				if (Snake.front().X == i->X && Snake.front().Y == i->Y)
				{
					GameRunning = false;
					DrawingBoundries(consoleBuffer, L'\x2501', L'\x2503', 12);
					for (auto i = Snake.begin(); i != Snake.end(); i++)
					{
						consoleBuffer[i->X + WIDTH * i->Y].Char.UnicodeChar = '-';
						consoleBuffer[i->X + WIDTH * i->Y].Attributes = 12;

					}
					consoleBuffer[Snake.front().X + WIDTH * Snake.front().Y].Char.UnicodeChar = 'X';
					consoleBuffer[FoodX + WIDTH * FoodY].Char.UnicodeChar = L' ';
					WriteText(consoleBuffer, L"GAME OVER", COORD{ 50,20 }, 207);
					WriteText(consoleBuffer, L"PRESS ENTER TO PLAY AGAIN OR ESC FOR EXIT", COORD{ 35,21 }, 207);
					WriteText(consoleBuffer, L"MADE BY : SHEHAB GAD", COORD{ 5,1 }, 207);


				}
			}

			// writing to the console
			WriteConsoleOutputW(wHND, consoleBuffer, bufferSize, pos, &writingArea);
		}
		bool Answer = false;
		while (!Answer) {

		DWORD numberOfEventsHappend = 0;
		GetNumberOfConsoleInputEvents(rHND, &numberOfEventsHappend);
		DWORD numberOfEventsRead = 0;
		if (numberOfEventsHappend != 0)
		{
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numberOfEventsHappend];
			ReadConsoleInput(rHND, eventBuffer, numberOfEventsHappend, &numberOfEventsRead);
			for (int i = 0; i < numberOfEventsRead; i++)
			{
				if (eventBuffer[i].EventType == KEY_EVENT)
				{
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
					{
						Playing = false;
						Answer = true;
					}
					else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
					{
						Answer = true;
						break;
					}
				}
			}
		}
		}








	
	}

	return 0;
}
