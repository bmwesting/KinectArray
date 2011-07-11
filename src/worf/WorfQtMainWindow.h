#ifndef WORF_QT_MAIN_WINDOW_H
#define WORF_QT_MAIN_WINDOW_H

#include "WorfQtGLWindow.h"
#include <QMainWindow>

class WorfQtGLWindow;

class WorfQtMainWindow : public QMainWindow {

    public:

        WorfQtGLWindow * qtGLWindow_;

        // keyboard callback, external function set by the user
        void (* keyboard_)(int key);

        // member keyboard function, implemented in subclasses
        virtual void keyboard(int /*key*/) { }

        // member display function, implemented in subclasses
        virtual void display() { }

        WorfQtMainWindow(const char * name);

        void keyPressEvent(QKeyEvent *event);
        void setKeyboardFunction(void (*function)(int key));
        WorfQtGLWindow * getGLWindow();
};

#endif
