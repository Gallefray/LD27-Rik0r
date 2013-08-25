#include "QL.h"
#include <string.h> // for memset

// #defines
#define maxAngle 360
#define minAngle 0
#define maxBullets 1500
#define maxEnemy 20

// Typedef structs & enums
typedef struct 
{
	bool active;
	// moving:
	int origX, origY, x, y;
	int moveR;
	float angle;
	int speed;
	// damage is directly in realtion to the size of the bullet.
} bullet;

typedef struct
{
	bool active; // only applicable for enemies :3
	// moving:
	int x, y, r; // x, y, radius
	int moveSpeed; // Move speed
	// gun:
	int gunX, gunY, gunRad; // gun x, y, radius
	float gunAngle; // gun angle
	float targetAngle; // For the enemy, the angle of the player in relation to the enemy :3
	bullet bullets[maxBullets]; // array for the bullets
	// health:
	float sheilds;
} entity;

typedef struct
{
	int x, y;
	int r;
	char *type;
} powerup;

// Function declarations, because -fpic.
int main();
void load();
void draw();
void controls();
void logic();
bullet *GetNextAvailableBullet(); // Thanks to quin :3
entity *GetNextAvailableEnemy(); // basically as above but for entities :3
void genEnemy();

// Variables
entity player;
int enemyCount;
entity enemies[maxEnemy];

bool fullscreen = false;

int main(int argc, char *argv[])
{
	// if ((strcmp(argv[1], "-full")) == 0)
	// {
	// 	fullscreen = true;
	// }
	initWindow(1024, 768, fullscreen, "Rik0r");
	load();
	while(keyInput.ESC != true)
	{
		clear();
		draw();
		update();
		controls();
		logic();
		capFrameRate(60); // 60 FPS
	}
	return 0;
}

void load()
{
	player.x = scrWidth/2;
	player.y = scrHeight/2;
	player.r = 30;
	player.moveSpeed = 4;
	player.gunAngle = 20.0;
	player.gunRad = 40;
	player.sheilds = 100;
	genEnemy();
}

void draw()
{
	setColour(0, 255, 0, 255);
	circle("line", player.x, player.y, player.r);
	setColour(255, 255, 255, 255);
	circle("line", player.gunX, player.gunY, 3);
	int i;
	// enemy drawing
	for (i = 0; i <= enemyCount; i++)
	{
		setColour(255, 255, 0, 255);
		circle("line", enemies[i].x, enemies[i].y, enemies[i].r);
		setColour(255, 255, 0, 255);
		circle("line", enemies[i].gunX, enemies[i].gunY, 3);
	}
	// player bullets :3
	setColour(200, 50, 0, 255);
	for (i = 0; i <= maxBullets; i++)
	{
		if (player.bullets[i].active == true)
		{
			pixel(player.bullets[i].x, player.bullets[i].y);
		}
	}
}

void controls() // Some input lag as such, but nothing game breaking yet :S
{
	// Player movement:
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
	// Player gun movement:
	if (keyInput.left)
	{
		player.gunAngle -= 6;
	}
	if (keyInput.right)
	{
		player.gunAngle += 6;
	}

	if (keyInput.spacebar)
	{
		bullet *newBullet = GetNextAvailableBullet(); 

		newBullet->active = true; 
		newBullet->origX = player.gunX;
		newBullet->origY = player.gunY;
		newBullet->moveR = 0;
		// rand stuff to allow variance in the bullets :3
		int chance = randomNum(0, 6);
		int variation = randomNum(1, 20); // 10 looks nice 
		switch (chance)
		{
			case 0:
				newBullet->angle = player.gunAngle;
				break;

			case 1 : case 3 : case 5:
				newBullet->angle = player.gunAngle;
				newBullet->angle += variation;
				break;

			case 2 : case 4 : case 6:
				newBullet->angle = player.gunAngle;
				newBullet->angle -= variation;
				break;

			default:
				break;
		}
		newBullet->speed = randomNum(2, 4);
		newBullet->x = cos(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origX;
		newBullet->y = sin(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origY;
	}

}

void logic()
{
	player.gunX = cos(degreesToRadians(player.gunAngle))*player.gunRad + player.x;
	player.gunY = sin(degreesToRadians(player.gunAngle))*player.gunRad + player.y;

	int i; // movinf the player's bullets :3
	int j;
	for (i = 0; i < maxBullets; i++)
	{
		if (player.bullets[i].active)
		{
			player.bullets[i].moveR++;
			player.bullets[i].x = cos(degreesToRadians(player.bullets[i].angle))*player.bullets[i].moveR + player.bullets[i].origX;
			player.bullets[i].y = sin(degreesToRadians(player.bullets[i].angle))*player.bullets[i].moveR + player.bullets[i].origY;
			if (player.bullets[i].y > scrHeight || player.bullets[i].y < 0 || player.bullets[i].x > scrWidth || player.bullets[i].x < 0)
			{
				player.bullets[i].active = false;
			}
		}
	}

	for (i = 0; i < maxEnemy; i++) // Enemies
	{
		if (enemies[i].active == true)
		{
			enemies[i].gunX = cos(degreesToRadians(enemies[i].gunAngle))*enemies[i].gunRad + enemies[i].x;
			enemies[i].gunY = sin(degreesToRadians(enemies[i].gunAngle))*enemies[i].gunRad + enemies[i].y;
			// player - enemy collision
			if ((circleCircleCollision(player.x, player.y, player.r, enemies[i].x, enemies[i].y, enemies[i].r)) == false)
			{
				float angle = giveAngle(enemies[i].x, enemies[i].y, player.x, player.y);
				player.x = cos(angle)*2 + player.x;
				player.y = sin(angle)*2 + player.y;
				player.sheilds--;
				enemies[i].sheilds--;
			}
			int xDelta, yDelta;
			for (j = 0; j < maxBullets; j++) // player bullets - enemy collision
			{
				if (player.bullets[j].active == true)
				{
					xDelta = (player.bullets[j].x - enemies[i].x);
					yDelta = (player.bullets[j].y - enemies[i].y);
					if (xDelta*xDelta+yDelta*yDelta <= enemies[i].r*enemies[i].r)
					{
						enemies[i].sheilds--;
						player.bullets[j].active = false;
					}
				}
			}
			// get the player's angle:
			enemies[i].targetAngle = giveAngle(player.x, player.y, enemies[i].x, enemies[i].y);
			// move the gun to the player's angle!
			if (!(enemies[i].gunAngle < enemies[i].targetAngle+4 && enemies[i].gunAngle > enemies[i].targetAngle-4))
			{
				enemies[i].gunAngle += 3.0;
			}
			// if (enemies[i].gunAngle > enemies[i].targetAngle)
			// {
			// 	enemies[i].gunAngle -= 6.0;
			// }
			// correct the angle :3
			if (radiansToDegrees(enemies[i].gunAngle) < 0)
			{
				enemies[i].gunAngle = degreesToRadians(maxAngle);
			}
			else if (radiansToDegrees(enemies[i].gunAngle) < maxAngle)
			{
				enemies[i].gunAngle = degreesToRadians(0);	
			}
		}
	}
}

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

entity *GetNextAvailableEnemy()
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

void genEnemy()
{
	entity *newEnemy = GetNextAvailableEnemy(); 

	newEnemy->active = true; 
	newEnemy->r = 30;
	newEnemy->x = randomNum(0, scrWidth - newEnemy->r);
	newEnemy->y = randomNum(0, scrHeight - newEnemy->r);
	newEnemy->moveSpeed = randomNum(4, 6);
	newEnemy->gunRad = 40;
	newEnemy->gunAngle = degreesToRadians(randomNum(0, 359));
	newEnemy->gunX = cos(newEnemy->gunAngle)*newEnemy->gunRad + newEnemy->x;
	newEnemy->gunY = sin(newEnemy->gunAngle)*newEnemy->gunRad + newEnemy->y;

	newEnemy->targetAngle = giveAngle(newEnemy->x, newEnemy->y, player.x, player.x);

}