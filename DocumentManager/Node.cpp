#include "Node.h"

#include <QVariant>
#include "DocumentManager.h"

namespace Docs {

class NodePrivate
{
public:
    NodePrivate(Node *pq)
        : q(pq)
    {
    }

private:
    Node *q;
    QMap<int, QVariant> mData;

    friend class Node;
};

Node::Node(QObject *parent) :
    QObject(parent)
  , d(new NodePrivate(this))
{
}

Node::~Node()
{
    if (d) {
        delete d;
    }
}

QVariant Node::data(int role)
{
    return d->mData.value(role, QVariant());
}

void Node::setData(QVariant data, int role)
{
    d->mData.insert(role, data);
}

int Node::type() const {
    return NoType;
}

QIcon Node::icon() const
{
    return QIcon();
}

class CatalogNodePrivate
{
public:
    CatalogNodePrivate(CatalogNode *pq)
        : q(pq)
    {
    }

private:
    CatalogNode *q;

    friend class CatalogNode;
};

CatalogNode::CatalogNode(QObject *parent) :
    Node(parent)
  , d(new CatalogNodePrivate(this))
{
}

CatalogNode::~CatalogNode()
{
    if (d) {
        delete d;
    }
}

bool CatalogNode::isCatalog() const
{
    return true;
}



} //namespace Docs
