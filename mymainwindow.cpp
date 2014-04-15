#include "mymainwindow.h"
#include "ui_mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMainWindow)
{
    ui->setupUi(this);
    ui->widget->setFocus();

    ui->widget->faceList = ui->faceListWidget;
    ui->widget->vertexList = ui->vertexListWidget;
    ui->widget->halfEdgeList = ui->halfEdgeListWidget;

    for (int i = 0; i < ui->widget->cube->faces->size(); i++) {
        ui->faceListWidget->addItem(ui->widget->cube->faces->operator [](i));
    }
    for (int i = 0; i < ui->widget->cube->halfEdges->size(); i++) {
        ui->halfEdgeListWidget->addItem(ui->widget->cube->halfEdges->operator [](i));
    }
    for (int i = 0; i < ui->widget->cube->vertices->size(); i++) {
        ui->vertexListWidget->addItem(ui->widget->cube->vertices->operator [](i));
    }



}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::on_actionQuit_triggered()
{
    close();
}
