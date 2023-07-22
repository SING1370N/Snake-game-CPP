#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

void SetColor(int color[2])
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((color[1] << 4) | color[0]));
}


string snake_cmd = "X";
int snake_color[2] = { 8, 14 };
string border_cmd = "#";
int border_color[2] = { 0, 0 };
string apple_cmd = "+";
int apple_color[2] = { 4, 4 };
string background_cmd = " ";
int background_color[2] = { 0, 15 };
int bonus_point = 1;

class Point {
public:
	int x, y;

	Point(int x = 0, int y = 0) : x(x), y(y) {}
};

class Snake {
private:
	Point* body;
	int size;

public:
	Snake(int x = 10, int y = 10) {
		size = 1;
		body = new Point[size];
		body[0] = Point(x, y);
	}

	~Snake() {
		delete[] body;
	}

	void restart(int x = 10, int y = 10) {
		size = 1;
		body = new Point[size];
		body[0] = Point(x, y);
	}

	void grow() {
		Point* newBody = new Point[size + 1];
		for (int i = 0; i < size; ++i) {
			newBody[i] = body[i];
		}
		newBody[size] = body[size - 1];
		delete[] body;
		body = newBody;
		++size;
	}

	Point& operator[](int index) {
		return body[index];
	}

	int getSize() const {
		return size;
	}
};

class Food
{
public:
	virtual ~Food() = default;
	virtual void generateFood(int& x, int& y, int& width, int& height, Snake& snake) const = 0;;
};

class Apple : public Food
{
public:
	void generateFood(int& x, int& y, int& width, int& height, Snake& snake) const override
	{
		bonus_point = 1;
		apple_cmd = "+";
		apple_color[0] = 4;
		apple_color[1] = 4;
		srand(time(0));
		x = rand() % width;
		y = rand() % height;
		for (int i = 0; i < snake.getSize(); ++i) {
			if (x == snake[i].x && y == snake[i].y) {
				generateFood(x, y, width, height, snake);
				break;
			}
		}
	}
};

class GoldenApple : public Food
{
public:
	void generateFood(int& x, int& y, int& width, int& height, Snake& snake) const override
	{
		bonus_point = 10;
		apple_cmd = "O";
		apple_color[0] = 7;
		apple_color[1] = 4;
		srand(time(0));
		x = rand() % width;
		y = rand() % height;
		for (int i = 0; i < snake.getSize(); ++i) {
			if (x == snake[i].x && y == snake[i].y) {
				generateFood(x, y, width, height, snake);
				break;
			}
		}
	}
};

class Game {
private:
	Snake snake;
	Point food;
	bool gameOver;

	void draw() {
		system("cls");
		SetColor(border_color);
		for (int i = 0; i < width + 2; ++i) {
			cout << border_cmd;
		}
		cout << endl;
		SetColor(background_color);
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				if (j == 0) {
					SetColor(border_color);
					cout << border_cmd;
					SetColor(background_color);
				}

				bool isSnakePart = false;
				for (int k = 0; k < snake.getSize(); ++k) {
					if (j == snake[k].x && i == snake[k].y) {
						isSnakePart = true;
						break;
					}
				}

				if (isSnakePart) {
					SetColor(snake_color);
					cout << snake_cmd;
					SetColor(background_color);
				}
				else if (j == food.x && i == food.y) {
					SetColor(apple_color);
					cout << apple_cmd;
					SetColor(background_color);
				}
				else {
					SetColor(background_color);
					cout << background_cmd;
				}

				if (j == width - 1) {
					SetColor(border_color);
					cout << border_cmd;
					SetColor(background_color);
				}
			}
			cout << endl;
		}
		SetColor(border_color);
		for (int i = 0; i < width + 2; ++i) {
			cout << border_cmd;
		}
		SetColor(background_color);
		cout << endl << endl << " Points: " << points << "\n Size: " << width << "x" << height;
	};

	void input() {
		int key = _getch();
		Point& head = snake[0];
		Point newHead = head;

		switch (key) {
		case 'w':
		case 72:
			newHead.y--;
			break;
		case 'a':
		case 75:
			newHead.x--;
			break;
		case 's':
		case 80:
			newHead.y++;
			break;
		case 'd':
		case 77:
			newHead.x++;
			break;
		case 'x':
			gameOver = true;
			break;
		}
		if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height) {
			gameOver = true;
		}
		else {
			for (int i = 1; i < snake.getSize(); ++i) {
				if (newHead.x == snake[i].x && newHead.y == snake[i].y) {
					gameOver = true;
					break;
				}
			}

			if (!gameOver) {
				for (int i = snake.getSize() - 1; i > 0; --i) {
					snake[i] = snake[i - 1];
				}
				snake[0] = newHead;
			}
		}
		if (gameOver)
		{
			cout << endl << " GAME OVER!" << endl;
			system("pause");
		}
	};

	void logic() {
		if (snake[0].x == food.x && snake[0].y == food.y) {
			points += bonus_point;
			snake.grow();
			generateFood();
		}
	};

	



public:
	int width, height;
	int points = 0;
	unique_ptr<Food> food_;

	void set_strategy(unique_ptr<Food>&& food)
	{
		food_ = move(food);
	}

	void generateFood() {
		if ((rand() % 100) % 9 != 0) set_strategy(make_unique<Apple>());
		else set_strategy(make_unique<GoldenApple>());

		food_->generateFood(food.x, food.y, width, height, snake);
	};

	Game(int width = 20, int height = 20) : width(width), height(height), snake(width / 2, height / 2) {
		generateFood();
		gameOver = false;
	}

	void run() {
		while (!gameOver) {
			draw();
			input();
			logic();
		}
	};

	void settings() {
		while (true)
		{
			system("cls");
			cout << "\tSETTINGS:\n" <<
				" 1. Size\n" <<
				" 2. Chars\n" <<
				" 3. Go menu\n >>> ";
			int key = _getch();
			string ch, y;
			switch (key)
			{
			case '1':
				int temp;
				cout << "Size: \nWidth: ";
				cin >> temp;
				if (temp >= 10)
				{
					width = temp;
				}
				else cout << "Error! Width >= 10!";
				cout << "Height: ";
				cin >> temp;
				if (temp >= 10)
				{
					height = temp;
				}
				else cout << "Error! Height >= 10!";
				break;
			case '2':
				system("cls");				
				while (true)
				{
					cout << "Old char: " + snake_cmd << endl << " >>> ";
					cin >> ch;
					cout << "New char: " + ch << endl << " Okay? (y/n): ";
					cin >> y;
					if (y == "y" || y == "yes")
					{
						snake_cmd = ch;
						break;
					}
				}
				break;
			case '3':
				return;
			default:
				break;
			}
		}
	};

	void menu() {
		while (true)
		{
			gameOver = false;
			system("cls");
			cout << "\tMENU:\n" <<
				" 1. Start\n" <<
				" 2. Next\n" <<
				" 3. Settings\n" <<
				" 4. Exit\n >>> ";
			int key = _getch();
			switch (key)
			{
			case '1':
				snake.restart();
				run();
				break;
			case '2':
				run();
				break;
			case '3':
				settings();
				break;
			case '4':
				return;
			default:
				break;
			}
		}
	};


};

int main(int argc, char* argv[]) {
	Game game;
	game.menu();
}