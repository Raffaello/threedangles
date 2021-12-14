#pragma once

#include <Vec3d.hpp>
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

    Vec3d cam;
    Vec3d lookat;
    Vec3d up;
    Vec3d target;

    float yaw;
    float pitch;

    Mat4 matCamRot;
    Mat4 matCam;
    Mat4 matView;
};
