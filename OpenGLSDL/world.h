#pragma once

#include <list>

#include "object.h"

class World {
public:
	std::list<std::unique_ptr<Object>> objects{};

	void addObject(std::unique_ptr<Object>, glm::mat4);
	void addObject(std::unique_ptr<Model>, std::vector<glm::mat4>);
	void Draw(Shader&);
	void Draw(Shader&, int);
};

inline void World::addObject(std::unique_ptr<Object> obj, glm::mat4 location)
{
	obj->location = location;
	objects.push_back(std::move(obj));
}

inline void World::addObject(std::unique_ptr<Model> obj, std::vector<glm::mat4> locations)
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, locations.size() * sizeof(glm::mat4), locations.data(), GL_STATIC_DRAW);

    for (unsigned int i = 0; i < obj->meshes.size(); i++)
    {
        unsigned int VAO = obj->meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    obj->locations = locations;
    objects.push_back(std::move(obj));
}

inline void World::Draw(Shader& shader)
{
	for (auto&& obj : objects) {

        if (obj->locations.size() > 0) {
            obj->Draw(shader);
        }

		shader.setValue("model", obj->location);
		obj->Draw(shader);
	}
}