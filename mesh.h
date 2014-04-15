#ifndef MESH_H
#define MESH_H
#include "face.h"
#include "vertex.h"
#include "halfedge.h"
#include <vector>


using namespace std;

class Mesh {
public:
    Mesh();

    vector<Face*>* faces;
    vector<HalfEdge*>* halfEdges;
    vector<Vertex*>* vertices;

    void split(Face*);
    void addEdge(Face*, Face*, Vertex*);
    void addVertex(HalfEdge*);
    void deleteVertex(HalfEdge*);

    int numTriangles;

    void updateNumTriangles();
    void makePlanar();

};

#endif // MESH_H
