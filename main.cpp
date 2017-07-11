#include <QCoreApplication>
#include "saleaesocket.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    saleaesocket cTest;
    cTest.host = QString("localhost");
    cTest.port = 10429;

    cTest.Connect();

    cTest.~saleaesocket();

    return app.exec();
}
