#include "firefly.h"
#include <cstdlib>

FireflySystem::FireflySystem(int count){
    fireflies.reserve(count);

    for(int i = 0; i < count; i++){
        Firefly f;
        f.pos = glm::vec3((randf() - 0.5f) * 200.0f, 2.0f + randf() * 3.0f, (randf() - 0.5f) * 200.f);
        f.vel = glm::vec3(0.0f);

        //warm glow
        f.color = glm::vec3(1.0f,0.9f, 0.5f);

        //radius controls
        f.radius = 6.0f;
    }
}

float FireflySystem::randf() {
    return rand() / (float)RAND_MAX;
}

void FireflySystem::update(float deltaTime) {
    const float jitter = 0.5f;
    const float damping = 0.98f;

    for (auto& f : fireflies) {
        f.vel += glm::vec3((randf() - 0.5f) * jitter, (randf() - 0.5f) * jitter, (randf() - 0.5f) * jitter) * deltaTime;

        f.vel *= damping;
        f.pos += f.vel * deltaTime;
    }
}