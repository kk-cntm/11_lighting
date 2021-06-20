#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraArgs {
    glm::vec3 position;
    glm::vec3 direction;
    float yaw;
    float pitch;
    float deltaTime;
};

class Camera {
    glm::vec3 m_position;
    glm::vec3 m_direction;
    float m_yaw;
    float m_pitch;
    float m_deltaTime;
    
    static const glm::vec3 s_up;
    static constexpr float s_moveNormalizer = 2.5f;
    
    void calculateDirection();
    
public:
    Camera(CameraArgs args);
    glm::mat4 view();
    void addYaw(float yaw);
    void addPitch(float pitch);
    void setDeltaTime(float deltaTime);
    void moveForward();
    void moveBackward();
    void moveRight();
    void moveLeft();
    void moveForwardLeft();
    void moveForwardRight();
    void moveBackwardLeft();
    void moveBackwardRight();
};
