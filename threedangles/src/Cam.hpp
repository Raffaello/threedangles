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

    // Camera offset from player character position
    // For a y-up world, the vector would be (0, value, 0)
    Vec4 verticalOffset;


    Vec4 cam;
    //Vec4 lookat;
    Vec4 up;
    // Target position offset from camera
    // For a z-forward world, vector would be (0, 0, value)
    Vec4 targetOffset;
    Vec4 eye;
    Vec4 target;

    float yaw = 0.0f;
    float pitch = 0.0f;

    Mat4 matCamRot;
    Mat4 matCam;
    Mat4 matView;

    void CreateLookAt(const Vec4& eye, const Vec4& target, const Vec4& Up) noexcept;
    void Update(const float delta_yaw, const float delta_pitch) noexcept;

private:
    Mat4 matLookAt;

    float minPitch = 0.0f;
    float maxPitch = 0.0f;
};
