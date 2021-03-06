#pragma once

#include <Vec4.hpp>
#include <Mat4.hpp>

/**
 * @brief FPS Camera
 * @link https://www.3dgep.com/understanding-the-view-matrix/
 * @todo:
   - roll factor
   - move to Engine?
*/
class Cam
{
public:
    Cam() = delete;
    Cam(const Vec4& pos, const Vec4& up);

    Mat4 matrixView() noexcept;

    void moveForward() noexcept;
    void moveBackward() noexcept;
    void turnLeft() noexcept;
    void turnRight() noexcept;
    void turnUp() noexcept;
    void turnDown() noexcept;
    //void rollCW() noexcept;
    //void rollCCW() noexcept;

    Vec4 position;
    Vec4 lookAt;

    float yaw = 0.0f;
    float pitch = 0.0f;
    //float roll = 0.0f;
    float moveFactor = 0.5f;
    float turnFactor = 0.1f;
    //float rollFactor = 0.2f;

private:
    Mat4 matrixLookAt() const;
    Mat4 matrixLookAtInverse(const Mat4& m) const;
    const Vec4 up;
    const Vec4 znorm = Vec4(0.0f, 0.0f, 1.0f);
    Vec4 target;
};
