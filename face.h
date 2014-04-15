#ifndef FACE_H
#define FACE_H
#include "halfedge.h"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <QListWidgetItem>

class Face : public QListWidgetItem {
public:
    Face(int);
    Face(HalfEdge*, int);

    HalfEdge* halfEdge;
    glm::vec3 color;
    int id;
    bool isCurrent;
    bool isHovered;

    int numSides();
    void setID(int);

    glm::vec3 getNormal();

};

#endif // FACE_H
