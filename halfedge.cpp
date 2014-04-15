#include "face.h"
#include "vertex.h"
#include "halfedge.h"

HalfEdge::HalfEdge(Vertex* v, int newID) : QListWidgetItem() {
    vert = v;
    id = newID;
    setText("HalfEdge " + QString::number(id));
    isCurrent = false;
}

HalfEdge::HalfEdge(int newID): QListWidgetItem() {
    id = newID;
    setText("HalfEdge " + QString::number(id));
    isCurrent = false;
}

//HalfEdge::~HalfEdge() {
//    if (vert) {
//        delete vert;
//    }
//    if (face) {
//        delete face;
//    }
//}

bool HalfEdge::operator ==(const HalfEdge &h) const{
    return (id == h.id);
}

bool HalfEdge::operator !=(const HalfEdge &h) const{
    return (id != h.id);
}

void HalfEdge::setID(int newID) {
    id = newID;
    setText("HalfEdge " + QString::number(id));
}
