#include "WorfQtMainWindow.h"
#include "WorfQtGLWindow.h"
#include <QtGui>

WorfQtMainWindow::WorfQtMainWindow(const char * name)
{
    setWindowTitle(name);

    qtGLWindow_ = new WorfQtGLWindow();
    qtGLWindow_->setMainWindow(this);
    setCentralWidget(qtGLWindow_);

    keyboard_ = NULL;

    statusBar()->showMessage("Ready");
}

void WorfQtMainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);

    // call keyboard function if it exists
    if(keyboard_ != NULL)
        keyboard_(e->key());

    // member keyboard function
    keyboard(e->key());
}

void WorfQtMainWindow::setKeyboardFunction(void (*function)(int key))
{
    keyboard_ = function;
}

WorfQtGLWindow * WorfQtMainWindow::getGLWindow()
{
    return qtGLWindow_;
}
