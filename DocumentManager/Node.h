#ifndef DOCNODE_H
#define DOCNODE_H

#include "DocumentManager_global.h"

#include "DocumentManager.h"
#include <QObject>
#include <QMetaType>
#include <QIcon>

namespace Docs {

enum NodeType {
    NoType = 0
    ,DocumentType
    ,CatalogType
    ,DocumentGeneratorType
    ,RootType
    ,UserType = 100
};

class NodePrivate;
class DOCUMENTMANAGERSHARED_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QObject *parent = 0);
    virtual ~Node();
    QVariant data(int role = CustomRole);
    void setData(QVariant data, int role = CustomRole);

protected:
    Node *mParent;

public: //virtual
    virtual bool isCatalog() const = 0;
    virtual int type() const;
    virtual QString displayName() const = 0;
    virtual QIcon icon() const;
    
private:
    NodePrivate *d;

    friend class NodePrivate;
};

class CatalogNodePrivate;
class DOCUMENTMANAGERSHARED_EXPORT CatalogNode : public Node
{
    Q_OBJECT
public:
    CatalogNode(QObject *parent);
    ~CatalogNode();



    bool isCatalog() const;


protected:
    QList<Node*> childrenNodes;

private:
    CatalogNodePrivate *d;

    friend class CatalogNodePrivate;

};


} //namespace Docs
Q_DECLARE_METATYPE(Docs::Node*)

#endif // DOCNODE_H

//class UBDocumentTreeNode
//{
//public:
//    friend class UBDocumentTreeModel;

//    enum Type {
//        Catalog = 0
//        , Document
//    };

//    UBDocumentTreeNode(Type pType, const QString &pName, const QString &pDisplayName = QString(), UBDocumentProxy *pProxy = 0);
//    UBDocumentTreeNode() : mType(Catalog), mParent(0), mProxy(0) {;}
//    ~UBDocumentTreeNode();

//    QList<UBDocumentTreeNode*> children() const {return mChildren;}
//    UBDocumentTreeNode *parentNode() {return mParent;}
//    Type nodeType() const {return mType;}
//    QString nodeName() const {return mName;}
//    QString displayName() const {return mDisplayName;}
//    void setNodeName(const QString &str) {mName = str; mDisplayName = str;}
//    void addChild(UBDocumentTreeNode *pChild);
//    void insertChild(int pIndex, UBDocumentTreeNode *pChild);
//    void removeChild(int index);
//    UBDocumentProxy *proxyData() const {return mProxy;}
//    bool isRoot() {return !mParent;}
//    bool isTopLevel() {return mParent && !mParent->mParent;}
//    UBDocumentTreeNode *clone();
//    QString dirPathInHierarchy();

//private:
//    Type mType;
//    QString mName;
//    QString mDisplayName;
//    UBDocumentTreeNode *mParent;
//    QList<UBDocumentTreeNode*> mChildren;
//    QPointer<UBDocumentProxy> mProxy;
//};
