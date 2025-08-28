#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Object {
public:
	glm::mat4 location{};
	std::vector<glm::mat4> locations{};
	int instanceNo;

	virtual void Draw(Shader&) = 0;
};