#include "QL.h"

#define maxBullets 10000
#define maxEnemy 20

typedef struct
{
	bool active;
	int x, y;
	float moveAng; int moveRad;
	int moveSpeed;
	int origX, origY;
} bullet;

typedef struct
{
	bool active;
	int x, y, r;
	int gunX, gunY, gunR;
	float gunAngle;
	int moveSpeed;
	int sheilds;
	bullet bullets[maxBullets];
} hero;

typedef struct
{
	bool active;
	int x, y, r;
	float targAngle, moveAngle;
	int moveSpeed;
	int sheilds;
} zomb;

typedef struct
{
	bool active;
	int x, y, r;
	int repair;
} powerUp;

int level = 1;
hero player;
zomb enemies[maxEnemy];

bullet *GetNextAvailableBullet()
{
	int i;
	for(i = 0; i < maxBullets; i++) 
	{ 
		if (player.bullets[i].active) 
		{
			continue;
		}
		return player.bullets + i;
	} 
	return NULL;
}

zomb *GetNextAvailableEnemy()
{
	int i;
	for(i = 0; i < maxEnemy; i++) 
	{ 
		if (enemies[i].active) 
		{
			continue;
		}
		return enemies + i;
	} 
	return NULL;
}

void load()
{
	player.x = scrWidth/2;
	player.y = scrHeight/2;
	player.r = 30;
	player.moveSpeed = 6;
	player.sheilds = -1;

	player.gunR = 40;
}

void genEnemy()
{
	zomb *newEnemy = GetNextAvailableEnemy();

	newEnemy->r = 30;
	newEnemy->x = randomNum(0+newEnemy->r, scrWidth-newEnemy->r);
	newEnemy->y = randomNum(0+newEnemy->r, scrHeight-newEnemy->r);
	newEnemy->sheilds = -1;
}

void draw()
{
	setColour(abs(-player.sheilds), player.sheilds, 0, 255);
	circle("line", player.x, player.y, player.r);
	setColour(255, 255, 255, 255);
	circle("line", player.gunX, player.gunY, 3.5);


	int i; 
	// bullets
	for (i = 0; i <= maxBullets; i++)
	{
		if (player.bullets[i].active == true)
		{
			setColour(255, 255, 0, 0);
			pixel(player.bullets[i].x, player.bullets[i].y);
		}
	}
}

void logic()
{
	player.gunX = cos(player.gunAngle)*player.gunR + player.x;
	player.gunY = sin(player.gunAngle)*player.gunR + player.y;

	int i;
	for (i = 0; i < maxBullets; i++)
	{
		if (player.bullets[i].active)
		{
			player.bullets[i].moveRad += player.bullets[i].moveSpeed;
			player.bullets[i].x = cos(player.bullets[i].moveAng)*player.bullets[i].moveRad + player.bullets[i].origX;
			player.bullets[i].y = sin(player.bullets[i].moveAng)*player.bullets[i].moveRad + player.bullets[i].origY;

			if (player.bullets[i].y > scrHeight || player.bullets[i].y < 0 || player.bullets[i].x > scrWidth || player.bullets[i].x < 0)
			{
				player.bullets[i].active = false;
			}
		}
	}

	for (i = 0; i < maxEnemy; i++)
	{
		if (enemies[i].active)
		{
			// Bullet - enemy collisions
			int j;
			for (j = 0; j < maxBullets; j++)
			{
				if (player.bullets[j].active)
				{
					if (pointCircleCollision(player.bullets[j].x, player.bullets[j].y, enemies[i].x, enemies[i].y, enemies[i].r))
					{
						enemies[i].sheilds--;
						player.bullets[j].active = false;
					}
				}
			}

			if (circleCircleCollision(player.x, player.y, player.r, enemies[i].x, enemies[i].y, enemies[i].r))
			{
				player.sheilds -= 2;
				enemies[i].sheilds--;
			}

			if (enemies[i].sheilds <= -256)
			{
				enemies[i].active = false;
			}

			enemies[i].targAngle = giveAngle(enemies[i].x, enemies[i].y, player.x, player.y);
			enemies[i].x = cos(enemies[i].targAngle)*enemies[i].moveSpeed + enemies[i].x;
			enemies[i].y = sin(enemies[i].targAngle)*enemies[i].moveSpeed + enemies[i].y;
		}
	}
}

void controls()
{
	if (keyInput.a)
	{
		player.x -= player.moveSpeed;
	}
	if (keyInput.d)
	{
		player.x += player.moveSpeed;
	}
	if (keyInput.w)
	{
		player.y -= player.moveSpeed;
	}
	if (keyInput.s)
	{
		player.y += player.moveSpeed;
	}

	if (keyInput.left)
	{
		player.gunAngle -= 0.1;
	}
	else if (keyInput.right)
	{
		player.gunAngle += 0.1;
	}

	if (keyInput.spacebar)
	{
		bullet *newBullet = GetNextAvailableBullet();
		newBullet->active = true;
		newBullet->origX = player.gunX;
		newBullet->origY = player.gunY;
		newBullet->moveRad = 0;

		int variation, chance;
		chance = randomNum(0, 8);
		variation = randomNum(5, 20);
		switch (chance)
		{
			case 0:
				newBullet->moveAng = player.gunAngle;
				break;
			case 1: case 3: case 5: case 7:
				newBullet->moveAng = player.gunAngle;
				newBullet->moveAng -= degreesToRadians(variation);
				break;
			case 2: case 4: case 6: case 8:
				newBullet->moveAng = player.gunAngle;
				newBullet->moveAng += degreesToRadians(variation);
				break;
		}
		newBullet->moveSpeed = randomNum(2, 4);
		newBullet->x = cos(newBullet->moveAng)*newBullet->moveRad + newBullet->origX;
		newBullet->y = sin(newBullet->moveAng)*newBullet->moveRad + newBullet->origY;
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
	initWindow(1024, 768, fullscreen, "test :3");
	load();
	img menuScreen = loadImage("./res/menu.bmp", BMP);
	while (!keyInput.ESC)
	{
		clear();
		if (level == -1) // death screen
		{

		}
		else if (level == 0 ) // menu
		{
			drawImage(menuScreen, 2, 2);
			update();
			capFrameRate(60);
		}
		else if (level > 0) // game
		{
			draw();
			update();
			controls();
			logic();
			capFrameRate(60);
		}
	}
	return 0;
}