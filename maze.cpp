// maze.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <math.h>

#define OFFSET 5
#define QUEUE_SIZE 500

const int gridX = 30;
const int gridY = 30;

using namespace cv;
using namespace std;

#define M_PI   3.14159265358979323846264338327950288
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

char wndname[] = "TurtleWindow";
Mat image = Mat::zeros(1000, 1000, CV_8UC3);
Scalar WHITE(255, 255, 255);
Scalar BLACK(0, 0, 0);

const int DELAY = 1;
Point _curPosition(250, 250);
int _direction = 0;

/**/
// 
// Must be called in main() before any other drawing function
//
void init()
{
	imshow(wndname, image);
	waitKey(DELAY);
}

//
// Move the pen to the given coordinates without leaving a mark
// 
// Note (0,0)     refers to the upper left corner
//      (500,500) refers to the bottom right corner  
//
void changePosition(int x, int y)
{
	_curPosition.x = x + OFFSET;
	_curPosition.y = y + OFFSET;
	// cout << "Changed position to " << _curPosition.x << "," << _curPosition.y << endl;
}

//
// point in the direction given in degrees
// 0   ==> point right
// 90  ==> point down
// 180 ==> point left
// 270 ==> point up
//
void changeDirection(int direction)
{
	_direction = direction;
}

//
// Moves the pen forward the given number of pixels
// Note leaves a mark creating a line from the previous point
// to the new point
//
void moveForward(int nPixels, cv::Scalar color, bool repaint = true)
{
	int x = static_cast<int>(round(nPixels * cos(degToRad(_direction))));
	int y = static_cast<int>(round(nPixels * sin(degToRad(_direction))));

	Point newPoint = Point(x + _curPosition.x, y + _curPosition.y);
	line(image, _curPosition, newPoint, color);
	_curPosition = newPoint;
	// cout << "moved to " << newPoint.x << "," << newPoint.y << " dir:" << _direction << endl;
	if (repaint)
	{
		imshow(wndname, image);
		waitKey(DELAY);
	}
}

void moveForward(int nPixels, bool repaint = true)
{
	moveForward(nPixels, WHITE, repaint);
}
int myDirection = 0;

class Square
{
private:
	int _pixelX;
	int _pixelY;
	bool _top;
	bool _bottom;
	bool _left;
	bool _right;
public:
	int xGrid;
	int yGrid;
	
	bool isVisited = false;

	Square(int x = 0, int y = 0)
	{
		_pixelX = x;
		_pixelY = y;
		_top = _bottom = _left = _right = true;
		xGrid = x/25;
		yGrid = y/25;
	}
	void setCoordinates(int x, int y)
	{
		_pixelX = x;
		_pixelY = y;
		xGrid = x/25;
		yGrid = y/25;
	}
	void removeTop() { _top = false; };
	void removeBottom() { _bottom = false; };
	void removeRight() { _right = false; };
	void removeLeft() { _left = false; };

	bool hasTop() { return _top; }
	bool hasBottom() { return _bottom; }
	bool hasLeft() { return _left; }
	bool hasRight() { return _right; }

	void markVisited() { isVisited = true; }
	bool visited() { return isVisited; }

	void drawSquare()
	{
		changePosition(_pixelX, _pixelY);
		Scalar color;
		changeDirection(0);

		if (_top)
			color = WHITE;
		else
			color = BLACK;
		moveForward(25, color);
		myDirection += 90;
		changeDirection(myDirection);

		if (_right)
			color = WHITE;
		else
			color = BLACK;
		moveForward(25, color);
		myDirection += 90;
		changeDirection(myDirection);

		if (_bottom)
			color = WHITE;
		else
			color = BLACK;
		moveForward(25, color);
		myDirection += 90;
		changeDirection(myDirection);

		if (_left)
			color = WHITE;
		else
			color = BLACK;
		moveForward(25, color);
		myDirection += 90;
		changeDirection(myDirection);
	}
};

class SquareQueue
{
private:
	int front;
	int rear;
	int size;
public:
	Square s[QUEUE_SIZE];
	SquareQueue()
	{
		front = -1;
		rear = -1;
		size = 0;
	}
	bool isEmpty()
	{
		return size == 0;
	}
	bool isFull()
	{
		return size == QUEUE_SIZE;
	}
	void enq(Square square)
	{
		if (size == QUEUE_SIZE)
		{
			cout << "Queue overflow";
			//exit(0);
			return;
		}
		rear = (rear + 1) % QUEUE_SIZE;
		s[rear] = square;
		size++;
	}
	Square deq()
	{
		if (0 == size)
		{
			cout << "Queue underflow";
			//exit(1);
		}
		front += 1;
		front = front % QUEUE_SIZE;
		Square sq = s[front];
		size--;
		return sq;
	}
	Square getFront()
	{
		if (front == -1)
			return 0;
		return s[front];
	}
};

class Maze
{
private:
	int _gridHeight;
	int _gridWidth;
	Square grid[gridX][gridY];
	SquareQueue sq;

	bool _aboveVisited;
	bool _rightVisited;
	bool _belowVisited;
	bool _leftVisited;
public:
	Maze(int height, int width)
	{
		_gridHeight = height;
		_gridWidth = width;

		for (int i = 0; i < _gridHeight; i++)
		{
			for (int j = 0; j < _gridWidth; j++)
			{
				grid[i][j].setCoordinates(j * 25, i * 25);
				grid[i][j].drawSquare();
			}
		}
	}	

	//Move through the cells
	void goUp(int x, int y)
	{
		//x--;
		grid[x][y].removeBottom();
		grid[x][y].drawSquare();
		grid[x][y].markVisited();
		sq.enq(grid[x][y]);
	}
	
	void goRight(int x, int y)
	{	
		//y++;
		grid[x][y].removeLeft();
		grid[x][y].drawSquare();
		grid[x][y].markVisited();
		sq.enq(grid[x][y]);
	}
	void goDown(int x, int y)
	{
		//x++;
		grid[x][y].removeTop();
		grid[x][y].drawSquare();
		grid[x][y].markVisited();
		sq.enq(grid[x][y]);
	}
	void goLeft(int x, int y)
	{
		//y--;
		grid[x][y].removeRight();
		grid[x][y].drawSquare();
		grid[x][y].markVisited();
		sq.enq(grid[x][y]);
	}

	//Determine whether neighboring cells have been visited
	bool aboveVisited(int x, int y) 
	{ 
		if (x == 0) return true;
		return grid[x - 1][y].isVisited; 
	}
	bool rightVisited(int x, int y) 
	{ 
		if (y == gridY - 1) return true;
		return grid[x][y + 1].isVisited; 
	}
	bool belowVisited(int x, int y) 
	{ 
		if (x == gridX - 1) return true;
		return grid[x + 1][y].isVisited; 
	}
	bool leftVisited(int x, int y)  
	{ 
		if (y == 0) return true;
		return grid[x][y - 1].isVisited; 
	}

	bool hasUnvisitedNeighbors(int x, int y)
	{ 
		bool a = !aboveVisited(x, y);
		bool b = !rightVisited(x, y);
		bool c = !belowVisited(x, y);
		bool d = !leftVisited(x, y);

		return (a || b|| c|| d);
	}

	void createMaze() 
	{
		int endX = 0;
		int endY = 0;
		int startX = 0;
		int startY = 0;
		int count = 0;
		
		//Start
		sq.enq(grid[0][0]);
		grid[0][0].removeLeft();
		grid[0][0].drawSquare();
		grid[0][0].markVisited();
		count++;
		
		//End
		grid[gridX - 1][gridY - 1].removeRight();
		grid[gridX - 1][gridY - 1].drawSquare();

		srand(time(NULL));
		Square temp;
		
		while (count < (gridX * gridY))
		{
			while(!hasUnvisitedNeighbors(endX, endY))
			{
				temp = sq.getFront();
				startX = temp.xGrid;
				startY = temp.yGrid;
				

				if (startX == -1 && startY == 10)
				{
					temp = sq.deq();
					continue;
				}
				if (hasUnvisitedNeighbors(startX, startY))
				{
					// Clear queue untill start square
					sq.enq(grid[startX][startY]);
					endX = startX;
					endY = startY;
				}
				else 
				{ 
					if(!sq.isEmpty())
					{ 
						temp = sq.deq(); 
					}
					else
					{
						startX = 0;
						startY = 0;
					}
				}
			}

			//(0,0)
			if (endX == 0 && endY == 0)
			{
				if (!rightVisited(endX, endY) && !belowVisited(endX, endY))
				{
					int r = rand() % 2 + 1;
					switch (r)
					{
					case 1: endY++; goRight(endX, endY); count++;
					case 2: endX++; goDown(endX, endY); count++;
					} 
					continue;
				}
			}

			//Chooses random direction to move to
			int r = rand() % 4 + 1;
			while (((aboveVisited(endX,endY) && r == 1)  || (endX == 0 && r == 1)) 
				|| ((rightVisited(endX, endY) && r == 2) || (endY == gridY - 1 && r == 2)) 
				|| ((belowVisited(endX, endY) && r == 3) || (endX == gridX - 1 && r == 3)) 
				|| ((leftVisited (endX, endY) && r == 4) || (endY == 0 && r == 4)))
			{
				r = rand() % 4 + 1;
			}
			if (r == 1)
			{
				endX--;
				goUp(endX, endY);
			}
			else if (r == 2)
			{
				endY++;
				goRight(endX, endY);
			}
			else if (r == 3)
			{
				endX++;
				goDown(endX, endY);
			}
			else if (r == 4)
			{
				endY--;
				goLeft(endX, endY);
			}
			count++;
		}	
	}
};

int main()
{
	init();
	image.setTo(BLACK);
	Maze m(gridX, gridY);
	m.createMaze();

	waitKey();
}

