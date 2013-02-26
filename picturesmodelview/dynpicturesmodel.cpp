#include "dynpicturesmodel.h"

#include <QtGui>
#include "private/qlistview_p.h"
#include <private/qlistview_p.h>

static const int cellSize = 80;
namespace Constants {
static const QSize gridSize(cellSize, cellSize);
static const QSize iconSize(cellSize / 1.4, cellSize / 1.4);
}

struct Page
{
    Page(const QUrl &pSourceUrl, QImage *pPix = 0)
        : sourceUrl(pSourceUrl)
        , w(0)
        , h(0)
        , titleNo(0)
        , pix(pPix)
    {
    }

    void setSourcePix(const QUrl &pixUrl)
    {
        QImage *newPix = new QImage();
        if (newPix->load(pixUrl.toLocalFile())) {
            if (pix) {
                delete pix;
                pix = 0;
            }
            if (newPix->size() != Constants::iconSize) {
                QImage scaled = newPix->scaled(Constants::iconSize, Qt::KeepAspectRatio);
                delete newPix;
                newPix = new QImage(scaled);
            }
            pix = newPix;
            w = newPix->width();
            h = newPix->height();
        }
    }

    void setImage(QImage *img)
    {
        if (img && !img->isNull()) {
            if (pix) {
                delete pix;
                pix = 0;
            }
            if (img->size() != Constants::iconSize) {
                QImage scaled = img->scaled(Constants::iconSize, Qt::KeepAspectRatio);
                delete img;
                img = new QImage(scaled);
            }
            pix = img;
            w = img->width();
            h = img->height();
        }
    }

    QUrl sourceUrl;
    int w;
    int h;
    int titleNo;
    QImage *pix;
};

class DynPicturesManagerlPrivate
{
public:
    DynPicturesManagerlPrivate(const QUrl &dataUrl)
        : storageUrl(dataUrl)
        , mView(0)
        , mModel(0)
        , mGenerator(0)
    {
        createPages();
        mView = new DPListView();
        mModel = new DPListModel(&pageList);
        mView->setModel(mModel);
        mView->hide();
    }

    ~DynPicturesManagerlPrivate()
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

    void createPages()
    {
        if (!mGenerator) {
            return;
        }

        qint64 pageCount = mGenerator->imageCount();
        QTime currentTime = QTime::currentTime();
        for (int i = 0; i < pageCount; i++) {
            Page *curPage = new Page(QUrl());
            if (i < 100) {
                curPage->setImage(mGenerator->imageForindex(i));
            }
            pageList.append(curPage);
        }
        qDebug() << "creation time " << currentTime.msecsTo(QTime::currentTime());

//        if (storageUrl.isEmpty()) {
//            return;
//        }

//        QDir dir(storageUrl.toLocalFile());
//        if (!dir.exists()) {
//            return;
//        }
//        qint64 counter = 0;
//        QTime currentTime = QTime::currentTime();
//        foreach (QFileInfo fileInfo, dir.entryInfoList(QStringList() << "*.thumbnail.jpg", QDir::Files | QDir::NoDotAndDotDot, QDir::Name)) {

//            QUrl sourceUrl(QUrl(fileInfo.absoluteFilePath()));
//            Page *curPage = new Page(sourceUrl);
//            if (counter < 100) { //tmp test constranints
//               curPage->setSourcePix(sourceUrl);
//            }
//            pageList.append(curPage);
//            counter++;
//        }
//        qDebug() << "elapsed time " << currentTime.msecsTo(QTime::currentTime());

//    //    currentTime = QTime::currentTime();
//    //    foreach (Page *page, pageList){
//    //        delete page->pix;
//    //        page->pix = 0;
//    //    }
//        qDebug() << "deletion time " << currentTime.msecsTo(QTime::currentTime());
    }

private:
    QUrl storageUrl;
    QList<Page*> pageList;
    DPListView *mView;
    DPListModel *mModel;
    DPImageGenerator* mGenerator;

    friend class DynPicturesManager;
};

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

void DynPicturesManager::installPageGenerator(DPImageGenerator *generator)
{
    d->mGenerator = generator;
    d->createPages();
}

QString DynPicturesManager::sizeToString(const QSize &pSize)
{
    return QString("%1x%2").arg(pSize.width()).arg(pSize.height());
}

class DPListModelPrivate
{

public:
    DPListModelPrivate(QList<Page*> *pPageList)
        : pageList(pPageList)
    {
        QImage *newPix = new QImage(Constants::iconSize, QImage::Format_RGB888);
        newPix->fill(Qt::gray);
        emptyImagePatterns.insert(DynPicturesManager::sizeToString(Constants::iconSize), newPix);
    }

    ~DPListModelPrivate()
    {
        foreach(QImage *pix, emptyImagePatterns) {
            delete pix;
            pix = 0;
        }
    }

private:
    QList<Page*> *pageList;
    QMap<QString, QImage*> emptyImagePatterns;

    friend class DPListModel;
};

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
    case Qt::TextAlignmentRole :
        return Qt::AlignBottom;
        break;
    case Qt::DisplayRole :
        return QString("Page %1").arg(index.row());
        break;
    case Qt::DecorationRole :
        QImage *curPix = d->pageList->at(index.row())->pix;
        if (curPix) {
            return *curPix;
        } else {
            Q_ASSERT(d->emptyImagePatterns.values().first());
            return *d->emptyImagePatterns.values().first();
        }
        break;
    }

    return QVariant();
}

DPListView::DPListView(QWidget *parent)
    : QListView(parent)
{
    setViewMode(IconMode);
    setGridSize(Constants::gridSize);
    setResizeMode(Adjust);
    setSelectionMode(ContiguousSelection);
}

QVector<QModelIndex> DPListView::visibleInArea(const QRect &pArea)
{
    Q_UNUSED(pArea)
    QRect rect(pArea.x() + horizontalOffset(), pArea.y() + verticalOffset(), pArea.width(), pArea.height());
    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex> intersectVector = privPtr()->intersectingSet(rect);
    qDebug() << "counting vector" << currentTime.msecsTo(QTime::currentTime());

    currentTime = QTime::currentTime();
    for (int i  = 0; i < intersectVector.count(); i++) {
        QModelIndex index = intersectVector.at(i);
        if (!index.isValid() || !visualRect(index).intersects(pArea)) {
            intersectVector.remove(i);
        }
    }
    qDebug() << "processing vector" << currentTime.msecsTo(QTime::currentTime());

    return intersectVector;
}

void DPListView::resizeEvent(QResizeEvent *e)
{
//    qDebug() << "indecies" << visibleInArea(viewport()->geometry()).count();
    QListView::resizeEvent(e);
}

QListViewPrivate* DPListView::privPtr()
{
    return reinterpret_cast<QListViewPrivate*>(qGetPtrHelper(d_ptr));
}

DPItemDelegate::DPItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

class DPImageGeneratorPrivate
{
public:
    DPImageGeneratorPrivate()
    {
    }

    void processRequestStack() {;}

    friend class DPImageGenerator;
};

DPImageGenerator::DPImageGenerator()
    : d(new DPImageGeneratorPrivate)
{

}

DPImageGenerator::~DPImageGenerator()
{
    if (d) {
        delete d;
    }
}

QImage *DPImageGenerator::imageForindex(int index)
{
    Q_UNUSED(index);
    return 0;
}

void DPImageGenerator::processRequestStack()
{
    d->processRequestStack();
}

//#include "dynpicturesmodel.moc" //add include if going to implement QObject subclasses
