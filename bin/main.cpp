#include <QtGui>

#include "dynpicturesmodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DynPicturesManager manager(QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236"));
    manager.setVisible(true);
    qDebug() << "started";
    return a.exec();
}
