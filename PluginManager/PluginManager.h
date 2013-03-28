#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "PluginManager_global.h"
#include <QObject>
#include <QReadWriteLock>

namespace Plugins {

class PluginManagerPrivate;
class PLUGINMANAGERSHARED_EXPORT PluginManager : public QObject
{
    Q_OBJECT

public:
    static PluginManager *instance() {return mInstance;}
    PluginManager();
    ~PluginManager();

private:
    PluginManagerPrivate *d;
    static PluginManager *mInstance;
    mutable QReadWriteLock mLocker;

    friend class PluginManagerPrivate;
};

} //namespace Plugins

#endif // PLUGINMANAGER_H
