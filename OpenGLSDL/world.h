#pragma once

#include <list>

#include "object.h"

class World {
public:
	std::list<std::unique_ptr<Object>> objects{};

	void addObject(std::unique_ptr<Object>, glm::mat4);
	void Draw(Shader&);
};

inline void World::addObject(std::unique_ptr<Object> obj, glm::mat4 location)
{
	obj->location = location;
	objects.push_back(std::move(obj));
}

inline void World::Draw(Shader& shader)
{
	for (auto&& obj : objects) {
		shader.setValue("model", obj->location);
		obj->Draw(shader);
	}
}