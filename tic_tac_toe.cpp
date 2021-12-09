#include<iostream>
#include<string>
#include<random>
#include<stdlib.h>
#include<chrono>//работа со временем: секунды или милисекунды

using namespace std;

//=================================================================



class PlayerDate {
public:
	string fName;
	string lName;
	string theDateOfBirth;
	int experienceOfTheGame;
	

	void printdate()
	{
		cout << "First Name: " << fName << endl <<
			"Last Name: " << lName << endl <<
			"The date of birth: " << theDateOfBirth << endl <<
			"Game's experience: " << experienceOfTheGame << " year('s)" <<
			endl;
	}

	

};

enum class Cell : char {//наши обозначения
	CROSS = 'x',
	ZERO = '0',
	EMPTY = '_'
};

struct Coord {//координаты
	size_t y;
	size_t x;
};

//DRY don't repeat yourself, STL "Standart Template Library"

enum class Progress {//4 состояния игры
	IN_PROGRESS,
	WON_HUMAN,
	WON_AI, 
	DRAW
};

#pragma pack(push, 1)//нужно для сжатие структуры (экономии памяти)
struct Game { //структуры игры
	Cell** ppField = nullptr; //сделаем динамическое поле для игры
	const size_t SIZE = 3;// 3x3
	Progress progress = Progress::IN_PROGRESS;
	Cell human; //кто чем играет? (чей ход)
	Cell ai;
	size_t turn{ 0 }; //четные ходит человек, нечетные ходит ПК
};
#pragma pack(pop)

//=================================================================

void clearScr()//функция для очистки экрана
{
	system("cls");
}

int32_t getRandomNum(int32_t min, int32_t max)
{
	const static auto seed = chrono::system_clock::now().time_since_epoch().count();
	static mt19937_64 generator(seed);
	uniform_int_distribution<int32_t> dis(min, max);
	return dis(generator);
}

void initGame(Game& g)//будем передавать игру
{
	//выделяем наше поле в RAM
	g.ppField = new Cell * [g.SIZE];
	for (size_t i = 0; i < g.SIZE; i++)
	{
		g.ppField[i] = new Cell[g.SIZE];
	}

	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			g.ppField[y][x] = Cell::EMPTY;
		}
	}
	//кто чем ходит?
	if (getRandomNum(0, 1000) > 500)
	{
		g.human = Cell:: CROSS;
		g.ai = Cell:: ZERO;
		g.turn = 0;
	}
	else
	{
		g.human = Cell::ZERO;
		g.ai = Cell::CROSS;
		g.turn = 1;
	}
}

void deinitGame(Game& g)//освобождает память после игры
{
	//освобождаем наше поле из RAM
	for (size_t i = 0; i < g.SIZE; i++)
	{
		delete[] g.ppField[i];
	}
	delete[] g.ppField;
	g.ppField = nullptr;
}

int printGame(const Game& g, int theNumberOfTheGames, int theNumberOfDraws, int theNumberOfWins, int theNumberOfLoses)
{
	cout << "Номер партии: " << theNumberOfTheGames<< endl;
	cout << "   # ";
	for (size_t x = 0; x < g.SIZE; x++)
	{
		cout << x + 1 << " # ";
	}
	cout << endl;

	for (size_t y = 0; y < g.SIZE; y++)
	{
		cout << "#" << y + 1 << "#| ";
		for (size_t x = 0; x < g.SIZE; x++)
		{
			cout << (char)g.ppField[y][x] << " | ";
		}
		cout << endl;
	}


	cout << "\nКоличество ничейных партий: " << theNumberOfDraws << endl <<
		"Количество  выиграшей: " << theNumberOfWins << endl <<
		"Количество проиграшей: " << theNumberOfLoses << endl;


	cout << endl << " Human: " << (char)g.human << endl;
	cout << " Computer: " << (char)g.ai << endl << endl;

	return theNumberOfDraws, theNumberOfWins, theNumberOfLoses;
}

int congrats(const Game& g, int& theNumberOfWins, int& theNumberOfLoses, int& theNumberOfDraws)
{
	if (g.progress == Progress::WON_HUMAN)
	{
		cout << " Human won! =)" << endl;
		theNumberOfWins++;
	}

	else if (g.progress == Progress::WON_AI)
	{
		cout << " Computer won! =(" << endl;
		theNumberOfLoses++;
	}

	else if (g.progress == Progress::DRAW)
	{
		cout << " DRAW =/" << endl;
		theNumberOfDraws++;
	}
	return theNumberOfWins, theNumberOfLoses, theNumberOfDraws;
}



Coord getHumanCoord(const Game& g)
{
	Coord c;
	do {
		cout << "Enter X(1..3): ";
		cin >> c.x;
		cout << "Enter Y(1..3): ";
		cin >> c.y;

		c.x--;
		c.y--;

		if (g.ppField[c.y][c.x] != Cell::EMPTY)
		{
			cout << "Error! It seems that cell is NOT empty... Select other cell.";
		}

	} while (c.x > 2 || c.y > 2 || g.ppField[c.y][c.x] != Cell::EMPTY);

	return c;
}


Progress getWon(const Game& g)//перебираем комбинации выиграша
{
	//победа в строках?
	for (size_t y = 0; y < g.SIZE; y++)
	{
		if (g.ppField[y][0] == g.ppField[y][1] && g.ppField[y][0] == g.ppField[y][2])
		{
			if (g.ppField[y][0] == g.human)
			{
				return Progress::WON_HUMAN;
			}
			if (g.ppField[y][0] == g.ai)
			{
				return Progress::WON_AI;
			}
		}
	}

	//победа в столбцах
	for (size_t x = 0; x < g.SIZE; x++)
	{
		if (g.ppField[0][x] == g.ppField[1][x] && g.ppField[0][x] == g.ppField[2][x])
		{
			if (g.ppField[0][x] == g.human)
			{
				return Progress::WON_HUMAN;
			}
			if (g.ppField[0][x] == g.ai)
			{
				return Progress::WON_AI;
			}
		}
	}

	//диагонали

	if (g.ppField[0][0] == g.ppField[1][1] && g.ppField[0][0] == g.ppField[2][2])
	{
		if (g.ppField[1][1] == g.human)
		{
			return Progress::WON_HUMAN;
		}
		if (g.ppField[1][1] == g.ai)
		{
			return Progress::WON_AI;
		}
	}

	if (g.ppField[2][0] == g.ppField[1][1] && g.ppField[0][2] == g.ppField[1][1])
	{
		if (g.ppField[1][1] == g.human)
		{
			return Progress::WON_HUMAN;
		}
		if (g.ppField[1][1] == g.ai)
		{
			return Progress::WON_AI;
		}
	}

	//ничья
	bool draw = true;
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == Cell::EMPTY)
			{
				draw = false;
				break;
			}
		}
		if (!draw)
		{
			break;
		}
	}
	if (draw)
	{
		return Progress::DRAW;
	} 
	return Progress::IN_PROGRESS;
}

Coord getAICoord(Game& g) //для компьютера не делаем const, он будет хитрее
{
	//1.Компьютер должен заботиться о своем выйграше
	//  PRE WIN SITUATION
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == Cell::EMPTY)
			{
				g.ppField[y][x] = g.ai;
				if (getWon(g) == Progress::WON_AI)
				{
					g.ppField[y][x] = Cell::EMPTY;
					return { y,x };
				}
				g.ppField[y][x] = Cell:: EMPTY;
			}
		}
	}

	//2.Компьютер должен мешать человеку выиграть
	//  PRE FAIL SITUATION
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == Cell::EMPTY)
			{
				g.ppField[y][x] = g.human;
				if (getWon(g) == Progress::WON_HUMAN)
				{
					g.ppField[y][x] = Cell::EMPTY;
					return { y,x };
				}
				g.ppField[y][x] = Cell::EMPTY;
			}
		}
	}

	//3. Ход по приоритетам + Ранд

	//Анализируем центр
	if (g.ppField[1][1] == Cell::EMPTY)
	{
		return { 1,1 };
	}

	//Анализируем углы 
	Coord buf[4];
	size_t n{ 0 };

	if (g.ppField[0][0] == Cell::EMPTY)
	{
		buf[n] = { 0, 0 };
		n++;
	}

	if (g.ppField[2][2] == Cell::EMPTY)
	{
		buf[n] = { 2,2 };//в нашей индексации первым идет y
		n++;
	}
	
	if (g.ppField[0][2] == Cell::EMPTY)
	{
		buf[n] = { 0, 2 };
		n++;
	}

	if (g.ppField[2][0] == Cell::EMPTY)
	{
		buf[n] = { 2, 0 };
		n++;
	}
	
	if (n > 0)
	{
		const size_t index = getRandomNum(0, 3000) % n;//случайный индекс свободного угла
		return buf[index];
	}

	//Анализируем НЕ углы
	n = 0;
	if (g.ppField[1][0] == Cell::EMPTY)
	{
		buf[n] = { 1, 0 };
		n++;
	}

	if (g.ppField[1][2] == Cell::EMPTY)
	{
		buf[n] = { 1,2 };//в нашей индексации первым идет y
		n++;
	}

	if (g.ppField[0][1] == Cell::EMPTY)
	{
		buf[n] = { 0, 1 };
		n++;
	}

	if (g.ppField[2][1] == Cell::EMPTY)
	{
		buf[n] = { 2, 1 };
		n++;
	}

	if (n > 0)
	{
		const size_t index = getRandomNum(0, 3000) % n;//случайный индекс свободного НЕ угла
		return buf[index];
	}
}

//=================================================================

int main()
{
	setlocale(LC_ALL, "Rus");
	PlayerDate player1{ "Sasha", "Sinitsyn", "27/10/1997", 2 };

	int theNumberOfTheGames= 1;
	int theNumberOfWins = 0;
	int theNumberOfLoses = 0;
	int theNumberOfDraws = 0;

	while (theNumberOfTheGames <= 3)
	{
		Game g;//можно сделать тоже динамическую вместо на стеке
		initGame(g);
		clearScr();
		printGame(g, theNumberOfTheGames, theNumberOfDraws, theNumberOfWins, theNumberOfLoses);

		do {
			if (g.turn % 2 == 0)
			{
				//ходит человек
				Coord c = getHumanCoord(g);
				g.ppField[c.y][c.x] = g.human;
			}
			else
			{
				//ходит ПК
				Coord c = getAICoord(g);
				g.ppField[c.y][c.x] = g.ai;
			}
			clearScr();
			printGame(g, theNumberOfTheGames, theNumberOfDraws, theNumberOfWins, theNumberOfLoses);
			g.turn++;
			g.progress = getWon(g);

		    } while (g.progress == Progress::IN_PROGRESS);
		congrats(g, theNumberOfWins, theNumberOfLoses, theNumberOfDraws);
		theNumberOfTheGames++;
		deinitGame(g);
	} 

	return 0;
}