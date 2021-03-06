
#include "Tank.h"
#include <cmath>
#include <iostream>
#include "../Assets/Assets.h"
#include "../Logic/Game.h"

const float Tank::MAX_SPEED = 0.3;
const float Tank::SPEED_DIFFRENTIAL = 0.02;
const float Tank::TURN_DIFFRENTIAL = 10;
const float Tank::BRAKING_DIFFRENTIAL = 0.01;

Tank::Tank() {
	speed = 0;
	position = {0, 0, 0};
	angle = {0, 0, 0};

	destroyed = false;

	movement = BRAKE;

	towerQuadric = gluNewQuadric();
	gluQuadricNormals(towerQuadric, GLU_SMOOTH);
	gluQuadricTexture(towerQuadric, GL_TRUE);

	collisionRadius = 1.5;
}


void Tank::destroy() {destroyed = true; }

bool Tank::isDestroyed() { return destroyed; }

Tank::Tank(Vector3D beginPosition) : Tank()
{

	position = beginPosition;

}


Missle * Tank::shoot()
{

	Vector3D misslePos = { position.x, position.y + 1.25f, position.z };
	return new Missle(2, misslePos, angle);
}

void Tank::move(int direction)
{
	if (direction == 1)
	{
		if (movement == MOVE_BACK)
			movement = BRAKE;
		else
			if (movement == BRAKE)
				movement = MOVE_FRONT;
	}
	else
	if (direction == -1)
	{
		if (movement == BRAKE)
			movement = MOVE_BACK;
		else
		if (movement == MOVE_FRONT)
			movement = BRAKE;
	}

	
}
void Tank::turn(int direction)
{
	if(destroyed)
		return;
	
	angle.y = (angle.y - direction*TURN_DIFFRENTIAL*speed*movement);
	
	while (angle.y > 360) angle.y -= 360;
	while (angle.y < 0) angle.y += 360;
	
}

void Tank::bounce(SceneObject * so)
{
	GLfloat differenceX;
	GLfloat differenceZ;
	if (so!= nullptr) {
		 differenceX = position.x - so->position.x;
		 differenceZ = position.z - so->position.z;
	}
	else
	{
		differenceX = position.x ;
		differenceZ = position.z ;
	}
	double desiredAngle = 0;

	if(differenceX==0)
		differenceX=1;

	if(differenceX>=0)
		desiredAngle = std::atan(-(differenceZ/differenceX))*(180 / 3.1415);
	else {
		desiredAngle = std::atan(-(differenceZ/differenceX))*(180 / 3.1415);
		desiredAngle+=180;
	}

	if(so == nullptr) desiredAngle+=180;

	while (desiredAngle > 360) desiredAngle -= 360;
	while (desiredAngle < 0) desiredAngle += 360;
	bounceAngle = desiredAngle;

	bounceSpeed = MAX_SPEED/2;
	speed=0;


}

GLfloat Tank::getSpeed() { return speed;}
GLfloat Tank::getAngle() { return angle.y;}



void Tank::update()
{
	if(destroyed)
		return;

	if (movement == BRAKE)
	{
		if (speed - BRAKING_DIFFRENTIAL > 0) speed -= BRAKING_DIFFRENTIAL;
		else
			if (speed > 0) speed = 0;
			else
				if (speed + BRAKING_DIFFRENTIAL < 0) speed += BRAKING_DIFFRENTIAL;
				else
					if (speed < 0) speed = 0;
	}
	else
	{
		if (speed + SPEED_DIFFRENTIAL*movement < MAX_SPEED &&
			speed + SPEED_DIFFRENTIAL*movement > -MAX_SPEED)
		{
			speed += SPEED_DIFFRENTIAL*movement;
		}


	}

	position.x = position.x + speed*cos(angle.y* (3.1415 / 180.0));
	position.z = position.z - speed*sin(angle.y* (3.1415 / 180.0));


	if (bounceSpeed - BRAKING_DIFFRENTIAL > 0) bounceSpeed -= BRAKING_DIFFRENTIAL;
	else
	if (bounceSpeed > 0) bounceSpeed = 0;
	else
	if (bounceSpeed + BRAKING_DIFFRENTIAL < 0) bounceSpeed += BRAKING_DIFFRENTIAL;
	else
	if (bounceSpeed < 0) bounceSpeed = 0;

	position.x = position.x + bounceSpeed*cos(bounceAngle* (3.1415 / 180.0));
	position.z = position.z - bounceSpeed*sin(bounceAngle* (3.1415 / 180.0));

}

void Tank::draw()
{
	if(isWired())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();


	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle.y, 0, 1, 0);

	glEnable(GL_TEXTURE_2D);


	if(destroyed)
		glBindTexture(GL_TEXTURE_2D, Assets::getAssets().destroyedTankTexture);
	else
		glBindTexture(GL_TEXTURE_2D, Assets::getAssets().tankTexture);


	//tower
	glPushMatrix();
	glTranslatef(0, 1.25, 0);
	gluSphere(towerQuadric, 0.7, 10, 10);
	glPopMatrix();


	//gun
	glPushMatrix();
	glTranslatef(0, 1.6, 0);
	glRotatef(90, 0, 1, 0);
	gluCylinder(towerQuadric, 0.1, 0.1, 1, 10, 1);
	glPopMatrix();


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, Tank::BOX_TEXTURE);
	glVertexPointer(3, GL_FLOAT, 0, Tank::BOX_VERTEX);

	glPushMatrix();
	glTranslatef(0.0, 0.75, 0.0);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_BYTE, Tank::BOX_INDICES);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);

	if(destroyed)
		glBindTexture(GL_TEXTURE_2D, Assets::getAssets().destroyedTankTexture);
	else
	glBindTexture(GL_TEXTURE_2D, Assets::getAssets().panzerCaterpillarTexture);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, Tank::CATERPILLAR_TEXTURE);
	glVertexPointer(3, GL_FLOAT, 0, Tank::CATERPILLAR_VERTEX);

	glPushMatrix();
	glTranslatef(-1.5, 0.5, 1.0);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_BYTE, Tank::CATERPILLAR_INDICES);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 0.5, -0.7);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_BYTE, Tank::CATERPILLAR_INDICES);
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

}
Tank::~Tank() {}






const GLfloat Tank::CATERPILLAR_VERTEX[] = {

	0.0,	0.0,	 0.0,	//0
	0.5,	-0.5,	 0.0,	//1
	2.5,	-0.5,	 0.0,	//2
	3.0,	0.0,	0.0,	//3
	0.0,	0.0,	-0.3,	//4
	0.5,	-0.5,	-0.3,	//5
	3.0,	0.0,	-0.3,	//6
	2.5,	-0.5,	-0.3,	//7

	0.0,	0.0,	-0.3,	//8
	0.5,	-0.5,	-0.3,	//9
	2.5,	-0.5,	-0.3,	//10
	3.0,	0.0,	-0.3,	//11
};

const GLfloat Tank::CATERPILLAR_TEXTURE[] = {

		1.0/10.0,	0.0/2.0,	//0
		2.0/10.0,	1.0/2.0,	//1
		8.0/10.0,	1.0/2.0,	//2
		9.0/10.0,	0.0/2.0,	//3
		0.0/10.0,	1.0/2.0,	//4
		1.0/10.0,	2.0/2.0,	//5
		10.0/10.0,	1.0/2.0,	//6
		9.0/10.0,	2.0/2.0,	//7
		1.0/10.0,	0.0/2.0,	//8
		2.0/10.0,	1.0/2.0,	//9
		8.0/10.0,	1.0/2.0,	//10
		9.0/10.0,	0.0/2.0,	//11



};

const GLubyte Tank::CATERPILLAR_INDICES[] = {
	0,	1,	3,
	1,	2,	3,

	4,  5,  0,
	5,  1,  0,

	3,  2,  6,
	2,	7,	6,

	8,	9,	10,
	8,	10, 11
};

const GLfloat Tank::BOX_VERTEX[] =
{
	-1.5,	0.25,	-1,	//0
	-1.5,	-0.25,	-1,	//1
	-1.5,	-0.25,	1,	//2
	-1.5,	0.25,	1,	//3
	1.5,	0.25,	1,	//4
	1.5,	-0.25,	1,	//5
	1.5,	-0.25,	-1,	//6
	1.5,	0.25,	-1,	//7
};

const GLfloat Tank::BOX_TEXTURE[] =
		{
				1.0/6.0, 7.0/8.0,
				0.0/6.0, 8.0/8.0,
				6.0/6.0, 8.0/8.0,
				5.0/6.0, 7.0/8.0,
				5.0/6.0, 1.0/8.0,
				6.0/6.0, 0.0/8.0,
				0.0/6.0, 0.0/8.0,
				1.0/6.0, 1.0/8.0,
		};


const GLubyte Tank::BOX_INDICES[] =
{
	0,	1,	2,
	0,	2,	3,

	3,	2,	4,
	2,	5,	4,

	5,  6,	4,
	4,  6,  7,

	6,	1,	0,
	6,  0, 7,

	3,	4,	7,
	3,	7,	0,

};