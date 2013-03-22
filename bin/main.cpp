#include <QtGui>

#include "dynpicturesmodel.h"
#include "tstgenerator.h"
#include "DocumentManager.h"

enum crole
{
    CustomRole = 0
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



//    QPixmapCache::setCacheLimit(2048);
    DynPicturesManager manager(QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236"));
    manager.installPageGenerator(new TstGenerator);
    manager.setVisible(true);
    qDebug() << "started";
    Docs::DocumentManager docMan;
    docMan.print();
    return a.exec();
}
