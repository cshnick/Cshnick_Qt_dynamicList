#ifndef EXPLORERMODEL_H
#define EXPLORERMODEL_H

#include "DocumentManager_global.h"
#include <QtGui>

namespace Docs {

class ExplorerModelPrivate;
class Node;
class CatalogNode;
class IDocumentGenerator;


class DOCUMENTSSHARED_EXPORT ExplorerModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ExplorerModel(QObject *parent = 0);
    ~ExplorerModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

//   reimplemented
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

////    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    Qt::ItemFlags flags ( const QModelIndex & index ) const;
//    Qt::DropActions supportedDropActions() const {return Qt::MoveAction | Qt::CopyAction;}
//    QStringList mimeTypes() const;
//    QMimeData *mimeData (const QModelIndexList &indexes) const;
//    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
//    bool removeRows(int row, int count, const QModelIndex &parent);

//    QModelIndex indexForNode(UBDocumentTreeNode *pNode) const;
//    QPersistentModelIndex persistentIndexForNode(UBDocumentTreeNode *pNode);

    Node *nodeFromIndex(const QModelIndex &pIndex) const;
    void registerGenerator(IDocumentGenerator *pGenerator);
    QModelIndex firstGeneratorIndex() const;
    
private:
    ExplorerModelPrivate *d;

    friend class ExplorerModelPrivate;
};

} //namespace Docs
#endif // EXPLORERMODEL_H

//class UBDocumentTreeModel : public QAbstractItemModel {
//    Q_OBJECT

//public:
//    enum eAncestors {
//        aMyDocuments
//        , aUntitledDocuments
//        , aModel
//        , aTrash
//    };

//    enum eCopyMode {
//        aReference
//        , aContentCopy
//    };

//    enum eAddItemMode {
//        aEnd = 0          //Add to the end of the children list
//        , aDetectPosition //Detect the appropriate position (sorting)
//    };

//    UBDocumentTreeModel(QObject *parent = 0);
//    ~UBDocumentTreeModel();

//    QModelIndex index(int row, int column, const QModelIndex &parent) const;
//    QModelIndex parent(const QModelIndex &child) const;

//    int rowCount(const QModelIndex &parent) const;
//    int columnCount(const QModelIndex &parent) const;
//    QVariant data(const QModelIndex &index, int role) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    Qt::ItemFlags flags ( const QModelIndex & index ) const;
//    Qt::DropActions supportedDropActions() const {return Qt::MoveAction | Qt::CopyAction;}
//    QStringList mimeTypes() const;
//    QMimeData *mimeData (const QModelIndexList &indexes) const;
//    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
//    bool removeRows(int row, int count, const QModelIndex &parent);

//    QModelIndex indexForNode(UBDocumentTreeNode *pNode) const;
//    QPersistentModelIndex persistentIndexForNode(UBDocumentTreeNode *pNode);
////    bool insertRow(int row, const QModelIndex &parent);

//    QPersistentModelIndex copyIndexToNewParent(const QModelIndex &source, const QModelIndex &newParent, eCopyMode pMode = aReference);
//    void moveIndex(const QModelIndex &source, const QModelIndex &newParent);
//    void moveNode(const QModelIndex &source, const QModelIndex &newParent);
//    UBDocumentTreeNode *currentNode() const {return mCurrentNode;} //work around for sorting model.
//    void setCurrentNode(UBDocumentTreeNode *pNode) {mCurrentNode = pNode;}
//    QModelIndex currentIndex() {return indexForNode(mCurrentNode);} //index representing a current document
//    QModelIndex indexForProxy(UBDocumentProxy *pSearch) const;
//    void setCurrentDocument(UBDocumentProxy *pDocument);
//    void setRootNode(UBDocumentTreeNode *pRoot);
//    UBDocumentTreeNode *rootNode() const {return mRootNode;}
//    UBDocumentProxy *proxyForIndex(const QModelIndex &pIndex) const;
//    QString virtualDirForIndex(const QModelIndex &pIndex) const;
//    QString virtualPathForIndex(const QModelIndex &pIndex) const;
//    QStringList nodeNameList(const QModelIndex &pIndex) const;
//    bool newNodeAllowed(const QModelIndex &pSelectedIndex)  const;
//    QModelIndex goTo(const QString &dir);
//    bool inTrash(const QModelIndex &index) const;
//    bool inModel(const QModelIndex &index) const;
//    bool inUntitledDocuments(const QModelIndex &index) const;
//    bool isCatalog(const QModelIndex &index) const {return nodeFromIndex(index)->nodeType() == UBDocumentTreeNode::Catalog;}
//    bool isDocument(const QModelIndex &index) const {return nodeFromIndex(index)->nodeType() == UBDocumentTreeNode::Document;}
//    bool isToplevel(const QModelIndex &index) const {return nodeFromIndex(index) ? nodeFromIndex(index)->isTopLevel() : false;}
//    bool isConstant(const QModelIndex &index) const {return isToplevel(index) || (index == mUntitledDocuments);}
//    bool isOkToRename(const QModelIndex &index) const {return flags(index) & Qt::ItemIsEditable;}
//    UBDocumentProxy *proxyData(const QModelIndex &index) const {return nodeFromIndex(index)->proxyData();}
//    void addDocument(UBDocumentProxy *pProxyData, const QModelIndex &pParent = QModelIndex());
//    void addNewDocument(UBDocumentProxy *pProxyData, const QModelIndex &pParent = QModelIndex());
//    void addCatalog(const QString &pName, const QModelIndex &pParent);
//    QList<UBDocumentProxy*> newDocuments() {return mNewDocuments;}
//    void markDocumentAsNew(UBDocumentProxy *pDoc) {if (indexForProxy(pDoc).isValid()) mNewDocuments << pDoc;}
//    void setNewName(const QModelIndex &index, const QString &newName);
//    QString adjustNameForParentIndex(const QString &pName, const QModelIndex &pIndex);

//    QPersistentModelIndex myDocumentsIndex() const {return mMyDocuments;}
//    QPersistentModelIndex modelsIndex() const {return mModels;}
//    QPersistentModelIndex trashIndex() const {return mTrash;}
//    QPersistentModelIndex untitledDocumentsIndex() const {return mUntitledDocuments;}
//    UBDocumentTreeNode *nodeFromIndex(const QModelIndex &pIndex) const;
//    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
//    void sortChilds(const QModelIndex &parentIndex);
//    void sortIndexes(QList<UBDocumentTreeNode *> &unsortedIndexList);
//    static bool nodeLessThan(const UBDocumentTreeNode *firstIndex, const UBDocumentTreeNode *secondIndex);
//    void setHighLighted(const QModelIndex &newHighLighted) {mHighLighted = newHighLighted;}
//    QModelIndex highLighted() {return mHighLighted;}

//signals:
//    void indexChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex);
//    void currentIndexMoved(const QModelIndex &newIndex, const QModelIndex &previous); /* Be aware that when you got the signal
//                                                                                       "previous" index would have allready been deleted.
//                                                                                        check it for "valid" first */

//private:
//    UBDocumentTreeNode *mRootNode;
//    UBDocumentTreeNode *mCurrentNode;

//    UBDocumentTreeNode *findProxy(UBDocumentProxy *pSearch, UBDocumentTreeNode *pParent) const;
//    QModelIndex pIndexForNode(const QModelIndex &parent, UBDocumentTreeNode *pNode) const;
//    bool isDescendantOf(const QModelIndex &pPossibleDescendant, const QModelIndex &pPossibleAncestor) const;
//    QModelIndex addNode(UBDocumentTreeNode *pFreeNode, const QModelIndex &pParent, eAddItemMode pMode = aDetectPosition);
//    int positionForParent(UBDocumentTreeNode *pFreeNode, UBDocumentTreeNode *pParentNode);
//    QPersistentModelIndex mRoot;
//    QPersistentModelIndex mMyDocuments;
//    QPersistentModelIndex mModels;
//    QPersistentModelIndex mTrash;
//    QPersistentModelIndex mUntitledDocuments;
//    QList<UBDocumentProxy*> mNewDocuments;
//    QModelIndex mHighLighted;

//};

