/******************************************************************************
* БИБЛИОТЕКА СОДЕРЖИТ:
*******************************************************************************
* * стандартные библиотеки
* * пространство имен std
* * глобальные переменные, необходимые для ввода/вывода информации
* * структуру корабля
* * перечисление использованных для окрашивания текстур
* * вспомогательные функции, которые можно использовать и в других программах
******************************************************************************/

#include <iostream> 
#include <windows.h>
#include <ctime>
#include <string.h>
#include <conio.h>
#include <stdio.h>

/******************************************************************************/

using namespace std;

/******************************************************************************/

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

/******************************************************************************/

enum textures {
	GENERAL = 0 * 16 + 15,
	FRAME = 2 * 16 + 15,
	BUTTON = 5 * 16 + 15,
	BLINK = 13 * 16 + 15,
	DATA_INPUT = 2 * 16 + 15,
	MENU = 4 * 16 + 15,
	DATA = 0 * 16 + 14,
	CALM_WATER = 1 * 16 + 9,
	HIT_WATER = 1 * 16 + 15,
	OK_SHIP = 10 * 16 + 2,
	DAMAGED_SHIP = 14 * 16 + 12,
	SELECTED_SHIP = 15 * 16 + 10,
	FROZEN_DECK = 11 * 16 + 15,
	DROWN_SHIP = 4 * 16 + 12,
};

/******************************************************************************/

struct Counter
{
	int one_deck;
	int two_decks;
	int three_decks;
	int four_decks;
	int five_decks;
	int eight_decks;
};

/******************************************************************************/

void SetCursor(short x, short y, int color);
void PaintButton(short x, short y, short width, short height, int color, string text);
void ScreenCleaner(char * name);
void ClearMemory(int ** pc, int ** user, char * user_name);
void ChangeValues(int ** player, int in, int out, int scenario);

/******************************************************************************/

void SetCursor(short x, short y, int color)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(hOut, c);
	SetConsoleTextAttribute(hOut, color);
}

void PaintButton(short x, short y, short width, short height, int color, string text)
{
	for (short j = y; j < y + height; ++j)
	{
		for (short i = x; i < x + width; ++i)
		{
			SetCursor(i, j, color);
			cout << " ";
		}
	}

	COORD t;
	t.X = x + (width - strlen(text.c_str())) / 2;
	t.Y = y + (height / 2);

	for (short i = 0; i < short(strlen(text.c_str())); ++i)
	{
		SetCursor(t.X + i, t.Y, color);
		cout << char(toupper(text[i]));
	}
}

void ScreenCleaner(char * name)
{
	system("cls");
	SetCursor(5, 0, GENERAL);
	cout << "Player: ";
	SetConsoleTextAttribute(hOut, DATA);
	cout << name << "\t";
	SetConsoleTextAttribute(hOut, GENERAL);
}

void ClearMemory(int ** pc, int ** user, char * user_name)
{
	for (int y = 0; y < 10; ++y)
	{
		delete[]user[y];
		delete[]pc[y];
	}
	delete[]user_name;
	delete[]user;
	delete[]pc;
}

void ChangeValues(int ** player, int in, int out, int scenario)
{
	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (player[x][y] == in)
			{
				player[x][y] = out;
			}
		}
	}
}