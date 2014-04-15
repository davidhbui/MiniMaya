#include "face.h"
#include "vertex.h"
#ifndef HALFEDGE_H
#define HALFEDGE_H
#include <QListWidgetItem>

using namespace std;

class Face;

class HalfEdge : public QListWidgetItem {
public:
    HalfEdge(Vertex* v, int newID);
    HalfEdge(int newID);
//    ~HalfEdge();

    int id;

    Face* face;
    Vertex* vert;
    HalfEdge* next;
    HalfEdge* sym;

    bool operator==(const HalfEdge &h) const;
    bool operator!=(const HalfEdge &h) const;

    bool isCurrent;
    void setID(int);

};

#endif // HALFEDGE_H
