#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "PluginManager_global.h"

#include <QObject>
#include <QReadWriteLock>
#include <QtXml/qdom.h>
#include <QPointer>
#include <QPluginLoader>

namespace Plugins {


enum Platform {
   linuxOS
    ,macosx
    ,windowsOS
};



class PInfoHandler;

class PluginManagerPrivate;
class PLUGINMANAGERSHARED_EXPORT PluginManager : public QObject
{
    Q_OBJECT

public:

    typedef QList<PInfoHandler> PInfoList;
    static PluginManager *instance() {return mInstance;}

    PluginManager();
    ~PluginManager();

    static QList<QPluginLoader*> getAllPlugins();
    static PInfoList pluginMetas();

    static QString suffixForCurrentPlatform();
    static QString adjustedPluginName(const QString &givenName);

    template <typename T>
    static QList<T> getObjects() {
        QList<T> result;
        foreach (QPluginLoader *loader, getAllPlugins()) {
            T nextCandidate = qobject_cast<T>(loader->instance());
            if (nextCandidate) {
                result.append(nextCandidate);
            }
        }
        return result;
    }

    static bool removeObject(QObject *object);

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
    PInfoHandler(const PInfoHandler &other);
    PInfoHandler(const QDomDocument &data);
    PInfoHandler(const QString &dataFile);
    ~PInfoHandler();
    void setData(const QDomDocument &pData);
    void setFileData(const QString &fileData);
    void setEnabled(bool pEnabled);
    bool isEnabled() const;
    bool isValid() const;
    operator bool() const {return isValid();}
    void save();
    QString absolutePluginPath() const;
    QString displayName() const;

    static QString stringForBool(bool pArgument);
    static bool boolForString(const QString &pArgument);
    PInfoHandler &operator=(const PInfoHandler &other);


private:
    PInfoHandlerPrivate *d;

    friend class PInfoHandlerPrivate;
};
} //namespace Plugins

Q_DECLARE_METATYPE(Plugins::PInfoHandler)


#endif // PLUGINMANAGER_H
