#include <QApplication>
#include "TabMgr.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    TabMgr tabMgr;
    (void)tabMgr;
    app.exec();
}
