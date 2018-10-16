#include <QApplication>
#include "TabMgr.h"

int main(int argc, char **argv)
{
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");
    QApplication app(argc, argv);
    TabMgr tabMgr;
    (void)tabMgr;
    app.exec();
}
