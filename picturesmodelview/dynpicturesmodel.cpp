#include "dynpicturesmodel.h"

#include <QtGui>
#include <private/qlistview_p.h>

static const int cellSize = 80;
namespace Constants {
static const QSize gridSize(cellSize, cellSize);
static const QSize iconSize(cellSize / 1.4, cellSize / 1.4);
static const int pageRole = Qt::UserRole + 1;
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
Q_DECLARE_METATYPE(Page*)

class DynPicturesManagerlPrivate
{
public:
    DynPicturesManagerlPrivate(const QUrl &dataUrl)
        : storageUrl(dataUrl)
        , mView(0)
        , mModel(0)
        , mGenerator(0)
        , mGeneratorThread(0)
    {
        mView = new DPListView();
        mModel = new DPListModel(&pageList);
        emptyImagePatterns.insert(DynPicturesManager::sizeToString(Constants::iconSize), createBlancImage(Constants::iconSize));
        mModel->setEmptyImagePatterns(&emptyImagePatterns);
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
        if (mGeneratorThread) {
            delete mGeneratorThread;
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
//            if (i < 100) {
//                curPage->setImage(mGenerator->imageForindex(i));
//            }
            pageList.append(curPage);
        }
        qDebug() << "creation time " << currentTime.msecsTo(QTime::currentTime());
    }

    QImage *createBlancImage(const QSize &size)
    {
        QImage *newPix = new QImage(size, QImage::Format_ARGB32);
        newPix->fill(Qt::transparent);
        QPainter painter;
        painter.begin(newPix);
        painter.setRenderHints(QPainter::Antialiasing, true);

        int shadowThickness = 3;
        int width = size.width() - 1;
        int height = size.height() - 1;

        QLinearGradient gradient;
        gradient.setStart(0,0);
        gradient.setFinalStop(width,0);
        QColor grey1(150,150,150,125);
        QColor grey2(225,225,225,125);

        gradient.setColorAt((qreal)0, grey1);
        gradient.setColorAt((qreal)1, grey2);

        QBrush brush(gradient);
        painter.setBrush( brush);

        painter.setPen(Qt::NoPen);

        QPointF topLeft (shadowThickness+0, shadowThickness+0);
        QPointF bottomRight (width, height);

        QRectF rect (topLeft, bottomRight);

        painter.drawRoundRect(rect,20,20);

        // draw the top box, the visible one
        QBrush brush2(QColor(250,250,250,255),Qt::SolidPattern);

        painter.setBrush(brush2);

        QPointF topLeft2 (0, 0);
        QPointF bottomRight2 (width - shadowThickness, height - shadowThickness);

        QRectF rect2 (topLeft2, bottomRight2);

        painter.drawRoundRect(rect2,25,25);
        painter.end();

        return newPix;
    }

    void installPageGenerator(DPImageGenerator *generator)
    {
        mGenerator = generator;
        createPages();
        if (!mGeneratorThread) {
            qRegisterMetaType<DPImageRequest>("DPImageRequest");
            qRegisterMetaType<DPImageReply>("DPImageReply");
            mGeneratorThread = new DPImageServicer();


            QObject::connect(mView, SIGNAL(sendRequest(DPImageRequest)), mGeneratorThread, SLOT(replyOnRequest(DPImageRequest)));
            QObject::connect(mGeneratorThread, SIGNAL(sendReply(DPImageReply)), mModel, SLOT(reactOnImageReply(DPImageReply)));

            mGeneratorThread->start(QThread::LowPriority);
        }

        createPages();
    }

private:
    QUrl storageUrl;
    QList<Page*> pageList;
    DPListView *mView;
    DPListModel *mModel;
    DPImageGenerator* mGenerator;
    DynPicturesManager::EmptyPatterns emptyImagePatterns;
    DPImageServicer *mGeneratorThread;

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
    d->installPageGenerator(generator);
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
        , emptyImagePatterns(0)
    {
    }

    ~DPListModelPrivate()
    {
        foreach(QImage *pix, *emptyImagePatterns) {
            delete pix;
            pix = 0;
        }
        emptyImagePatterns->clear();
    }

private:
    QList<Page*> *pageList;
    DynPicturesManager::EmptyPatterns *emptyImagePatterns;

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

void DPListModel::setEmptyImagePatterns(DynPicturesManager::EmptyPatterns *pPatterns)
{
    d->emptyImagePatterns = pPatterns;
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
    case Constants::pageRole :
        return QVariant::fromValue(d->pageList->at(index.row()));

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
            Q_ASSERT(d->emptyImagePatterns && d->emptyImagePatterns->values().first());
            return *d->emptyImagePatterns->values().first();
        }
        break;
    }

    return QVariant();
}

void DPListModel::reactOnImageReply(DPImageReply reply)
{
    int indexRow = reply.pageNo;
    if (d->pageList && indexRow != -1 && indexRow < d->pageList->count()) {
        Page *curPage = d->pageList->at(indexRow);
        Q_ASSERT(curPage);
        curPage->pix = reply.image;
    }
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
    updateEmptyPagesData();
    QListView::resizeEvent(e);
}

void DPListView::scrollContentsBy(int dx, int dy)
{
//    qDebug() << "content is scrolled by dx" << dx << "dy" << dy;
    updateEmptyPagesData();
    QListView::scrollContentsBy(dx, dy);
}

QListViewPrivate* DPListView::privPtr()
{
    return reinterpret_cast<QListViewPrivate*>(qGetPtrHelper(d_ptr));
}

void DPListView::updateEmptyPagesData()
{
    QTime currentTime = QTime::currentTime();
    foreach (QModelIndex index, visibleInArea(viewport()->geometry())) {
        Page *curPage = index.data(Constants::pageRole).value<Page*>();
        if (!curPage->pix) {
            DPImageRequest request(Constants::iconSize.width(), Constants::iconSize.height(), index.row(), 0);
            emit sendRequest(request);
        }
    }
    qDebug() << "sendRequest time " << currentTime.msecsTo(QTime::currentTime());
}

DPItemDelegate::DPItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

class DPImageGeneratorPrivate
{
public:
    DPImageGeneratorPrivate(DPImageGenerator *pq)
        : q(pq)
    {
    }

    void replyOnRequest(DPImageRequest request)
    {
        int newWidth = request.w;
        int newHeight = request.h;
        qint64 pageIndex = request.pageNo;

        QImage *image = q->imageForindex(pageIndex);

        if (image->size() != QSize(newWidth, newHeight)) {
            QImage scaled = image->scaled(Constants::iconSize, Qt::KeepAspectRatio);
            delete image;
            image = new QImage(scaled);
        }

        emit q->sendReply(DPImageReply(request, image));
    }
private:
    DPImageGenerator* q;

    friend class DPImageGenerator;
};

DPImageGenerator::DPImageGenerator()
    : d(new DPImageGeneratorPrivate(this))
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

void DPImageGenerator::replyOnRequest(DPImageRequest request)
{
    QMutexLocker locker(&mutex);
    d->replyOnRequest(request);
}

class DPImageServicerPrivate
{
    DPImageServicerPrivate(DPImageServicer *pq)
        : abort(false)
        , q(pq)
    {

    }
    ~DPImageServicerPrivate()
    {
        mMutex.lock();
        abort = true;
        mCondition.wakeOne();
        mMutex.unlock();

        q->wait();
    }

    void addRequest(DPImageRequest req)
    {
        mMutex.lock();
        switch (req.pr) {
        case DPImageRequest::veryLow :
            requests.append(req);
            break;
        case DPImageRequest::veryHigh :
            requests.prepend(req);
            break;
        }
        mCondition.wakeOne();
        mMutex.unlock();
    }

private:
    QMutex mMutex;
    QWaitCondition mCondition;
    bool abort;
    DPImageServicer *q;
    QList<DPImageRequest> requests;

    friend class DPImageServicer;
};

DPImageServicer::DPImageServicer(QObject *parent)
    : QThread(parent)
    , d(new DPImageServicerPrivate(this))
{

}

DPImageServicer::~DPImageServicer()
{
    if (d) {
        delete d;
    }
}

void DPImageServicer::addRequest(DPImageRequest req)
{
    d->addRequest(req);
}

void DPImageServicer::run()
{
    forever {
        d->mMutex.lock();
        DPImageRequest curReq = d->requests.isEmpty() ? DPImageRequest() : d->requests.takeFirst();
        d->mMutex.unlock();

        if (d->abort) {
            return;
        }
        if (curReq.isValid()) {
            QImage *image = new QImage(curReq.w, curReq.h, QImage::Format_ARGB32);
            image->fill(Qt::black);
            emit sendReply(DPImageReply(curReq, image));
        }

        d->mMutex.lock();
        if (d->requests.isEmpty())
            d->mCondition.wait(&(d->mMutex));
        d->mMutex.unlock();
    }
}

void DPImageServicer::replyOnRequest(DPImageRequest request)
{
    addRequest(request);
}

//#include "dynpicturesmodel.moc" //add include if going to implement QObject subclasses
