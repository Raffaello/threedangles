#include <Cam.hpp>

Cam::Cam(const Vec4& pos, const Vec4& up): position(pos), up(up), znorm(0.0f, 0.0f, 1.0f)
{
}

Mat4 Cam::matrixView() noexcept
{
    // Camera Matrix
    Mat4 matCamRot = Mat4::createRotationY(yaw);
    lookAt = matCamRot * znorm;
    // TODO: i am computing twice the F,forward vector here and in matrixLookAt(), just compute once.
    Vec4 F = lookAt.normalize();
    Vec4 L = up.crossProd(F).normalize();
    lookAt = Mat4::createRotation(pitch, L) * lookAt;
    target = position + lookAt;
    Mat4 matCam = matrixLookAt();
    return matrixLookAtInverse(matCam);
}

void Cam::moveForward() noexcept
{
    position = position + lookAt * moveFactor;
}

void Cam::moveBackward() noexcept
{
    position = position - lookAt * moveFactor;
}

void Cam::turnLeft() noexcept
{
    yaw -= turnFactor;
}

void Cam::turnRight() noexcept
{
    yaw += turnFactor;
}

void Cam::turnUp() noexcept
{
    pitch += turnFactor;
}

void Cam::turnDown() noexcept
{
    pitch -= turnFactor;
}

//void Cam::rollCW() noexcept
//{
//    roll += rollFactor;
//}
//
//void Cam::rollCCW() noexcept
//{
//    roll -= rollFactor;
//}

Mat4 Cam::matrixLookAt() const
{
    /// @see https://www.3dgep.com/understanding-the-view-matrix/
    // this is the same as lookAt.normalize()
    Vec4 forward = (target - position).normalize();

    Vec4 t = (forward * up.dotProd(forward));
    Vec4 newUp = (up - t).normalize();
    Vec4 newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newUp.x; m.m[0][2] = forward.x; m.m[0][3] = position.x;
    m.m[1][0] = newRight.y; m.m[1][1] = newUp.y; m.m[1][2] = forward.y; m.m[1][3] = position.y;
    m.m[2][0] = newRight.z; m.m[2][1] = newUp.z; m.m[2][2] = forward.z; m.m[2][3] = position.z;
    m.m[3][0] = 0.0f;       m.m[3][1] = 0.0f;    m.m[3][2] = 0.0f;      m.m[3][3] = 1.0f;

    return m;
}

Mat4 Cam::matrixLookAtInverse(const Mat4& m) const
{
    Mat4 matrix;

    matrix.m[0][0] = m.m[0][0];
    matrix.m[1][0] = m.m[0][1];
    matrix.m[2][0] = m.m[0][2];
    matrix.m[3][0] = 0.0f;
    matrix.m[0][1] = m.m[1][0];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[3][1] = 0.0f;
    matrix.m[0][2] = m.m[2][0];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[3][2] = 0.0f;

    matrix.m[0][3] = -(m.m[0][3] * matrix.m[0][0] + m.m[1][3] * matrix.m[0][1] + m.m[2][3] * matrix.m[0][2]);
    matrix.m[1][3] = -(m.m[0][3] * matrix.m[1][0] + m.m[1][3] * matrix.m[1][1] + m.m[2][3] * matrix.m[1][2]);
    matrix.m[2][3] = -(m.m[0][3] * matrix.m[2][0] + m.m[1][3] * matrix.m[2][1] + m.m[2][3] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;

    return matrix;
}
