#include <QtGui>

#include "ThumbnailManager.h"
#include "tstgenerator.h"
#include "DocumentManager.h"
#include "tstdocgenerator1.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //    QPixmapCache::setCacheLimit(2048);
    DynPicturesManager manager(QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236"));
//    manager.installPageGenerator(new TstGenerator);
//    manager.setVisible(true);
    qDebug() << "started";
    Docs::DocumentManager docMan;
    docMan.setVisible(true);
    return a.exec();
}
