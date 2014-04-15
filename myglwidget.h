#ifndef MYGLWIDGET
#define MYGLWIDGET

#include <QGLWidget>
#include <QGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QTimer>

// Use radians everywhere (no degrees!)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "camera.h"
#include "mesh.h"
#include "face.h"

class MyGLWidget : public QGLWidget, QGLFunctions
{
    Q_OBJECT

public:
    MyGLWidget(QWidget* parent);
    ~MyGLWidget();
    glm::mat4 getView(glm::vec3 eye, glm::vec3 target, glm::vec3 up);
    glm::mat4 getProj(float, float, float, float);

    glm::mat4 cubeTransformation;
    glm::mat4 myViewMat;
    glm::mat4 myProjMat;
    glm::mat4 myViewProj;

    Camera* camera;

    bool isPlaying;
    float cubeRotation;
    float prevMouseX;
    float prevMouseY;

    int cubeRotationDirection;

    Mesh* cube;

    QListWidget* faceList;
    QListWidget* halfEdgeList;
    QListWidget* vertexList;

    Face* currentFace;
    int faceIndex;

    HalfEdge* currentHalfEdge;
    int halfEdgeIndex;

    Vertex* currentVertex;
    int vertexIndex;    

    float newRed;
    float newGreen;
    float newBlue;

    float newX;
    float newY;
    float newZ;

    glm::vec3 ray_wor;

    bool mousePressed;

protected:
    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void traverse();
    void refreshLists();

public slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();

    void startRotation();
    void stopRotation();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void zoomIn();
    void zoomOut();

    void setCurrentFace(int);
    void splitCurrentFace();
    void setCurrentVertex(int);
    void setRed(double);
    void setGreen(double);
    void setBlue(double);
    void changeColor();

    void setX(double);
    void setY(double);
    void setZ(double);
    void changePos();
    void setCurrentHalfEdge(int);

    void insertVertex();
    void deleteVertex();
    void insertEdge();


private:
    /// Timer for animation
    QTimer *timer;

    // Buffer objects for positions, colors, normals, indices
    QOpenGLVertexArrayObject *vao;
    GLuint vboPositions;
    GLuint vboNormals;
    GLuint vboColors;
    GLuint vboIndices;
    GLuint vboWirePositions;
    GLuint vboWireColors;
    GLuint vboWireIndices;

    // Shader attribute locations
    GLuint attrPositions;
    GLuint attrNormals;
    GLuint attrColors;

    // Shader uniform locations
    GLuint unifViewProj;
    GLuint unifModel;
    GLuint unifModelInvTranspose;

    /// Shaders and shader programs
    GLuint progDiffuse;

    /// Print out some information about the current OpenGL context
    void debugContextVersion();
    /// Set up the shaders
    void shaderSetup();
    /// Upload the sample cube
    void sampleUploadCube();
    /// Draw the sample cube
    void sampleDrawCube(const glm::mat4 &modelmat);

    void shaderSetupWire();
    GLuint attrWirePositions;
    GLuint attrWireColors;
    GLuint unifWireViewProj;
    GLuint unifWireModel;
    GLuint progWire;
    void sampleUploadWire();
    void sampleDrawWire(const glm::mat4 &modelmat, int count);

    /// Read a text file or resource into a string.
    std::string readFile(const char* fileName);

    /// Check for GL errors, and print and crash if there is one.
    void printGLErrorLog();

    /// Check for shader program linking errors, and print if there is one.
    void printLinkInfoLog(int prog);

    /// Check for shader compilation errors, and print if there is one.
    void printShaderInfoLog(int shader);
};

#endif
