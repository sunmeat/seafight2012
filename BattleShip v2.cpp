#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <cstdio>
using namespace std;

HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
HWND hwnd = GetConsoleWindow(); // дескриптор окна, используемый консолью
HDC hdc = GetDC(hwnd); // данные типа HDC представляют собой 32-разрядное целое беззнаковое число.

const int field_size = 10;
const int ramka_size = (field_size * 3) + 2;
char *ramka;
const char letters[11] = "АБВГДЕЖЗИК";
char *name;
int screen_cols;
int screen_lines;
COORD field_start;
COORD * gamer_hits;
COORD * comp_hits;

struct Point
{
	COORD p;
	bool status;
}**ar_ships_gamer, **ar_ships_comp; 


struct ForGame
{
	int size;
	COORD null_coo;
	bool horiz;
	COORD first;
	COORD last;
	int direct;
	int count_of_good_hit;
}for_comp, for_gamer;

struct Color
{
	int R,G,B;
}color;


void Load(Point **& ar_ships_gamer, Point **& ar_ships_comp, COORD *& comp_hits, COORD *& gamer_hits, bool &hod, int &gamer_kills, int &comp_kills);
void Save(Point ** ar_ships_gamer, Point ** ar_ships_comp, COORD * comp_hits, COORD * gamer_hits, bool &hod, int &gamer_kills, int &comp_kills);
void DeleteArrays(Point **& ar_ships_gamer, Point ** &ar_ships_comp, COORD *& comp_hits, COORD * &gamer_hits, bool take_name);

bool CheckPointInAr(Point **ar, COORD c, bool from_hit, int &ar_y, int &ar_x, int from_el = 0);

bool GoodShip(Point **ar, COORD &c, bool &horizontal, int &size, int &el);
void SetShip(Point **ar, int el, int size);
void AutomaticShips(Point **ar);

void WriteToReg(COORD * reg, COORD c);

void GiveCoord(COORD * comp_hits, COORD &comp_hit);
void HitOrNo(Point ** ar, COORD hit, int &hit_result, int &ar_y, int &ar_x);
void ShowCompHits(COORD *comp_hits);

bool CheckDirect(COORD *comp_hits);
void Chance0(COORD * comp_hits); // рандомит направление
void Chance1(COORD *comp_hits); //продолжает в том же направлении
void Chance2(COORD *comp_hits); // меняет направление на противоположное
void Chance3(COORD *comp_hits); // меняет направление на 90 градусов

void ShowPoint(COORD d, int color, bool show_hits = 0);
void ShowHits(COORD *ar_hits, Point **ar, COORD c, bool gamer_call);
bool BoatInFire(Point **ar, int boat, int size);
void Show(Point **ar, COORD c, bool gamer_call);
void SetNullPoints(COORD *ar, ForGame &fg);
void ShowRamka(COORD c, int color);
void PrintMenuElement(COORD c, Color color, int what);

void Init();
void Game(Point ** ar_ships_gamer, Point ** ar_ships_comp, COORD * comp_hits, COORD * gamer_hits, bool first_move, int gamer_kills = 0, int comp_kills = 0);
void Emblema(COORD c, int size, int width, Color color, int what);

void ShowRamki();
void Menu(bool main, bool first_move, int gamer_kills, int comp_kills);




void Save(Point ** ar_ships_gamer, Point ** ar_ships_comp, COORD * comp_hits, COORD * gamer_hits, bool &hod, int &gamer_kills, int &comp_kills)
{
	FILE *f;
	fopen_s(&f, "Save", "wb");
	
	//Записываем корабли игрока
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		
		fwrite(ar_ships_gamer[y], sizeof(Point), temp, f);
	}
	
	
	//Записываем корабли компа
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		
		fwrite(ar_ships_comp[y], sizeof(Point), temp, f);
	}
	fwrite(gamer_hits, sizeof(COORD), 100, f);
	fwrite(comp_hits, sizeof(COORD), 100, f);
	fwrite(&hod, sizeof(bool), 1, f);
	fwrite(&gamer_kills, sizeof(int), 1, f);
	fwrite(&comp_kills, sizeof(int), 1, f);
	
	int name_size = strlen(name);
	fwrite(&name_size, sizeof(int), 1, f);
	fwrite(name, sizeof(char), name_size, f);
	fclose(f);
}

void Load(Point **& ar_ships_gamer, Point **& ar_ships_comp, COORD *& comp_hits, COORD *& gamer_hits, bool &hod, int &gamer_kills, int &comp_kills, bool first)
{
	FILE *f;
	fopen_s(&f, "Save", "r");

	if(!first) DeleteArrays(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, 1);
	ar_ships_gamer = new Point*[10];
	ar_ships_comp = new Point * [10];
	comp_hits = new COORD [100];
	gamer_hits = new COORD [100];
	name = new char [30];
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		ar_ships_gamer[y] = new Point[temp];
		ar_ships_comp[y] = new Point[temp];
	}
	if(first)
	{
		ramka = new char [ramka_size];
		for(int x = 0; x < ramka_size; x++)
		{
			ramka[x] = (char) 178;
		}
	}

	//Считываем корабли игрока
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		
		fread(ar_ships_gamer[y], sizeof(Point), temp, f);
	}
	//Считыааем корабли компа
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		
		fread(ar_ships_comp[y], sizeof(Point), temp, f);
	}
	fread(gamer_hits, sizeof(COORD), 100, f);
	fread(comp_hits, sizeof(COORD), 100, f);
	fread(&hod, sizeof(bool), 1, f);
	fread(&gamer_kills, sizeof(int), 1, f);
	fread(&comp_kills, sizeof(int), 1, f);
	int name_size = 0;
	fread(&name_size, sizeof(int), 1, f);
	fread(name, sizeof(char), name_size, f);
	*(name + name_size) = '\0';
	fclose(f);
	
	ShowRamki();
	Game(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, hod, gamer_kills, comp_kills);	
}

void DeleteArrays(Point **& ar_ships_gamer, Point ** &ar_ships_comp, COORD *& comp_hits, COORD * &gamer_hits, bool take_name)
{
	for(int y = 0; y < 10; y++)
	{
		delete[]ar_ships_gamer[y];
		delete[]ar_ships_comp[y];
	}
	delete[]comp_hits;
	delete[]gamer_hits;
	
	ar_ships_gamer = ar_ships_comp =  nullptr;
	comp_hits = gamer_hits = nullptr;
	
	if(take_name)
	{
		delete[]name;
		name = nullptr;
	}
}
bool CheckPointInAr(Point **ar, COORD c, bool from_hit, int &ar_y, int &ar_x, int from_el)
{
	for(int i = from_el; i < 10; i++)
	{
		int temp;
		if(i < 4) temp = 1;
		else if(i > 3 && i < 7) temp = 2;
		else if(i > 6 && i < 9) temp = 3;
		else temp = 4;

		for(int j = 0; j < temp; j++)
		{
			if(ar[i][j].p.X == c.X && ar[i][j].p.Y == c.Y) 
			{
				if(from_hit)
				{
					ar_y = i;
					ar_x = j;
				}
				return true; 
			}
		}
	}
	return false;

}
bool GoodShip(Point **ar, COORD &c, bool &horizontal, int &size, int &el)
{
	int for_size_x;
	int for_size_y;
	if(horizontal) 
	{
		for_size_y = c.Y + 1;
		for_size_x = c.X + size;
	}
	else 
	{
		for_size_y = c.Y + size;
		for_size_x = c.X + 1;
	}

	for(int y = c.Y - 1; y <= for_size_y; y++)
	{
		for(int x = c.X - 1; x <= for_size_x; x++)
		{
			if(y >= 0 && y < field_size && x >= 0 && x < field_size)
			{
				COORD temp;
				temp.X = x;
				temp.Y = y;
				if(CheckPointInAr(ar, temp, 0, x, y, el)) return false; 
			}
		}
	}
	return true;
}


void SetShip(Point **ar, int el, int size)
{
	COORD u;
	bool horizontal = rand()%2;
	do
	{
		do
		{
			u.X = rand() % field_size;
		}while(horizontal && u.X > field_size - size);

		do
		{
			u.Y = rand() % field_size;
		}while(!horizontal && u.Y > field_size - size);
	}while(!GoodShip(ar, u, horizontal, size, el));

	
	for(int i = 0; i < size; i++)
	{
		ar[el][i].p.X = u.X;
		ar[el][i].p.Y = u.Y;
		if(horizontal) u.X++;
		else u.Y++;
	}
}

void AutomaticShips(Point **ar)
{
	for(int y = 9; y >= 0; y--)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		SetShip(ar, y, temp);
	}
}


void WriteToReg(COORD * reg, COORD c)
{
	int t = c.Y * field_size + c.X;
	reg[t].Y = -5;
}

//void GiveCoord(COORD * comp_hits, COORD &comp_hit)
//{
//	int t;
//	do
//	{
//		srand(time(0));
//		t = rand()%100;
//	}while(comp_hits[t].Y < 0);
//
//	comp_hit.X = t % field_size;
//	comp_hit.Y = t / field_size;
//}

void GiveCoord(COORD * comp_hits, COORD &comp_hit)
{
	int t;

	t = rand()%100;
	if(comp_hits[t].Y < 0)
	{
		for(int i = t+1; i < 100; i++)
		{
			if(i == 99 && comp_hits[i].Y < 0)
			{
				for(int j = 0; j < t; j++)
				{
					if(comp_hits[i].Y < 0) continue;
					else
					{
						t = j;
						break;
					}
				}
			}
			else if(comp_hits[i].Y < 0) continue;
			else
			{
				t = i;
				break;
			}
		}
	}
	comp_hit.X = t % field_size;
	comp_hit.Y = t / field_size;
}

void HitOrNo(Point ** ar, COORD hit, int &hit_result, int &ar_y, int &ar_x)
{	
	bool b_hit = CheckPointInAr(ar, hit, 1, ar_y, ar_x);
	if(b_hit)
	{
		ar[ar_y][ar_x].status = false;
		int temp;
		if(ar_y < 4) temp = 1;
		else if(ar_y > 3 && ar_y < 7) temp = 2;
		else if(ar_y > 6 && ar_y < 9) temp = 3;
		else temp = 4;
		while(temp--)
		{
			if(ar[ar_y][temp].status) 
			{
				hit_result = 2; //ранил
				return;
			}
		}
		hit_result = 1; // убил
		return;
	}
	hit_result = 0;//не попал
}


bool CheckDirect(COORD *comp_hits)
{
	switch(for_comp.direct)
	{
	case 0:
		{
			COORD temp = for_comp.last;
			temp.Y--;
			if(for_comp.last.Y - 1 < 0 || comp_hits[temp.Y * field_size + temp.X].Y < 0) 
			{
				return false;
			}
			for_comp.last.Y--; //up
		}break;
	case 1:
		{
			COORD temp = for_comp.last;
			temp.X--;
			if(for_comp.last.X - 1 < 0 || comp_hits[temp.Y * field_size + temp.X].Y < 0) 
			{
				return false;
			}
			for_comp.last.X--; //left
		}break;
	case 2:
		{
			COORD temp = for_comp.last;
			temp.Y++;
			if(for_comp.last.Y + 1 == field_size || comp_hits[temp.Y * field_size + temp.X].Y < 0) 
			{
				return false;
			}
			for_comp.last.Y++; // down
		}break;
	case 3:
		{
			COORD temp = for_comp.last;
			temp.X++;
			if(for_comp.last.X + 1 == field_size || comp_hits[temp.Y * field_size + temp.X].Y < 0) 
			{
				return false;
			}
			for_comp.last.X++; // right
		}break;
	}
	return true;
}


void Chance0(COORD * comp_hits) // рандомит направление
{
	do
	{
		srand(time(0));
		for_comp.direct = rand()%4;
		for_comp.last = for_comp.first;
		
	}while(!CheckDirect(comp_hits));
}

void Chance1(COORD *comp_hits) //продолжает в том же направлении
{
	if(CheckDirect(comp_hits)) return;
	else Chance2(comp_hits);
}

void Chance2(COORD *comp_hits) // меняет направление на противоположное
{
	for_comp.last = for_comp.first;
	if(for_comp.direct >= 2) for_comp.direct %= 2;
	else for_comp.direct += 2;
	if(CheckDirect(comp_hits)) return;
	else Chance0(comp_hits);
}

void Chance3(COORD *comp_hits) // меняет направление на 90 градусов
{
	for_comp.last = for_comp.first;
	ForGame temp = for_comp;
	bool b = rand() % 2;
	switch(for_comp.direct)
	{
	case 0: // up
		{
			if(b) for_comp.direct = 1; // left
			else for_comp.direct = 3; // right
		}break;
	case 1: // left
		{
			if(b) for_comp.direct = 2; // down
			else for_comp.direct = 0; // up
		}break;
	case 2:
		{
			if(b) for_comp.direct = 3;
			else for_comp.direct = 1;
		}break;
	case 3:
		{
			if(b) for_comp.direct = 0;
			else for_comp.direct = 2;
		}break;
	}
	if(CheckDirect(comp_hits)) return;
	else
	{
		for_comp = temp;
		Chance2(comp_hits);
	}
}

void ShowPoint(COORD d, int color, bool show_hits)
{
	if(show_hits)
	{
		int temp;
		SetConsoleTextAttribute(hOut, 11*16+0);
		int rows = 2;
		while(rows--)
		{
			SetConsoleCursorPosition(hOut, d);
			temp = 3;
			while(temp--)
			{
				cout << '*';
			}
			d.Y++;
		}
		return;
	}
	SetConsoleTextAttribute(hOut, 11*16+color);
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xC9;
	d.Y++;
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xC8;
	d.X++;
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xCD;
	d.Y--;
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xCD;
	d.X++;
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xBB;
	d.Y++;
	SetConsoleCursorPosition(hOut, d);
	cout << (char)0xBC;

}

void ShowHits(COORD *ar_hits, Point **ar, COORD c, bool gamer_call)
{
	int temp;
	COORD copy = c;
	for(int i = 0; i < 100; i++)
	{
		if(ar_hits[i].Y < 0)
		{
			COORD t;
			t.X = i % field_size;
			t.Y = i / field_size;
			if(!CheckPointInAr(ar, t, 0, temp, temp))
			{
				c.X += (i % field_size) * 3;
				c.Y += (i / field_size) * 2;
				ShowPoint(c, 0, 1);
			}
			c = copy;
	
		}
	}
}

bool BoatInFire(Point **ar, int boat, int size)
{
	for(int j = 0; j < size; j++)
	{
		if(ar[boat][j].status) return false;
	}
	return true;
}



void Show(Point **ar, COORD c, bool gamer_call)
{
	COORD copy = c;
	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;

		for(int j = 0; j < temp; j++)
		{
			c.X += ar[y][j].p.X * 3;
			c.Y += ar[y][j].p.Y * 2;
			if(ar[y][j].status)
			{
				if(!gamer_call)
				ShowPoint(c, 6);
			}
			else
			{
				if(BoatInFire(ar, y, temp))
				{					
					ShowPoint(c, 4);
				}
				else 
				{
					ShowPoint(c, 12);
				}
			}
			c = copy;
		}
	}
}

void SetNullPoints(COORD *ar, ForGame &fg)
{
	int for_size_x;
	int for_size_y;
	if(fg.horiz) 
	{
		for_size_y = fg.null_coo.Y + 1;
		for_size_x = fg.null_coo.X + fg.size;
	}
	else 
	{
		for_size_y = fg.null_coo.Y + fg.size;
		for_size_x = fg.null_coo.X + 1;
	}

	for(int y = fg.null_coo.Y - 1; y <= for_size_y; y++)
	{
		for(int x = fg.null_coo.X - 1; x <= for_size_x; x++)
		{
			if(y >= 0 && y < field_size && x >= 0 && x < field_size)
			{
				COORD temp;
				temp.X = x;
				temp.Y = y;
				if(ar[y * field_size + x].Y >= 0)
				{
					WriteToReg(ar, temp); 
				}
			}
		}
	}
}

void ShowRamka(COORD c, int color)
{
	COORD copy_c = c;
	int temp = 2;
	//рисуем буквы на поле
	c.X += 2;
	c.Y--;
	setlocale(LC_ALL, "RUS");
	SetConsoleTextAttribute(hOut, 11*16+0);
	for(int i = 0; i < 10; i++)
	{
		SetConsoleCursorPosition(hOut, c);
		cout << letters[i];
		c.X += 3;
	}
	c = copy_c;
	//рисуем цифры на поле
	c.Y++;
	c.X--;
	SetConsoleTextAttribute(hOut, 11*16+0);
	for(int i = 0; i < 10; i++)
	{
		if(i == 9) c.X--;
		SetConsoleCursorPosition(hOut, c);
		cout << i+1;
		c.Y += 2;
	}
	setlocale(LC_ALL, "C");
	c = copy_c;

	SetConsoleTextAttribute(hOut, 11*16+color);
	while(temp--)
	{
		SetConsoleCursorPosition(hOut, c);
		for(int x = 0; x < ramka_size; x++)
		{
			cout << ramka[x];
		}
		c.Y += ramka_size - 10 - 1;
	}
	c = copy_c;
	temp = 2;
	c.Y++;
	while(temp--)
	{
		for(int x = 0; x < ramka_size - 10 - 2; x++)
		{
			SetConsoleCursorPosition(hOut, c);
			cout << ramka[x];
			c.Y++;
		}
		c.Y -= ramka_size - 10 - 2;
		c.X += ramka_size - 1;
	}

}


void Mouse(COORD &c, int &job, bool menu, bool main, bool first_move, int gamer_kills, int comp_kills)
{
	INPUT_RECORD ir; //структура для считывания события консоли
	COORD mouse;
	DWORD x;//сколько реально считано событий
	SetConsoleMode(hIn, ENABLE_MOUSE_INPUT);
	bool h = false;
	while (ReadConsoleInput(hIn, &ir, 1, &x))//считываем событие
	{
		if (ir.EventType == MOUSE_EVENT)
		{
			mouse = ir.Event.MouseEvent.dwMousePosition;//координаты мышки
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if(!menu)
				{
					if (mouse.X >= field_start.X + field_size*4 && mouse.X <= field_start.X + field_size*7 && mouse.Y >= field_start.Y && mouse.Y <= field_start.Y + field_size*2)
					{
						mouse.X -= field_start.X + field_size*4;
						mouse.X /= 3;
						mouse.Y -= field_start.Y;
						mouse.Y /= 2;
						int t = (mouse.Y * field_size) + mouse.X;
						if(gamer_hits[t].Y < 0) continue;
						c = mouse;
						return;
					}
					else if(mouse.X >= 0 && mouse.X <= 9 && mouse.Y >= 0 && mouse.Y <= 3)
					{
						Menu(0, first_move, gamer_kills, comp_kills);
					}
					else if(mouse.X >= screen_cols - 9 && mouse.X <= screen_cols && mouse.Y >= 0 && mouse.Y <= 3)
					{
						exit(0);
					}
				}
				else
				{
					if(main)
					{
						if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 - 2 && mouse.Y <= screen_lines / 2 - 2 + 3)
						{
							job = 7;
							return;
						}
						else if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 + 3 && mouse.Y <= screen_lines / 2 + 3 + 3)
						{
							job = 5;
							return;
						}
						else if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 + 6 && mouse.Y <= screen_lines / 2 + 6 + 3);
						{
							job = 6;
							return;
						}
					}	
					else if(!main)
					{
						if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 - 2 && mouse.Y <= screen_lines / 2 - 2 + 3)
						{
							job = 3;
							return;
						}
						else if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 + 3 && mouse.Y <= screen_lines / 2 + 3 + 3)
						{
							job = 4;
							return;
						}
						else if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 + 8 && mouse.Y <= screen_lines / 2 + 8 + 3)
						{
							job = 5;
							return;
						}
						else if(mouse.X >= screen_cols / 2 - 8 && mouse.X <= screen_cols / 2 + 8 && mouse.Y >= screen_lines / 2 + 13 && mouse.Y <= screen_lines / 2 + 13 + 3)
						{
							job = 6;
							return;
						}
					}
				}
			}
		}
	}
}

void TheWinner(bool win)
{
	COORD write = {0,0};
	HBRUSH menu_brush = CreateSolidBrush(RGB(0, 255, 255));
	HPEN back_pen = CreatePen(PS_SOLID,1,RGB(0,255,255));
	SelectObject(hdc,back_pen);
	SelectObject(hdc, menu_brush);
	Rectangle(hdc, 8*0, 12*0, 8*screen_cols, 12*screen_lines);
	DeleteObject(back_pen);
	DeleteObject(menu_brush);
	
	write.X = field_start.X - 9;
	write.Y = 2;
	color.R = 0;
	color.G = 0;
	color.B = 128;
	
	Emblema(write, 95, 45, color, 0);
	COORD c = {screen_cols/2 - 13,(screen_lines - field_start.Y)/2+5};
	if(win)
	{
		color.R = 34;
		color.G = 109;
		color.B = 34;
		Emblema(c, 45, 20, color, 8);
	}
	else
	{
		color.R = 255;
		color.G = 64;
		color.B = 64;
		Emblema(c, 45, 20, color, 9);
	}
	c.X = screen_cols/2 - 8;
	c.Y = screen_lines/2 + 8;
	PrintMenuElement(c, color, 10);
	c.Y+=5;
	PrintMenuElement(c, color, 6);
	int job;
	Mouse(c, job, 1, 0, 0, 0, 0);
	
	if(job == 5)
	{	
		DeleteArrays(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, 0);	
		Init();
	}
	else if(job == 6)
	{
		DeleteArrays(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, 1);
		exit(0);
	}
}


void Game(Point ** ar_ships_gamer, Point ** ar_ships_comp, COORD * comp_hits, COORD * gamer_hits, bool first_move, int gamer_kills,int comp_kills)
{																												  
	int hit_result_comp = 0;
	int hit_result_gamer = 0;
	int chance = 0;
	int ar_y;
	int ar_x;
	COORD log;
	bool first_open = true;
	while(gamer_kills != 10 && comp_kills != 10)
	{
		COORD t = field_start;
		if(first_open)
		{
			Show(ar_ships_gamer, t, 0);
			ShowHits(comp_hits, ar_ships_gamer, t, 0);
			t.X += field_size*4;
			Show(ar_ships_comp, t, 1);
			ShowHits(gamer_hits, ar_ships_comp, t, 1);
			first_open = false;
		}
		t = field_start;
		if(first_move)
		{
			COORD gamer_hit;
			//пишет что мой ход 
			log.X = screen_cols/2 - 5;
			log.Y = field_start.Y - 4;
			SetConsoleCursorPosition(hOut, log);
			SetConsoleTextAttribute(hOut, 11*16+2);
			setlocale(LC_ALL, "RUS");
			cout << "Your move " << name << "\t";

			//пишет куда попал под полем
			log.Y = screen_lines - 2;
			log.X = t.X + field_size*4;
			SetConsoleCursorPosition(hOut, log);
			cout << name << ": ";
			
			Mouse(gamer_hit, ar_x, 0, 0, first_move, gamer_kills, comp_kills);
			HitOrNo(ar_ships_comp, gamer_hit, hit_result_gamer, ar_y, ar_x);
			WriteToReg(gamer_hits, gamer_hit);

			if(hit_result_gamer == 0) 
			{									
				SetConsoleTextAttribute(hOut, 11*16+0);
				cout << "Do not hit the " << letters[gamer_hit.X] << gamer_hit.Y + 1 << "\t";
				first_move = false;
				setlocale(LC_ALL, "C");
			}
			else if(hit_result_gamer == 1 || hit_result_gamer == 2)
			{
				for_gamer.null_coo = ar_ships_comp[ar_y][0].p;
				if(ar_ships_comp[ar_y][0].p.Y == ar_ships_comp[ar_y][1].p.Y) for_gamer.horiz = true;
				else for_gamer.horiz = false;
				if(ar_y < 4) for_gamer.size = 1;
				else if(ar_y > 3 && ar_y < 7) for_gamer.size = 2;
				else if(ar_y > 6 && ar_y < 9) for_gamer.size = 3;
				else for_gamer.size = 4;

				if(hit_result_gamer == 2)
				{
					SetConsoleTextAttribute(hOut, 11*16 + 12);
					cout << "Hit the point " << letters[gamer_hit.X] << gamer_hit.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
				}
				else
				{
					gamer_kills++;
					SetConsoleTextAttribute(hOut, 11*16 + 4);
					cout << "Kill in point " << letters[gamer_hit.X] << gamer_hit.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
					SetNullPoints(gamer_hits, for_gamer);
				}
				first_move = true;
			}
			t.X += field_size*4;
			Show(ar_ships_comp, t, 1);
			ShowHits(gamer_hits, ar_ships_comp, t, 1);
		}
		else 
		{
			SetConsoleTextAttribute(hOut, 11*16+12);
			log.X = screen_cols/2 - 5;
			log.Y = field_start.Y - 4;
			SetConsoleCursorPosition(hOut, log);
			setlocale(LC_ALL, "RUS");
			cout << "Computer goes\t\t";
			
			log.X = field_start.X;
			log.Y = screen_lines - 2;
			SetConsoleCursorPosition(hOut, log);
			cout << "Computer: ";
			if(!hit_result_comp || hit_result_comp == 1)
			{
				COORD comp_hit;
				GiveCoord(comp_hits, comp_hit);
				WriteToReg(comp_hits, comp_hit);
				HitOrNo(ar_ships_gamer, comp_hit, hit_result_comp, ar_y, ar_x);
				if(hit_result_comp > 0) 
				{
						for_comp.first = ar_ships_gamer[ar_y][ar_x].p;
						for_comp.null_coo = ar_ships_gamer[ar_y][0].p;
						if(ar_ships_gamer[ar_y][0].p.Y == ar_ships_gamer[ar_y][1].p.Y) for_comp.horiz = true;
						else for_comp.horiz = false;
						if(ar_y < 4) for_comp.size = 1;
						else if(ar_y > 3 && ar_y < 7) for_comp.size = 2;
						else if(ar_y > 6 && ar_y < 9) for_comp.size = 3;
						else for_comp.size = 4;
						if(hit_result_comp == 1) 
						{
							SetNullPoints(comp_hits, for_comp);
							comp_kills++;
							SetConsoleTextAttribute(hOut, 11*16 + 4);
							cout << "Kill in point " << letters[comp_hit.X] << comp_hit.Y + 1 << "\t";
							setlocale(LC_ALL, "C");
							first_move = false;
						}
						else
						{
							SetConsoleTextAttribute(hOut, 11*16 + 12);
							cout << "Hit the point " << letters[comp_hit.X] << comp_hit.Y + 1 << "\t";
							setlocale(LC_ALL, "C");
							first_move = false;
						}
				}
				else
				{
					SetConsoleTextAttribute(hOut, 11*16+0);
					cout << "Do not hit the " << letters[comp_hit.X] << comp_hit.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
					first_move = true;
				}
			}
			else
			{
				switch(chance)
				{
				case 0:
					Chance0(comp_hits);
					break;
				case 1:
					Chance1(comp_hits);
					break;
				case 2:
					Chance2(comp_hits);
					break;
				case 3:
					Chance3(comp_hits);
					break;
				}
				WriteToReg(comp_hits, for_comp.last);
				HitOrNo(ar_ships_gamer, for_comp.last, hit_result_comp, ar_y, ar_x);
				switch(hit_result_comp)
				{
				case 1:
					chance = 0;
					
					for_comp.count_of_good_hit = 0;
					comp_kills++;
					
					SetNullPoints(comp_hits, for_comp);
					
					SetConsoleTextAttribute(hOut, 11*16 + 4);
					cout << "Kill in point " << letters[for_comp.last.X] << for_comp.last.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
					first_move = false;
					break;
				case 2:
					chance = 1;
					
					for_comp.count_of_good_hit++;
					SetConsoleTextAttribute(hOut, 11*16 + 12);
					cout << "Hit the point " << letters[for_comp.last.X] << for_comp.last.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
					first_move = false;
					break;
				case 0:
					SetConsoleTextAttribute(hOut, 11*16+0);
					cout << "Do not hit in " << letters[for_comp.last.X] << for_comp.last.Y + 1 << "\t";
					setlocale(LC_ALL, "C");
					hit_result_comp = 2;
					if(!for_comp.count_of_good_hit || for_comp.count_of_good_hit > 2)	chance = 3;
					else chance = 2;
					first_move = true;
					break;
				}
			}
			Show(ar_ships_gamer, t, 0);
			ShowHits(comp_hits, ar_ships_gamer, t, 0);
		}
	}
	Sleep(3000);
	if(gamer_kills == 10) TheWinner(true);
	else if(comp_kills == 10) TheWinner(false);
}

void Emblema(COORD c, int size, int width, Color color, int what)
{
	LOGFONT font;
	font.lfHeight = size;// Устанавливает высоту шрифта или символа
	font.lfWidth =  width;// Устанавливает среднюю ширину символов в шрифте
	font.lfEscapement = 0;// Устанавливает угол, между вектором наклона и осью X устройства
	font.lfOrientation = 0;// Устанавливает угол, между основной линией каждого символа и осью X устройства
	font.lfWeight = 900;// Устанавливает толщину шрифта в диапазоне от 0 до 1000
	font.lfItalic = 0;// Устанавливает курсивный шрифт
	font.lfUnderline = 0;// Устанавливает подчеркнутый шрифт
	font.lfStrikeOut = 0;// Устанавливает зачеркнутый шрифт
	font.lfCharSet = ANSI_CHARSET;// Устанавливает набор символов
	font.lfOutPrecision = 0;// Устанавливает точность вывода
	font.lfClipPrecision = 0;// Устанавливает точность отсечения
	font.lfQuality = PROOF_QUALITY;// Устанавливает качество вывода
	font.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;// Устанавливает ширину символов и семейство шрифта
	lstrcpy(font.lfFaceName , L"Showcard Gothic");//  устанавливает название шрифта
	
	HFONT hfont;
	hfont = CreateFontIndirect(&font);
	SetTextColor(hdc,RGB(color.R, color.G, color.B));
		
	if(what < 3)SetBkColor(hdc,RGB(0,255,255));
	else SetBkColor(hdc,RGB(0,255,127));
	SelectObject(hdc,hfont);
	switch(what)
	{
	
	case 0:
		TextOutA(hdc,8*c.X, 12*c.Y, "B A T T L E S H I P S", 23);
		break;
	case 1:
		TextOutA(hdc,8*c.X, 12*c.Y, "M E N U", 8);
		break;
	case 2:
		TextOutA(hdc,8*c.X, 12*c.Y, "E X I T", 8);
		break;
	case 3:
		TextOutA(hdc,8*c.X, 12*c.Y, "CONTINUE", 9);
		break;
	case 4:
		TextOutA(hdc,8*c.X, 12*c.Y, "SAVE GAME", 10);
		break;
	case 5:
		TextOutA(hdc,8*c.X, 12*c.Y, "LOAD GAME", 10);
		break;
	case 6:
		TextOutA(hdc,8*c.X, 12*c.Y, "EXIT GAME", 10);
		break;
	case 7:
		TextOutA(hdc,8*c.X, 12*c.Y, "NEW GAME", 9);
		break;
	case 8:
		TextOutA(hdc,8*c.X, 12*c.Y, "You WIN!!!", 11);
		break;
	case 9:
		TextOutA(hdc,8*c.X, 12*c.Y, "You lose ='(", 13);
		break;
	case 10:
		TextOutA(hdc,8*c.X, 12*c.Y, "Play again", 11);
		break;
	}
}

void ShowRamki()
{
	COORD write = {0,0};
	HBRUSH menu_brush = CreateSolidBrush(RGB(0, 255, 255));
	HPEN back_pen = CreatePen(PS_SOLID,1,RGB(0,255,255));
	SelectObject(hdc,back_pen);
	SelectObject(hdc, menu_brush);
	Rectangle(hdc, 8*0, 12*0, 8*screen_cols, 12*screen_lines);
	DeleteObject(back_pen);
	DeleteObject(menu_brush);
	
	write.X = field_start.X - 9;
	write.Y = 2;
	color.R = 0;
	color.G = 0;
	color.B = 128;
	
	Emblema(write, 95, 45, color, 0);
	
	color.R = 138;
	color.G = 43;
	color.B = 226;
	
	write.X = 1;
	write.Y = 0;
	
	Emblema(write, 25, 0, color, 1);
	
	write.X = screen_cols - 9;
	Emblema(write, 25, 0, color, 2);


	COORD t = field_start;
	t.X--;
	t.Y--;
	ShowRamka(t, 10);
	t.X += field_size*4;
	ShowRamka(t, 12);
}

void EnterName()
{
	COORD write = {0, field_start.Y - 3};
	DWORD p;


	system("CLS");
	write.Y = screen_lines / 2 - 3;
	write.X = screen_cols / 2 - 9;
	SetConsoleCursorPosition(hOut, write);
	SetConsoleTextAttribute(hOut, 11*16+12);
	cout << "ENTER YOUR NAME, PLEASE:";
	write.X += 1;
	while(write.Y < screen_lines/2+1)
	{
		write.Y++;
		FillConsoleOutputAttribute(hOut, 14*16, 21, write, &p);
	}
	write.Y--;
	//write.X += 7;
	write.Y--;
	SetConsoleCursorPosition(hOut, write);
	name = new char[30];
	SetConsoleTextAttribute(hOut, 14*16);
	gets_s(name, 30);
	OemToCharA(name, name);

}


void Init()
{
	srand(time(0));
	ar_ships_gamer = new Point*[10];
	ar_ships_comp = new Point * [10];
	comp_hits = new COORD [100];
	gamer_hits = new COORD [100];


	for(int i = 0; i < 100; i++)
	{
		comp_hits[i].Y = i / 10;
		comp_hits[i].X = i % 10;

		gamer_hits[i].Y = i / 10;
		gamer_hits[i].X = i % 10;
	}


	for(int y = 0; y < 10; y++)
	{
		int temp;
		if(y < 4) temp = 1;
		else if(y > 3 && y < 7) temp = 2;
		else if(y > 6 && y < 9) temp = 3;
		else temp = 4;
		ar_ships_gamer[y] = new Point[temp];
		ar_ships_comp[y] = new Point[temp];
	}

	ramka = new char [ramka_size];
	for(int x = 0; x < ramka_size; x++)
	{
		ramka[x] = (char) 178;
	}
	AutomaticShips(ar_ships_gamer);
	AutomaticShips(ar_ships_comp);
	bool first_move = rand()%2;
	ShowRamki();
	Game(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, first_move, 0, 0);
}

void PrintMenuElement(COORD c, Color color, int what)
{
	HBRUSH menu_brush = CreateSolidBrush(RGB(0, 255, 127));
	SelectObject(hdc, menu_brush);
	RoundRect(hdc, 8 * c.X, 12 * c.Y - 3, 8 * c.X + 12*(screen_cols/2 - c.X-1)*2, 12 * (c.Y + 3)-3, 15, 15);
	color.R = 105;
	color.G = 89;
	color.B = 205;
	c.X+= (screen_cols/2 - c.X)/2;
	Emblema(c, 30, 10, color, what);
}

void Menu(bool main, bool first_move, int gamer_kills, int comp_kills)
{
	COORD write = {0, 0};
	DWORD p;
	HBRUSH menu_brush = CreateSolidBrush(RGB(0, 255, 255));
	HPEN back_pen = CreatePen(PS_SOLID,1,RGB(0,255,255));
	SelectObject(hdc,back_pen);
	SelectObject(hdc, menu_brush);
	Rectangle(hdc, 8*0, 12*0, 8*screen_cols, 12*screen_lines);
	DeleteObject(back_pen);
	DeleteObject(menu_brush);

	write.X = field_start.X - 9;
	write.Y = 2;
	color.R = 0;
	color.G = 0;
	color.B = 128;
	Emblema(write, 95, 45, color, 0);

	write.Y = screen_lines / 2 - 2;
	write.X = screen_cols / 2 - 8;
	if(main)PrintMenuElement(write, color, 7); //NEW GAME
	else PrintMenuElement(write, color, 3); // CONTINUE
	
	if(!main)
	{
		write.Y += 5;
		PrintMenuElement(write, color, 4); //SAVE GAME
	}
	write.Y += 5;
	PrintMenuElement(write, color, 5); //LOAD GAME
	write.Y += 5;
	PrintMenuElement(write, color, 6); // EXIT GAME
	COORD c;
	int job;
	Mouse(c, job, 1, main, 0, 0, 0);
	
	
	if(job == 3)
	{
		ShowRamki();
		Game(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, first_move, gamer_kills, comp_kills);
	}
	else if(job == 4)
	{
		Save(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, first_move, gamer_kills, comp_kills);
		write.X = screen_cols/2+15;
		write.Y = screen_lines/2+4;
		SetConsoleCursorPosition(hOut, write);
		SetConsoleTextAttribute(hOut, 11*16+2);
		cout << "SAVED!";
		Sleep(1200);
		ShowRamki();
		Game(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, first_move, gamer_kills, comp_kills);
	}
	else if(job == 5) 
	{
		Load(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, first_move, gamer_kills, comp_kills, main);
		
	}
	else if(job == 6)
	{
		if(!main)DeleteArrays(ar_ships_gamer, ar_ships_comp, comp_hits, gamer_hits, 1);
		exit(0);
	}
	else if(job == 7)
	{
		EnterName();
		Init();
	}
}



void main()
{	
	
	system("mode con cols=110 lines=37");
	CONSOLE_CURSOR_INFO cinfo = {1, false};
	SetConsoleCursorInfo(hOut, &cinfo);

	CONSOLE_SCREEN_BUFFER_INFO csbInfo; 
	GetConsoleScreenBufferInfo(hOut, &csbInfo);
	screen_cols = csbInfo.dwMaximumWindowSize.X;
	screen_lines = csbInfo.dwMaximumWindowSize.Y;
	field_start.X = screen_cols/2 - 35;
	field_start.Y = screen_lines - 24;
	
	Menu(1,0,0,0);
	
	

}
