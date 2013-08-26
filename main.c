#include "QL.h"
#include <string.h> // for memset

// #defines -------------------------------------------------------------------- 
#define maxAngle 360
#define minAngle 0
#define maxBullets 1500
#define maxEnemy 20

// Typedef structs & enums -----------------------------------------------------
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
	int sheilds;
	// int r, g;
} entity;

typedef struct
{
	bool active;
	int x, y, r;
	int healthToGive; // self explanitory
} powerup;

// Function declarations, because -fpic -------------------------------------------------------------------- 
int main();
void load();
void draw();
void controls();
void logic();
bullet *GetNextAvailableBullet(); // Thanks to quin :3
entity *GetNextAvailableEnemy(); // basically as above but for entities :3
void genEnemy();
void genPowerup();
bool enemyAlive();
void titleDraw();
void titleUpdate();
void breakDraw();
void breakUpdate();

// Variables ------------------------------------------------------------------ 
int level; int levelScore = 0; int enemyScore = 0; int deathScore = 0;
entity player;
entity enemies[maxEnemy];
powerup OneUp;
img menuScreen;
img breakScreen;
bool fullscreen = false;

// -------------------------------------------------------------------- 
int main(int argc, char *argv[])
{
	menuScreen = loadImage("./res/menu.bmp", BMP);
	breakScreen = loadImage("./res/break.bmp", BMP);
	if ((strcmp(argv[2], "full")) == 0)
	{
		fullscreen = true;
	}
	initWindow(1024, 768, fullscreen, "Rik0r");
	printf("blop");
	load();
	printf("sqleeeerg");
	while(keyInput.ESC != true)
	{
		printf("quloppity quop");
		clear();
		if (level == 0)
		{
			printf("plop!");
			titleDraw();
			printf("bleaugh");
			titleUpdate();
		}
		else if (level > 0)
		{
			draw();
			update();
			controls();
			logic();
		}
		else if (level == -1) // This is the bit inbetween the levels
		{
			breakDraw();
			breakUpdate();
		}
		capFrameRate(60); // 60 FPS
	}
	return 0;
}

// loading variables -------------------------------------------------------------------- 
void load()
{
	player.x = scrWidth/2;
	player.y = scrHeight/2;
	player.r = 30;
	player.moveSpeed = 4;
	player.gunAngle = 20.0;
	player.gunRad = 40;
	player.sheilds = -1;
}

// boilerplate code -------------------------------------------------------------------- 
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
	newEnemy->moveSpeed = randomNum(3, 6);
	newEnemy->sheilds = -1;
	newEnemy->targetAngle = giveAngle(newEnemy->x, newEnemy->y, player.x, player.x);
}

void genPowerup()
{
	OneUp.active = true;
	OneUp.x = scrWidth/2;
	OneUp.y = scrHeight/2;
	OneUp.r = 30;
	OneUp.healthToGive = randomNum(50, 75);
}

bool enemyAlive()
{
	int i;
	for (i = 0; i < maxEnemy; i++)
	{
		if (enemies[i].active) 
		{
			return true;
		}
	}
	return false;
}

void loadNextLevel()
{
	level++;
	int i;
	for (i = 0; i <= level; i++)
	{
		genEnemy();
	}
}

// mainGame -------------------------------------------------------------------- 
void draw()
{
	setColour(abs(-player.sheilds), player.sheilds, 0, 255);
	circle("line", player.x, player.y, player.r);
	setColour(255, 255, 255, 255);
	circle("line", player.gunX, player.gunY, 3);
	int i;
	// enemy drawing
	for (i = 0; i <= maxEnemy; i++)
	{
		if (enemies[i].active == true)
		{
			setColour(abs(-enemies[i].sheilds), enemies[i].sheilds, 0, 255);
			circle("line", enemies[i].x, enemies[i].y, enemies[i].r);
		}
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
		int chance = randomNum(0, 8);
		int variation = randomNum(6, 20); // 10 looks nice 
		switch (chance)
		{
			case 0:
				newBullet->angle = player.gunAngle;
				break;

			case 1 : case 3 : case 5: case 7:
				newBullet->angle = player.gunAngle;
				newBullet->angle += variation;
				break;

			case 2 : case 4 : case 6: case 8:
				newBullet->angle = player.gunAngle;
				newBullet->angle -= variation;
				break;

			default:
				break;
		}
		newBullet->speed = randomNum(20, 40);
		newBullet->x = cos(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origX;
		newBullet->y = sin(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origY;
	}

}

void logic()
{
	player.gunX = cos(degreesToRadians(player.gunAngle))*player.gunRad + player.x;
	player.gunY = sin(degreesToRadians(player.gunAngle))*player.gunRad + player.y;

	int i; // moving the player's bullets :3
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

	// Player death
	if (player.sheilds < -256)
	{
		// Cut to commertial:
	}

	// Enemies
	for (i = 0; i < maxEnemy; i++) 
	{
		if (enemies[i].active == true)
		{
			// player - enemy collision
			if ((circleCircleCollision(player.x, player.y, player.r, enemies[i].x, enemies[i].y, enemies[i].r)) == false)
			{
				float angle = giveAngle(enemies[i].x, enemies[i].y, player.x, player.y);
				player.x = cos(angle)*2 + player.x;
				player.y = sin(angle)*2 + player.y;
				player.sheilds--; player.sheilds--;
				enemies[i].sheilds--;
			}
			// enemy death
			if (enemies[i].sheilds <= -256)
			{
				enemies[i].active = false;
				enemyScore++;
			}
			// player bullets - enemy collision
			int xDelta, yDelta;
			for (j = 0; j < maxBullets; j++)
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
			enemies[i].targetAngle = giveAngle(enemies[i].x, enemies[i].y, player.x, player.y);
			// Move the enemy to the player
			int *x = &enemies[i].x; // Pointers FTW!!!!
			int *y = &enemies[i].y;
			float targ = enemies[i].targetAngle;
			int speed = enemies[i].moveSpeed;
			*x = cos(targ) * speed/1.1 + *x; // sine and cosine are awesome :3
			*y = sin(targ) * speed/1.1 + *y;
		}
	}

	if (enemyAlive() == false)
	{
		level = -1;
		levelScore++;
		genPowerup();
	}
}

// title -------------------------------------------------------------------- 

void titleDraw()
{
	drawImage(menuScreen, 0, 0);
}

void titleUpdate()
{
	if (keyInput.p)
	{
		loadNextLevel();
	}
}

// break -------------------------------------------------------------------- 

void breakDraw()
{
	drawImage(breakScreen, 0, 0);
	setColour(abs(-player.sheilds), player.sheilds, 0, 255);
	circle("line", player.x, player.y, player.r);
	setColour(255, 255, 255, 255);
	circle("line", player.gunX, player.gunY, 3);
	// player bullets :3
	setColour(200, 50, 0, 255);
	int i;
	for (i = 0; i <= maxBullets; i++)
	{
		if (player.bullets[i].active == true)
		{
			pixel(player.bullets[i].x, player.bullets[i].y);
		}
	}
}

void breakUpdate()
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
		int chance = randomNum(0, 8);
		int variation = randomNum(6, 20); // 10 looks nice 
		switch (chance)
		{
			case 0:
				newBullet->angle = player.gunAngle;
				break;

			case 1 : case 3 : case 5: case 7:
				newBullet->angle = player.gunAngle;
				newBullet->angle += variation;
				break;

			case 2 : case 4 : case 6: case 8:
				newBullet->angle = player.gunAngle;
				newBullet->angle -= variation;
				break;

			default:
				break;
		}
		newBullet->speed = randomNum(20, 40);
		newBullet->x = cos(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origX;
		newBullet->y = sin(degreesToRadians(newBullet->angle))*newBullet->moveR + newBullet->origY;
	}
	if (keyInput.e)
	{
		level = levelScore;
		loadNextLevel();
	}
}