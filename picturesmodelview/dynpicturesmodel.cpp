#include "dynpicturesmodel.h"

#include <QtGui>

struct Page
{
    Page(const QUrl &pSourceUrl)
        : sourceUrl(pSourceUrl)
    {
        w = 0;
        h = 0;
    }

    int w;
    int h;
    QUrl sourceUrl;
};

DynPicturesManagerlPrivate::DynPicturesManagerlPrivate(const QUrl &dataUrl)
    : storageUrl(dataUrl)
    , mView(0)
    , mModel(0)
{
    createPages();
    mView = new DPListView();
    mModel = new DPListModel(&pageList);
    mView->setModel(mModel);
    mView->hide();
}

DynPicturesManagerlPrivate::~DynPicturesManagerlPrivate()
{
    if (mView) {
        delete mView;
    }
    qDeleteAll(pageList);
    pageList.clear();

    if (mModel) {
        delete mModel;
    }
}
void DynPicturesManagerlPrivate::createPages()
{
    if (storageUrl.isEmpty()) {
        return;
    }
    QDir dir(storageUrl.toLocalFile());
    if (!dir.exists()) {
        return;
    }
    qint64 counter = 0;
    foreach (QFileInfo fileInfo, dir.entryInfoList(QStringList() << "*.thumbnail.jpg", QDir::Files | QDir::NoDotAndDotDot, QDir::Name)) {
        pageList.append(new Page(QUrl(fileInfo.absoluteFilePath())));
        counter++;
    }
}


DynPicturesManager::DynPicturesManager(const QUrl &dataUrl, QObject *parent)
    :QObject(parent)
    , d(new DynPicturesManagerlPrivate(dataUrl))
{
}

DynPicturesManager::~DynPicturesManager()
{
    if (d) {
        delete d;
    }
}

void DynPicturesManager::setVisible(bool pVisible)
{
    Q_ASSERT(d->mView);
    d->mView->setVisible(pVisible);
}

DPListView *DynPicturesManager::view() const
{
    return d->mView;
}

QString DynPicturesManager::sizeToString(const QSize &pSize)
{
    return QString("%1x%2").arg(pSize.width()).arg(pSize.height());
}


DPListModelPrivate::DPListModelPrivate(QList<Page*> *pPageList)
    : pageList(pPageList)
{
    QSize defaultPixSize(64, 64);
    QPixmap *newPix = new QPixmap(defaultPixSize);
    newPix->fill(Qt::gray);
    emptyImagePatterns.insert(DynPicturesManager::sizeToString(defaultPixSize), newPix);
}

DPListModelPrivate::~DPListModelPrivate()
{
    foreach(QPixmap *pix, emptyImagePatterns) {
        delete pix;
        pix = 0;
    }
}

DPListModel::DPListModel(QList<Page*> *pPageList, QObject *parent)
    : QAbstractListModel(parent)
    , d(new DPListModelPrivate(pPageList))
{
}

DPListModel::~DPListModel()
{
    if (d) {
        delete d;
    }
}

int DPListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->pageList->count();
}

int DPListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DPListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole :
        return QString("Page %1").arg(index.row());
        break;
    case Qt::DecorationRole :
        Q_ASSERT(*d->emptyImagePatterns.values().first());
        return *d->emptyImagePatterns.values().first();
        break;
    }

    return QVariant();
}
DPListView::DPListView(QWidget *parent)
    : QListView(parent)
{
    setViewMode(IconMode);
    setGridSize(QSize(80, 80));
    setResizeMode(Adjust);
    setSelectionMode(ContiguousSelection);
}

DPItemDelegate::DPItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{
}
