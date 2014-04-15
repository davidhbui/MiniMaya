#include "face.h"

Face::Face(HalfEdge* h, int newID) : QListWidgetItem() {
    halfEdge = h;
    color = glm::vec3(1.0);
    id = newID;
    setText("Face " + QString::number(id));
    isCurrent = false;
    isHovered = false;

}

Face::Face(int newID) : QListWidgetItem(){
    color = glm::vec3(1.0);
    id = newID;
    setText("Face " + QString::number(id));
    isCurrent = false;
    isHovered = false;
}

void Face::setID(int newID) {
    id = newID;
    setText("Face " + QString::number(id));
}

int Face::numSides() {
    if (!halfEdge) {
        printf("NO HALFEDGES!\n");
        return 0;
    }
    int count = 0;
    HalfEdge* he = halfEdge;
    do {
        count++;
        he = he->next;
    } while (he!= halfEdge);

    return count;
}

glm::vec3 Face::getNormal() {

    // Get the normal
    Vertex* firstVert  = halfEdge->vert;
    Vertex* secondVert = halfEdge->next->vert;
    Vertex* thirdVert  = halfEdge->next->next->vert;

    glm::vec3 firstEdge = glm::vec3((secondVert->X - firstVert->X), (secondVert->Y - firstVert->Y), (secondVert->Z - firstVert->Z));
    glm::vec3 secondEdge = glm::vec3((thirdVert->X - secondVert->X), (thirdVert->Y - secondVert->Y), (thirdVert->Z - secondVert->Z));

    glm::vec3 C = glm::cross(firstEdge, secondEdge);
    return C;

}
