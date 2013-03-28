#include "PluginManager.h"

#include <QtGui>

static const QString DocGeneratorPath_1 = "../TstDocGenerator1/libTstDocGenerator1.so";

namespace Plugins {

class PluginManagerPrivate {
public:
    PluginManagerPrivate(PluginManager *pq)
        : q(pq)
    {

    }

    void loadPlugins()
    {
//        QPluginLoader loader_1(DocGeneratorPath_1);
//        if (!loader_1.fileName().isEmpty()) {
//            qDebug() << "invalid path" << DocGeneratorPath_1 << "for plugin";
//        }
//        loader_1.load();

//        TstDocGenerator1 *generator = static_cast<TstDocGenerator1*>(loader_1.instance());
//        QAction *action = generator->associatedAction();
//        QObject::connect(action, SIGNAL(toggled(bool)), q, SLOT(actionMenuChecked(bool)));
//        mPluginsMenu->addAction(action);
//        bool generator_1Exists_inSettings = mSettings->value(DocGeneratorPath_1).toBool();
//        action->setChecked(generator_1Exists_inSettings);
//        if (generator_1Exists_inSettings) {
//            generator->createNodeTree();
//            q->registerGenerator(generator);

//        } else {
//            loader_1.unload();
//        }
    }

private:
    PluginManager *q;
};

PluginManager::PluginManager()
    : d(new PluginManagerPrivate(this))
{
    mInstance = this;
}
PluginManager::~PluginManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

} // namespace Plugins
