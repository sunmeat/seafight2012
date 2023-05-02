#include "MyLib.h"

//Ключевые функции//
int Menu(FILE* f);
int Game(int** pc, int** user, char* name, Counter& count_pc_ships, Counter& count_user_ships, Counter& decrement_pc_ships, Counter& decrement_user_ships, short scenario, short pace);

//Функции для автоматической расстановки кораблей//
int** SetFleet(int** player, int scenario, Counter& count_ships);
int** SetRussian(int** player);
int** SetAmerican(int** player);
int** SetDutch(int** player);
int** SetShip(int** player, int decks);
bool ChoosePlaceForShip(int** player, COORD head, COORD tail);
int** SetDutchShip(int** player, int decks, int iterations);
int** HidePcFleet(int** pc);

//Функции для ручной расстановки кораблей//
void MoveShip(int** user, short scenario);
bool IsSmthSelected(int** user);
COORD* SelectShip(COORD deck, int** player, int scenario);
void SetNewPosition(COORD head, COORD* new_position, int width, int height, int** user, int scenario);
void ChangeLocation(COORD* ship, COORD click, int** user, int scenario);
void MakeShift(int** user, COORD* ship, int scenario);
void RotateShip(int** user, COORD* ship, int scenario);
void ResetDutchShip(int** user);

//Функции, участвующие в игровом процессе//
int CheckFields(int** pc, int** user);
int** DrownShip(int** player, Counter& decrement_ships, int x, int y, int** pc_check, int scenario);
Counter DecrementShips(int** player, Counter& decrement_ships, COORD c1, COORD c2);
int** PaintItDrown(int** player, COORD c1, COORD c2, int** pc_check, int scenario);
int** PaintDrownDutch(int** player);
int** PCShots(int** user, int** pc, Counter& count_user_ships, Counter& decrement_user_ships, short scenario, short pace);
int** UserShots(int** pc, int** user, Counter& count_pc_ships, Counter& decrement_pc_ships, short scenario, Counter count_user_ships, Counter decrement_user_ships);
int Click(COORD c, int** player, int x_shift, int scenario);
void ShowCounter(Counter& ship_counter, Counter& decrement_ship, int x_shift);
void FinishGame(int game);

//Функции компьютерного интеллекта//
bool SearchHitCells(int** user);
int CountCellsToShoot(int** user);
COORD* SearchCellsToShoot(int** user, int s);
COORD SmartPcShot(int** user, int scenario);

//Функции для вывода информации на экран//
void ShowFields(int** user, int** pc, char* name, int scenario);
void ShowCell(int value, int x, int y, int scenario);
void ShowFrame(int x, int y);

//Функции по настройке программы//
FILE* SetDefaultSettings();
void ShowSettings(FILE* f);
void EditSettings(FILE* f);
void ImplementSettings(FILE* f, short& scenario, short& pace, char*& name);
char* SetName(char* name, int size);
int ChooseScenario();
int ChoosePace();
void EditUserName(FILE* f);
void EditScenario(FILE* f);
void EditPace(FILE* f);

//Функции для работы со статистикой//
FILE* NewFile();
void ShowStatistics(FILE* f);
void ChangeResults(int game, int scenario, FILE* f);
void ChangeResultByLine(int move, int line, FILE* f);

//Функции для сохранения игры//
int CheckSavings();
FILE* SaveGame(int** player, string path);
int** OpenField(string path);
FILE* SaveCounters(Counter count_user_ships, Counter decrement_user_ships, Counter count_pc_ships, Counter decrement_pc_ships);
void GetCounters(Counter& count_user_ships, Counter& decrement_user_ships, Counter& count_pc_ships, Counter& decrement_pc_ships, int scenario);

///////////////////////////////////////////////////////////////

void main()
{
	system("title Морской бой");
	system("mode con cols=90 lines=40");
	srand((unsigned int)time(0)); // без приведения типа возникает ошибка C4244, т.к. time - 64 бита, а srand предполагает только 32 бита

	CONSOLE_CURSOR_INFO cinfo = { 1, false };
	SetConsoleCursorInfo(hOut, &cinfo);

	char* user_name = new char[100];
	short scenario, pace;
	int game;
	int** user = new int* [10];
	int** pc = new int* [10];
	Counter count_user_ships;
	Counter count_pc_ships;
	Counter decrement_user_ships;
	Counter decrement_pc_ships;

	FILE* results = NewFile();
	fclose(results);

	FILE* settings = SetDefaultSettings();
	fclose(settings);

	ImplementSettings(settings, scenario, pace, user_name);

	int play = Menu(results);

	if (play == 1 || (play == 2 && CheckSavings() != 0))
	{
		user = SetFleet(user, scenario, count_user_ships);
		pc = SetFleet(pc, scenario, count_pc_ships);
		pc = HidePcFleet(pc);

		decrement_user_ships = count_user_ships;
		decrement_pc_ships = count_pc_ships;

		ScreenCleaner(user_name);
		ShowFields(user, pc, user_name, scenario);

		(scenario == 3) ? ResetDutchShip(user) : MoveShip(user, scenario);

		game = Game(pc, user, user_name, count_pc_ships, count_user_ships, decrement_pc_ships, decrement_user_ships, scenario, pace);
		ChangeResults(game, scenario, results);

		ClearMemory(user, pc, user_name);

		main();
		return;
	}
	else if (play == 2 && CheckSavings() == 0)
	{
		user = OpenField("user_field.txt");
		pc = OpenField("pc_field.txt");
		GetCounters(count_user_ships, decrement_user_ships, count_pc_ships, decrement_pc_ships, scenario);

		ScreenCleaner(user_name);
		ShowFields(user, pc, user_name, scenario);
		game = Game(pc, user, user_name, count_pc_ships, count_user_ships, decrement_pc_ships, decrement_user_ships, scenario, pace);
		ChangeResults(game, scenario, results);

		ClearMemory(user, pc, user_name);

		main();
		return;
	}
	else if (play == 3)
	{
		ShowStatistics(results);
		main();
		return;
	}
	else if (play == 4)
	{
		EditSettings(settings);
		main();
		return;
	}
	else if (play == 9)
		return;
}

///////////////////////////////////////////////////////////////

FILE* SetDefaultSettings()
{
	FILE* f;

	short err = fopen_s(&f, "Settings.txt", "rb");

	if (err != NULL)
	{
		fopen_s(&f, "Settings.txt", "wb");

		short scenario = 1, scw;
		short pace = 300, paw;
		short length = 20, real_length, lew;
		char* name = new char[length];
		short naw;

		scw = fwrite(&scenario, sizeof(short), 1, f);
		paw = fwrite(&pace, sizeof(short), 1, f);
		SetName(name, length);
		real_length = strlen(name);
		lew = fwrite(&real_length, sizeof(short), 1, f);
		naw = fwrite(name, sizeof(char), real_length + 1, f);

		delete[]name;
		fclose(f);
	}
	return f;
}

void EditUserName(FILE* f)
{
	short err = fopen_s(&f, "Settings.txt", "r+b");

	if (err == NULL)
	{
		SetConsoleTextAttribute(hOut, GENERAL);
		system("cls");

		short length = 20, real_length, lew, naw;
		char* name = new char[length];

		SetName(name, length);
		real_length = strlen(name);

		fseek(f, sizeof(short) * 2, SEEK_SET);
		lew = fwrite(&real_length, sizeof(short), 1, f);

		fseek(f, sizeof(short) * 3, SEEK_SET);
		naw = fwrite(name, sizeof(char), real_length + 1, f);

		delete[]name;
		fclose(f);
	}
	EditSettings(f);
	return;
}

void EditScenario(FILE* f)
{
	short err = fopen_s(&f, "Settings.txt", "r+b");

	if (err == NULL)
	{
		SetConsoleTextAttribute(hOut, GENERAL);
		system("cls");

		short scenario, scw;
		scenario = ChooseScenario();

		fseek(f, 0, SEEK_SET);
		scw = fwrite(&scenario, sizeof(short), 1, f);

		fclose(f);
	}
	EditSettings(f);
	return;
}

void EditPace(FILE* f)
{
	short err = fopen_s(&f, "Settings.txt", "r+b");

	if (err == NULL)
	{
		SetConsoleTextAttribute(hOut, GENERAL);
		system("cls");

		short pace, paw;
		pace = ChoosePace();

		fseek(f, sizeof(short), SEEK_SET);
		paw = fwrite(&pace, sizeof(short), 1, f);

		fclose(f);
	}
	EditSettings(f);
	return;
}

void ImplementSettings(FILE* f, short& scenario, short& pace, char*& name)
{
	short err = fopen_s(&f, "Settings.txt", "rb");

	if (err == NULL)
	{
		short scr, par, ler, nar;
		short length = 100, real_length;

		scr = fread(&scenario, sizeof(short), 1, f);
		par = fread(&pace, sizeof(short), 1, f);
		ler = fread(&real_length, sizeof(short), 1, f);
		nar = fread(name, sizeof(char), real_length + 1, f);

		fclose(f);
	}
}

void ShowSettings(FILE* f)
{
	short err = fopen_s(&f, "Settings.txt", "rb");

	if (err == NULL)
	{
		short scenario, scr;
		short pace, par;
		short length = 100, real_length, ler;
		char* name = new char[length];
		short nar;

		scr = fread(&scenario, sizeof(short), 1, f);
		par = fread(&pace, sizeof(short), 1, f);
		ler = fread(&real_length, sizeof(short), 1, f);
		nar = fread(name, sizeof(char), real_length + 1, f);

		SetCursor(35, 6, 7);
		cout << "Current settings: \n\n";

		PaintButton(30, 10, 14, 1, GENERAL, "       Player:");
		PaintButton(45, 10, strlen(name), 1, DATA, name);

		for (int j = 8; j <= 18; j += 2)
		{
			if (j == 8 || j == 12 || j == 18)
			{
				for (int i = 30; i < 70; ++i)
				{
					SetCursor(i, j, 7);
					cout << char(0xc4);
				}
			}
		}

		PaintButton(30, 14, 14, 1, GENERAL, "     Scenario:");
		switch (scenario)
		{
		case 1:
			PaintButton(45, 14, 12, 1, DATA, "Russian     ");
			break;
		case 2:
			PaintButton(45, 14, 12, 1, DATA, "American    ");
			break;
		case 3:
			PaintButton(45, 14, 12, 1, DATA, "Flying Dutch");
			break;
		}

		PaintButton(30, 16, 14, 1, GENERAL, "         Pace:");
		switch (pace)
		{
		case 500:
			PaintButton(45, 16, 8, 1, DATA, "Slow    ");
			break;
		case 300:
			PaintButton(45, 16, 8, 1, DATA, "Moderate");
			break;
		case 100:
			PaintButton(45, 16, 8, 1, DATA, "Fast    ");
			break;
		}

		for (int j = 10; j <= 16; j += 2)
		{
			if (j != 12)
				PaintButton(60, j, 6, 1, DATA_INPUT, "Edit");
		}
		delete[]name;
		fclose(f);
	}
}

void EditSettings(FILE* f)
{
	short err = fopen_s(&f, "Settings.txt", "rb");
	fclose(f);

	if (err == NULL)
	{
		ShowSettings(f);

		PaintButton(38, 23, 14, 3, BUTTON, "OK");

		INPUT_RECORD ir; //структура для считывания события консоли
		COORD c;
		DWORD d;//сколько реально считано событий

		SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

		while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
		{
			if (ir.EventType == MOUSE_EVENT)
			{
				c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
				if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					if (c.X >= 60 && c.X <= 65 && c.Y == 10)
					{
						//смена имени
						EditUserName(f);
						return;
					}
					else if (c.X >= 60 && c.X <= 65 && c.Y == 14)
					{
						//выбор сценария
						EditScenario(f);
						return;
					}
					else if (c.X >= 60 && c.X <= 65 && c.Y == 16)
					{
						//выбор скорости
						EditPace(f);
						return;
					}
					else if (c.X >= 38 && c.X <= 51 && c.Y >= 23 && c.Y <= 25)
					{
						//выход из настроек
						SetConsoleTextAttribute(hOut, GENERAL);
						system("cls");
						return;
					}
				}
			}
		}
	}
}

void ChangeResults(int game, int scenario, FILE* f)
{
	int line = 9; //символов в строке
	int shift_result = (scenario - 1) * line * 3; //получаем смещение на 0, 3 или 6 строк - запись победы
	int shift_total = shift_result + (line * 2); // запись общего результата (смещение на 2, 5 или 8 строк)

	if (game == -1)
		shift_result += line; // запись поражения (смещение на 1, 4 или 7 строк)

	ChangeResultByLine(shift_result, line, f);
	ChangeResultByLine(shift_total, line, f);
}

void ChangeResultByLine(int move, int line, FILE* f)
{
	int err = fopen_s(&f, "Statistics.txt", "r+");

	if (err == 0)
	{
		int temp;
		char* data_read = new char[line];
		char* data_write = new char[line];

		fseek(f, move, SEEK_SET);
		fgets(data_read, line, f);

		temp = atoi(data_read);
		++temp;

		_itoa_s(temp, data_write, line, 10); //10 - значит в десятичной системе
		strcat_s(data_write, strlen(data_write) + 2, "\n");

		fseek(f, move + (line - 1 - strlen(data_write)), SEEK_SET);
		fputs(data_write, f);

		delete[]data_read;
		delete[]data_write;
		fclose(f);
	}
}

FILE* NewFile()
{
	FILE* f;

	int err = fopen_s(&f, "Statistics.txt", "rt");
	if (err != 0)
	{
		fopen_s(&f, "Statistics.txt", "wt");
		for (int i = 0; i < 9; ++i)
			fputs("0000000\n", f);
		fclose(f);
	}
	return f;
}

void ShowStatistics(FILE* f)
{
	int err = fopen_s(&f, "Statistics.txt", "rt");

	system("cls");

	if (err == 0)
	{
		for (int j = 11; j <= 23; j += 2)
		{
			if (j == 11 || j == 15 || j == 23)
			{
				for (int i = 20; i < 69; ++i)
				{
					SetCursor(i, j, 7);
					cout << char(0xc4);
				}
			}
		}

		SetCursor(25, 9, 7);
		cout << "Statistics:";
		SetCursor(20, 13, GENERAL);
		cout << "  SCENARIO          WINS      LOSSES    TOTALLY";
		SetCursor(20, 17, GENERAL);
		cout << "  Russian";
		SetCursor(20, 19, GENERAL);
		cout << "  American";
		SetCursor(20, 21, GENERAL);
		cout << "  Flying Dutch";

		char** stat = new char* [9];

		for (int i = 0; i < 9; ++i)
		{
			stat[i] = new char[9];
			fgets(stat[i], 9, f);
			SetCursor(41 + (i % 3) * 10, 17 + (i / 3) * 2, DATA);
			cout << atoi(stat[i]);
		}
		cout << "\n\n\n";

		for (int i = 0; i < 9; ++i)
			delete[]stat[i];
		delete[]stat;
		fclose(f);

		PaintButton(38, 27, 14, 3, BUTTON, "OK");

		INPUT_RECORD ir; //структура для считывания события консоли
		COORD c;
		DWORD d;//сколько реально считано событий

		SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

		while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
		{
			if (ir.EventType == MOUSE_EVENT)
			{
				c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
				if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
					c.X >= 38 && c.X <= 51 && c.Y >= 27 && c.Y <= 29)
				{
					fclose(f);
					SetConsoleTextAttribute(hOut, GENERAL);
					system("cls");
					return;
				}
			}
		}
	}
}

char* SetName(char* name, int size)
{
	SetCursor(30, 15, GENERAL);
	cout << "Input your name and press ENTER:";
	PaintButton(35, 17, 22, 3, DATA_INPUT, "");

	char symbol;
	int i = 0;
	strcpy_s(name, 1, "\0");

	do
	{
		SetCursor(36 + i, 18, DATA_INPUT);
		symbol = _getch();
		if (int(strlen(name)) < size - 1 && symbol != 13)
		{
			name[i] = symbol;
			name[i + 1] = '\0';
			cout << name[i];
		}
		if (symbol == 8 && i > 0)
		{
			--i;
			SetCursor(36 + i, 18, DATA_INPUT);
			cout << " ";
			name[i] = '\0';
		}
		else if (i < size - 1)
			++i;
	} while (symbol != 13);

	if (strlen(name) == 0)
		strcpy_s(name, 10, "Anonymous");

	SetConsoleTextAttribute(hOut, GENERAL);
	system("cls");
	return name;
}

int ChoosePace()
{
	SetCursor(34, 10, GENERAL);
	cout << "Choose optional pace:";

	PaintButton(37, 12, 14, 3, BUTTON, "Slow");
	PaintButton(37, 16, 14, 3, BUTTON, "Moderate");
	PaintButton(37, 20, 14, 3, BUTTON, "Fast");

	int delay = 0;
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD c;
	DWORD d;//сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				c.X >= 37 && c.X <= 50)
			{
				if (c.Y >= 12 && c.Y <= 14)
					delay = 500;
				else if (c.Y >= 16 && c.Y <= 18)
					delay = 300;
				else if (c.Y >= 20 && c.Y <= 22)
					delay = 100;
				if (delay > 0)
					break;
			}
		}
	}

	SetConsoleTextAttribute(hOut, GENERAL);
	system("cls");
	return delay;
}

int ChooseScenario()
{
	SetCursor(34, 10, GENERAL);
	cout << "Choose game scenario:";

	PaintButton(37, 12, 14, 3, BUTTON, "Russian");
	PaintButton(37, 16, 14, 3, BUTTON, "American");
	PaintButton(37, 20, 14, 3, BUTTON, "Flying Dutch");

	int choice = 0;
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD c;
	DWORD d;//сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				c.X >= 37 && c.X <= 50)
			{
				if (c.Y >= 12 && c.Y <= 14)
					choice = 1;
				else if (c.Y >= 16 && c.Y <= 18)
					choice = 2;
				else if (c.Y >= 20 && c.Y <= 22)
					choice = 3;
				if (choice > 0)
					break;
			}
		}
	}

	SetConsoleTextAttribute(hOut, GENERAL);
	system("cls");
	return choice;
}

int Menu(FILE* f)
{
	PaintButton(37, 10, 14, 1, GENERAL, "Menu");
	PaintButton(37, 12, 14, 3, MENU, "New game");
	PaintButton(37, 16, 14, 3, MENU, "Continue");
	PaintButton(37, 20, 14, 3, MENU, "Statistics");
	PaintButton(37, 24, 14, 3, MENU, "Settings");
	PaintButton(37, 28, 14, 3, MENU, "Exit");

	int choice = 0;
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD c;
	DWORD d;//сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				c.X >= 37 && c.X <= 50)
			{
				if (c.Y >= 12 && c.Y <= 14)
					choice = 1;//new game
				else if (c.Y >= 16 && c.Y <= 18)
					choice = 2;//continue
				else if (c.Y >= 20 && c.Y <= 22)
				{
					int err = fopen_s(&f, "Statistics.txt", "r+");
					if (err == 0)
					{
						choice = 3;//statistics
						fclose(f);
					}
				}
				else if (c.Y >= 24 && c.Y <= 26)
					choice = 4;//settings
				else if (c.Y >= 28 && c.Y <= 30)
					choice = 9;//exit
				if (choice > 0)
					break;
			}
		}
	}

	SetConsoleTextAttribute(hOut, GENERAL);
	system("cls");
	return choice;
}

void ShowFields(int** user, int** pc, char* name, int scenario)
{
	SetCursor(20, 2, 12);
	cout << "PC's fleet";
	SetCursor((50 + (30 - strlen(name) - 9) / 2), 2, 12);
	cout << name << "\'s fleet";

	ShowFrame(10, 5);
	ShowFrame(50, 5);
	for (int y = 5; y < 25; y += 2)
	{
		for (int x = 10; x < 40; x += 3)
		{
			ShowCell(pc[(x - 10) / 3][(y - 5) / 2], x, y, scenario);
		}
		for (int x = 50; x < 80; x += 3)
		{
			ShowCell(user[(x - 50) / 3][(y - 5) / 2], x, y, scenario);
		}
		SetConsoleTextAttribute(hOut, GENERAL);
		cout << "\n";
	}
	cout << "\n\n";
}

void ShowFrame(int x, int y)
{
	COORD c = { x, y };

	int lines[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char columns[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };

	for (int j = y - 1, jj = -2; j < y + 21; ++j, ++jj)
	{
		for (int i = x - 2; i < x + 32; ++i)
		{
			SetCursor(i, j, FRAME);
			cout << " ";
		}
		if (jj >= 0 && jj < 20 && jj % 2 == 0)
		{
			SetCursor(x - 2, j, FRAME);
			cout << lines[jj / 2];
		}
	}
	for (int ii = 0; ii < 10; ++ii)
	{
		SetCursor(x + 1 + (ii * 3), y - 1, FRAME);
		cout << columns[ii];
	}
	cout << "\n";
}

void ShowCell(int value, int x, int y, int scenario)
{
	COORD cc = { x, y };
	unsigned char board; // без unsigned возникает ошибка C4309: '=' : truncation of constant value

	for (int i = x; i < x + 3; ++i)
	{
		for (int j = y; j < y + 2; ++j)
		{
			if (i == x && j == y)
				board = 0xda;
			else if (i == x + 2 && j == y)
				board = 0xbf;
			else if (i == x && j == y + 1)
				board = 0xc0;
			else if (i == x + 2 && j == y + 1)
				board = 0xd9;
			else if (i == x + 1)
				board = 0xc4;

			if (value <= 0) //0 - моя пустая ячейка, меньше 0 - еще не открытая ячейка противника
			{
				SetCursor(i, j, CALM_WATER);
				cout << char(0x7e);
			}
			else if (value == 1 || value == 2)
			{
				SetCursor(i, j, HIT_WATER);
				cout << char(0xfd);
			}
			else if (value == 3)
			{
				SetCursor(i, j, DROWN_SHIP);
				cout << char(0xfd);
			}
			else if (value == 4 && scenario != 3)
			{
				SetCursor(i, j, DAMAGED_SHIP);
				cout << board;
			}
			else if (value == 4 && scenario == 3)
			{
				SetCursor(i, j, FROZEN_DECK);
				cout << board;
			}
			else if (value == 5)
			{
				SetCursor(i, j, OK_SHIP);
				cout << board;
			}
			else if (value == 15)
			{
				SetCursor(i, j, SELECTED_SHIP);
				cout << board;
			}
		}
	}
}

int** SetFleet(int** player, int scenario, Counter& count_ships)
{
	//создание двумерного массива и заполнение его нулями
	player = new int* [10];
	for (int y = 0; y < 10; ++y)
	{
		player[y] = new int[10];
	}
	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			player[x][y] = 0;
		}
	}

	//расстановка кораблей в соответствии со сценарием
	switch (scenario)
	{
	case 1:
		SetRussian(player);
		count_ships = { 4, 3, 2, 1, -1, -1 };
		break;
	case 2:
		SetAmerican(player);
		count_ships = { -1, 1, 2, 1, 1, -1 };
		break;
	case 3:
		SetDutch(player);
		count_ships = { -1, -1, -1, -1, -1, 8 }; // отнимать будем попалубно, а не по кораблям
		break;
	}

	return player;
}

int** HidePcFleet(int** pc)
{
	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (pc[x][y] == 5 || pc[x][y] == 0)
				pc[x][y] -= 10;
		}
	}

	return pc;
}

int** SetShip(int** player, int decks)
{
	int direction;
	COORD s1, s2;

	direction = rand() % 2;
	bool good_place = true;

	if (direction == 0)//vertical
	{
		s1.X = rand() % 10;
		s2.X = s1.X;
		s1.Y = rand() % (10 - decks + 1);
		s2.Y = s1.Y + decks - 1;
	}
	else if (direction == 1)//horizontal
	{
		s1.X = rand() % (10 - decks + 1);
		s2.X = s1.X + decks - 1;
		s1.Y = rand() % 10;
		s2.Y = s1.Y;
	}

	good_place = ChoosePlaceForShip(player, s1, s2);

	if (good_place)
	{
		for (int y = s1.Y; y <= s2.Y; ++y)
		{
			for (int x = s1.X; x <= s2.X; ++x)
			{
				player[x][y] = 5;
			}
		}
	}
	else SetShip(player, decks);
	return player;
}

bool ChoosePlaceForShip(int** player, COORD head, COORD tail)
{
	bool is_place_free = true;

	for (int y = head.Y - 1; y <= tail.Y + 1; ++y)
	{
		for (int x = head.X - 1; x <= tail.X + 1; ++x)
		{
			if (x >= 0 && x < 10 && y >= 0 && y < 10 && player[x][y] != 0 && player[x][y] != 15)
			{
				is_place_free = false;
				break;
			}
			continue;
		}
	}
	return is_place_free;
}

int** SetDutchShip(int** player, int decks, int iterations)
{
	ChangeValues(player, -10, 0, 3);
	ChangeValues(player, -5, 20, 3);
	ChangeValues(player, 5, 20, 3);

	int direction;
	COORD* ship = new COORD[decks];
	COORD move[4] = { { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };

	do
	{
		ship[0] = { rand() % 10, rand() % 10 };
	} while (player[ship[0].X][ship[0].Y] != 0 && player[ship[0].X][ship[0].Y] != 20);
	player[ship[0].X][ship[0].Y] = 5;

	if (decks == 1)
	{
		ChangeValues(player, 20, 0, 3);
		delete[]ship;
		return player;
	}

	for (int i = 1; i < decks; ++i)
	{
		if ((ship[i - 1].Y == 9 || player[ship[i - 1].X][ship[i - 1].Y + 1] != 0) &&
			(ship[i - 1].Y == 0 || player[ship[i - 1].X][ship[i - 1].Y - 1] != 0) &&
			(ship[i - 1].X == 0 || player[ship[i - 1].X - 1][ship[i - 1].Y] != 0) &&
			(ship[i - 1].X == 9 || player[ship[i - 1].X + 1][ship[i - 1].Y] != 0) &&
			(ship[i - 1].Y == 9 || player[ship[i - 1].X][ship[i - 1].Y + 1] != 20) &&
			(ship[i - 1].Y == 0 || player[ship[i - 1].X][ship[i - 1].Y - 1] != 20) &&
			(ship[i - 1].X == 0 || player[ship[i - 1].X - 1][ship[i - 1].Y] != 20) &&
			(ship[i - 1].X == 9 || player[ship[i - 1].X + 1][ship[i - 1].Y] != 20))
		{
			++iterations;
			if (iterations < 100)
			{
				SetDutchShip(player, decks, iterations);
				delete[]ship;
				return player;
			}
			else
			{
				ChangeValues(player, 20, 5, 3);
				delete[]ship;
				return player;
			}
		}

		int x = -1, y = -1;
		do {
			direction = rand() % 4;
			x = ship[i - 1].X + move[direction].X;
			y = ship[i - 1].Y + move[direction].Y;

		} while (x < 0 || x >= 10 || y < 0 || y >= 10 || (player[x][y] != 0 && player[x][y] != 20));
		player[x][y] = 5;
		ship[i] = { (SHORT)x, (SHORT)y };
	}
	ChangeValues(player, 20, 0, 3);
	delete[]ship;

	return player;
}

int** SetRussian(int** player)
{
	const int size = 10;
	int russian[size] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };

	for (int i = 0; i < size; ++i)
	{
		SetShip(player, russian[i]);
	}
	return player;
}

int** SetAmerican(int** player)
{
	const int size = 5;
	int american[size] = { 5, 4, 3, 3, 2 };

	for (int i = 0; i < size; ++i)
	{
		SetShip(player, american[i]);
	}
	return player;
}

int** SetDutch(int** player)
{
	const int size = 1;
	int dutch[size] = { 8 };

	for (int i = 0; i < size; ++i)
	{
		SetDutchShip(player, dutch[i], 0);
	}
	return player;
}

int Game(int** pc, int** user, char* name, Counter& count_pc_ships, Counter& count_user_ships, Counter& decrement_pc_ships, Counter& decrement_user_ships, short scenario, short pace)
{
	int game = 0; //0 означает продолжение игры

	ShowCounter(count_pc_ships, decrement_pc_ships, 18);
	ShowCounter(count_user_ships, decrement_user_ships, 58);
	PaintButton(30, 36, 14, 3, BUTTON, "Save");
	PaintButton(46, 36, 14, 3, BUTTON, "Stop");

	while (game == 0) // пока игра продолжается, игроки стреляют по очереди
	{
		UserShots(pc, user, count_pc_ships, decrement_pc_ships, scenario, count_user_ships, decrement_user_ships);//сначала стреляет пользователь
		game = CheckFields(pc, user);
		if (game != 0)
			break;
		PCShots(user, pc, count_user_ships, decrement_user_ships, scenario, pace); //потом стреляет компьютер
		game = CheckFields(pc, user);
	}
	FinishGame(game); // игра заканчивается, если функция возвращает не ноль
	return game;
}

int CheckFields(int** pc, int** user)
{
	int game;
	int user_health = 0, pc_health = 0;

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (user[x][y] == 5)
				++user_health;
			if (pc[x][y] == -5)
				++pc_health;
		}
	}

	if (user_health > 0 && pc_health > 0)
		game = 0;//igra prodolzhaetsya
	else if (user_health > 0 && pc_health == 0)
		game = 1;//pobeda
	else if (user_health == 0 && pc_health > 0)
		game = -1;//porazhenie

	return game;
}

int** DrownShip(int** player, Counter& decrement_ships, int x, int y, int** pc_check, int scenario)
{
	bool is_all_drown;
	int healthy_cells = 0;
	int i, j;
	COORD c1, c2;


	i = x, j = y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 &&
		(player[i][j] == 4 || player[i][j] == 5 || player[i][j] == -5))
	{

		if (player[i][j] == 5 || player[i][j] == -5)
			++healthy_cells;
		c2.X = i;
		++i;
	}

	i = x, j = y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 &&
		(player[i][j] == 4 || player[i][j] == 5 || player[i][j] == -5))
	{

		if (player[i][j] == 5 || player[i][j] == -5)
			++healthy_cells;
		c1.X = i;
		--i;
	}

	i = x, j = y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 &&
		(player[i][j] == 4 || player[i][j] == 5 || player[i][j] == -5))
	{

		if (player[i][j] == 5 || player[i][j] == -5)
			++healthy_cells;
		c2.Y = j;
		++j;
	}

	i = x, j = y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 &&
		(player[i][j] == 4 || player[i][j] == 5 || player[i][j] == -5))
	{

		if (player[i][j] == 5 || player[i][j] == -5)
			++healthy_cells;
		c1.Y = j;
		--j;
	}


	if (healthy_cells == 0)
		is_all_drown = true;
	else if (healthy_cells > 0)
		is_all_drown = false;

	if (is_all_drown)
	{
		if (scenario != 3)
			PaintItDrown(player, c1, c2, pc_check, scenario);
		DecrementShips(player, decrement_ships, c1, c2);
	}
	return player;
}

int** PaintItDrown(int** player, COORD c1, COORD c2, int** pc_check, int scenario)
{
	bool is_pc_shooting;
	int pc_cells = 0;
	for (int j = 0; j < 10; ++j)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (player[i][j] == pc_check[i][j])
				++pc_cells;
		}
	}
	if (pc_cells == 100)
		is_pc_shooting = true; // hodit komp, pole igroka
	else
		is_pc_shooting = false; // hodit igrok, pole kompa


	for (int y = c1.Y - 1; y <= c2.Y + 1; ++y)
	{
		for (int x = c1.X - 1; x <= c2.X + 1; ++x)
		{
			if (x >= 0 && x < 10 && y >= 0 && y < 10)
			{
				if (player[x][y] == 4)
					player[x][y] = 3;
				else if (player[x][y] == 0 || player[x][y] == 1 || player[x][y] == -10)
					player[x][y] = 2;

				int i = x * 3;
				if (is_pc_shooting)
					i += 10;
				else i += 50;
				int j = y * 2 + 5;
				ShowCell(player[x][y], i, j, scenario);
			}
		}
	}
	return player;
}

int** PaintDrownDutch(int** player)
{
	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (player[x][y] == 4)
			{
				player[x][y] = 3;
				for (int j = y - 1; j <= y + 1; ++j)
				{
					for (int i = x - 1; i <= x + 1; ++i)
					{
						if (j < 0 || j >= 10 || i < 0 || i >= 10)
							continue;
						if (player[i][j] != 3 && player[i][j] != 4)
							player[i][j] = 2;
					}
				}
			}
		}
	}
	return player;
}

Counter DecrementShips(int** player, Counter& decrement_ships, COORD c1, COORD c2)
{
	int ship_size;
	ship_size = max(c2.X - c1.X + 1, c2.Y - c1.Y + 1);

	if (ship_size == 8)
		--decrement_ships.eight_decks;
	else if (ship_size == 5)
		--decrement_ships.five_decks;
	else if (ship_size == 4)
		--decrement_ships.four_decks;
	else if (ship_size == 3)
		--decrement_ships.three_decks;
	else if (ship_size == 2)
		--decrement_ships.two_decks;
	else if (ship_size == 1)
		--decrement_ships.one_deck;

	return decrement_ships;
}

bool SearchHitCells(int** user)
{
	bool is_smth_damaged = false;

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (user[x][y] == 4)
			{
				is_smth_damaged = true;
				break;
			}
		}
	}
	return is_smth_damaged;
}

int CountCellsToShoot(int** user)
{
	int size = 0;

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (user[x][y] == 4)
			{
				for (int j = y - 1; j <= y + 1; ++j)
				{
					for (int i = x - 1; i <= x + 1; ++i)
					{
						if (i >= 0 && i < 10 && j >= 0 && j < 10 &&
							((i == x && (x - 1 < 0 || user[x - 1][y] != 4) && (x + 1 > 9 || user[x + 1][y] != 4)) ||
								(j == y && (y - 1 < 0 || user[x][y - 1] != 4) && (y + 1 > 9 || user[x][y + 1] != 4))) &&
							(user[i][j] == 0 || user[i][j] == 5))
						{
							++size;
						}
					}
				}
			}
		}
	}
	return size;
}

COORD* SearchCellsToShoot(int** user, int s)
{
	COORD* cells = new COORD[s];
	int k = 0;

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (user[x][y] == 4)
			{
				for (int j = y - 1; j <= y + 1; ++j)
				{
					for (int i = x - 1; i <= x + 1; ++i)
					{
						if (i >= 0 && i < 10 && j >= 0 && j < 10 &&
							((i == x && (x - 1 < 0 || user[x - 1][y] != 4) && (x + 1 > 9 || user[x + 1][y] != 4)) ||
								(j == y && (y - 1 < 0 || user[x][y - 1] != 4) && (y + 1 > 9 || user[x][y + 1] != 4))) &&
							(user[i][j] == 0 || user[i][j] == 5))
						{
							cells[k] = { (SHORT)i, (SHORT)j };
							++k;
						}
					}
				}
			}
		}
	}
	return cells;
}

COORD SmartPcShot(int** user, int scenario)
{
	COORD click;

	if (scenario == 3 || !SearchHitCells(user))									//если поврежденных ячеек нет
	{
		do {														//выбираются случайные координаты
			click.X = rand() % 10;
			click.Y = rand() % 10;
		} while (user[click.X][click.Y] != 0 &&
			user[click.X][click.Y] != 5);	//по которым еще не было выстрела
	}
	else										//если есть поврежденные ячейки
	{
		int size = CountCellsToShoot(user);		//считается к-во ячеек, потенциально пригодных для выстрела
		COORD* cells = new COORD[size];		//формируется массив соответствующего размера
		cells = SearchCellsToShoot(user, size);	//и заполняется соответствующими координатами
		click = cells[rand() % size];			//выбираются случайные координаты по индексу сформированного массива
		delete[]cells;
	}
	return click;
}

int** PCShots(int** user, int** pc, Counter& count_user_ships, Counter& decrement_user_ships, short scenario, short pace)
{
	int game;
	game = CheckFields(pc, user);
	if (game != 0)
		return user;
	Sleep(pace);

	COORD click = SmartPcShot(user, scenario);

	user[click.X][click.Y] = Click(click, user, 50, scenario);

	if (user[click.X][click.Y] == 4 && scenario != 3)
		DrownShip(user, decrement_user_ships, click.X, click.Y, pc, scenario);
	else if (user[click.X][click.Y] == 4 && scenario == 3)
		--decrement_user_ships.eight_decks;
	ShowCounter(count_user_ships, decrement_user_ships, 58);

	if ((user[click.X][click.Y] == 4 ||
		user[click.X][click.Y] == 3)) //если попал 
	{
		if (scenario == 3)
		{
			if (decrement_user_ships.eight_decks > 0)
			{
				SetDutchShip(user, decrement_user_ships.eight_decks, 0);
			}
			else if (decrement_user_ships.eight_decks == 0)
			{
				PaintDrownDutch(user);
			}
			for (int y = 5; y < 25; y += 2)
			{
				for (int x = 50; x < 80; x += 3)
				{
					ShowCell(user[(x - 50) / 3][(y - 5) / 2], x, y, scenario);
				}
			}
		}
		PCShots(user, pc, count_user_ships, decrement_user_ships, scenario, pace); //то стреляет заново
	}
	return user; //иначе его серия ходов заканчивается
}

int** UserShots(int** pc, int** user, Counter& count_pc_ships, Counter& decrement_pc_ships, short scenario, Counter count_user_ships, Counter decrement_user_ships)
{
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD click;	 //для записи координат поля
	COORD el;		 //для перевода координат в индексы элемента массива
	DWORD d;		 //сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			int game = CheckFields(pc, user);
			if (game != 0)
				return pc;

			click = ir.Event.MouseEvent.dwMousePosition; //координаты мышки
			el = { (SHORT)((click.X - 10) / 3), (SHORT)((click.Y - 5) / 2) };

			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				click.X >= 30 && click.X <= 43 && click.Y >= 36 && click.Y <= 38)
			{
				SaveGame(user, "user_field.txt");
				SaveGame(pc, "pc_field.txt");
				SaveCounters(count_user_ships, decrement_user_ships, count_pc_ships, decrement_pc_ships);
				PaintButton(30, 36, 14, 3, BLINK, "Save");
				Sleep(50);
				PaintButton(30, 36, 14, 3, BUTTON, "Save");
			}
			else if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				click.X >= 46 && click.X <= 59 && click.Y >= 36 && click.Y <= 38)
			{
				SetConsoleTextAttribute(hOut, GENERAL);
				system("cls");
				main();
				return pc;
			}
			else if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				click.X >= 10 && click.X < 40 && click.Y >= 5 && click.Y < 25 &&
				pc[el.X][el.Y] < 0)
				//ход будет выполнен, если нажата левая кнопка мыши, 
				//координаты соответствуют полю противника
				//ранее не было клика по данной ячейке
			{
				pc[el.X][el.Y] = Click(el, pc, 10, scenario);

				if (pc[el.X][el.Y] == 4 && scenario != 3)
					DrownShip(pc, decrement_pc_ships, el.X, el.Y, pc, scenario);
				else if (pc[el.X][el.Y] == 4 && scenario == 3)
					--decrement_pc_ships.eight_decks;
				ShowCounter(count_pc_ships, decrement_pc_ships, 18);

				if (pc[el.X][el.Y] == 3 || pc[el.X][el.Y] == 4) // если утопил(3) или задел(4)
				{
					if (scenario == 3)
					{
						if (decrement_pc_ships.eight_decks > 0)
						{
							SetDutchShip(pc, decrement_pc_ships.eight_decks, 0);
							HidePcFleet(pc);
						}
						else if (decrement_pc_ships.eight_decks == 0)
						{
							PaintDrownDutch(pc);
						}
						for (int y = 5; y < 25; y += 2)
						{
							for (int x = 10; x < 40; x += 3)
							{
								ShowCell(pc[(x - 10) / 3][(y - 5) / 2], x, y, scenario);
							}
						}
					}
					UserShots(pc, user, count_pc_ships, decrement_pc_ships, scenario, count_user_ships, decrement_user_ships); // следующий ход
					break;
				}
				return pc; //иначе серия ходов заканчивается
			}
		}
	}
	return pc; // для устранения ошибки C4715: not all control paths return a value
}

int Click(COORD c, int** player, int x_shift, int scenario)
{
	if (player[c.X][c.Y] == 5 || player[c.X][c.Y] == -5)
		player[c.X][c.Y] = 4;
	else if (player[c.X][c.Y] == 0 || player[c.X][c.Y] == -10)
		player[c.X][c.Y] = 1;

	COORD print = { c.X * 3 + x_shift, c.Y * 2 + 5 };

	ShowCell(player[c.X][c.Y], print.X, print.Y, scenario);
	return player[c.X][c.Y];
}

void ShowCounter(Counter& ship_counter, Counter& decrement_ship, int x_shift)
{
	int line = 27;
	unsigned char ch = 0xfe; // без unsigned возникает ошибка C4309: '=' : truncation of constant value
	if (ship_counter.eight_decks >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << ch << ch << ch << ch << ch << ch << ch << ch << "  ";
		//cout << ship_counter.eight_decks << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.eight_decks << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.eight_decks - decrement_ship.eight_decks;
	}
	if (ship_counter.five_decks >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << ch << ch << ch << ch << ch << "  ";
		//cout << ship_counter.five_decks << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.five_decks << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.five_decks - decrement_ship.five_decks;
	}
	if (ship_counter.four_decks >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << " " << ch << ch << ch << ch << "  ";
		//cout << ship_counter.four_decks << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.four_decks << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.four_decks - decrement_ship.four_decks;
	}
	if (ship_counter.three_decks >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << "  " << ch << ch << ch << "  ";
		//cout << ship_counter.three_decks << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.three_decks << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.three_decks - decrement_ship.three_decks;
	}
	if (ship_counter.two_decks >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << "   " << ch << ch << "  ";
		//cout << ship_counter.two_decks << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.two_decks << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.two_decks - decrement_ship.two_decks;
	}
	if (ship_counter.one_deck >= 0)
	{
		++line;
		SetCursor(x_shift, line, GENERAL);
		cout << "    " << ch << "  ";
		//cout << ship_counter.one_deck << ":  ";
		SetConsoleTextAttribute(hOut, 10);
		cout << char(0xfb) << decrement_ship.one_deck << "  ";
		SetConsoleTextAttribute(hOut, 12);
		cout << char(0xfd) << ship_counter.one_deck - decrement_ship.one_deck;
	}
}

void FinishGame(int game)
{
	if (game == 1)
	{
		SetCursor(32, 34, 10);
		cout << "Congratulations! You won!";
	}
	else if (game == -1)
	{
		SetCursor(33, 34, 12);
		cout << "We're sorry, you lost!";
	}

	PaintButton(30, 36, 30, 3, GENERAL, "");
	PaintButton(38, 36, 14, 3, BUTTON, "OK");

	INPUT_RECORD ir; //структура для считывания события консоли
	COORD c;
	DWORD d;//сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			c = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED &&
				c.X >= 38 && c.X <= 51 && c.Y >= 36 && c.Y <= 38)
			{
				SetConsoleTextAttribute(hOut, GENERAL);
				system("cls");
				return;
				//здесь будет появляться меню, и можно будет сыграть заново
			}
		}
	}
}

int CheckSavings()
{
	FILE* f;
	int err;
	int errors = 3;

	err = fopen_s(&f, "user_field.txt", "rb+");
	if (err == NULL)
	{
		--errors;
		fclose(f);
	}
	err = fopen_s(&f, "pc_field.txt", "rb+");
	if (err == NULL)
	{
		--errors;
		fclose(f);
	}
	err = fopen_s(&f, "saved_counters.txt", "rb+");
	if (err == NULL)
	{
		--errors;
		fclose(f);
	}
	return errors;
}

FILE* SaveGame(int** player, string path)
{
	FILE* f;
	int temp[10][10];

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			temp[x][y] = player[x][y];
		}
	}

	int err = fopen_s(&f, path.c_str(), "wb+");
	if (err == NULL)
	{
		int write = fwrite(temp, sizeof(int), 10 * 10, f);
		fclose(f);
	}

	return f;
}

FILE* SaveCounters(Counter count_user_ships, Counter decrement_user_ships, Counter count_pc_ships, Counter decrement_pc_ships)
{
	FILE* f;

	int err = fopen_s(&f, "saved_counters.txt", "wb+");
	if (err == NULL)
	{
		int w1 = fwrite(&count_user_ships, sizeof(Counter), 1, f);
		int w2 = fwrite(&decrement_user_ships, sizeof(Counter), 1, f);
		int w3 = fwrite(&count_pc_ships, sizeof(Counter), 1, f);
		int w4 = fwrite(&decrement_pc_ships, sizeof(Counter), 1, f);

		fclose(f);
	}
	return f;
}

int** OpenField(string path)
{
	FILE* f;
	int temp[10][10] = { 0 };

	int** player = new int* [10];
	for (int y = 0; y < 10; ++y)
	{
		player[y] = new int[10];
	}

	int err = fopen_s(&f, path.c_str(), "rb+");
	if (err == NULL)
	{
		int read = fread(temp, sizeof(int), 10 * 10, f);
		fclose(f);
	}

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			player[x][y] = temp[x][y];
		}
	}

	return player;
}

void GetCounters(Counter& count_user_ships, Counter& decrement_user_ships, Counter& count_pc_ships, Counter& decrement_pc_ships, int scenario)
{
	FILE* f;

	int err = fopen_s(&f, "saved_counters.txt", "rb+");
	if (err == NULL)
	{
		int r1 = fread(&count_user_ships, sizeof(Counter), 1, f);
		int r2 = fread(&decrement_user_ships, sizeof(Counter), 1, f);
		int r3 = fread(&count_pc_ships, sizeof(Counter), 1, f);
		int r4 = fread(&decrement_pc_ships, sizeof(Counter), 1, f);

		fclose(f);
	}
}

COORD* SelectShip(COORD deck, int** player, int scenario)
{
	int i, j;
	COORD* c = new COORD[2];
	c[0] = deck;
	c[1] = deck;

	i = deck.X, j = deck.Y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 && (player[i][j] == 5 || player[i][j] == 15))
	{
		player[i][j] = 15;
		c[1].X = i;
		++i;
	}

	i = deck.X, j = deck.Y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 && (player[i][j] == 5 || player[i][j] == 15))
	{
		player[i][j] = 15;
		c[0].X = i;
		--i;
	}

	i = deck.X, j = deck.Y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 && (player[i][j] == 5 || player[i][j] == 15))
	{
		player[i][j] = 15;
		c[1].Y = j;
		++j;
	}

	i = deck.X, j = deck.Y;
	while (i >= 0 && i < 10 && j >= 0 && j < 10 && (player[i][j] == 5 || player[i][j] == 15))
	{
		player[i][j] = 15;
		c[0].Y = j;
		--j;
	}

	for (int y = c[0].Y; y <= c[1].Y; ++y)
	{
		for (int x = c[0].X; x <= c[1].X; ++x)
		{
			ShowCell(15, x * 3 + 50, y * 2 + 5, scenario);
		}
	}

	return c;
}

void SetNewPosition(COORD head, COORD* new_position, int width, int height, int** user, int scenario)
{
	new_position[0].X = head.X;
	new_position[0].Y = head.Y;
	new_position[1].X = head.X + width;
	new_position[1].Y = head.Y + height;

	if (new_position[1].Y >= 10)
	{
		new_position[1].Y = 9;
		new_position[0].Y = 9 - height;
	}
	if (new_position[1].X >= 10)
	{
		new_position[1].X = 9;
		new_position[0].X = 9 - width;
	}

	if (ChoosePlaceForShip(user, new_position[0], new_position[1]))
	{
		for (int y = new_position[0].Y; y <= new_position[1].Y; ++y)
		{
			for (int x = new_position[0].X; x <= new_position[1].X; ++x)
			{
				user[x][y] = 5;
			}
		}
		ChangeValues(user, 15, 0, scenario);
	}
	else
		ChangeValues(user, 15, 5, scenario);

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			ShowCell(user[x][y], x * 3 + 50, y * 2 + 5, scenario);
		}
	}
}

void ChangeLocation(COORD* ship, COORD click, int** user, int scenario)
{
	COORD* new_position = new COORD[2];
	int width = ship[1].X - ship[0].X;
	int height = ship[1].Y - ship[0].Y;

	SetNewPosition(click, new_position, width, height, user, scenario);
	delete[]new_position;
}

void RotateShip(int** user, COORD* ship, int scenario)
{
	COORD* new_position = new COORD[2];
	int height = ship[1].X - ship[0].X;
	int width = ship[1].Y - ship[0].Y;

	SetNewPosition(ship[0], new_position, width, height, user, scenario);
	delete[]new_position;
}

bool IsSmthSelected(int** user)
{
	bool is_selected = false;

	for (int y = 0; y < 10; ++y)
	{
		for (int x = 0; x < 10; ++x)
		{
			if (user[x][y] == 15)
			{
				is_selected = true;
				break;
			}
		}
	}

	return is_selected;
}

void MakeShift(int** user, COORD* ship, int scenario)
{
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD click;	 //для записи координат поля
	COORD el;		 //для перевода координат в индексы элемента массива
	DWORD d;		 //сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			click = ir.Event.MouseEvent.dwMousePosition; //координаты мышки
			el = { (click.X - 50) / 3, (click.Y - 5) / 2 };

			if (IsSmthSelected(user) && ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if (click.X >= 50 && click.X < 80 && click.Y >= 5 && click.Y < 25)
				{
					if (user[el.X][el.Y] == 0 || user[el.X][el.Y] == 15)
					{
						ChangeLocation(ship, el, user, scenario);
						return;
					}
					else if (user[el.X][el.Y] == 5)
					{
						ChangeValues(user, 15, 5, scenario);
						for (int y = 0; y < 10; ++y)
						{
							for (int x = 0; x < 10; ++x)
							{
								ShowCell(user[x][y], x * 3 + 50, y * 2 + 5, scenario);
							}
						}
						ship = SelectShip(el, user, scenario);
						MakeShift(user, ship, scenario);
						return;
					}
				}

			}
			else if (ir.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED &&
				user[el.X][el.Y] == 15)
			{
				ship = SelectShip(el, user, scenario);
				RotateShip(user, ship, scenario);
				return;
			}
		}
	}
	return;
}

void MoveShip(int** user, short scenario)
{
	SetCursor(48, 28, GENERAL);
	cout << "Use your mouse to change ship locations: ";
	SetCursor(50, 30, GENERAL);
	cout << "LEFT click - select and move";
	SetCursor(50, 31, GENERAL);
	cout << "RIGHT click - rotate";
	SetCursor(50, 33, GENERAL);
	cout << "When you're ready, click START!";
	PaintButton(58, 35, 14, 3, BUTTON, "Start!");

	INPUT_RECORD ir; //структура для считывания события консоли
	COORD click;	 //для записи координат поля
	COORD el;		 //для перевода координат в индексы элемента массива
	DWORD d;		 //сколько реально считано событий

	COORD* ship = new COORD[2];

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			click = ir.Event.MouseEvent.dwMousePosition; //координаты мышки
			el = { (click.X - 50) / 3, (click.Y - 5) / 2 };

			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if (click.X >= 58 && click.X <= 71 && click.Y >= 35 && click.Y < 37)
				{
					ChangeValues(user, 15, 5, scenario);
					for (int y = 0; y < 10; ++y)
					{
						for (int x = 0; x < 10; ++x)
						{
							ShowCell(user[x][y], x * 3 + 50, y * 2 + 5, scenario);
						}
					}
					PaintButton(48, 28, 40, 10, GENERAL, "");
					return;
				}
				else if (click.X >= 50 && click.X < 80 && click.Y >= 5 && click.Y < 25 &&
					user[el.X][el.Y] == 5)
				{
					ChangeValues(user, 15, 5, scenario);
					for (int y = 0; y < 10; ++y)
					{
						for (int x = 0; x < 10; ++x)
						{
							ShowCell(user[x][y], x * 3 + 50, y * 2 + 5, scenario);
						}
					}
					ship = SelectShip(el, user, scenario);
					MakeShift(user, ship, scenario);
				}
			}
		}
	}
}

void ResetDutchShip(int** user)
{
	SetCursor(50, 27, GENERAL);
	cout << "Click RESET to change location";
	SetCursor(50, 28, GENERAL);
	cout << "When you're ready, click START!";

	PaintButton(50, 30, 14, 3, BUTTON, "Reset ship");
	PaintButton(66, 30, 14, 3, BUTTON, "Start!");

	INPUT_RECORD ir; //структура для считывания события консоли
	COORD click;	 //для записи координат поля
	DWORD d;		 //сколько реально считано событий

	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	while (ReadConsoleInput(hIn, &ir, 1, &d))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			click = ir.Event.MouseEvent.dwMousePosition; //координаты мышки

			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if (click.X >= 66 && click.X <= 79 && click.Y >= 30 && click.Y < 32)
				{
					PaintButton(50, 27, 40, 10, GENERAL, "");
					return;
				}
				else if (click.X >= 50 && click.X < 63 && click.Y >= 30 && click.Y < 32)
				{
					SetDutchShip(user, 8, 0);
					for (int y = 0; y < 10; ++y)
					{
						for (int x = 0; x < 10; ++x)
						{
							ShowCell(user[x][y], x * 3 + 50, y * 2 + 5, 3);
						}
					}
				}
			}
		}
	}
}