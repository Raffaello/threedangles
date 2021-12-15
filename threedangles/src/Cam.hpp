#pragma once

#include <Vec4.hpp>
#include <Mat4.hpp>

/**
 * @brief FPS Camera
 * @link https://www.3dgep.com/understanding-the-view-matrix/
*/
/*
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
*/

class Cam
{
public:
    Cam() = delete;
    Cam(const Vec4& pos, const Vec4& up);

    Mat4 matrixView(const float yaw, const float pitch);

    Vec4 position;
    Vec4 lookAt;


    //float yaw = 0.0f;
    //float pitch = 0.0f;

protected:
    Mat4 matrixLookAt(const Vec4& pos, const Vec4& target, const Vec4& up) const;
    Mat4 matrixLookAtInverse(const Mat4& m) const;
    const Vec4 up; //(0.0f, 1.0f, 0.0f);
    Vec4 target; // (0.0f, 0.0f, 1.0f);

};
