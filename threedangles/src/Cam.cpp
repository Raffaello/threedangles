#include <Cam.hpp>
/*
Cam::Cam(const Vec4& verticalOffset, const Vec4& targetOffset, const Vec4& cam_position) : verticalOffset(verticalOffset), targetOffset(targetOffset), cam(cam_position)
{
    eye = verticalOffset + cam_position;
    target = eye + targetOffset;
    up = Vec4(0.0f, 1.0f, 0.0f);
    createLookAt();
}

void Cam::createLookAt() noexcept
{
    Vec4 F = (target - eye).normalize();
    Vec4 L = up.crossProd(F).normalize();
    Vec4 U = F.crossProd(L);
    Vec4 T(-L.dotProd(eye), -U.dotProd(eye), -F.dotProd(eye));

    //// Create and return look-at matrix from F, L, U, and T
    matLookAt.m[0][0] = L.x; matLookAt.m[0][1] = U.x; matLookAt.m[0][2] = F.x; matLookAt.m[0][3] = 0.0f;
    matLookAt.m[1][0] = L.y; matLookAt.m[1][1] = U.y; matLookAt.m[1][2] = F.y; matLookAt.m[1][3] = 0.0f;
    matLookAt.m[2][0] = L.z; matLookAt.m[2][1] = U.z; matLookAt.m[2][2] = F.z; matLookAt.m[2][3] = 0.0f;
    matLookAt.m[3][0] = T.x; matLookAt.m[3][1] = T.y; matLookAt.m[3][2] = T.z; matLookAt.m[3][3] = 1.0f;
}

void Cam::update(const float delta_yaw, const float delta_pitch) noexcept
{
    //yaw += delta_yaw;
    //pitch += delta_pitch;
    //// TODO: limit pitch to a min max pitch
    ///*if (pitch < minPitch) {
    //    pitch = minPitch;
    //}
    //else if (pitch > maxPitch) {
    //    pitch = maxPitch;
    //}* /

    //// TODO: use Quaternion
    //Mat4 matY = Mat4::createRotationY(yaw);
    //Vec4 actualOffset = matY * targetOffset;
    //Vec4 F = actualOffset.normalize();
    //Vec4 L = up.crossProd(F).normalize();
    //// Rotate actual offset about left for pitch
    //actualOffset = Mat4::createRotation(pitch, L) * actualOffset;
    //eye = cam + verticalOffset;
    //target = eye + actualOffset;
    //createLookAt();
    set(yaw + delta_yaw, pitch + delta_pitch);
}

void Cam::set(const float yaw, const float pitch) noexcept
{
    this->yaw = yaw;
    this->pitch = pitch;

    // TODO: limit pitch to a min max pitch
    /*if (pitch < minPitch) {
        pitch = minPitch;
    }
    else if (pitch > maxPitch) {
        pitch = maxPitch;
    }* /

    // TODO: use Quaternion
    Mat4 matY = Mat4::createRotationY(yaw);
    Vec4 actualOffset = matY * targetOffset;
    Vec4 F = actualOffset.normalize();
    Vec4 L = up.crossProd(F).normalize();
    // Rotate actual offset about left for pitch
    actualOffset = Mat4::createRotation(pitch, L) * actualOffset;
    eye = cam + verticalOffset;
    target = eye + actualOffset;
    createLookAt();
}
*/

// ----------------------------------------------------------------

Cam::Cam(const Vec4& pos, const Vec4& up): position(pos), up(up)
{
}

Mat4 Cam::matrixLookAt(const Vec4& pos, const Vec4& target, const Vec4& up) const
{
    /// @see https://www.3dgep.com/understanding-the-view-matrix/
    Vec4 forward = (target - pos).normalize();

    Vec4 t = (forward * up.dotProd(forward));
    Vec4 newUp = (up - t).normalize();
    Vec4 newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newUp.x; m.m[0][2] = forward.x; m.m[0][3] = pos.x;
    m.m[1][0] = newRight.y; m.m[1][1] = newUp.y; m.m[1][2] = forward.y; m.m[1][3] = pos.y;
    m.m[2][0] = newRight.z; m.m[2][1] = newUp.z; m.m[2][2] = forward.z; m.m[2][3] = pos.z;
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

Mat4 Cam::matrixView(const float yaw, const float pitch)
{
    // Camera Matrix
    Mat4 matCamRot = Mat4::createRotationY(yaw);
    // TODO: this doesn't look right
    target.x = 0.0f; target.y = 0.0f; target.z = 1.0f;
    lookAt = matCamRot * target;
    Vec4 F = lookAt.normalize();
    Vec4 L = up.crossProd(F).normalize();
    lookAt = Mat4::createRotation(pitch, L) * lookAt;
    target = position + lookAt;
    Mat4 matCam = matrixLookAt(position, target, up);
    return matrixLookAtInverse(matCam);
}

