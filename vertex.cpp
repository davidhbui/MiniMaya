#include "vertex.h"

Vertex::Vertex(float newX, float newY, float newZ, float newW, int newID) : QListWidgetItem(){
    X = newX;
    Y = newY;
    Z = newZ;
    W = newW;
    id = newID;
    setText("Vertex " + QString::number(id));
    isCurrent = false;
    isEndPoint = false;
}

Vertex::Vertex(const Vertex &v2) : QListWidgetItem() {
    X = v2.X;
    Y = v2.Y;
    Z = v2.Z;
    W = v2.X;
    id = 10 * v2.id;
    setText("Vertex " + QString::number(id));
}

void Vertex::setID(int newID) {
    id = newID;
    setText("Vertex " + QString::number(id));
}
