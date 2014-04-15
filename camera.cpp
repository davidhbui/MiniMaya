#include "camera.h"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

using namespace std;

static const float PI = 3.141592653589f;

Camera::Camera() {

    theta = 36;
    phi = 48;
    r = 3;
    viewingAngle = 45;

    float rTheta = (theta * PI/180);
    float rPhi = (phi * PI/180);
    position = glm::vec3(r * sinf(rTheta) * sinf(rPhi), r * cosf(rPhi), r * sinf(rPhi) * cosf(rTheta));

}

glm::vec3 Camera::getPos() {
    return position;
}

glm::vec3 Camera::getUp() {

//    Can't get Norm's workaround to work - this is my attempt (swapping the up vector when I'm on or near the y-axis)
//    if (position[0] <= 0.0001 && position[1] == r && position[2] <= 0.0001) {
//        return glm::vec3(0, 0, 1);
//    } else {
//        return glm::vec3(0, 1, 0);
//    }

    return glm::vec3(0, 1, 0);

}

void Camera::updatePos() {
    float rTheta = (theta * PI/180);
    float rPhi = (phi * PI/180);
    position = glm::vec3(r * sinf(rTheta) * sinf(rPhi), r * cosf(rPhi), r * sinf(rPhi) * cosf(rTheta));
}

void Camera::rotateWest() {
    theta-=3;
    updatePos();
}

void Camera::rotateEast() {
    theta+=3;
    updatePos();
}

void Camera::rotateNorth() {
    phi+=3;
    updatePos();
}

void Camera::rotateSouth() {
    phi-=3;
    updatePos();
}

void Camera::zoomIn() {
    r-=0.5;
    updatePos();
}

void Camera::zoomOut() {
    r+=0.5;
    updatePos();
}

float Camera::getViewingAngle() {
    return viewingAngle;
}

