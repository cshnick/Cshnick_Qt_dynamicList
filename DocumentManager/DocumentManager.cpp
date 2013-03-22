#include "DocumentManager.h"

#include "Node.h"
#include "ExplorerModel.h"
#include "ExplorerView.h"

#include <QtGui>

namespace Docs {

class DocumentManagerPrivate
{
public:
    DocumentManagerPrivate(DocumentManager *pq)
        : q(pq)
    {
//        mExplorerModel = new ExplorerModel();
    }

private:
    DocumentManager *q;

    QList<DocumentGenerator *> mRegisteredGenerators;
    ExplorerModel *mExplorerModel;
    ExplorerView *mExplorerView;


    friend class DocumentManager;
};


DocumentManager::DocumentManager(QObject *parent)
    : QObject(parent)
    , d(new DocumentManagerPrivate(this))
{
}

DocumentManager::~DocumentManager()
{
    if (d) {
        delete d;
    }
}

void DocumentManager::registerGenerator(DocumentGenerator *pGenerator)
{
    if (d->mRegisteredGenerators.contains(pGenerator)) {
        return;
    }
    d->mRegisteredGenerators.append(pGenerator);
}

void DocumentManager::print()
{
    qDebug() << "DocumentmManager print";
}

DocumentGenerator::DocumentGenerator(QObject *parent)
    : QObject(parent)
{
}

DocumentGenerator::~DocumentGenerator()
{

}

QIcon DocumentGenerator::visibleIcon() const
{
    return QIcon();
}

void DocumentGenerator::createNodeTree()
{
}


} //namespace Docs

