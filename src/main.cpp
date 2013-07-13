/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#include <QtGui/QApplication>
#include "ui.h"


int main(int argc, char *argv[])
{
    using namespace circles;

    QApplication a(argc, argv);

    ReflectiveCirclesUI w;
    w.show();

    return a.exec();
}
