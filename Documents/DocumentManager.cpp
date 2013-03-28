#include "DocumentManager.h"

#include "Node.h"
#include "ExplorerModel.h"
#include "ExplorerView.h"
#include "tstdocgenerator1.h"

#include <QtGui>

namespace Docs {

class DocumentManagerPrivate
{
public:
    DocumentManagerPrivate(DocumentManager *pq)
        : q(pq)
        , mExplorerModel(0)
        , mExplorerView(0)
        , mTopWidget(0)
        , mPluginBox(0)
        , mPluginsMenu(0)
        , mSettings(0)
    {
        mSettings = new QSettings("settings", QSettings::IniFormat);

        mExplorerModel = new ExplorerModel();
        mExplorerView = new ExplorerView();
        mExplorerView->setModel(mExplorerModel);
        setupUi();
    }

    ~DocumentManagerPrivate()
    {
        if (mTopWidget) {
            delete mTopWidget;
        }
        if (mExplorerModel) {
            delete mExplorerModel;
        }

    }
    void registerGenerator(IDocumentGenerator *pGenerator) {
        if (mRegisteredGenerators.contains(pGenerator)) {
            return;
        }
        mRegisteredGenerators.append(pGenerator);
        mExplorerModel->registerGenerator(pGenerator);
        if (mRegisteredGenerators.count() < 2 && false) {
            mExplorerView->setRootIndex(mExplorerModel->index(0, 0, QModelIndex()));
        } else {
            mExplorerView->setRootIndex(QModelIndex());
        }
        mExplorerView->expandAll();
    }

    void setupUi()
    {
        mTopWidget = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout;

        mPluginsMenu = new QMenu("Plugins", 0);
        QPushButton *mPluginBox = new QPushButton("Plugins");
        mPluginBox->setMenu(mPluginsMenu);
        mainLayout->addWidget(mPluginBox);
        mainLayout->addWidget(mExplorerView);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        mTopWidget->setLayout(mainLayout);
    }

private:
    DocumentManager *q;

    QList<IDocumentGenerator *> mRegisteredGenerators;
    ExplorerModel *mExplorerModel;
    ExplorerView *mExplorerView;
    QWidget *mTopWidget;
    QPushButton *mPluginBox;
    QMenu *mPluginsMenu;
    QSettings *mSettings;

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

void DocumentManager::registerGenerator(IDocumentGenerator *pGenerator)
{
    d->registerGenerator(pGenerator);
}

void DocumentManager::print()
{
    qDebug() << "DocumentmManager print";
}

void DocumentManager::setVisible (bool pVisible)
{
    if (d->mTopWidget) {
        d->mTopWidget->setVisible(pVisible);
    }
}

QWidget *DocumentManager::topWidget() const
{
    return d->mTopWidget;
}

void DocumentManager::actionMenuChecked(bool checked)
{
    d->mSettings->setValue("", checked);
}

} //namespace Docs

