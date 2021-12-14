#include <Cam.hpp>

Cam::Cam(const Vec4& verticalOffset, const Vec4& targetOffset, const Vec4& cam_position) : verticalOffset(verticalOffset), targetOffset(targetOffset), cam(cam_position)
{
    eye = verticalOffset + cam_position;
    target = eye + targetOffset;
    up = Vec4(0.0f, 1.0f, 0.0f);
    CreateLookAt();
}

void Cam::CreateLookAt() noexcept
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

void Cam::Update(const float delta_yaw, const float delta_pitch) noexcept
{
    yaw += delta_yaw;
    pitch += delta_pitch;
    // TODO: limit pitch to a min max pitch
    /*if (pitch < minPitch) {
        pitch = minPitch;
    }
    else if (pitch > maxPitch) {
        pitch = maxPitch;
    }*/

    // TODO: use Quaternion
    Mat4 matY = Mat4::createRotationY(yaw);
    Vec4 actualOffset = matY * targetOffset;
    Vec4 F = actualOffset.normalize();
    Vec4 L = up.crossProd(F).normalize();
    // Rotate actual offset about left for pitch
    


    // Cross products to calculate camera left, then camera up
    /*Vector3 cameraLeft = CrossProduct(tUp, cameraForward)
        cameraLeft.Normalize()
        Vector3 cameraUp = CrossProduct(cameraForward, cameraLeft)
        cameraUp.Normalize()*/


        // Rotate actual offset about left for pitch
    //Quaternion quatPitch = CreateFromAxisAngle(left, totalPitch)
    //    actualOffset = Transform(acutalOffset, quatPitch)
    //    // Now construct the camera matrix
    //    Vector3 eye = player.position + verticalOffset
    //    Vector3 target = eye + actualOffset
    //    // In this case we can just pass in world up, since we can never
    //    // rotate upside-down.
    //    cameraMatrix = CreateLookAt(eye, target, Vector3(0, 1, 0))

}
