#pragma once
#include <vector>
#include <glm/glm.hpp>


struct Firefly {
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 color;
    float radius;
};

class FireflySystem {
public:
    FireflySystem(int count);

    void update(float deltaTime);
    const std::vector<Firefly>& getFireflies() const { return fireflies; }

private:
    std::vector<Firefly> fireflies;

    float randf();
    
};
