#pragma once

#include <Vec4.hpp>
#include <Mat4.hpp>

/**
 * @brief FPS Camera
 * @link https://www.3dgep.com/understanding-the-view-matrix/
*/
class Cam
{
    // Must "return" the View matrix
    // from a player position and "lookAt"
public:
    Cam(const Vec4& verticalOffset, const Vec4& targetOffset, const Vec4& cam_position);

    Vec4 cam;
    Vec4 up;
    
    void createLookAt() noexcept;
    void update(const float delta_yaw, const float delta_pitch) noexcept;
    void set(const float yaw, const float pitch) noexcept;

//protected:
    // Camera offset from player character position
    // For a y-up world, the vector would be (0, value, 0)
    Vec4 verticalOffset;
    // Target position offset from camera
    // For a z-forward world, vector would be (0, 0, value)
    Vec4 targetOffset;
    float yaw = 0.0f;
    float pitch = 0.0f;
    //float roll = 0.0f; // TODO: not implemented yet

    Mat4 matLookAt;

    float minPitch = 0.0f;
    float maxPitch = 0.0f;

    Vec4 eye;
    Vec4 target;
};
