#include "mesh.h"
#include <stdio.h>

// V0: (+x, +y, +z)
// V1: (-x, +y, +z)
// V2: (-x, -y, +z)
// V3: (+x, -y, +z)

// V4: (+x, -y, -z)
// V5: (+x, +y, -z)
// V6: (-x, +y, -z)
// V7: (-x, -y, -z)

// Face 0: +z --> (V0, V1, V2, V3) (red)
// Face 1: +x --> (V0, V3, V4, V5) (yellow)
// Face 2: +y --> (V0, V5, V6, V1) (blue)
// Face 3: -x --> (V1, V6, V7, V2) (yellow)
// Face 4: -y --> (V7, V4, V3, V2) (blue)
// Face 5: -z --> (V4, V7, V6, V5) (red)

using namespace std;

Mesh::Mesh() {

    Vertex* v0 = new Vertex( 0.5,  0.5,  0.5, 1, 0);
    Vertex* v1 = new Vertex(-0.5,  0.5,  0.5, 1, 1);
    Vertex* v2 = new Vertex(-0.5, -0.5,  0.5, 1, 2);
    Vertex* v3 = new Vertex( 0.5, -0.5,  0.5, 1, 3);
    Vertex* v4 = new Vertex( 0.5, -0.5, -0.5, 1, 4);
    Vertex* v5 = new Vertex( 0.5,  0.5, -0.5, 1, 5);
    Vertex* v6 = new Vertex(-0.5,  0.5, -0.5, 1, 6);
    Vertex* v7 = new Vertex(-0.5, -0.5, -0.5, 1, 7);

    vector<HalfEdge*>* hez = new vector<HalfEdge*>;
    vector<Vertex*>* vertz = new vector<Vertex*>;

    vertz->push_back(v0);
    vertz->push_back(v1);
    vertz->push_back(v2);
    vertz->push_back(v3);
    vertz->push_back(v4);
    vertz->push_back(v5);
    vertz->push_back(v6);
    vertz->push_back(v7);

    HalfEdge* h00 = new HalfEdge(v0, 0);
    HalfEdge* h01 = new HalfEdge(v1, 1);
    HalfEdge* h02 = new HalfEdge(v2, 2);
    HalfEdge* h03 = new HalfEdge(v3, 3);
    h00->next = h01;
    h01->next = h02;
    h02->next = h03;
    h03->next = h00;

    HalfEdge* h10 = new HalfEdge(v0, 4);
    HalfEdge* h11 = new HalfEdge(v3, 5);
    HalfEdge* h12 = new HalfEdge(v4, 6);
    HalfEdge* h13 = new HalfEdge(v5, 7);
    h10->next = h11;
    h11->next = h12;
    h12->next = h13;
    h13->next = h10;

    HalfEdge* h20 = new HalfEdge(v0, 8);
    HalfEdge* h21 = new HalfEdge(v5, 9);
    HalfEdge* h22 = new HalfEdge(v6, 10);
    HalfEdge* h23 = new HalfEdge(v1, 11);
    h20->next = h21;
    h21->next = h22;
    h22->next = h23;
    h23->next = h20;

    HalfEdge* h30 = new HalfEdge(v1, 12);
    HalfEdge* h31 = new HalfEdge(v6, 13);
    HalfEdge* h32 = new HalfEdge(v7, 14);
    HalfEdge* h33 = new HalfEdge(v2, 15);
    h30->next = h31;
    h31->next = h32;
    h32->next = h33;
    h33->next = h30;

    HalfEdge* h40 = new HalfEdge(v7, 16);
    HalfEdge* h41 = new HalfEdge(v4, 17);
    HalfEdge* h42 = new HalfEdge(v3, 18);
    HalfEdge* h43 = new HalfEdge(v2, 19);
    h40->next = h41;
    h41->next = h42;
    h42->next = h43;
    h43->next = h40;

    HalfEdge* h50 = new HalfEdge(v4, 20);
    HalfEdge* h51 = new HalfEdge(v7, 21);
    HalfEdge* h52 = new HalfEdge(v6, 22);
    HalfEdge* h53 = new HalfEdge(v5, 23);
    h50->next = h51;
    h51->next = h52;
    h52->next = h53;
    h53->next = h50;

    h00->sym = h11;
    h01->sym = h20;
    h02->sym = h30;
    h03->sym = h43;

    h10->sym = h21;
    h11->sym = h00;
    h12->sym = h42;
    h13->sym = h50;

    h20->sym = h01;
    h21->sym = h10;
    h22->sym = h53;
    h23->sym = h31;

    h30->sym = h02;
    h31->sym = h23;
    h32->sym = h52;
    h33->sym = h40;

    h40->sym = h33;
    h41->sym = h51;
    h42->sym = h12;
    h43->sym = h03;

    h50->sym = h13;
    h51->sym = h41;
    h52->sym = h32;
    h53->sym = h22;

    Face* f0 = new Face(h00, 0);
    Face* f1 = new Face(h10, 1);
    Face* f2 = new Face(h20, 2);
    Face* f3 = new Face(h30, 3);
    Face* f4 = new Face(h40, 4);
    Face* f5 = new Face(h50, 5);

    h00->face = f0;
    h01->face = f0;
    h02->face = f0;
    h03->face = f0;

    h10->face = f1;
    h11->face = f1;
    h12->face = f1;
    h13->face = f1;

    h20->face = f2;
    h21->face = f2;
    h22->face = f2;
    h23->face = f2;

    h30->face = f3;
    h31->face = f3;
    h32->face = f3;
    h33->face = f3;

    h40->face = f4;
    h41->face = f4;
    h42->face = f4;
    h43->face = f4;

    h50->face = f5;
    h51->face = f5;
    h52->face = f5;
    h53->face = f5;

//    glm::vec3 blue    = glm::vec3(0.f, 0.2f, 0.6f);
//    glm::vec3 red     = glm::vec3(0.6f, 0.f, 0.f);
//    glm::vec3 yellow  = glm::vec3(0.6f, 0.6f, 0.f);

    glm::vec3 blue    = glm::vec3(0.f, 0.3f, 1.f);
    glm::vec3 red     = glm::vec3(1.f, 0.f, 0.f);
    glm::vec3 yellow  = glm::vec3(1.f, 1.f, 0.f);

    f0->color = red;
    f1->color = yellow;
    f2->color = blue;
    f3->color = yellow;
    f4->color = blue;
    f5->color = red;

    vector<Face*>* f = new vector<Face*>;
    f->push_back(f0);
    f->push_back(f1);
    f->push_back(f2);
    f->push_back(f3);
    f->push_back(f4);
    f->push_back(f5);

    hez->push_back(h00);
    hez->push_back(h01);
    hez->push_back(h02);
    hez->push_back(h03);
    hez->push_back(h10);
    hez->push_back(h11);
    hez->push_back(h12);
    hez->push_back(h13);
    hez->push_back(h20);
    hez->push_back(h21);
    hez->push_back(h22);
    hez->push_back(h23);
    hez->push_back(h30);
    hez->push_back(h31);
    hez->push_back(h32);
    hez->push_back(h33);
    hez->push_back(h40);
    hez->push_back(h41);
    hez->push_back(h42);
    hez->push_back(h43);
    hez->push_back(h50);
    hez->push_back(h51);
    hez->push_back(h52);
    hez->push_back(h53);

    faces = f;
    halfEdges = hez;
    vertices = vertz;

    numTriangles = 12;

}


void Mesh::updateNumTriangles() {

    int count = 0;
    Face* thisFace;

    // Count up the faces
    for (int i = 0; i < faces->size(); i++) {
        thisFace = faces->operator [](i);
        count = count + (thisFace->numSides() - 2);
    }
    numTriangles = count;
}


void Mesh::split(Face* face1) {

    // Make sure there are 4 edges
    if (face1->numSides() != 4) {
        return;
    }

    HalfEdge* halfEdge0 = face1->halfEdge;
    Face* face2 = new Face(10 * face1->id);
    face2->color = face1->color;

    HalfEdge* halfEdgeLeft = new HalfEdge(10 * halfEdge0->id);
    HalfEdge* halfEdgeRight = new HalfEdge(10 * halfEdge0->id + 1);
    halfEdgeLeft->face = face1;
    halfEdgeRight->face = face2;
    face2->halfEdge = halfEdgeRight;
    halfEdgeLeft->sym = halfEdgeRight;
    halfEdgeRight->sym = halfEdgeLeft;

    halfEdgeLeft->vert = halfEdge0->next->next->vert;
    halfEdgeLeft->next = halfEdge0->next->next->next;
    halfEdgeRight->next = halfEdge0->next;
    halfEdgeRight->next->face = face2;
    halfEdgeRight->next->next->next = halfEdgeRight;
    halfEdgeRight->next->next->face = face2;
    halfEdge0->next = halfEdgeLeft;
    halfEdgeRight->vert = halfEdge0->vert;

    std::vector<Face*>::iterator it = faces->begin();
    while (*it != face1) {
        it++;
    }
    faces->insert(it + 1, face2);

    halfEdges->push_back(halfEdgeLeft);
    halfEdges->push_back(halfEdgeRight);

    updateNumTriangles();
}

void Mesh::addEdge(Face* fa, Face* fb, Vertex* v) {

    HalfEdge* hea0 = fa->halfEdge;
    HalfEdge* heb0 = fb->halfEdge;
    if (hea0->vert != heb0->vert) {
        printf("Can't split this vertex");
        return;
    }

    Vertex* v1 = new Vertex(*v);
    v1->X = v->X;
    v1->Y = v->Y;
    v1->Z = v->Z;

    HalfEdge* halfEdgeNewLeft = new HalfEdge(10 * hea0->id);
    HalfEdge* halfEdgeNewRight = new HalfEdge(10 * heb0->id);

    halfEdgeNewLeft->next = hea0->next;
    hea0->next = halfEdgeNewLeft;
    halfEdgeNewLeft->vert = v1;
    halfEdgeNewLeft->face = fa;
    halfEdgeNewLeft->sym = halfEdgeNewRight;

    halfEdgeNewRight->next = heb0->next;
    heb0->next = halfEdgeNewRight;
    halfEdgeNewRight->vert = hea0->vert;
    heb0->vert = v1;
    halfEdgeNewRight->face = fb;
    halfEdgeNewRight->sym = halfEdgeNewLeft;

    halfEdges->push_back(halfEdgeNewLeft);
    halfEdges->push_back(halfEdgeNewRight);
    vertices->push_back(v1);

    halfEdgeNewLeft->next->sym->vert = v1;

    updateNumTriangles();

}


void Mesh::addVertex(HalfEdge* he0) {

    Vertex* newV = new Vertex(0, 0, 0, 1, 10 * he0->vert->id);
    HalfEdge* he1 = he0->next;
    HalfEdge* he2 = he1->sym;

    HalfEdge* halfEdgeNewLeft = new HalfEdge(10 * he0->id);
    HalfEdge* halfEdgeNewRight = new HalfEdge(1 + 10 * he0->id);

    halfEdgeNewLeft->next = he1;
    he0->next = halfEdgeNewLeft;
    halfEdgeNewRight->next = he2->next;
    he2->next = halfEdgeNewRight;

    halfEdgeNewLeft->sym = halfEdgeNewRight;
    halfEdgeNewRight->sym = halfEdgeNewLeft;

    halfEdgeNewLeft->face = he0->face;
    halfEdgeNewRight->face = he2->face;

    halfEdgeNewLeft->vert = newV;
    halfEdgeNewRight->vert = he2->vert;
    he2->vert = newV;

    newV->X = (he0->vert->X + he1->vert->X)/2;
    newV->Y = (he0->vert->Y + he1->vert->Y)/2;
    newV->Z = (he0->vert->Z + he1->vert->Z)/2;

    halfEdges->push_back(halfEdgeNewLeft);
    halfEdges->push_back(halfEdgeNewRight);
    vertices->push_back(newV);

    updateNumTriangles();

}

void Mesh::deleteVertex(HalfEdge* he) {

    // Need at least 3 faces
    if (he->face->numSides() < 3) {
        return;
    }

    // Store all the faces that touch the vertex
    vector<Face*> temp;
    HalfEdge* thisHE;
    HalfEdge* startingHE;
    HalfEdge* currentHE;


    // Collect a bunch of copies of the faces to be deleted (one per halfedge)
    for (int i = 0; i < halfEdges->size(); i++) {
       thisHE = halfEdges->operator [](i);
       if (thisHE->vert == he->vert) {
           startingHE = thisHE;
           currentHE = thisHE;
           do {
               temp.push_back(currentHE->face);
               currentHE = currentHE->next;
           } while (startingHE != currentHE);

       }
    }

    for (int i = 0; i < temp.size(); i++) {
        for (int j = 0; j < halfEdges->size(); j++) {
            thisHE = halfEdges->operator[](j);
            if (thisHE->face == temp.operator [](i)) {
                halfEdges->erase(halfEdges->begin() + j);
            }
        }
    }

    // Erase the vertex
    for (int i = 0; i < vertices->size(); i++) {
        if (vertices->operator [](i) == he->vert) {
            vertices->erase(vertices->begin() + i);
        }
    }

    // Erase the face from faces
    for (int i = temp.size() - 1; i >= 0; i--) {
        for (int j = faces->size() - 1; j >= 0; j--) {
            if (temp.at(i)==faces->at(j))
                faces->erase(faces->begin()+j);
        }
    }

    updateNumTriangles();

}

void Mesh::makePlanar() {

    Face* thisFace;
    HalfEdge* start;
    HalfEdge* thisHE;
    Vertex* firstVert;
    Vertex* secondVert;
    Vertex* thirdVert;
    glm::vec3 firstEdge;
    glm::vec3 secondEdge;

    for (int i = 0; i < faces->size(); i++) {

        thisFace = faces->operator [](i);
        start = thisFace->halfEdge;
        thisHE = start->next;

        // Get the normal
        firstVert = thisHE->vert;
        secondVert = thisHE->next->vert;
        thirdVert = thisHE->next->next->vert;

        firstEdge = glm::vec3((secondVert->X - firstVert->X), (secondVert->Y - firstVert->Y), (secondVert->Z - firstVert->Z));
        secondEdge = glm::vec3((thirdVert->X - secondVert->X), (thirdVert->Y - secondVert->Y), (thirdVert->Z - secondVert->Z));

        glm::vec3 cross = glm::cross(firstEdge, secondEdge);
        cross = cross/glm::sqrt((glm::exp2(cross[0]) + glm::exp2(cross[1]) + glm::exp2(cross[2])));

        while (thisHE != start) {

            firstVert = thisHE->vert;
            secondVert = thisHE->next->vert;
            firstEdge = glm::vec3((secondVert->X - firstVert->X), (secondVert->Y - firstVert->Y), (secondVert->Z - firstVert->Z));
            firstEdge = firstEdge/glm::sqrt((glm::pow(firstEdge[0], 2.f) + glm::pow(firstEdge[1], 2.f) + glm::pow(firstEdge[2], 2.f)));

            float ans = glm::dot(firstEdge, cross);

            if (ans > 0.0001) {
                split(thisHE->face);
            } else {
                thisHE = thisHE->next;
            }

        }

    }

}


