#ifndef OPENGLDEMO_H
#define OPENGLDEMO_H

#include <QMainWindow>

namespace Ui
{
class MyMainWindow;
}

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();

private slots:
    void on_actionQuit_triggered();

private:
    Ui::MyMainWindow *ui;
};

#endif // OPENGLDEMO_H
