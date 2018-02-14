#include "Player.h"

Player::Player()
{

}

Player::~Player()
{

}

mat4 Player::getView()const
{
	return this->camera.getView();
}

vec3 Player::getLookAtVector()const
{
	return this->camera.getLookAtVector();
}

float Player::getSpeed()const
{
	return this->camera.getSpeed();
}

void Player::moveCameraPosition(vec3 movement)
{
	this->camera.moveCameraPosition(movement);
}

vec3 Player::getUpVector()const
{
	return this->camera.getUpVector();
}

void Player::mouseMovement(float xoffset, float yoffset)
{
	this->camera.mouseMovement(xoffset, yoffset);
}

vec3 Player::getPosition()const
{
	return this->camera.getPosition();
}

void Player::setHeights(vector<vector<float>> heights)
{
	this->terrainHeights = heights;
}

float Player::getHeightOfTerrain(float worldX, float worldZ)const
{
	float terrainX = worldX - this->camera.getPosition().x;
	float terrainZ = worldZ - this->camera.getPosition().z;

	float gridSquareSize = this->terrainHeights.size() / this->terrainHeights.size();

	return 0.0f;
}