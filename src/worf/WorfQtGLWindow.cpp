#include "WorfQtGLWindow.h"
#include "WorfQtMainWindow.h"
#include "log.h"
#include <QtGui>
#include <QtOpenGL>

std::vector<WorfQtGLWindow *> WorfQtGLWindow::windows_;

WorfQtGLWindow * WorfQtGLWindow::currentWindow_;

// to specify a custom GL format, do something like:
WorfQtGLWindow::WorfQtGLWindow() : QGLWidget(QGLFormat(QGL::SampleBuffers))
{
    // add this object to the windows vector
    windows_.push_back(this);

    // on construction, make this the current window
    currentWindow_ = this;

    display = NULL;

    continuousRendering_ = false;

    mainWindow_ = NULL;
}

WorfQtGLWindow::~WorfQtGLWindow()
{
    // erase this object from the windows vector
    windows_.erase(std::find (windows_.begin(), windows_.end(), this));

    // if this was the current window, change it to NULL
    if(currentWindow_ == this)
        currentWindow_ = NULL;
}

QSize WorfQtGLWindow::sizeHint() const
{
    return QSize(800, 600);
}

void WorfQtGLWindow::initializeGL()
{

}

void WorfQtGLWindow::paintGL()
{
    camera_.setView(width(), height());

    if(display != NULL)
        display();

    if(mainWindow_ != NULL)
        mainWindow_->display();

    // continuous updating...
    if(continuousRendering_ == true || camera_.speed_ != 0.)
        update();
}

void WorfQtGLWindow::resizeGL(int width, int height)
{
    glViewport (0, 0, width, height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    update();
}

void WorfQtGLWindow::mousePressEvent(QMouseEvent *event)
{
    lastPos_ = event->pos();

    update();
}

void WorfQtGLWindow::mouseReleaseEvent(QMouseEvent *event)
{
    lastPos_ = event->pos();
    camera_.stopMotion();

    update();
}

void WorfQtGLWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos_.x();
    int dy = event->y() - lastPos_.y();

    if(event->buttons() & Qt::LeftButton)
    {
        camera_.motion(1, dx,dy);
    }
    else if(event->buttons() & Qt::MidButton)
    {
        camera_.motion(2, dx,dy);
    }
    else if(event->buttons() & Qt::RightButton)
    {
        camera_.motion(3, dx,dy);
    }

    lastPos_ = event->pos();

    update();
}

void WorfQtGLWindow::setDisplayFunction(void (*function)(void))
{
    put_flog(LOG_DEBUG, "updated display function");
    display = function;
}

void WorfQtGLWindow::push2DMode()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width(), 0, height());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void WorfQtGLWindow::pop2DMode()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void WorfQtGLWindow::drawRectangle(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);

    glVertex2f(x,y);
    glVertex2f(x+w,y);
    glVertex2f(x+w,y+h);
    glVertex2f(x,y+h);

    glEnd();
}

WorfCamera * WorfQtGLWindow::getCamera()
{
    return &camera_;
}

void WorfQtGLWindow::renderText(int x, int y, int size, float r, float g, float b, const char * text)
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);

    glColor4f(r,g,b,1.);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(size);

    QGLWidget::renderText(x,y, QString(text), font);

    glPopAttrib();
}

void WorfQtGLWindow::renderText(double x, double y, double z, int size, float r, float g, float b, const char * text)
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);

    glColor4f(r,g,b,1.);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(size);

    QGLWidget::renderText(x,y,z, QString(text), font);

    glPopAttrib();
}

WorfQtGLWindow * WorfQtGLWindow::getCurrent()
{
    return currentWindow_;
}

void WorfQtGLWindow::setContinuousRendering(bool set)
{
    continuousRendering_ = set;
}

void WorfQtGLWindow::setMainWindow(WorfQtMainWindow * mainWindow)
{
    mainWindow_ = mainWindow;
}
