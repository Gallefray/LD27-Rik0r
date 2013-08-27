#include "QL.h"

typedef struct 
{
	int x, y;
	int moveRad, moveSpeed, angle;
	int origX, origY;
} star;

#define maxStars 1000

star stars[maxStars];

void load()
{
	int i;
	for (i = 0; i < maxStars; i++)
	{
		stars[i].origX = scrWidth/2;
		stars[i].origY = scrHeight/2;
		stars[i].moveRad = randomNum(5, 10);
		stars[i].angle = degreesToRadians(randomNum(0, 360));
		stars[i].moveSpeed = randomNum(2, 6);
	}
}

void logic()
{
	int i;
	for (i = 0; i < maxStars; i++)
	{
		stars[i].moveRad += stars[i].moveSpeed;
		stars[i].x = cos(stars[i].angle)*stars[i].moveRad + stars[i].origX;
		stars[i].y = sin(stars[i].angle)*stars[i].moveRad + stars[i].origY;
		if (!rectRectCollision(0, 0, scrWidth, scrHeight, stars[i].x, stars[i].y, 1, 1))
		{
			stars[i].moveRad = 0;
		}
	}
}

int main(int argc, char *argv[])
{
	bool fullscreen = false;
	if (argc == 2)
	{
		if (strcmp(argv[1], "-full") == 0)
		{
			fullscreen = true;
		}
	}
	initWindow(1024, 768, fullscreen, "Space!!! :D");
	load();
	while (!keyInput.ESC)
	{
		clear();
		int i, r, g, b;
		for (i = 0; i < maxStars; i++)
		{
			r = randomNum(0, 255);
			g = randomNum(0, 255);
			b = randomNum(0, 255);
			setColour(r, g, b, 255);
			pixel(stars[i].x, stars[i].y);
		}
		update();
		logic();
		capFrameRate(60);
	}
	return 0;
}