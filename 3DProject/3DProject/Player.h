#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"
#include <vector>

using namespace glm;

class Player
{
private:
	vector<vector<float>> terrainHeights;

	Camera camera;
public:
	Player();
	~Player();

	void moveCameraPosition(vec3 movement);
	void mouseMovement(float xoffset, float yoffset);

	mat4 getView()const;
	vec3 getLookAtVector()const;
	float getSpeed()const;
	vec3 getUpVector()const;
	vec3 getPosition()const;
	void setHeights(vector<vector<float>> heights);

	float getHeightOfTerrain(float worldX, float worldZ)const;
};

#endif // !PLAYER_H
