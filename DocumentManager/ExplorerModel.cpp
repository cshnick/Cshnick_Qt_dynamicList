#include "ExplorerModel.h"

//#include <QFont>
//#include <QBrush>
#include <QtGui>

#include "Node.h"

namespace Docs {

class ExplorerModelPrivate
{
public:
    ExplorerModelPrivate(ExplorerModel *pq)
        : q(pq)
    {
        mRootNode = new RootNode();
    }
    ~ExplorerModelPrivate()
    {
        if (mRootNode) {
            delete mRootNode;
        }
    }
    QModelIndex index(int row, int column, const QModelIndex &parent) const
    {
        if (!mRootNode || row < 0 || column < 0) {
            return QModelIndex();
        }

        CatalogNode *nodeParent = catalogNodeFromIndex(parent);
        if (!nodeParent || row > nodeParent->childrenNodes().count() - 1) {
            return QModelIndex();
        }

        Node *requiredNode = nodeParent->childrenNodes().at(row);
        if(!requiredNode) {
            return QModelIndex();
        }

        QModelIndex resIndex = q->createIndex(row, column, requiredNode);

        return resIndex;
    }

    QModelIndex parent(const QModelIndex &child) const
    {
        Node *nodeChild = nodeFromIndex(child);
        if (!nodeChild) {
            return QModelIndex();
        }

        CatalogNode *nodeParent = nodeChild->parentNode();
        if (!nodeParent) {
            return QModelIndex();
        }

        CatalogNode *nodePreParent = nodeParent->parentNode();
        if (!nodePreParent) {
            return QModelIndex();
        }

        int row = nodePreParent->childrenNodes().indexOf(nodeParent);

        QModelIndex resIndex = q->createIndex(row, 0, nodeParent);

        return resIndex;
    }

    int rowCount(const QModelIndex &parent) const
    {
        if (parent.column() > 0) {
            return 0;
        }

        CatalogNode *nodeParent = catalogNodeFromIndex(parent);
        if (!nodeParent || !nodeParent->isCatalog()) {
            return 0;
        }

        return nodeParent->childrenNodes().count();
    }

    int columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return 1;
    }

    Node *nodeFromIndex(const QModelIndex &pIndex) const
    {
        if (pIndex.isValid()) {
            return static_cast<Node*>(pIndex.internalPointer());
        } else {
            return mRootNode;
        }
    }

    CatalogNode *catalogNodeFromIndex(const QModelIndex &pIndex) const
    {
        if (pIndex.isValid()) {
            return static_cast<CatalogNode*>(pIndex.internalPointer());
        } else {
            return mRootNode;
        }
    }

private:
    ExplorerModel *q;
    RootNode *mRootNode;
    QList<DocumentGenerator *> mRegisteredGenerators;


    friend class ExplorerModel;
    friend class DocumentManagerPrivate;
};

ExplorerModel::ExplorerModel(QObject *parent) :
    QAbstractItemModel(parent)
  , d(new ExplorerModelPrivate(this))
{
}

ExplorerModel::~ExplorerModel()
{
    if (d) {
        delete d;
    }
}

QModelIndex ExplorerModel::index(int row, int column, const QModelIndex &parent) const
{
    return d->index(row, column, parent);
}
QModelIndex ExplorerModel::parent(const QModelIndex &child) const
{
    return d->parent(child);
}
int ExplorerModel::rowCount(const QModelIndex &parent) const
{
    return d->rowCount(parent);
}
int ExplorerModel::columnCount(const QModelIndex &parent) const
{
    return d->columnCount(parent);
}
QVariant ExplorerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Node *dataNode = d->nodeFromIndex(index);
    if (!dataNode)
        return QVariant();

    if (index.column() == 0) {

        switch (role) {
        case (Qt::DecorationRole) :
            return dataNode->displayIcon();
            break;

        case (Qt::DisplayRole) :
            return dataNode->displayName();
            break;

        case (Qt::UserRole +1):
            return QVariant::fromValue(dataNode);
            break;

        case (Qt::FontRole) :
            if (dataNode->displayFont() != QFont()) {
                dataNode->displayFont();
            }
            break;

        case (Qt::ForegroundRole) :
            if (dataNode->backgroundColor().style() != Qt::NoBrush) {
                return dataNode->backgroundColor();
            }
            break;

        case (Qt::BackgroundRole) :
            if (dataNode->foregroundColor() != Qt::NoBrush) {
                return dataNode->foregroundColor();
            }
            break;

        default:
//            return QAbstractItemModel::data(index, role);
            break;
        }
    }

    return QVariant();
}
Node *ExplorerModel::nodeFromIndex(const QModelIndex &pIndex) const
{
    return d->nodeFromIndex(pIndex);
}

void ExplorerModel::registerGenerator(DocumentGenerator *pGenerator)
{
    beginResetModel();
    d->mRegisteredGenerators.append(pGenerator);
    d->mRootNode->addChild(pGenerator->rootNode());
    endResetModel();
}

QModelIndex ExplorerModel::firstGeneratorIndex() const
{
    if (d->mRegisteredGenerators.count()) {
        return createIndex(0, 0, d->mRegisteredGenerators.first());
    }

    return QModelIndex();
}


} //namespace Docs



//UBDocumentTreeModel::UBDocumentTreeModel(QObject *parent) :
//    QAbstractItemModel(parent)
//  , mRootNode(0)
//{
//    UBDocumentTreeNode *rootNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, "root");

//    QString trashName = UBSettings::trashedDocumentGroupNamePrefix;

//    UBDocumentTreeNode *myDocsNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, UBPersistenceManager::myDocumentsName, tr("My documents"));
//    rootNode->addChild(myDocsNode);
////    mMyDocuments = createIndex(0, 0, myDocsNode);
//    UBDocumentTreeNode *modelsNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, UBPersistenceManager::modelsName, tr("Models"));
//    rootNode->addChild(modelsNode);
////    mModels = createIndex(1, 0, modelsNode);
//    UBDocumentTreeNode *trashNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, trashName, tr("Trash"));
//    rootNode->addChild(trashNode);
////    mTrash = createIndex(2, 0, trashNode);
//    UBDocumentTreeNode *untitledDocumentsNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, UBPersistenceManager::untitledDocumentsName, tr("Untitled documents"));
//    myDocsNode->addChild(untitledDocumentsNode);
////    mUntitledDocuments = createIndex(0, 0, untitledDocumentsNode);

//    setRootNode(rootNode);

//    mRoot = index(0, 0, QModelIndex());
//    mMyDocuments =  index(0, 0, QModelIndex());
//    mModels =  index(1, 0, QModelIndex());
//    mTrash =  index(2, 0, QModelIndex());
//    mUntitledDocuments = index(0, 0, mMyDocuments);
//}

//bool UBDocumentTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    switch (role) {
//    case Qt::EditRole:
//        if (!index.isValid() || value.toString().isEmpty()) {
//            return false;
//        }
//        setNewName(index, value.toString());
//        return true;
//    }
//    return QAbstractItemModel::setData(index, value, role);
//}

//Qt::ItemFlags UBDocumentTreeModel::flags (const QModelIndex &index) const
//{
//    Qt::ItemFlags resultFlags = QAbstractItemModel::flags(index);
//    UBDocumentTreeNode *indexNode = nodeFromIndex(index);

//    if ( index.isValid() ) {
//        if (!indexNode->isRoot() && !isConstant(index)) {
//            if (!inTrash(index)) {
//                resultFlags |= Qt::ItemIsEditable;
//            }
//            resultFlags |= Qt::ItemIsDragEnabled;
//        }
//        if (indexNode->nodeType() == UBDocumentTreeNode::Catalog) {
//            resultFlags |= Qt::ItemIsDropEnabled;
//        }
//    }

//    return resultFlags;
//}

//QStringList UBDocumentTreeModel::mimeTypes() const
//{
//    QStringList types;
//    types << "text/uri-list" << "image/png" << "image/tiff" << "image/gif" << "image/jpeg";
//    return types;
//}

//QMimeData *UBDocumentTreeModel::mimeData (const QModelIndexList &indexes) const
//{
//    UBDocumentTreeMimeData *mimeData = new UBDocumentTreeMimeData();
//    QList <QModelIndex> indexList;
//    QList<QUrl> urlList;

//    foreach (QModelIndex index, indexes) {
//        if (index.isValid()) {
//            indexList.append(index);
//            urlList.append(QUrl());
//        }
//    }

//    mimeData->setUrls(urlList);
//    mimeData->setIndexes(indexList);

//    return mimeData;
//}

//bool UBDocumentTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
//{
//    if (data->hasFormat(UBApplication::mimeTypeUniboardPage)) {
//        UBDocumentTreeNode *curNode = nodeFromIndex(index(row - 1, column, parent));
//        UBDocumentProxy *targetDocProxy = curNode->proxyData();
//        const UBMimeData *ubMime = qobject_cast <const UBMimeData*>(data);
//        if (!targetDocProxy || !ubMime || !ubMime->items().count()) {
//            qDebug() << "an error ocured while parsing " << UBApplication::mimeTypeUniboardPage;
//            return false;
//        }

//        int count = 0;
//        int total = ubMime->items().size();

//        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

//        foreach (UBMimeDataItem sourceItem, ubMime->items())
//        {
//            count++;

//            UBApplication::applicationController->showMessage(tr("Copying page %1/%2").arg(count).arg(total), true);

//            // TODO UB 4.x Move following code to some controller class
//            UBGraphicsScene *scene = UBPersistenceManager::persistenceManager()->loadDocumentScene(sourceItem.documentProxy(), sourceItem.sceneIndex());
//            if (scene) {
//                UBGraphicsScene* sceneClone = scene->sceneDeepCopy();

//                foreach (QUrl relativeFile, scene->relativeDependencies()) {
//                    QString source = scene->document()->persistencePath() + "/" + relativeFile.toString();
//                    QString target = targetDocProxy->persistencePath() + "/" + relativeFile.toString();

//                    QFileInfo fi(target);
//                    QDir d = fi.dir();

//                    d.mkpath(d.absolutePath());
//                    QFile::copy(source, target);
//                }

//                UBPersistenceManager::persistenceManager()->insertDocumentSceneAt(targetDocProxy, sceneClone, targetDocProxy->pageCount());

//                //due to incorrect generation of thumbnails of invisible scene I've used direct copying of thumbnail files
//                //it's not universal and good way but it's faster
//                QString from = sourceItem.documentProxy()->persistencePath() + UBFileSystemUtils::digitFileFormat("/page%1.thumbnail.jpg", sourceItem.sceneIndex());
//                QString to  = targetDocProxy->persistencePath() + UBFileSystemUtils::digitFileFormat("/page%1.thumbnail.jpg", targetDocProxy->pageCount());
//                QFile::remove(to);
//                QFile::copy(from, to);
//            }
//        }

//        QApplication::restoreOverrideCursor();

//        UBApplication::applicationController->showMessage(tr("%1 pages copied", "", total).arg(total), false);

//        return true;
//    }



//    const UBDocumentTreeMimeData *mimeData = qobject_cast<const UBDocumentTreeMimeData*>(data);
//    if (!mimeData) {
//        qDebug() << "Incorrect mimeData, only internal one supported";
//        return false;
//    }
//    if (!parent.isValid()) {
//        return false;
//    }
//    UBDocumentTreeNode *newParentNode = nodeFromIndex(parent);

//    if (!newParentNode) {
//        qDebug() << "incorrect incoming parent node;";
//        return false;
//    }

//    QList<QModelIndex> incomingIndexes = mimeData->indexes();

//    foreach (QModelIndex curIndex, incomingIndexes) {
//        QModelIndex clonedTopLevel = copyIndexToNewParent(curIndex, parent, action == Qt::MoveAction ? aReference : aContentCopy);
//        if (nodeFromIndex(curIndex) == mCurrentNode && action == Qt::MoveAction) {
//            emit currentIndexMoved(clonedTopLevel, curIndex);
//        }
//    }

//    Q_UNUSED(action)
//    Q_UNUSED(row)
//    Q_UNUSED(column)
//    Q_UNUSED(parent)

//    return true;
//}

//bool UBDocumentTreeModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    if (row < 0 || row + count > rowCount(parent))
//        return false;

//    beginRemoveRows( parent, row, row + count - 1);

//    UBDocumentTreeNode *parentNode = nodeFromIndex(parent);
//    for (int i = row; i < row + count; i++) {
//        UBDocumentTreeNode *curChildNode = parentNode->children().at(i);
//        QModelIndex curChildIndex = parent.child(i, 0);
//        if (curChildNode) {
//            if (rowCount(curChildIndex)) {
//                while (rowCount(curChildIndex)) {
//                    removeRows(0, 1, curChildIndex);
//                }
//            }
//        }
//        mNewDocuments.removeAll(curChildNode->proxyData());
//        parentNode->removeChild(i);

//    }

//    endRemoveRows();
//    return true;
//}

//QModelIndex UBDocumentTreeModel::indexForNode(UBDocumentTreeNode *pNode) const
//{
//    if (pNode == 0) {
//        return QModelIndex();
//    }

//    return pIndexForNode(QModelIndex(), pNode);
//}

//QPersistentModelIndex UBDocumentTreeModel::persistentIndexForNode(UBDocumentTreeNode *pNode)
//{
//    return QPersistentModelIndex(indexForNode(pNode));
//}

//UBDocumentTreeNode *UBDocumentTreeModel::findProxy(UBDocumentProxy *pSearch, UBDocumentTreeNode *pParent) const
//{
//    foreach (UBDocumentTreeNode *curNode, pParent->children())
//    {
//        if (UBDocumentTreeNode::Catalog != curNode->nodeType())
//        {
//            if (curNode->proxyData()->theSameDocument(pSearch))
//                return curNode;
//        }
//        else if (curNode->children().count())
//        {
//            UBDocumentTreeNode *recursiveDescendResult = findProxy(pSearch, curNode);
//            if (recursiveDescendResult)
//                return findProxy(pSearch, curNode);
//        }
//    }

//    return 0;
//}

//QModelIndex UBDocumentTreeModel::pIndexForNode(const QModelIndex &parent, UBDocumentTreeNode *pNode) const
//{
//    for (int i = 0; i < rowCount(parent); i++) {
//        QModelIndex curIndex = index(i, 0, parent);
//        if (curIndex.internalPointer() == pNode) {
//            return curIndex;
//        } else if (rowCount(curIndex) > 0) {
//            QModelIndex recursiveDescendIndex = pIndexForNode(curIndex, pNode);
//            if (recursiveDescendIndex.isValid()) {
//                return recursiveDescendIndex;
//            }
//        }
//    }
//    return QModelIndex();
//}

//QPersistentModelIndex UBDocumentTreeModel::copyIndexToNewParent(const QModelIndex &source, const QModelIndex &newParent, eCopyMode pMode)
//{
//    UBDocumentTreeNode *nodeParent = nodeFromIndex(newParent);
//    UBDocumentTreeNode *nodeSource = nodeFromIndex(source);

//    if (!nodeParent || !nodeSource) {
//        return QModelIndex();
//    }

//    beginInsertRows(newParent, rowCount(newParent), rowCount(newParent));

//    UBDocumentTreeNode *clonedNodeSource = 0;
//    switch (static_cast<int>(pMode)) {
//    case aReference:
//        clonedNodeSource = nodeSource->clone();
//        if (mNewDocuments.contains(nodeSource->proxyData())) { //update references for session documents
//            mNewDocuments << clonedNodeSource->proxyData();

//            UBPersistenceManager::persistenceManager()->reassignDocProxy(clonedNodeSource->proxyData(), nodeSource->proxyData());
//        }
//        break;

//    case aContentCopy:
//        UBDocumentProxy* duplicatedProxy = 0;
//        if (nodeSource->nodeType() == UBDocumentTreeNode::Document && nodeSource->proxyData()) {
//            duplicatedProxy = UBPersistenceManager::persistenceManager()->duplicateDocument(nodeSource->proxyData());
//            duplicatedProxy->setMetaData(UBSettings::documentUpdatedAt, UBStringUtils::toUtcIsoDateTime(QDateTime::currentDateTime()));
//            UBMetadataDcSubsetAdaptor::persist(duplicatedProxy);
//        }
//        clonedNodeSource = new UBDocumentTreeNode(nodeSource->nodeType()
//                                                  , nodeSource->nodeName()
//                                                  , nodeSource->displayName()
//                                                  , duplicatedProxy);
//        break;
//    }

//    // Determine whether to provide a name with postfix if the name in current level allready exists
//    QString newName = clonedNodeSource->nodeName();
//    if ((source.parent() != newParent
//            || pMode != aReference)
//            && (newParent != trashIndex() || !inTrash(newParent))) {
//        newName = adjustNameForParentIndex(newName, newParent);
//        clonedNodeSource->setNodeName(newName);
//    }

//    if (clonedNodeSource->proxyData()) {
//        clonedNodeSource->proxyData()->setMetaData(UBSettings::documentGroupName, virtualPathForIndex(newParent));
//        clonedNodeSource->proxyData()->setMetaData(UBSettings::documentName, newName);
//        UBPersistenceManager::persistenceManager()->persistDocumentMetadata(clonedNodeSource->proxyData());
//    }

//    addNode(clonedNodeSource, newParent);
////    nodeParent->addChild(clonedNodeSource);
//    endInsertRows();

//    QPersistentModelIndex newParentIndex = createIndex(rowCount(newParent), 0, clonedNodeSource);

//    if (rowCount(source)) {
//        for (int i = 0; i < rowCount(source); i++) {
//            QModelIndex curNewParentIndexChild = source.child(i, 0);
//            copyIndexToNewParent(curNewParentIndexChild, newParentIndex, pMode);
//        }
//    }

//    return newParentIndex;
//}

//void UBDocumentTreeModel::moveIndex(const QModelIndex &source, const QModelIndex &newParent)
//{
//    UBDocumentTreeNode *sourceNode = nodeFromIndex(source);
//    QPersistentModelIndex clonedTopLevel = copyIndexToNewParent(source, newParent);
//    if (sourceNode == mCurrentNode) {
//        mCurrentNode = nodeFromIndex(clonedTopLevel);
//        emit currentIndexMoved(clonedTopLevel, source);
//    }
//    removeRow(source.row(), source.parent());
//}

//void UBDocumentTreeModel::moveNode(const QModelIndex &source, const QModelIndex &newParent)
//{
//    Q_ASSERT(source.parent().isValid());

//    UBDocumentTreeNode *sourceNode = nodeFromIndex(source);
//    UBDocumentTreeNode *newParentNode = nodeFromIndex(newParent);

//    int destinationPosition = positionForParent(sourceNode, newParentNode);

//    if (source.row() != destinationPosition || source.parent() == newParent) {
//        beginMoveRows(source.parent(), source.row(), source.row(), newParent, destinationPosition);
//        sourceNode->parentNode()->children().removeAt(source.row());
//        newParentNode->insertChild(destinationPosition, sourceNode);
//        endMoveRows();
//    }
//}

//void UBDocumentTreeModel::setCurrentDocument(UBDocumentProxy *pDocument)
//{
//    UBDocumentTreeNode *testCurNode = findProxy(pDocument, mRootNode);

//    if (testCurNode) {
//        setCurrentNode(testCurNode);
//    }
//}

//QModelIndex UBDocumentTreeModel::indexForProxy(UBDocumentProxy *pSearch) const
//{
////    qDebug() << "looking for proxy" << pSearch;
//    UBDocumentTreeNode *proxy = findProxy(pSearch, mRootNode);
//    if (!proxy) {
//        return QModelIndex();
//    }

//    return indexForNode(proxy);
//}

//void UBDocumentTreeModel::setRootNode(UBDocumentTreeNode *pRoot)
//{
//    mRootNode = pRoot;
//    reset();
//}

//UBDocumentProxy *UBDocumentTreeModel::proxyForIndex(const QModelIndex &pIndex) const
//{
//    UBDocumentTreeNode *node = nodeFromIndex(pIndex);
//    if (!node) {
//        return 0;
//    }

//    return node->proxyData();
//}

//QString UBDocumentTreeModel::virtualDirForIndex(const QModelIndex &pIndex) const
//{
//    QString result;
//    UBDocumentTreeNode *curNode = nodeFromIndex(pIndex);
//    //protect the 2nd level items
//    while (curNode->parentNode() && !curNode->isTopLevel()) {
//        result.prepend(curNode->parentNode()->nodeName() + "/");
//        curNode = curNode->parentNode();
//    }

//    if (result.endsWith("/")) {
//        result.truncate(result.count() - 1);
//    }

//    return result;
//}

//QString UBDocumentTreeModel::virtualPathForIndex(const QModelIndex &pIndex) const
//{
//    UBDocumentTreeNode *curNode = nodeFromIndex(pIndex);
//    Q_ASSERT(curNode);

//    return virtualDirForIndex(pIndex) + "/" + curNode->nodeName();
//}

//QStringList UBDocumentTreeModel::nodeNameList(const QModelIndex &pIndex) const
//{
//    QStringList result;

//    UBDocumentTreeNode *catalog = nodeFromIndex(pIndex);
//    if (catalog->nodeType() != UBDocumentTreeNode::Catalog) {
//        return QStringList();
//    }

//    foreach (UBDocumentTreeNode *curNode, catalog->children()) {
//        result << curNode->nodeName();
//    }

//    return result;
//}

//bool UBDocumentTreeModel::newNodeAllowed(const QModelIndex &pSelectedIndex)  const
//{
//    if (!pSelectedIndex.isValid()) {
//        return false;
//    }

//    if (inTrash(pSelectedIndex) || pSelectedIndex == trashIndex()) {
//        return false;
//    }

//    return true;
//}

//QModelIndex UBDocumentTreeModel::goTo(const QString &dir)
//{
//    QStringList pathList = dir.split("/", QString::SkipEmptyParts);

//    if (pathList.isEmpty()) {
//        return untitledDocumentsIndex();
//    }

//    if (pathList.first() != UBPersistenceManager::myDocumentsName
//            && pathList.first() != UBSettings::trashedDocumentGroupNamePrefix
//            && pathList.first() != UBPersistenceManager::modelsName) {
//        pathList.prepend(UBPersistenceManager::myDocumentsName);
//    }

//    QModelIndex parentIndex;

//    bool searchingNode = true;
//    while (!pathList.isEmpty())
//    {
//        QString curLevelName = pathList.takeFirst();
//        if (searchingNode) {
//            searchingNode = false;
//            for (int i = 0; i < rowCount(parentIndex); ++i) {
//                QModelIndex curChildIndex = index(i, 0, parentIndex);
//                if (nodeFromIndex(curChildIndex)->nodeName() == curLevelName) {
//                    searchingNode = true;
//                    parentIndex = curChildIndex;
//                    break;
//                }
//            }
//        }

//        if (!searchingNode) {
//            UBDocumentTreeNode *newChild = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, curLevelName);
//            parentIndex = addNode(newChild, parentIndex);
//        }
//    }

//    return parentIndex;
//}

//bool UBDocumentTreeModel::inTrash(const QModelIndex &index) const
//{
//    return isDescendantOf(index, trashIndex());
//}

//bool UBDocumentTreeModel::inModel(const QModelIndex &index) const
//{
//    return isDescendantOf(index, modelsIndex());
//}

//bool UBDocumentTreeModel::inUntitledDocuments(const QModelIndex &index) const
//{
//    return isDescendantOf(index, untitledDocumentsIndex());
//}

//void UBDocumentTreeModel::addDocument(UBDocumentProxy *pProxyData, const QModelIndex &pParent)
//{
//    if (!pProxyData) {
//        return;
//    }
//    QString docName = pProxyData->metaData(UBSettings::documentName).toString();
//    QString docGroupName = pProxyData->metaData(UBSettings::documentGroupName).toString();

//    if (docName.isEmpty()) {
//        return;
//    }

//    QModelIndex lParent = pParent;
//    UBDocumentTreeNode *freeNode = new UBDocumentTreeNode(UBDocumentTreeNode::Document
//                                                          , docName
//                                                          , QString()
//                                                          , pProxyData);
//    if (!pParent.isValid()) {
//        lParent = goTo(docGroupName);
//    }

//    addNode(freeNode, lParent);
//}

//void UBDocumentTreeModel::addNewDocument(UBDocumentProxy *pProxyData, const QModelIndex &pParent)
//{
//    addDocument(pProxyData, pParent);
//    mNewDocuments << pProxyData;
//}

//void UBDocumentTreeModel::addCatalog(const QString &pName, const QModelIndex &pParent)
//{
//    if (pName.isEmpty() || !pParent.isValid()) {
//        return;
//    }

//    UBDocumentTreeNode *catalogNode = new UBDocumentTreeNode(UBDocumentTreeNode::Catalog, pName);
//    addNode(catalogNode, pParent);
//}

//void UBDocumentTreeModel::setNewName(const QModelIndex &index, const QString &newName)
//{
//    if (!index.isValid()) {
//        return;
//    }

//    UBDocumentTreeNode *indexNode = nodeFromIndex(index);

//    QString magicSeparator = "+!##s";
//    if (isCatalog(index)) {
//        QString fullNewName = newName;
//        if (!newName.contains(magicSeparator)) {
//            indexNode->setNodeName(newName);
//            QString virtualDir = virtualDirForIndex(index);
//            fullNewName.prepend(virtualDir.isEmpty() ? "" : virtualDir + magicSeparator);
//        }
//        for (int i = 0; i < rowCount(index); i++) {
//            QModelIndex subIndex = this->index(i, 0, index);
//            setNewName(subIndex, fullNewName + magicSeparator + subIndex.data().toString());
//        }

//    } else if (isDocument(index)) {
//        Q_ASSERT(indexNode->proxyData());

//        int prefixIndex = newName.lastIndexOf(magicSeparator);
//        if (prefixIndex != -1) {
//            QString newDocumentGroupName = newName.left(prefixIndex).replace(magicSeparator, "/");
//            indexNode->proxyData()->setMetaData(UBSettings::documentGroupName, newDocumentGroupName);
//        } else {
//            indexNode->setNodeName(newName);
//            indexNode->proxyData()->setMetaData(UBSettings::documentName, newName);
//        }

//        UBPersistenceManager::persistenceManager()->persistDocumentMetadata(indexNode->proxyData());
//    }
//}

//QString UBDocumentTreeModel::adjustNameForParentIndex(const QString &pName, const QModelIndex &pIndex)
//{
//    int i = 0;
//    QString newName = pName;
//    QStringList siblingNames = nodeNameList(pIndex);
//    while (siblingNames.contains(newName)) {
//        newName = pName + " " + QVariant(++i).toString();
//    }

//    return newName;
//}

//bool UBDocumentTreeModel::isDescendantOf(const QModelIndex &pPossibleDescendant, const QModelIndex &pPossibleAncestor) const
//{
//    if (!pPossibleDescendant.isValid()) {
//        return false;
//    }

//    QModelIndex ancestor = pPossibleDescendant;
//    while (ancestor.parent().isValid()) {
//        ancestor = ancestor.parent();
//        if (ancestor == pPossibleAncestor) {
//            return true;
//        }
//    }

//    return false;
//}

//QModelIndex UBDocumentTreeModel::addNode(UBDocumentTreeNode *pFreeNode, const QModelIndex &pParent, eAddItemMode pMode)
//{
//    UBDocumentTreeNode *tstParent = nodeFromIndex(pParent);

//    if (!pParent.isValid() || tstParent->nodeType() != UBDocumentTreeNode::Catalog) {
//        return QModelIndex();
//    }
//    int newIndex = pMode == aDetectPosition ? positionForParent(pFreeNode, tstParent): tstParent->children().size();
//    beginInsertRows(pParent, newIndex, newIndex);
//    tstParent->insertChild(newIndex, pFreeNode);
//    endInsertRows();

//    return createIndex(newIndex, 0, pFreeNode);
//}

//int UBDocumentTreeModel::positionForParent(UBDocumentTreeNode *pFreeNode, UBDocumentTreeNode *pParentNode)
//{
//    Q_ASSERT(pFreeNode);
//    Q_ASSERT(pParentNode);
//    Q_ASSERT(pParentNode->nodeType() == UBDocumentTreeNode::Catalog);

//    int c = -1;
//    int childCount = pParentNode->children().count();
//    while (c <= childCount) {
//        if (++c == childCount || lessThan(pFreeNode, pParentNode->children().at(c))) {
//            break;
//        }
//    }
//    return c == -1 ? childCount : c;
//}

//UBDocumentTreeNode *UBDocumentTreeModel::nodeFromIndex(const QModelIndex &pIndex) const
//{
//    if (pIndex.isValid()) {
//        return static_cast<UBDocumentTreeNode*>(pIndex.internalPointer());
//    } else {
//        return mRootNode;
//    }
//}

//void UBDocumentTreeModel::sort(int column, Qt::SortOrder order)
//{
//    Q_UNUSED(order)
//    Q_UNUSED(column)

////  QModelIndex parentIndex = index(0, column, QModelIndex());
//    sortChilds(mRoot);
//}

//void UBDocumentTreeModel::sortChilds(const QModelIndex &parentIndex)
//{
//    int current_column = 0;
//    int current_row = 0;

//    QList <UBDocumentTreeNode*> catalogsForSort;
//    QList <UBDocumentTreeNode*> documentsForSort;

//    for (current_row = 0; current_row < rowCount(parentIndex); current_row++)
//    {
//        QModelIndex currentIndex = index(current_row, current_column, parentIndex);
//        if (isCatalog(currentIndex))
//            catalogsForSort << nodeFromIndex(currentIndex);
//        else
//            documentsForSort << nodeFromIndex(currentIndex);
//    }

//    sortIndexes(catalogsForSort);
//    sortIndexes(documentsForSort);



//    foreach (UBDocumentTreeNode *node, catalogsForSort)
//    {
//        sortChilds(indexForNode(node));
//    }

//    foreach(UBDocumentTreeNode *node, catalogsForSort)
//    {
//        QModelIndex currentIndex = indexForNode(node);
//        moveIndex(currentIndex, parentIndex);
//    }

//    for (int i = documentsForSort.count()-1; i >= 0 ; i--)
//    {
//        QModelIndex currentIndex = indexForNode(documentsForSort.at(i));
//        moveIndex(currentIndex, parentIndex);
//    }
//}


//void UBDocumentTreeModel::sortIndexes(QList<UBDocumentTreeNode *> &unsortedIndexList)
//{
//    qStableSort(unsortedIndexList.begin(), unsortedIndexList.end(), nodeLessThan);
//}

//bool UBDocumentTreeModel::nodeLessThan(const UBDocumentTreeNode *firstIndex, const UBDocumentTreeNode *secondIndex)
//{
//    return firstIndex->nodeName() < secondIndex->nodeName();
//}

//UBDocumentTreeModel::~UBDocumentTreeModel()
//{
//    delete mRootNode;
//}
