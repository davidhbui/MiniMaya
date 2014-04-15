#include "myglwidget.h"
#include "camera.h"
#include <iostream>
#include <cstdlib>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QKeyEvent>

#define GLM_FORCE_RADIANS
// This includes glm::translate, glm::rotate, and glm::scale.
// You should NOT use glm::perspective and glm::lookAt, which
// are also included!
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

using namespace std;

static const float PI = 3.141592653589f;

MyGLWidget::MyGLWidget(QWidget* parent) : QGLWidget(parent) {
    // Tell the widget to redraw 60 times per second
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

    cubeRotation = 0;
    cubeRotationDirection = 1;

    isPlaying = false;

    camera = new Camera();

    cubeTransformation = glm::mat4(1.f);

    myViewMat = getView(camera->getPos(), glm::vec3(0.f, 0.f, 0.f), camera->getUp());
    myProjMat = getProj(camera->getViewingAngle(), 641.f/481.f, 1.f, 100.f);

    myViewProj = myProjMat * myViewMat;

    prevMouseX = 0;
    prevMouseY = 0;

    cube = new Mesh();

    faceIndex = 0;
    currentFace = cube->faces->operator [](faceIndex);
    currentFace->isCurrent = true;

    vertexIndex = 0;
    currentVertex = cube->vertices->operator[](vertexIndex);
    currentVertex->isCurrent = true;

    halfEdgeIndex = 0;
    currentHalfEdge = cube->halfEdges->operator[](halfEdgeIndex);
    currentHalfEdge->isCurrent = true;
    currentHalfEdge->vert->isEndPoint = true;
    currentHalfEdge->sym->vert->isEndPoint = true;

    newRed = 0;
    newGreen = 0;
    newBlue = 0;
    newX = 0;
    newY = 0;
    newZ = 0;

    ray_wor = glm::vec3(1.0);
    mousePressed = false;
    this->setMouseTracking(true);

}

MyGLWidget::~MyGLWidget() {
    // Delete the buffers that were allocated on the GPU
    // (This is like deleting pointers on the CPU)
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboNormals);
    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboWirePositions);
    glDeleteBuffers(1, &vboWireColors);
    glDeleteBuffers(1, &vboWireIndices);

    // Also delete the shader program
    glDeleteProgram(progDiffuse);
    glDeleteProgram(progWire);

    // And the VAO, if we created one
    if (vao) {
        vao->destroy();
    }

    delete cube;
}

// Re-numbers all the elements in the face and vertex lists
void MyGLWidget::refreshLists() {

    // Clear the widgets
    while (faceList->count() > 0) {
        faceList->takeItem(0);
    }
    while (halfEdgeList->count() > 0) {
        halfEdgeList->takeItem(0);
    }
    while (vertexList->count() > 0) {
        vertexList->takeItem(0);
    }

    // Re-populate them
    for (int i = 0; i < cube->faces->size(); i++) {
        faceList->addItem(cube->faces->operator [](i));
    }
    for (int i = 0; i < cube->halfEdges->size(); i++) {
        halfEdgeList->addItem(cube->halfEdges->operator[](i));
    }
    for (int i = 0; i < cube->vertices->size(); i++) {
        vertexList->addItem(cube->vertices->operator [](i));
    }


    // Re-number the faces and vertices
    Face* thisFace;
    for (int i = 0; i < faceList->count(); i++) {
        thisFace = dynamic_cast<Face*>(faceList->item(i));
        thisFace->setID(i);
    }
    Vertex* thisVertex;
    for (int i = 0; i < vertexList->count(); i++) {
        thisVertex = dynamic_cast<Vertex*>(vertexList->item(i));
        thisVertex->setID(i);
    }
    HalfEdge* thisHalfEdge;
    for (int i = 0; i < halfEdgeList->count(); i++) {
        thisHalfEdge = dynamic_cast<HalfEdge*>(halfEdgeList->item(i));
        thisHalfEdge->setID(i);
    }

    updateGL();
    setFocus();
}

void MyGLWidget::changeColor() {
    currentFace->color = glm::vec3(newRed, newGreen, newBlue);
    traverse();
    updateGL();
}

void MyGLWidget::setRed(double r) {
    newRed = (float)r;
}

void MyGLWidget::setGreen(double g) {
    newGreen = (float)g;
}

void MyGLWidget::setBlue(double b) {
    newBlue = (float)b;
}

void MyGLWidget::setX(double x) {
    newX = (float)x;
}
void MyGLWidget::setY(double y) {
    newY = (float)y;
}
void MyGLWidget::setZ(double z) {
    newZ = (float)z;
}

void MyGLWidget::changePos() {
    currentVertex->X = currentVertex->X + newX;
    currentVertex->Y = currentVertex->Y + newY;
    currentVertex->Z = currentVertex->Z + newZ;
    traverse();
    updateGL();
}

void MyGLWidget::startRotation() {
    timer->start(16);
    isPlaying = true;
}

void MyGLWidget::stopRotation() {
    timer->stop();
    isPlaying = false;
}

void MyGLWidget::setCurrentFace(int index) {

    if (index >= cube->faces->size()) {
        index = cube->faces->size() - 1;
    }
    currentFace->isCurrent = false;

    faceIndex = index;
    currentFace = cube->faces->operator[](faceIndex);

    faceList->setCurrentItem(faceList->item(index));
    currentFace->isCurrent = true;
    traverse();
    updateGL();
    setFocus();
}

void MyGLWidget::setCurrentVertex(int index) {

    if (index >= cube->vertices->size()) {
        index = cube->vertices->size() - 1;
    }

    vertexIndex = index;
    currentVertex->isCurrent = false;

    currentVertex = cube->vertices->operator [](vertexIndex);
    vertexIndex = index;

    vertexList->setCurrentItem(vertexList->item(index));
    currentVertex->isCurrent = true;
    traverse();
    updateGL();
    setFocus();
}

void MyGLWidget::setCurrentHalfEdge(int index) {

    if (index >= cube->halfEdges->size()) {
        index = cube->halfEdges->size() - 1;
    }

    currentHalfEdge->isCurrent = false;
    currentHalfEdge->vert->isEndPoint = false;
    if (currentHalfEdge->sym) {
        currentHalfEdge->sym->vert->isEndPoint = false;
    }

    halfEdgeIndex = index;
    currentHalfEdge = cube->halfEdges->operator[](halfEdgeIndex);
    halfEdgeList->setCurrentItem(halfEdgeList->item(index));

    currentHalfEdge->isCurrent = true;
    currentHalfEdge->vert->isEndPoint = true;
    if (currentHalfEdge->sym) {
        currentHalfEdge->sym->vert->isEndPoint = true;
    }

    traverse();
    updateGL();
    setFocus();
}

void MyGLWidget::splitCurrentFace() {
    cube->split(currentFace);
    faceList->insertItem(faceIndex + 1, cube->faces->operator[](faceIndex + 1));
    traverse();
    updateGL();
    refreshLists();
    setFocus();
}

void MyGLWidget::insertVertex() {

   HalfEdge* thisHE = currentHalfEdge->next;
   while (thisHE->next != currentHalfEdge) {
       thisHE = thisHE->next;
   }

    cube->addVertex(thisHE);
    refreshLists();
    traverse();
    updateGL();
    setFocus();
}

void MyGLWidget::deleteVertex() {
    HalfEdge* vertEdge;
    for (int i = 0; i < cube->halfEdges->size(); i++) {
        if (cube->halfEdges->operator[](i)->vert == currentVertex) {
            vertEdge = cube->halfEdges->operator[](i);
            break;
        }
    }
    cube->deleteVertex(vertEdge);
    refreshLists();
    traverse();
    updateGL();
    setFocus();
}

void MyGLWidget::insertEdge() {

    Face* faceA = currentFace;
    Face* faceB;
    Face* thisFace;
    Vertex* thisVertex;

    HalfEdge* thisHE;
    HalfEdge* faceAEdge;
    HalfEdge* hea0 = currentFace->halfEdge;
    HalfEdge* heb0;

    bool foundOne = false;
    bool isTheRightFace = true;

    for (int i = 0; i < cube->faces->size(); i++) {
        thisFace = cube->faces->operator[](i);

        // Keep looking if it's the same face, or if they share an edge
        if (thisFace == currentFace) {
            continue;
        }

        thisVertex = thisFace->halfEdge->vert;
        if (thisVertex == currentVertex) {
            heb0 = thisFace->halfEdge;

            isTheRightFace = true;

            // Check to make sure the faces don't share an edge
            faceAEdge = hea0;
            do {
                thisHE = heb0;
                do {
                    if (faceAEdge == thisHE->sym) {
                        printf("These faces (%d and %d) share an edge so we can't split the vertex!\n", currentFace->id, thisFace->id);
                        isTheRightFace = false;
                    }
                    thisHE = thisHE->next;
                } while (thisHE != heb0);
                faceAEdge = faceAEdge->next;
            } while (faceAEdge != hea0);

            if (isTheRightFace) {
                faceB = thisFace;
                foundOne = true;
            }
        }
    }

    if (!foundOne) {
        printf("We couldn't find any faces that share the vertex but not an edge! :(\n");
        return;
    }

    printf("We're going in with face %d and face %d\n", faceA->id, faceB->id);
    cube->addEdge(faceA, faceB, currentVertex);
    traverse();
    updateGL();
    refreshLists();
    setFocus();
}

// Calculate the projection matrix
glm::mat4 MyGLWidget::getProj(float yViewingAngle, float aspectRatio, float zFront, float zBack) {

    float zRange = -(zFront - zBack);
    float tanHalfFOV = glm::tan(yViewingAngle/2);

    glm::mat4 proj = glm::mat4(
        glm::vec4(1.0f / (tanHalfFOV * aspectRatio),     0.f,                       0.f,                        0.f),
        glm::vec4(    0.f,                             1.0f / tanHalfFOV,           0.f,                        0.f),
        glm::vec4(    0.f,                               0.f,               (-zFront - zBack) / zRange,        -1.f),
        glm::vec4(    0.f,                               0.f,             -2.0f * zBack * zFront / zRange,      0.f)
    );

    return proj;

}

// Calculate the view matrix
glm::mat4 MyGLWidget::getView(glm::vec3 eye, glm::vec3 target, glm::vec3 up) {

    glm::vec3 zAxis = glm::normalize(eye - target);
    glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
    glm::vec3 yAxis = glm::cross(zAxis, xAxis);

    glm::mat4 view = glm::mat4(
        glm::vec4( xAxis[0],                        yAxis[0],              zAxis[0],                0),
        glm::vec4( xAxis[1],                        yAxis[1],              zAxis[1],                0),
        glm::vec4( xAxis[2],                        yAxis[2],              zAxis[2],                0),
        glm::vec4(-1*(glm::dot(xAxis, eye)), -1*(glm::dot(yAxis, eye)), -1*(glm::dot(zAxis, eye)),  1)
    );

    return view;
}

void MyGLWidget::keyPressEvent(QKeyEvent *e) {
    // http://qt-project.org/doc/qt-4.8/qt.html#Key-enum
    if (e->key() == Qt::Key_1) {
        cubeRotationDirection = 0;
    }
    if (e->key() == Qt::Key_2) {
        cubeRotationDirection = 1;
    }
    if (e->key() == Qt::Key_3) {
        cubeRotationDirection = 2;
    }
    if (e->key() == Qt::Key_Escape) {
        qApp->quit();
    }
    if (e->key() == Qt::Key_Up) {
        camera->rotateNorth();
        updateGL();
    }
    if (e->key() == Qt::Key_Down) {
        camera->rotateSouth();
        updateGL();
    }
    if (e->key() == Qt::Key_Left) {
        camera->rotateEast();
        updateGL();
    }
    if (e->key() == Qt::Key_Right) {
        camera->rotateWest();
        updateGL();
    }
    if (e->key() == Qt::Key_Space) {
        if (isPlaying) {
            stopRotation();
        } else {
            startRotation();
        }
    }
    if (e->key() == Qt::Key_Z) {
        camera->zoomIn();
        updateGL();
    }
    if (e->key() == Qt::Key_X) {
        camera->zoomOut();
        updateGL();
    }

    if (!isPlaying) {
        updateGL();
    }

}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e) {

    // Hover functionality
    if (!mousePressed) {

        // Screen coordinates of the mouse event
        float x = e->x();
        float y = e->y();

        if  (x != -1 && y != -1) {

            // Change them to NDCs
            x = x / this->width();
            y = 1 - y / this->height();

            // The position of the "eye" (camera) and the up vector
            glm::vec3 E = camera->getPos();
            glm::vec3 U = camera->getUp();

            // Viewing vector
            glm::vec3 center = glm::vec3(0, 0, 0);
            glm::vec3 C = center - E;

            // Viewing angles (theta is the half viewing angle in the x direction, phi is the half viewing angle in the y)
            float theta = glm::radians(camera->getViewingAngle() / 2 * this->width()/this->height());
            float phi = glm::radians(camera->getViewingAngle()/2);

            // Vector A = C x camUp, B = the REAL up vector, and M = the view center
            glm::vec3 A = glm::cross(C, U);
            glm::vec3 B = glm::cross(A, C);
            glm::vec3 M = E + C;

            // V || B, on the view plane
            glm::vec3 V = B * (1.0f/(glm::length(B)) * (glm::length(C) * tan(phi)));

            // H || A, on the view plane
            glm::vec3 H = A * (1.0f/(glm::length(A)) * (glm::length(C) * tan(theta)));

            // The point on the screen where you're clicking
            glm::vec3 P = M + (2 * x - 1) * H + (2 * y - 1) * V;

            float t = 100000000;
            glm::vec3 R = E + (t * (P - E))/glm::length(P - E);

            cube->makePlanar();

            // Iterate through the faces and check for intersections

            Face* thisFace;
            HalfEdge* start;
            float fastestTime = t;

            // Assume we are not hovering over anything
            for (int j = 0; j < cube->faces->size(); j++) {
                thisFace = cube->faces->operator[](j);
                thisFace->isHovered = false;
            }

            for (int i = 0; i < cube->faces->size(); i++) {

                thisFace = cube->faces->operator[](i);
                start = thisFace->halfEdge;

                glm::vec3 firstPoint(start->vert->X, start->vert->Y, start->vert->Z);
                glm::vec3 temp = firstPoint - E;
                glm::vec3 temp2 = P - E;

                glm::vec3 faceNormal = thisFace->getNormal();

                t = (glm::dot(faceNormal, temp)) / (glm::dot(faceNormal, temp2));

                if (t > 0) {

                    bool insideFace = false;
                    glm::vec3 m = glm::vec3(1, 1, 1);

                    glm::vec3 p = E + t * temp2;

                    //**************** Ray-Triangle Intersection Inside Test ****************

                    glm::vec3 point1 = glm::vec3(start->vert->X, start->vert->Y, start->vert->Z);
                    glm::vec3 point2 = glm::vec3(start->next->vert->X, start->next->vert->Y, start->next->vert->Z);
                    glm::vec3 point3 = glm::vec3(start->next->next->vert->X, start->next->next->vert->Y, start->next->next->vert->Z);

                    // Area of the whole triangle, calculated using determinants
                    float s  = 0.5 * glm::length(glm::cross(point3 - point1, point3 - point2));

                    // Area of ^P, P1, and P3
                    float s1 = 0.5 * glm::length(glm::cross(p - point3, p - point1))/s;
                    // Area of ^P, P1, and P2
                    float s2 = 0.5 * glm::length(glm::cross(p - point1, p - point2))/s;
                    // Area of ^P, P2, and P3
                    float s3 = 0.5 * glm::length(glm::cross(p - point2, p - point3))/s;

                   // Check if P is inside
                   if (s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && (s1 + s2 + s3) >= 0.95 && (s1 + s2 + s3) <= 1.05) {
                        insideFace = true;
                   }

                   // Check the other triangle
                   point2 = point3;
                   point3 = glm::vec3(start->next->next->next->vert->X, start->next->next->next->vert->Y, start->next->next->next->vert->Z);

                   s  = 0.5 * glm::length(glm::cross(point3 - point1, point3 - point2));
                   s1 = 0.5 * glm::length(glm::cross(p - point3, p - point1))/s;
                   s2 = 0.5 * glm::length(glm::cross(p - point1, p - point2))/s;
                   s3 = 0.5 * glm::length(glm::cross(p - point2, p - point3))/s;

                   if (s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && (s1 + s2 + s3) >= 0.95 && (s1 + s2 + s3) <= 1.05) {
                        insideFace = true;
                   }

                   // If our face is closer then it's the one we want
                   if (insideFace == true && t < fastestTime) {

                       // Clear the hover data for the other faces
                       for (int j = 0; j < cube->faces->size(); j++) {
                           Face* thisGuy = cube->faces->operator[](j);
                           thisGuy->isHovered = false;
                       }
                       thisFace->isHovered = true;
                       fastestTime = t;
                   }

                }

            }

        }
        traverse();
        updateGL();

    } else {

        if (e->x() < prevMouseX) {
            camera->rotateEast();
            prevMouseX = e->x();
        }
        if (e->x() > prevMouseX) {
            camera->rotateWest();
            prevMouseX = e->x();
        }
        if (e->y() < prevMouseY) {
            camera->rotateNorth();
            prevMouseY = e->y();
        }
        if (e->y() > prevMouseY) {
            camera->rotateSouth();
            prevMouseY = e->y();
        }
            traverse();
            updateGL();
    }

}
void MyGLWidget::mousePressEvent(QMouseEvent *e) {

    // Screen coordinates of the click
    float x = e->x();
    float y = e->y();

    // Change them to NDCs
    x = x / this->width();
    y = 1 - y / this->height();

    // The position of the "eye" (camera) and the up vector
    glm::vec3 E = camera->getPos();
    glm::vec3 U = camera->getUp();

    // Viewing vector
    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 C = center - E;

    // Viewing angles (theta is the half viewing angle in the x direction, phi is the half viewing angle in the y)
    float theta = glm::radians(camera->getViewingAngle() / 2 * this->width()/this->height());
    float phi = glm::radians(camera->getViewingAngle()/2);

    // Vector A = C x camUp, B = the REAL up vector, and M = the view center
    glm::vec3 A = glm::cross(C, U);
    glm::vec3 B = glm::cross(A, C);
    glm::vec3 M = E + C;

    // V || B, on the view plane
    glm::vec3 V = B * (1.0f/(glm::length(B)) * (glm::length(C) * tan(phi)));

    // H || A, on the view plane
    glm::vec3 H = A * (1.0f/(glm::length(A)) * (glm::length(C) * tan(theta)));

    // The point on the screen where you're clicking
    glm::vec3 P = M + (2 * x - 1) * H + (2 * y - 1) * V;

// Uncomment these to see the ray
    ray_wor = P;
    sampleUploadWire();

    float t = 100000000;
    glm::vec3 R = E + (t * (P - E))/glm::length(P - E);

    cube->makePlanar();

    // Iterate through the faces and check for intersections

    Face* thisFace;
    HalfEdge* start;
    float fastestTime = t;

    for (int i = 0; i < cube->faces->size(); i++) {

        thisFace = cube->faces->operator[](i);
        start = thisFace->halfEdge;

        glm::vec3 firstPoint(start->vert->X, start->vert->Y, start->vert->Z);
        glm::vec3 temp = firstPoint - E;
        glm::vec3 temp2 = P - E;

        glm::vec3 faceNormal = thisFace->getNormal();

        t = (glm::dot(faceNormal, temp)) / (glm::dot(faceNormal, temp2));

        if (t > 0) {

            bool insideFace = false;
            glm::vec3 m = glm::vec3(1, 1, 1);

            glm::vec3 p = E + t * temp2;

            //**************** Ray-Triangle Intersection Inside Test ****************

            glm::vec3 point1 = glm::vec3(start->vert->X, start->vert->Y, start->vert->Z);
            glm::vec3 point2 = glm::vec3(start->next->vert->X, start->next->vert->Y, start->next->vert->Z);
            glm::vec3 point3 = glm::vec3(start->next->next->vert->X, start->next->next->vert->Y, start->next->next->vert->Z);

            // Area of the whole triangle, calculated using determinants
            float s  = 0.5 * glm::length(glm::cross(point3 - point1, point3 - point2));

            // Area of ^P, P1, and P3
            float s1 = 0.5 * glm::length(glm::cross(p - point3, p - point1))/s;
            // Area of ^P, P1, and P2
            float s2 = 0.5 * glm::length(glm::cross(p - point1, p - point2))/s;
            // Area of ^P, P2, and P3
            float s3 = 0.5 * glm::length(glm::cross(p - point2, p - point3))/s;

           // Check if P is inside
           if (s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && (s1 + s2 + s3) >= 0.95 && (s1 + s2 + s3) <= 1.05) {
                insideFace = true;
           }

           // Check the other triangle
           point2 = point3;
           point3 = glm::vec3(start->next->next->next->vert->X, start->next->next->next->vert->Y, start->next->next->next->vert->Z);

           s  = 0.5 * glm::length(glm::cross(point3 - point1, point3 - point2));
           s1 = 0.5 * glm::length(glm::cross(p - point3, p - point1))/s;
           s2 = 0.5 * glm::length(glm::cross(p - point1, p - point2))/s;
           s3 = 0.5 * glm::length(glm::cross(p - point2, p - point3))/s;

           if (s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && (s1 + s2 + s3) >= 0.95 && (s1 + s2 + s3) <= 1.05) {
                insideFace = true;
           }

           // If our face is closer then it's the one we want
           if (insideFace == true && t < fastestTime) {
               setCurrentFace(i);
               fastestTime = t;
           }

        }

    }

    updateGL();
    mousePressed = true;

}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *e) {
    mousePressed = false;
}


void MyGLWidget::wheelEvent(QWheelEvent *e) {
    if (e->delta() > 0) {
        camera->zoomIn();
    } else {
        camera->zoomOut();
    }
    if (!isPlaying) {
        updateGL();
    }

}

void MyGLWidget::initializeGL() {
    // Create an OpenGL context
    initializeGLFunctions();

    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Create the buffer objects which hold vertex data on the GPU
    glGenBuffers(1, &vboPositions);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboColors);
    glGenBuffers(1, &vboIndices);
    glGenBuffers(1, &vboWirePositions);
    glGenBuffers(1, &vboWireColors);
    glGenBuffers(1, &vboWireIndices);

    // Set a few settings/modes
    glEnable(GL_DEPTH_TEST);
    //glClearDepth(1.0);
    //glDepthFunc(GL_LEQUAL);
    // Set the color which clears the screen between frames
    glClearColor(0, 0, 0, 1);

    // Set up the shaders
    shaderSetup();
    shaderSetupWire();

    // Upload the cube
    traverse();
}

void MyGLWidget::traverse() {

    int FACES = cube->faces->size();

    int vertexCount = 0;
    Face* thisFace;
    for (int i = 0; i < FACES; i++) {
        thisFace = cube->faces->operator[](i);
        vertexCount = vertexCount + thisFace->numSides();
    }

    int VERTICES = vertexCount;
    int TRIANGLES = cube->numTriangles;

    static const GLsizei SIZE_POS = 4 * sizeof(float);
    static const GLsizei SIZE_NOR = 4 * sizeof(float);
    static const GLsizei SIZE_COL = 4 * sizeof(float);
    static const GLsizei SIZE_TRI = 3 * sizeof(GLuint);

    GLfloat positions[VERTICES][4];
    GLfloat normals[VERTICES][4];
    GLfloat colors[VERTICES][4];
    GLuint indices[TRIANGLES][3];

    HalfEdge* he;
    HalfEdge* start;
    Vertex* thisVertex;
    Vertex* firstVert;
    Vertex* secondVert;
    Vertex* thirdVert;
    glm::vec3 firstEdge;
    glm::vec3 secondEdge;
    glm::vec3 normal;
    float normalX;
    float normalY;
    float normalZ;
    glm::vec3 col;

    // The index in the indices array to put the values
    int triangleCount = 0;
    int iP = 0;

    // The index in the positions/normals array to put the values
    int count = 0;

    // For each face in the cube
    for (int i = 0; i < FACES; i++) {

        thisFace = cube->faces->operator [](i);

        he = thisFace->halfEdge;
        start = thisFace->halfEdge;

        firstVert = he->vert;
        secondVert = he->next->vert;
        thirdVert = he->next->next->vert;

        firstEdge = glm::vec3((secondVert->X - firstVert->X), (secondVert->Y - firstVert->Y), (secondVert->Z - firstVert->Z));
        secondEdge = glm::vec3((thirdVert->X - secondVert->X), (thirdVert->Y - secondVert->Y), (thirdVert->Z - secondVert->Z));

        normal = glm::cross(firstEdge, secondEdge);
        normalX = normal[0];
        normalY = normal[1];
        normalZ = normal[2];

        if (thisFace->isCurrent) {
            col = thisFace->color;
        } else {
            glm::vec3 mutedColors = glm::vec3(thisFace->color[0] - 0.4, thisFace->color[1] - 0.4, thisFace->color[2] - 0.4);
            col = mutedColors;
        }

        if (thisFace->isHovered) {
            glm::vec3 hoverColors = glm::vec3(thisFace->color[0] - 0.2, thisFace->color[1] - 0.2, thisFace->color[2] - 0.2);
            col = hoverColors;
        }

        // Put the first vertex into the positions array and move the pointer up
        thisVertex = he->vert;

        positions[count][0] = thisVertex->X;
        positions[count][1] = thisVertex->Y;
        positions[count][2] = thisVertex->Z;
        positions[count][3] = thisVertex->W;
        normals[count][0] = normalX;
        normals[count][1] = normalY;
        normals[count][2] = normalZ;
        normals[count][3] = 0;

        // Uncomment this code to enable highlighting the active edge and point
//        if (thisVertex->isEndPoint) {
//            colors[count][0] = 0;
//            colors[count][1] = 1;
//            colors[count][2] = 1;
//            colors[count][3] = 1;
//        }
//        if (thisVertex->isCurrent) {
//            colors[count][0] = 1;
//            colors[count][1] = 0;
//            colors[count][2] = 1;
//            colors[count][3] = 1;
//        }
//        if (!thisVertex->isCurrent && !thisVertex->isEndPoint) {
            colors[count][0] = col[0];
            colors[count][1] = col[1];
            colors[count][2] = col[2];
            colors[count][3] = 1;
//        }



        count++;
        he = he->next;

        // Fill in the triangle indices
        for (int k = 0; k < thisFace->numSides() - 2; k++) {
            indices[triangleCount][0] = iP;
            indices[triangleCount][1] = iP + k + 1;
            indices[triangleCount][2] = iP + k + 2;
            triangleCount++;
        }
        iP = iP + thisFace->numSides();

        // For the rest of the half edges
        while (he != start) {

            // Pull out and add the vertex to the positions array
            thisVertex = he->vert;
            positions[count][0] = thisVertex->X;
            positions[count][1] = thisVertex->Y;
            positions[count][2] = thisVertex->Z;
            positions[count][3] = thisVertex->W;
            normals[count][0] = normalX;
            normals[count][1] = normalY;
            normals[count][2] = normalZ;
            normals[count][3] = 0;

            // Uncomment this code to enable highlighting the active edge and point
//            if (thisVertex->isEndPoint) {
//                colors[count][0] = 0;
//                colors[count][1] = 1;
//                colors[count][2] = 1;
//                colors[count][3] = 1;
//            }
//            if (thisVertex->isCurrent) {
//                colors[count][0] = 1;
//                colors[count][1] = 0;
//                colors[count][2] = 1;
//                colors[count][3] = 1;
//            }
//            if (!thisVertex->isCurrent && !thisVertex->isEndPoint) {
                colors[count][0] = col[0];
                colors[count][1] = col[1];
                colors[count][2] = col[2];
                colors[count][3] = 1;
//            }

            // Move the pointer up
            he = he->next;
            count++;
        }

    }

    // UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)
    // ===================================================

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, positions, GL_DYNAMIC_DRAW);

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_NOR, normals, GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_COL, colors, GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, indices, GL_DYNAMIC_DRAW);

    // Check for OpenGL errors
    printGLErrorLog();

}

void MyGLWidget::debugContextVersion() {
    const char* profile;
    switch(context()->format().profile()) {
    case QGLFormat::CoreProfile:
        profile = "Core";
        break;
    case QGLFormat::CompatibilityProfile:
        profile = "Compatibility";
        break;
    default:
        profile = "None";
    }
    qDebug() << "Widget version:" <<
        format().majorVersion() << "." <<
        format().minorVersion();
    qDebug() << "Context valid:" << context()->isValid();
    qDebug() << "Format version:" <<
        context()->format().majorVersion() << "." <<
        context()->format().minorVersion();
    qDebug() << "Profile:" << profile;
    qDebug() << "  VENDOR:  " << (const char*) glGetString(GL_VENDOR);
    qDebug() << "  RENDERER:" << (const char*) glGetString(GL_RENDERER);
    qDebug() << "  VERSION: " << (const char*) glGetString(GL_VERSION);
    qDebug() << "  GLSL:    " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
}

void MyGLWidget::shaderSetup() {
    // Read in the shader program source files
    std::string vertSourceS = MyGLWidget::readFile(":/shaders/diff.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = MyGLWidget::readFile(":/shaders/diff.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadDiffuseVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadDiffuseFrag = glCreateShader(GL_FRAGMENT_SHADER);
    progDiffuse = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource(shadDiffuseVert, 1, &vertSource, NULL);
    glCompileShader(shadDiffuseVert);
    printShaderInfoLog(shadDiffuseVert);
    // - Fragment shader
    glShaderSource(shadDiffuseFrag, 1, &fragSource, NULL);
    glCompileShader(shadDiffuseFrag);
    printShaderInfoLog(shadDiffuseFrag);

    // Link the shader program together from compiled bits
    glAttachShader(progDiffuse, shadDiffuseVert);
    glAttachShader(progDiffuse, shadDiffuseFrag);
    glLinkProgram(progDiffuse);
    // Check to make sure the program linked correctly
    printLinkInfoLog(progDiffuse);

    // Clean up the shaders now that they are linked
    glDetachShader(progDiffuse, shadDiffuseVert);
    glDetachShader(progDiffuse, shadDiffuseFrag);
    glDeleteShader(shadDiffuseVert);
    glDeleteShader(shadDiffuseFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    attrPositions = glGetAttribLocation(progDiffuse, "vs_Position");
    attrNormals = glGetAttribLocation(progDiffuse, "vs_Normal");
    attrColors = glGetAttribLocation(progDiffuse, "vs_Color");
    unifViewProj = glGetUniformLocation(progDiffuse, "u_ViewProj");
    unifModel = glGetUniformLocation(progDiffuse, "u_Model");
    unifModelInvTranspose = glGetUniformLocation(progDiffuse, "u_ModelInvTranspose");

    // The OpenGL 3.1+ Core profiles require a VAO to be bound at all times,
    // so we create one, bind it, and forget about it.
    if (context()->format().profile() != QGLFormat::CompatibilityProfile) {
        vao = new QOpenGLVertexArrayObject(this);
        vao->create();
        vao->bind();
        // This is the equivalent low-level OpenGL code:
        //glGenVertexArrays(1, &vao);
        //glBindVertexArray(vao);
    }

    printGLErrorLog();
}

void MyGLWidget::resizeGL(int w, int h) {
    float width = w;
    float height = h;
    // Set the size of the viewport area
    glViewport(0, 0, width, height);

    camera->updatePos();

    myViewMat = getView(camera->getPos(), glm::vec3(0.f, 0.f, 0.f), camera->getUp());
    myProjMat = getProj(camera->getViewingAngle(), 641.f/481.f, 1.f, 100.f);

    myViewProj = myProjMat * myViewMat;

    // Upload the projection matrix
    glUseProgram(progDiffuse);
    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &myViewProj[0][0]);
    glUseProgram(progWire);
    glUniformMatrix4fv(unifWireViewProj, 1, GL_FALSE, &myViewProj[0][0]);

    printGLErrorLog();
}

void MyGLWidget::moveLeft() {
    camera->rotateEast();
    updateGL();
}

void MyGLWidget::moveRight() {
    camera->rotateWest();
    updateGL();
}

void MyGLWidget::moveUp() {
    camera->rotateNorth();
    updateGL();
}

void MyGLWidget::moveDown() {
    camera->rotateSouth();
    updateGL();
}

void MyGLWidget::zoomIn() {
    camera->zoomIn();
    updateGL();
}

void MyGLWidget::zoomOut() {
    camera->zoomOut();
    updateGL();
}

void MyGLWidget::timerUpdate() {
    // This function is called roughly 60 times per second.
    // Use it to update your scene and then tell it to redraw.
    // (Don't update your scene in paintGL, because it
    // sometimes gets called automatically by Qt.)

    cubeRotation = cubeRotation + 0.03;
    updateGL();
}

void MyGLWidget::paintGL() {
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    // The default provided value is an identity matrix; you'll change this.

    glm::mat4 modelmat = glm::mat4(1.f);

    if (cubeRotationDirection == 0) {
        modelmat = glm::rotate(modelmat, cubeRotation, glm::vec3(1, 0, 0));
    } else if (cubeRotationDirection == 1) {
        modelmat = glm::rotate(modelmat, cubeRotation, glm::vec3(0, 1, 0));
    } else {
        modelmat = glm::rotate(modelmat, cubeRotation, glm::vec3(0, 0, 1));
    }

    // Draw a square
    sampleDrawCube(modelmat);

    sampleDrawWire(glm::mat4(1.f), 1);

    myViewMat = getView(camera->getPos(), glm::vec3(0.f, 0.f, 0.f), camera->getUp());
    myProjMat = getProj(camera->getViewingAngle(), 641.f/481.f, 1.f, 100.f);

    myViewProj = myProjMat * myViewMat;

    glUseProgram(progDiffuse);
    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &myViewProj[0][0]);
    glUseProgram(progWire);
    glUniformMatrix4fv(unifWireViewProj, 1, GL_FALSE, &myViewProj[0][0]);


    // Check for any GL errors that have happened recently
    printGLErrorLog();

}

void MyGLWidget::sampleUploadWire() {
    // Refer to the OpenGL lecture slides for extra explanation of functions

    // VERTEX/MESH DATA
    // ================

    // Number of vertices
    static const int VERTICES = 2;
    // Number of lines
    static const int LINES = 1;

    // Sizes of the various array elements below.
    static const GLsizei SIZE_POS = 4 * sizeof(float);
    static const GLsizei SIZE_COL = 4 * sizeof(float);
    static const GLsizei SIZE_LINE = 2 * sizeof(GLuint);

    // Initialize an array of floats to hold our cube's position data.
    // Each vertex is {x,y,z,w} where w is the homogeneous coordinate
//    GLfloat positions[VERTICES][4] = {
//        {-1, +1, +1, 1}, {-1, -1, +1, 1},
//    };

    glm::vec3 camPos = camera->getPos();

    GLfloat positions[VERTICES][4] = {
        {camPos[0], camPos[1], camPos[2], 1}, {ray_wor[0], ray_wor[1], ray_wor[2], 1},
    };

    // Initialize an array of floats to hold our cube's color data
    // Color elements are in the range [0, 1], {r,g,b,a} where a is alpha
    GLfloat colors[VERTICES][4] = {
        {1, 1, 1, 1}, {1, 1, 1, 1},
    };

    // Initialize an array of six unsigned ints to hold our cube's index data
    GLuint indices[LINES][2] = {
        {0, 1},
    };

    // UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)
    // ===================================================

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboWirePositions);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, positions, GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboWireColors);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_COL, colors, GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, LINES * SIZE_LINE, indices, GL_DYNAMIC_DRAW);

    // Check for OpenGL errors
    printGLErrorLog();
}

void MyGLWidget::sampleDrawWire(const glm::mat4 &model, int count) {
    // DRAWING CODE (GENERALLY, ONCE PER FRAME)
    // ========================================

    // Tell the GPU which shader program to use to draw things
    glUseProgram(progWire);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifWireModel, 1, GL_FALSE, &model[0][0]);
    // Also upload the inverse transpose for normal transformation

    // Enable the two vertex data inputs
    glEnableVertexAttribArray(attrWirePositions);
    glEnableVertexAttribArray(attrWireColors);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboWirePositions);
    glVertexAttribPointer(attrWirePositions, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboWireColors);
    glVertexAttribPointer(attrWireColors, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireIndices);

    // Draw the triangles using the data we have given to the GPU
    glDrawElements(GL_LINES, count * 2, GL_UNSIGNED_INT, 0);

    // Tell the GPU we are finished drawing using position and color
    glDisableVertexAttribArray(attrWirePositions);
    glDisableVertexAttribArray(attrWireColors);

    // Check for OpenGL errors
    printGLErrorLog();
}

void MyGLWidget::sampleDrawCube(const glm::mat4 &model) {
    // Number of faces (6 on a cube)
//    static const int FACES = cube->faces->size();
    int FACES = cube->faces->size();

    // Number of triangles (2 per face)
//    static const int TRIANGLES = cube->numTriangles;
    int TRIANGLES = cube->numTriangles;

    // DRAWING CODE (GENERALLY, ONCE PER FRAME)
    // ========================================

    // Tell the GPU which shader program to use to draw things
    glUseProgram(progDiffuse);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifModel, 1, GL_FALSE, &model[0][0]);

    // Also upload the inverse transpose for normal transformation
    const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(model));
    glUniformMatrix4fv(unifModelInvTranspose, 1, GL_FALSE, &modelInvTranspose[0][0]);

    // Enable the two vertex data inputs
    glEnableVertexAttribArray(attrPositions);
    glEnableVertexAttribArray(attrNormals);
    glEnableVertexAttribArray(attrColors);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glVertexAttribPointer(attrPositions, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the positions are: in the normal buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(attrNormals, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glVertexAttribPointer(attrColors, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);

    // Draw the triangles using the data we have given to the GPU
    glPointSize(5.0f);
    glDrawElements(GL_POINTS, TRIANGLES * 3, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, TRIANGLES * 3, GL_UNSIGNED_INT, 0);

    // Tell the GPU we are finished drawing using position and color
    glDisableVertexAttribArray(attrPositions);
    glDisableVertexAttribArray(attrNormals);
    glDisableVertexAttribArray(attrColors);

    // Check for OpenGL errors
    printGLErrorLog();
}

std::string MyGLWidget::readFile(const char *filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Failed to load file: " << filename << std::endl;
        exit(1);
    }
    QString source = QTextStream(&file).readAll();
    std::string ret = source.toStdString();
    //std::cout << ret << std::endl;
    file.close();
    return ret;
}

void MyGLWidget::printGLErrorLog() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char * e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            QString::number(error).toUtf8().constData();
        std::cerr << e << std::endl;
        // Throwing here allows us to use the debugger to track down the error.
        throw;
    }
}

void MyGLWidget::printLinkInfoLog(int prog) {
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void MyGLWidget::printShaderInfoLog(int shader) {
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void MyGLWidget::shaderSetupWire() {
    // Read in the shader program source files
    std::string vertSourceS = MyGLWidget::readFile(":/shaders/wire.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = MyGLWidget::readFile(":/shaders/wire.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadWireVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadWireFrag = glCreateShader(GL_FRAGMENT_SHADER);
    progWire = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadWireVert, 1, &vertSource, NULL);
    glCompileShader   (shadWireVert);
    printShaderInfoLog(shadWireVert);
    // - Wire fragment shader
    glShaderSource    (shadWireFrag, 1, &fragSource, NULL);
    glCompileShader   (shadWireFrag);
    printShaderInfoLog(shadWireFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (progWire, shadWireVert);
    glAttachShader  (progWire, shadWireFrag);
    glLinkProgram   (progWire);
    printLinkInfoLog(progWire);

    // Clean up the shaders now that they are linked
    glDetachShader(progWire, shadWireVert);
    glDetachShader(progWire, shadWireFrag);
    glDeleteShader(shadWireVert);
    glDeleteShader(shadWireFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    attrWirePositions         = glGetAttribLocation (progWire, "vs_Position");
    attrWireColors            = glGetAttribLocation (progWire, "vs_Color");
    unifWireViewProj          = glGetUniformLocation(progWire, "u_ViewProj");
    unifWireModel             = glGetUniformLocation(progWire, "u_Model");

    printGLErrorLog();
}
