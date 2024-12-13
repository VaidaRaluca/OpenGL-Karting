#pragma once
#include "Model.h"
enum ECarMovementType
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Car : public Model
{
public:
	Car(string const& path, bool bSmoothNormals, bool gamma = false);
	void SetRootTransf(glm::mat4 rootTransf);

	virtual void Draw(Shader& shader);

private:
	glm::mat4 _rootTransf;
};

