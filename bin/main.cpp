#include <QtGui>

#include "tstgenerator.h"
#include "DocumentManager.h"
#include "tstdocgenerator1.h"
#include "PluginManager.h"
#include "ICommonInterface.h"
#include "IDocumentGenerator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Plugins::PluginManager plugins;
    QList<Plugins::ICommonInterface*> commonPlugins = Plugins::PluginManager::getObjects<Plugins::ICommonInterface*>();
    QList<Docs::IDocumentGenerator*> documentGenerators = Plugins::PluginManager::getObjects<Docs::IDocumentGenerator*>();
    QList<DPImageServicer*> servicers = Plugins::PluginManager::getObjects<DPImageServicer*>();

     //    QPixmapCache::setCacheLimit(2048);

//    manager.installPageGenerator(new TstGenerator);
//    manager.setVisible(true);
    qDebug() << "started";
    Docs::DocumentManager docMan;
    docMan.setVisible(true);
    return a.exec();
}
