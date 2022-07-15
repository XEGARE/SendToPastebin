#include "sendtopastebin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SendToPastebin w;
	w.setAttribute(Qt::WA_TranslucentBackground);
	w.setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    w.show();

    return a.exec();
}
