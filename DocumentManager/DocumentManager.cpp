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
        mExplorerModel = new ExplorerModel();
        mExplorerView = new ExplorerView();
        mExplorerView->setModel(mExplorerModel);
    }
    ~DocumentManagerPrivate()
    {
        if (mExplorerView) {
            delete mExplorerView;
        }
        if (mExplorerModel) {
            delete mExplorerModel;
        }
    }
    void registerGenerator(DocumentGenerator *pGenerator) {
        if (mRegisteredGenerators.contains(pGenerator)) {
            return;
        }
        mRegisteredGenerators.append(pGenerator);
        mExplorerModel->registerGenerator(pGenerator);
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
    d->registerGenerator(pGenerator);
}

void DocumentManager::print()
{
    qDebug() << "DocumentmManager print";
}

void DocumentManager::setVisible (bool pVisible)
{
    if (d->mExplorerView) {
        d->mExplorerView->setVisible(pVisible);
    }
}

DocumentGenerator::DocumentGenerator(QObject *parent)
    : QObject(parent)
{
}

DocumentGenerator::~DocumentGenerator()
{

}

void DocumentGenerator::createNodeTree()
{
}


} //namespace Docs

