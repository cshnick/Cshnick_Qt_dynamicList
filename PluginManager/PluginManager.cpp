#include "PluginManager.h"
#include "ICommonInterface.h"

#include <QtGui>
#include <QtXml>

static const QString pluginPath1 = "../Plugins/TstDocGenerator1";
static const QString pluginPath2 = "../Plugins/TstGenerator";
static const QString pluginInfoPath = "../Plugins";
static const QString plugInfoSuffix = ".pinfo";

static const QString tEnabled = "Enabled";
static const QString tTopTag = "misc";
static const QString tDisplayName = "displayname";
static const QString tIconSource = "icon";
static const QString tPluginPath = "pathToPlugin";

static const QString argTrue = "true";
static const QString argFalse = "false";

static const QString lLibPrefix = "lib";
static const QString lLibLinuxSuffix = ".so";
static const QString lLibMacXSuffix = ".dylib";
static const QString lLibWindowsSuffix = ".dll";

static const QString dDefaultDisplayName = "Undefined pluglin";

namespace Plugins {

PluginManager *PluginManager::mInstance = 0;

class PluginManagerPrivate {
public:
    PluginManagerPrivate(PluginManager *pq)
        : q(pq)
    {
        mSearchDirs.append(pluginInfoPath);
        loadPlugins();
    }

    void loadPlugins()
    {
        foreach (QUrl nextDir, mSearchDirs) {
            QDir dir(nextDir.toLocalFile());
            Q_ASSERT(dir.exists());

            QFileInfoList pluginCandidates = dir.entryInfoList(QStringList() << "*"+plugInfoSuffix, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            foreach (QFileInfo nextPluginInfo, pluginCandidates) {
                qDebug() << nextPluginInfo.fileName();

                PInfoHandler hl(nextPluginInfo.absoluteFilePath());
                if (hl) {
                    mInfos.append(hl);
                    if (hl.isEnabled()) {
                        QPluginLoader *loader = new QPluginLoader(hl.absolutePluginPath());
                        loader->load();
                        if (!loader->isLoaded()) {
                            qDebug() << "Can't load" << hl.absolutePluginPath();
                            continue;
                        }
                        ICommonInterface *commonPlugin = qobject_cast<ICommonInterface*>(loader->instance());
                        if (!commonPlugin) {
                            qDebug() << "plugin" << hl.absolutePluginPath() << "does not implement the plugin IcommonInterface";
                            loader->unload();
                            continue;
                        }
                        commonPlugin->setPluginMeta(hl);
                        commonPlugin->setLoader(loader);

                        qDebug() << "Plugin loaded" << loader->isLoaded();
                        //                                loader.instance();
//                        loader->unload();
                        qDebug() << "After unload" << loader->isLoaded();

                        mPluginLoaders.append(loader);
                    }
                }
            }
        }

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

    static QString adjustedPluginName(const QString &pFileName) {
        QString filename = pFileName.section("/", -1);
        if (filename.isNull()) {
            return QString();
        }

        if (!filename.toLower().startsWith("lib")) {
            filename.prepend("lib");
        }
        if (!filename.endsWith(suffixForCurrentPlatformPatern())) {
            int firstNumberInex = -1;

            //checking library trailing version names for unix like
            QStringList extList = filename.split(".");
            for (int i = extList.count() - 1; i >= 0; i--) {
                bool ok = false;
                extList.at(i).toInt(&ok);
                if (ok) {
                    firstNumberInex = i;
                } else {
                    break;
                }
            }
            if (firstNumberInex != -1) {
                if (extList.at(firstNumberInex - 1) != suffixForCurrentPlatformPatern().remove(".")) {
                    return QString();
                }
            } else {
                filename.append(suffixForCurrentPlatformPatern());
            }
        }

        return pFileName.section("/", 0, -2) + "/" + filename;
    }

    static QString suffixForCurrentPlatformPatern()
    {
#if defined Q_OS_LINUX
        return lLibLinuxSuffix;
#elif defined Q_WS_MAC
        return lLibMacXSuffix;
#elif defined Q_OS_WIN
        return lLibWindowsSuffix;
#endif
        return QString();
    }

private:
    PluginManager *q;
    QList<QUrl> mSearchDirs;
    QList<QPluginLoader*> mPluginLoaders;
    PluginManager::PInfoList mInfos;

    friend class PluginManager;

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

QList<QPluginLoader*> PluginManager::getAllPlugins()
{
    if (mInstance) {
        return mInstance->d->mPluginLoaders;
    }

    return QList<QPluginLoader*>();
}

PluginManager::PInfoList PluginManager::pluginMetas()
{
    if (mInstance) {
        return mInstance->d->mInfos;
    }

    return PInfoList();
}

QString PluginManager::suffixForCurrentPlatform()
{
    return PluginManagerPrivate::suffixForCurrentPlatformPatern();
}

QString PluginManager::adjustedPluginName(const QString &givenName)
{
    return PluginManagerPrivate::adjustedPluginName(givenName);
}

bool PluginManager::removeObject(QObject *object)
{
    ICommonInterface *commonPlugin = qobject_cast<ICommonInterface*>(object);
    if (!commonPlugin) {
        qDebug() << "can't cast object to ICommonInterface for deletion";
        return false;
    }

    foreach (QPluginLoader *loader, mInstance->d->mPluginLoaders) {
        if (loader && loader == commonPlugin->loader()) {
            loader->unload();
            if (loader->isLoaded()) {
                return false;
            }
            mInstance->d->mPluginLoaders.removeAll(loader);
            delete loader;
        }
    }

    return true;
}

class PInfoHandlerPrivate
{
    PInfoHandlerPrivate(PInfoHandler *pq)
        :q(pq)
        , mEnabled(false)
    {
    }

    void parseFile(const QString &fileName)
    {
        Q_ASSERT(QFileInfo(fileName).exists());

        mOwnPath = QUrl::fromLocalFile(fileName);
        QDomDocument xmlDom;
        QFile inFile(fileName);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString domString(inFile.readAll());

            int errorLine = 0; int errorColumn = 0;
            QString errorStr;

            if (xmlDom.setContent(domString, true, &errorStr, &errorLine, &errorColumn)) {
                mData = xmlDom;

                QString enabled = mData.documentElement().firstChildElement(tEnabled).text();
                mEnabled = enabled.isEmpty() ? true : q->boolForString(enabled);
                QString displayName = mData.documentElement().firstChildElement(tDisplayName).text();
                mDisplayName = displayName.isEmpty() ? dDefaultDisplayName : displayName;
                mIconSource = mData.documentElement().firstChildElement(tIconSource).text();
                mPluginFileName = mData.documentElement().firstChildElement(tPluginPath).text();
                Q_ASSERT(QFileInfo(absolutePluginPath()).exists());

            } else {
                qDebug() << "Error reading content of " << fileName << endl
                         << "Error:"  << inFile.errorString()
                         << "Line:"   << errorLine
                         << "Column:" << errorColumn;
            }
            inFile.close();
        } else {
            qDebug() << "Error reading" << fileName << endl
                     << "Error:" << inFile.errorString();
        }
    }

    void saveDom()
    {
        Q_ASSERT(QFileInfo(mPluginFileName).exists());

        QFile stream(mPluginFileName);
        if (!stream.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open " << mPluginFileName << "for writing PInfoHandlerPrivate::saveDom()";
            return;
        }
        QTextStream txtStream(&stream);

        mData.save(txtStream, 0);
        stream.close();
    }

    void saveMembers()
    {
        QFile file(mPluginFileName);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open " << mPluginFileName << "for writing PInfoHandlerPrivate::saveMembers()";
            return;
        }

        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement(tTopTag);
            writer.writeTextElement(tPluginPath, mPluginFileName);
            writer.writeTextElement(tEnabled, q->stringForBool(mEnabled));
            writer.writeTextElement(tDisplayName, mDisplayName);
            writer.writeTextElement(tIconSource, mIconSource);
        writer.writeEndElement();
        writer.writeEndDocument();
        file.close();
    }

    QString absolutePluginPath() const
    {
        QString pluginRelativePath = PluginManager::adjustedPluginName(mPluginFileName);
        if (pluginRelativePath.isNull()) {
            return QString();
        }

        return QFileInfo(mOwnPath.toLocalFile()).dir().absolutePath() + "/" + pluginRelativePath;
    }

private:
    PInfoHandler *q;
    QDomDocument mData;
    QString mPluginFileName;
    bool mEnabled;
    QString mDisplayName;
    QString mIconSource;
    QUrl mOwnPath;

    friend class PInfoHandler;
};

PInfoHandler::PInfoHandler()
    :d(new PInfoHandlerPrivate(this))
{
}

PInfoHandler::PInfoHandler(const PInfoHandler &other)
    : d(new PInfoHandlerPrivate(*other.d))
{
    d->q = this;
}

PInfoHandler::PInfoHandler(const QDomDocument &data)
    :d(new PInfoHandlerPrivate(this))
{
    d->mData = data;
}
PInfoHandler::PInfoHandler(const QString &dataFile)
    :d(new PInfoHandlerPrivate(this))
{
    d->parseFile(dataFile);
}

PInfoHandler::~PInfoHandler()
{
    if (d) {
        delete d;
        d = 0;
    }
}
void PInfoHandler::setData(const QDomDocument &pData)
{
    d->mData = pData;
}

void PInfoHandler::setFileData(const QString &fileData)
{
    d->parseFile(fileData);
}

void PInfoHandler::setEnabled(bool pEnabled)
{
    d->mEnabled = pEnabled;
}

bool PInfoHandler::isEnabled() const
{
    return d->mEnabled;
}

bool PInfoHandler::isValid() const
{
    return !d->mData.isNull() && QFileInfo(absolutePluginPath()).exists();
}

void PInfoHandler::save()
{
    d->saveMembers();
}

QString PInfoHandler::absolutePluginPath() const
{
    return d->absolutePluginPath();
}

QString PInfoHandler::displayName() const
{
    return d->mDisplayName;
}

QString PInfoHandler::stringForBool(bool pArgument)
{
    return pArgument ? argTrue : argFalse;
}

bool PInfoHandler::boolForString(const QString &pArgument)
{
    return (pArgument == argTrue) ? true : false;
}

PInfoHandler &PInfoHandler::operator=(const PInfoHandler &other)
{
    *d = *other.d;
    d->q = this;
    return *this;
}

} // namespace Plugins
