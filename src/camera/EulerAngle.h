#ifndef EULERANGLE_H
#define EULERANGLE_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class EulerAngle{
    public:
        EulerAngle(float,float, float);
        EulerAngle();
        ~EulerAngle();

        glm::vec3 toVector();
        void toAngles(glm::vec3 direction);
        void constrain();
        const float PI = 3.141592;

        float getPitch();
        float getYaw();
        float getRoll();
        void setPitch(float);
        void setYaw(float);
        void setRoll(float);

        private:
            float pitch;
            float yaw;
            float roll;
};
#endif