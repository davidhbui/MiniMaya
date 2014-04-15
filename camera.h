#ifndef CAMERA_H
#define CAMERA_H
#include <QMatrix4x4>
#include <glm/glm.hpp>

class Camera {
public:

    float theta;
    float phi;
    float r;
    float viewingAngle;

    glm::vec3 position;

    void rotateNorth();
    void rotateSouth();
    void rotateEast();
    void rotateWest();

    void zoomIn();
    void zoomOut();

    Camera();

    glm::vec3 getPos();
    glm::vec3 getUp();

    void updatePos();

    float getViewingAngle();

};

#endif // CAMERA_H
