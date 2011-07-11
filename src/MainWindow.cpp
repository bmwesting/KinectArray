#include "MainWindow.h"
#include "worf/log.h"
#include <QtGui>

MainWindow::MainWindow(): WorfQtMainWindow("KinectArray")
{
    put_flog(LOG_DEBUG, "");
}
