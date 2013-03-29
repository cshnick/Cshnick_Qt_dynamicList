#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "PluginManager_global.h"
#include <QObject>
#include <QReadWriteLock>
#include <QtXml/qdom.h>

namespace Plugins {

enum Platform {
   linuxOS
    ,macosx
    ,windowsOS
};

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

class PInfoHandlerPrivate;
class PLUGINMANAGERSHARED_EXPORT PInfoHandler
{
public:
    PInfoHandler();
    PInfoHandler(const QDomDocument &data);
    PInfoHandler(const QString &dataFile);
    ~PInfoHandler();
    void setData(const QDomDocument &pData);
    void setFileData(const QString &fileData);
    bool isValid() const;
    operator bool() const {return isValid();}
    void save();

    static QString stringForBool(bool pArgument);
    static bool boolForString(const QString &pArgument);


private:
    PInfoHandlerPrivate *d;

    friend class PInfoHandlerPrivate;
};
} //namespace Plugins

#endif // PLUGINMANAGER_H
