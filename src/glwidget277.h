#ifndef GLWIDGET277_H
#define GLWIDGET277_H

#include <utils.h>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_2_Core>
#include <QTimer>


class GLWidget277
    : public QOpenGLWidget,
      public QOpenGLFunctions_3_2_Core
{
    Q_OBJECT

private:
    /// Timer for drawing new frames
    QTimer timer;

public:
    GLWidget277(QWidget *parent);
    ~GLWidget277();

    void debugContextVersion();

private slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();
};


#endif // GLWIDGET277_H
