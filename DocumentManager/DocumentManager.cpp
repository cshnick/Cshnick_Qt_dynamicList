#include "DocumentManager.h"

#include "Node.h"
#include "ExplorerModel.h"
#include "ExplorerView.h"
#include "tstdocgenerator1.h"

#include <QtGui>

static const QString DocGeneratorPath_1 = "../TstDocGenerator1/libTstDocGenerator1.so";

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
    void registerGenerator(DocumentGenerator *pGenerator) {
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

    void loadPlugins()
    {
        QPluginLoader loader_1(DocGeneratorPath_1);
        if (!loader_1.fileName().isEmpty()) {
            qDebug() << "invalid path" << DocGeneratorPath_1 << "for plugin";
        }
        loader_1.load();

        TstDocGenerator1 *generator = static_cast<TstDocGenerator1*>(loader_1.instance());

        QAction *action = generator->associatedAction();
        QObject::connect(action, SIGNAL(toggled(bool)), q, SLOT(actionMenuChecked(bool)));
        mPluginsMenu->addAction(action);
        bool generator_1Exists_inSettings = mSettings->value(DocGeneratorPath_1).toBool();
        action->setChecked(generator_1Exists_inSettings);
        if (generator_1Exists_inSettings) {
            generator->createNodeTree();
            q->registerGenerator(generator);

        } else {
            loader_1.unload();
        }
    }

private:
    DocumentManager *q;

    QList<DocumentGenerator *> mRegisteredGenerators;
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
    QTimer::singleShot(0, this, SLOT(loadPlugins()));
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
    if (d->mTopWidget) {
        d->mTopWidget->setVisible(pVisible);
    }
}

QWidget *DocumentManager::topWidget() const
{
    return d->mTopWidget;
}

void DocumentManager::loadPlugins()
{
    d->loadPlugins();
}

void DocumentManager::actionMenuChecked(bool checked)
{
    d->mSettings->setValue(DocGeneratorPath_1, checked);
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

