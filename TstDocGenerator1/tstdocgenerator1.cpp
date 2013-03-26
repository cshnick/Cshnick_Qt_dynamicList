#include "tstdocgenerator1.h"

#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include "Node.h"

static const QString tRDF = "RDF";
static const QString tDescription = "Description";

static const QString tTitle = "title";
static const QString tType = "type";
static const QString tDate = "date";
static const QString tFormat = "format";
static const QString tIdentifer = "indentifer";
static const QString tVersion = "version";
static const QString tSize = "size";
static const QString tUpdatedAt = "updated-at";
static const QString tSessionTitle = "sessionTitle";
static const QString tSessionAuthors = "sessionAuthors";
static const QString tSessionObjectives = "sessionObjectives";
static const QString tSessionKeywords = "sessionKeywords";
static const QString tSessionGradeLevel = "sessionGradeLevel";
static const QString tSessionSubjects = "sessionSubjects";
static const QString tSessionType = "sessionType";
static const QString tSessionLicence = "sessionLicence";

class MetaDataHandler {
public:
    MetaDataHandler(const QDomDocument &doc = QDomDocument())
        : mData(doc)
    {
    }
public:

    QDomElement takeDataContainerElement()
    {
        QDomElement rootElement  = mData.documentElement();
        if (rootElement.tagName() != tRDF) {
            qDebug() << "Firts element within metadata.rdf has't \"RDC\" tag.";
            return QDomElement();
        }
        QDomElement descElement = rootElement.firstChildElement(tDescription);
        if (descElement.isNull()) {
            qDebug() << "There is no element within metadata.rdf that has a \"Description\" tag.";
        }

        return descElement;
    }

    QString queryData(const QString &key)
    {
        QDomElement requiredElement = takeDataContainerElement().firstChildElement(key);
        if (requiredElement.isNull()) {
            qDebug() << "There is no element within metadata.rdf acceptable for the \"" + key + "\" tag.";
            return QString();
        }

        return requiredElement.text();
    }

    void insertData(const QString &key, const QString &data)
    {
        QDomElement descElement = takeDataContainerElement();
        if (descElement.isNull()  ) {
            return;
        }

        QDomElement requiredElement = descElement.firstChildElement(key);
        if (!requiredElement.isNull()) {
            descElement.removeChild(requiredElement);
        }

        requiredElement = mData.createElement(key);
        QDomText text = mData.createTextNode(data);
        requiredElement.appendChild(text);

        descElement.appendChild(requiredElement);
    }

    void print()
    {
        QDomElement nextDataElement = takeDataContainerElement().firstChildElement();
        qDebug() << "Printing metadata";
        while (!nextDataElement.isNull()) {
            qDebug() << "<" << nextDataElement.tagName() << ">" << nextDataElement.text();
            nextDataElement = nextDataElement.nextSiblingElement();
        }
    }

    QString queryName(const QString &defaultValue = QString())
    {
        QString result = queryData(tTitle);
        if (result.isNull()) {
            result = defaultValue;
        }

        return result;
    }

    QString queryDir(const QString &defaultValue = QString())
    {
        QString result = queryData(tType);
        if (result.isNull()) {
            result = defaultValue;
        }

        return result;
    }

    bool isValidMeta() const
    {
        return true;
    }

private:
    QDomDocument mData;
};

struct DocumentNodePrivate
{
    MetaDataHandler hl;

    friend class DocumentNode;
};

DocumentNode::DocumentNode(QObject *parent)
    : Docs::Node(parent)
    , d(new DocumentNodePrivate)
{
}
DocumentNode::~DocumentNode()
{
    if (d) {
        delete d;
    }
}

int DocumentNode::type() const
{
    return DocumentNodeType;
}

QString DocumentNode::displayName() const
{
    return "Undefined";
}

void DocumentNode::setMetadata(const QDomDocument &data)
{
    d->hl = MetaDataHandler(data);
}

class TstDocGenerator1Private
{
public:
    TstDocGenerator1Private(TstDocGenerator1 *pq)
        : q(pq)
        , mRootNode(0)
        , mMyDocumentsNode(0)
        , mUntitledDocumentsNode(0)
        , mModelsNode(0)
        , mTrashNode(0)
    {
        mRootPath = QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document");
    }
    ~TstDocGenerator1Private()
    {
    }
    QString metaDataFilePath(const QString &pDir)
    {
        QString result = pDir + "/metadata.rdf";
        Q_ASSERT(QFileInfo(result).exists());
        return result;
    }
    QDomDocument metadataFromDir(const QString metadataDir) {

        QString contentFile(metaDataFilePath(metadataDir));
        QDomDocument xmlDom;
        QFile inFile(contentFile);
        if (inFile.open(QIODevice::ReadOnly)) {
            QString domString(inFile.readAll());

            int errorLine = 0; int errorColumn = 0;
            QString errorStr;

            if (xmlDom.setContent(domString, true, &errorStr, &errorLine, &errorColumn)) {
//                loadFolderTreeFromXml("", xmlDom.firstChildElement());
            } else {
                qDebug() << "Error reading content of " << contentFile << endl
                         << "Error:"  << inFile.errorString()
                         << "Line:"   << errorLine
                         << "Column:" << errorColumn;
            }
            inFile.close();
        } else {
            qDebug() << "Error reading" << contentFile << endl
                     << "Error:" << inFile.errorString();
        }

        return xmlDom;
    }

    void createConstantNodes() {
        mRootNode = new Docs::GeneratorNode(q);
        mMyDocumentsNode = new Docs::CatalogNode;
        mMyDocumentsNode->setData("MyDocuments", Docs::displayNameRole);
        mUntitledDocumentsNode = new Docs::CatalogNode;
        mUntitledDocumentsNode->setData("UntitledDocuments", Docs::displayNameRole);
        mModelsNode = new Docs::CatalogNode;
        mModelsNode->setData("Models", Docs::displayNameRole);
        mTrashNode = new Docs::CatalogNode;
        mTrashNode->setData("Trash", Docs::displayNameRole);

        mRootNode->addChild(mMyDocumentsNode);
            mMyDocumentsNode->addChild(mUntitledDocumentsNode);
        mRootNode->addChild(mModelsNode);
        mRootNode->addChild(mTrashNode);

//        mRootNode->setDocGenerator(q);
    }

private:
    TstDocGenerator1 *q;
    Docs::GeneratorNode *mRootNode;
    Docs::CatalogNode *mMyDocumentsNode;
    Docs::CatalogNode *mUntitledDocumentsNode;
    Docs::CatalogNode *mModelsNode;
    Docs::CatalogNode *mTrashNode;

    QUrl mRootPath;


    friend class TstDocGenerator1;

};

TstDocGenerator1::TstDocGenerator1(QObject *parent)
    : Docs::DocumentGenerator(parent)
    , d(new TstDocGenerator1Private(this))
{

}

QString TstDocGenerator1::displayText() const
{
    return "DocumentGenerator1";
}

QIcon TstDocGenerator1::icon() const
{
    return QIcon();
}

void TstDocGenerator1::createNodeTree()
{
    d->createConstantNodes();

    QDir rootDir(d->mRootPath.toLocalFile());
    QFileInfoList contentList = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
    foreach(QFileInfo path, contentList)
    {
        QString fullPath = path.absoluteFilePath();
//        QDir dir(fullPath);

//        if (dir.entryList(QDir::Files | QDir::NoDotAndDotDot).size() > 0)
//        {
            QDomDocument metaData = d->metadataFromDir(fullPath);
            Q_ASSERT(!metaData.isNull());

            MetaDataHandler hl(metaData);
            hl.print();
            if (!hl.isValidMeta()) {
                continue;
            }

            QString dir = hl.queryDir();
            qDebug();
//            DocumentNode::queryData(metaData);

//            QDomDocument
//            QMap<QString, QVariant> metadatas = UBMetadataDcSubsetAdaptor::load(fullPath);
//            QString docGroupName = metadatas.value(UBSettings::documentGroupName, QString()).toString();
//            QString docName = metadatas.value(UBSettings::documentName, QString()).toString();

//            if (docName.isEmpty()) {
//                qDebug() << "Group name and document name are empty in UBPersistenceManager::createDocumentProxiesStructure()";
//                continue;
//            }

//            QModelIndex parentIndex = mDocumentTreeStructureModel->goTo(docGroupName);
//            if (!parentIndex.isValid()) {
//                return;
//            }

//            UBDocumentProxy* docProxy = new UBDocumentProxy(fullPath); // managed in UBDocumentTreeNode
//            foreach(QString key, metadatas.keys()) {
//                docProxy->setMetaData(key, metadatas.value(key));
//            }

//            docProxy->setPageCount(sceneCount(docProxy));
//            bool addDoc = false;
//            if (!interactive) {
//                addDoc = true;
//            } else if (processInteractiveReplacementDialog(docProxy) == QDialog::Accepted) {
//                addDoc = true;
//            }
//            if (addDoc) {
//                mDocumentTreeStructureModel->addDocument(docProxy, parentIndex);
//            }
//        }
    }

}

Docs::GeneratorNode *TstDocGenerator1::rootNode() const
{
    return d->mRootNode;
}

