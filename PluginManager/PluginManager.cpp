#include "PluginManager.h"

#include <QtGui>
#include <QtXml>

static const QString pluginPath1 = "../Plugins/TstDocGenerator1";
static const QString pluginPath2 = "../Plugins/TstGenerator";
static const QString plugInfoSuffix = ".pinfo";

static const QString tEnabled = "Enabled";
static const QString tTopTag = "misc";
static const QString tDisplayName = "displayname";
static const QString tIconSource = "icon";
static const QString tPluginPath = "pathToPlugin";

static const QString argTrue = "true";
static const QString argFalse = "false";

static const QString dDefaultDisplayName = "Undefined pluglin";

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
                qDebug() << nextPlugin.fileName();
                QPluginLoader loader(nextPlugin.absoluteFilePath());
//                QFile file(nextPlugin.absoluteFilePath().replace("." + nextPlugin.completeSuffix(), plugInfoSuffix));
//                file.open(QIODevice::WriteOnly);
//                QXmlStreamWriter writer(&file);
//                writer.setAutoFormatting(true);
//                writer.writeStartDocument();
//                writer.writeStartElement(tTopTag);
//                writer.writeTextElement(tPluginPath, "true");
//                writer.writeTextElement(tEnabled, "true");
//                writer.writeTextElement(tDisplayName, "und");
//                writer.writeTextElement(tIconSource, "und");

//                writer.writeEndElement();
//                writer.writeEndDocument();
//                file.close();

                loader.load();
                qDebug() << "Plugin loaded" << loader.isLoaded();
                loader.instance();
                loader.unload();
                qDebug() << "After unload" << loader.isLoaded();
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
    QList<QPluginLoader> mPlugins;

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
        , mEnabled(false)
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

//                qDebug() << mData.firstChild().firstChildElement(tEnabled).text();
                QString enabled = mData.documentElement().firstChildElement(tEnabled).text();
                mEnabled = enabled.isEmpty() ? true : q->boolForString(enabled);
                QString displayName = mData.documentElement().firstChildElement(tDisplayName).text();
                mDisplayName = displayName.isEmpty() ? dDefaultDisplayName : displayName;
                mIconSource = mData.documentElement().firstChildElement(tIconSource).text();
                QString pluginFile = mData.documentElement().firstChildElement(tPluginPath).text();
                mPluginFileName = QFileInfo(pluginFile).exists() ? pluginFile : QString();
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

private:
    PInfoHandler *q;
    QDomDocument mData;
    QString mPluginFileName;
    bool mEnabled;
    QString mDisplayName;
    QString mIconSource;

    friend class PInfoHandler;
};

PInfoHandler::PInfoHandler()
    :d(new PInfoHandlerPrivate(this))
{
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

bool PInfoHandler::isValid() const
{
    return !d->mData.isNull() && QFileInfo(d->mPluginFileName).exists();
}

void PInfoHandler::save()
{
    d->saveMembers();
}

QString PInfoHandler::stringForBool(bool pArgument)
{
    return pArgument ? argTrue : argFalse;
}

bool PInfoHandler::boolForString(const QString &pArgument)
{
    return (pArgument == argTrue) ? true : false;
}

} // namespace Plugins
