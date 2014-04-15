#ifndef VERTEX_H
#define VERTEX_H
#include <QListWidgetItem>

class Vertex : public QListWidgetItem {
public:
    Vertex(float, float, float, float, int);

    int id;
    float X;
    float Y;
    float Z;
    float W;

    Vertex(const Vertex &v2);

    void setID(int);

    bool isCurrent;
    bool isEndPoint;

};

#endif // VERTEX_H
