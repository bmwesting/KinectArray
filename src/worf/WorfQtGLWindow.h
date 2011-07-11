#ifndef WORF_QT_GL_WINDOW_H
#define WORF_QT_GL_WINDOW_H

#include "WorfCamera.h"
#include <QGLWidget>

class WorfQtMainWindow;

class WorfQtGLWindow : public QGLWidget
{

    public:

        // list of all active windows
        static std::vector<WorfQtGLWindow *> windows_;

        // current window
        static WorfQtGLWindow * currentWindow_;

        QPoint lastPos_;

        WorfCamera camera_;

        // display function
        void (* display)(void);

        // continuous rendering flag
        bool continuousRendering_;

        // parent WorfQtMainWindow, if it exists. used to call display() functions in the parent
        WorfQtMainWindow * mainWindow_;

        WorfQtGLWindow();
        ~WorfQtGLWindow();

        QSize sizeHint() const;
        void setDisplayFunction(void (*function)(void));
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void push2DMode();
        void pop2DMode();
        static void drawRectangle(float x, float y, float w, float h);
        WorfCamera * getCamera();
        void renderText(int x, int y, int size, float r, float g, float b, const char * text);
        void renderText(double x, double y, double z, int size, float r, float g, float b, const char * text);
        static WorfQtGLWindow * getCurrent();
        void setContinuousRendering(bool set);
        void setMainWindow(WorfQtMainWindow * mainWindow);
};

#endif
