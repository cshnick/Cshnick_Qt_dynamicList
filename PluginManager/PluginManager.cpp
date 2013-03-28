#include "PluginManager.h"

#include <QtGui>

static const QString pluginPath1 = "../Plugins/TstDocGenerator1";
static const QString pluginPath2 = "../Plugins/TstGenerator";
static const QString plugInfoSuffix = ".pinfo";

namespace Plugins {

PluginManager *PluginManager::mInstance = 0;

class PluginManagerPrivate {
public:
    PluginManagerPrivate(PluginManager *pq)
        : q(pq)
    {
        mSearchDirs.append(pluginPath1);
        mSearchDirs.append(pluginPath2);
        loadPlugins();

    }

    void loadPlugins()
    {
        foreach (QUrl nextDir, mSearchDirs) {
            QDir dir(nextDir.toLocalFile());
            Q_ASSERT(dir.exists());
            QFileInfoList pluginCandidates = dir.entryInfoList(QStringList() << suffixForCurrentPlatformPatern(), QDir::Files | QDir::Readable | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            foreach (QFileInfo nextPlugin, pluginCandidates) {
                const QString pInfo;
                qDebug() << nextPlugin.fileName();
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

    QString suffixForCurrentPlatformPatern()
    {
#if defined Q_OS_LINUX
        return "*.so*";
#elif defined Q_WS_MAC
        return "*.dll^";
#elif defined Q_OS_WIN
        return "*.dlib^";
#endif
        return QString();
    }

private:
    PluginManager *q;
    QList<QUrl> mSearchDirs;
    QList<QObject*> mPlugins;

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

class PInfoHandlerPrivate
{
    PInfoHandlerPrivate(PInfoHandler *pq)
        :q(pq)
    {
    }

    void parseFile(const QString &fileName)
    {
        Q_ASSERT(QFileInfo(fileName).exists());

        QDomDocument xmlDom;
        QFile inFile(fileName);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString domString(inFile.readAll());

            int errorLine = 0; int errorColumn = 0;
            QString errorStr;

            if (xmlDom.setContent(domString, true, &errorStr, &errorLine, &errorColumn)) {
                mData = xmlDom;
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

    void save()
    {
        Q_ASSERT(QFileInfo(mFileName).exists());

        QFile stream(mFileName);
        if (!stream.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open " << mFileName << "for writing PInfoHandlerPrivate::save()";
            return;
        }
        QTextStream txtStream(&stream);

        mData.save(txtStream, 0);
    }

private:
    PInfoHandler *q;
    QDomDocument mData;
    QString mFileName;

    friend class PInfoHandler;
};

PInfoHandler::PInfoHandler(const QString &fileName)
    :d(new PInfoHandlerPrivate(this))
{
    d->mFileName = fileName;
}
PInfoHandler::PInfoHandler(const QString &fileName, const QDomDocument &data)
    :d(new PInfoHandlerPrivate(this))
{
    d->mFileName = fileName;
    d->mData = data;
}
PInfoHandler::PInfoHandler(const QString &fileName, const QString &data)
    :d(new PInfoHandlerPrivate(this))
{
    d->mFileName = fileName;
    d->parseFile(data);
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

bool PInfoHandler::isValid() const
{
    return !d->mData.isNull() && QFileInfo(d->mFileName).exists();
}

void PInfoHandler::save()
{
    d->save();
}

} // namespace Plugins
