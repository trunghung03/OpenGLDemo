#pragma once

#include "glad/glad.h"
#include "object.h"
#include "FastNoiseLite.h"

#include <random>

class Shape : public Object {
public:
    
    unsigned int VAO{};
    unsigned int size{};

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 randomPoint();
	void generatePlane(int, int, float);
    void Draw(Shader& shader) override;
};

FastNoiseLite noiseGen()
{
    FastNoiseLite noise;

    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(1.0f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalLacunarity(1.0f);
    noise.SetFractalGain(0.5f);

    noise.SetNoiseType(FastNoiseLite::NoiseType_Value);

    // 2. Set up Fractal (FBM) parameters
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(9);         // Matching 'terrainM' loop count
    noise.SetFractalLacunarity(2.0f);   // Matching p*2.0
    noise.SetFractalGain(0.5f);         // Matching b*=0.5

    // 4. Set up Domain Warping to approximate the custom distortion
    // This is the closest feature to the `/(1.0+dot(d,d))` logic.
    // It uses a second noise source to warp the input coordinates of the main noise.
    noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);

    // Adjust the amplitude of the warp. You will need to experiment with this
    // value to get the desired visual result.
    noise.SetDomainWarpAmp(30.0f);

    return noise;
}

inline glm::vec3 Shape::randomPoint()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, (int) indices.size()); // distribution in range [1, 6]

    glm::vec3 result = { vertices[dist(rng)], vertices[dist(rng) + 1],vertices[dist(rng) + 2] };

    return result;
}

inline void Shape::generatePlane(int width, int height, float resolution)
{
    
    if (resolution <= 0.0f) {
        resolution = 1.0f;
    }

    // Calculate the number of vertices needed along each axis.
    // We use integer math for robustness. Add 1 because a line of N segments has N+1 points.
    const int vertsPerRow = static_cast<int>(width / resolution) + 1;
    const int vertsPerCol = static_cast<int>(height / resolution) + 1;

    

    vertices.reserve(vertsPerRow * vertsPerCol * 3);

    FastNoiseLite noise = noiseGen();

    float scaling = 0.12f;

    for (int j = 0; j < vertsPerCol; ++j) {
        for (int i = 0; i < vertsPerRow; ++i) {
            // Calculate the actual world position for the vertex.
            float x = i * resolution;
            float z = j * resolution;

            // Get the noise value for this position to determine the height (y).
            float y = (noise.GetNoise(z * scaling, x * scaling) + 0.5f) * 7.5f;

            vertices.push_back(x);
            vertices.push_back(y); // Apply a multiplier to the height.
            vertices.push_back(z);
        }
        //std::cout << "Generation: " << j << "/" << vertsPerCol << "\r";
    }

    const int size = vertices.size();

    // --- Index Generation ---
    // A grid of WxH quads has (W*H*6) indices.
    const int numQuadsX = vertsPerRow - 1;
    const int numQuadsZ = vertsPerCol - 1;
    indices.reserve(numQuadsX * numQuadsZ * 6);

    // Loop through the quads, not the vertices.
    for (int z_idx = 0; z_idx < numQuadsZ; ++z_idx) {
        for (int x_idx = 0; x_idx < numQuadsX; ++x_idx) {
            // Calculate the indices of the four corners of the current quad.
            int topLeft = (z_idx * vertsPerRow) + x_idx;
            int topRight = topLeft + 1;
            int bottomLeft = ((z_idx + 1) * vertsPerRow) + x_idx;
            int bottomRight = bottomLeft + 1;

            // Create the first triangle of the quad.
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Create the second triangle of the quad.
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            
        }
    }

    std::vector<glm::vec3> normals(vertsPerRow * vertsPerCol, glm::vec3(0.0f, 0.0f, 0.0f));

    for (int i = 0; i < indices.size(); i += 3) {
        int i1 = indices[i];
        int i2 = indices[i + 1];
        int i3 = indices[i + 2];

        glm::vec3 v1{ vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2] };
        glm::vec3 v2{ vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2] };
        glm::vec3 v3{ vertices[i3 * 3], vertices[i3 * 3 + 1], vertices[i3 * 3 + 2] };

        glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);
        
        normals[i1] += normal;
        normals[i2] += normal;
        normals[i3] += normal;
    }

    for (int i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(normals[i]);
        vertices.push_back(normals[i].x);
        vertices.push_back(normals[i].y);
        vertices.push_back(normals[i].z);
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(float) * size));
    glEnableVertexAttribArray(1);

    this->VAO = VAO;
    this->size = static_cast<unsigned int>(indices.size()); // ADD THIS LINE
}

inline void Shape::Draw(Shader& shader)
{
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    shader.setValue("material.color_diffuse", color);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
}