#include "dynpicturesmodel.h"

#include <QtGui>
#include <private/qlistview_p.h>

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

    void setImage(QImage *img)
    {
        if (img) {
            if (pix) {
                delete pix;
                pix = 0;
            }
//            if (img->size() != Globals::defaultIconSize) {
//                QImage scaled = img->scaled(Globals::defaultIconSize, Qt::KeepAspectRatio);
//                delete img;
//                img = new QImage(scaled);
//            }
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
    DynPicturesManagerlPrivate(DynPicturesManager *pq, const QUrl &dataUrl)
        : q(pq)
        , storageUrl(dataUrl)
        , mView(0)
        , mModel(0)
        , mGeneratorThread(0)
        , mCentralWidget(0)
        , mCleaningMemTimer(0)
    {
        mView = new DPListView();
        mModel = new DPListModel(&pageList);
        QSize emptyPageSize = QSize(mCellSize, mCellSize);
        emptyImagePatterns.insert(DynPicturesManager::sizeToString(emptyPageSize), createBlancImage(emptyPageSize));
        mModel->setEmptyImagePatterns(&emptyImagePatterns);
        mView->setModel(mModel);
        setupUi();
        mCleaningMemTimer = new QTimer();
        QObject::connect(mCleaningMemTimer, SIGNAL(timeout()), q, SLOT(cleanMemory()));
        QTimer::singleShot(0, q, SLOT(startCleaningTimer()));
    }

    ~DynPicturesManagerlPrivate()
    {
        if(mCleaningMemTimer) {
            delete mCleaningMemTimer;
        }
        if (mCentralWidget) {
            delete mCentralWidget;
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
        if (!mGeneratorThread) {
            return;
        }

        qint64 pageCount = mGeneratorThread->imageCount();
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

    void installPageGenerator(DPImageServicer *generator)
    {
        mGeneratorThread = generator;
        if (mGeneratorThread) {

            createPages();

            QObject::connect(mView, SIGNAL(sendRequest(DPImageRequest)), mGeneratorThread, SLOT(replyOnRequest(DPImageRequest)));
            QObject::connect(mGeneratorThread, SIGNAL(sendReply(DPImageReply)), mModel, SLOT(reactOnImageReply(DPImageReply)));

            mGeneratorThread->start(QThread::LowPriority);
        }
    }

    void setupUi()
    {
        mCentralWidget = new QWidget();
        QVBoxLayout *mainLayer = new QVBoxLayout();

        QHBoxLayout *widgetsLayout = new QHBoxLayout();
        widgetsLayout->addWidget(new QListView);
        widgetsLayout->addWidget(mView, 1);

        QHBoxLayout *sliderLayout = new QHBoxLayout();
        sliderLayout->addSpacerItem(new QSpacerItem(300, 0, QSizePolicy::Expanding));
        QPushButton *magicButton = new QPushButton("Don't push");
        QObject::connect(magicButton, SIGNAL(clicked()), q, SLOT(cleanMemory()));
        sliderLayout->addWidget(magicButton);
        QSlider *slider = new QSlider(Qt::Horizontal);
        slider->setMinimum(Globals::defaultCellSize);
        slider->setMaximum(Globals::maxCellSize);
        QObject::connect(slider, SIGNAL(valueChanged(int)), mView, SLOT(setNewGridSize(int)));
        sliderLayout->addWidget(slider);

        mainLayer->addLayout(widgetsLayout);
        mainLayer->addLayout(sliderLayout);

        mCentralWidget->setLayout(mainLayer);
        mCentralWidget->setGeometry(0, 0, 800, 600);
        mCentralWidget->setVisible(false);
    }

private:
    DynPicturesManager *q;
    QUrl storageUrl;
    QList<Page*> pageList;
    DPListView *mView;
    DPListModel *mModel;
    DynPicturesManager::EmptyPatterns emptyImagePatterns;
    DPImageServicer *mGeneratorThread;
    QWidget *mCentralWidget;
    QTimer *mCleaningMemTimer;

    static int mCellSize;

    friend class DynPicturesManager;
};
int DynPicturesManagerlPrivate::mCellSize = Globals::defaultCellSize;

DynPicturesManager::DynPicturesManager(const QUrl &dataUrl, QObject *parent)
    :QObject(parent)
    , d(new DynPicturesManagerlPrivate(this, dataUrl))
{
    qRegisterMetaType<DPImageRequest>("DPImageRequest");
    qRegisterMetaType<DPImageReply>("DPImageReply");
}

DynPicturesManager::~DynPicturesManager()
{
    if (d) {
        delete d;
    }
}

void DynPicturesManager::setVisible(bool pVisible)
{
    Q_ASSERT(d->mCentralWidget);
    d->mCentralWidget->setVisible(pVisible);
}

DPListView *DynPicturesManager::view() const
{
    return d->mView;
}

QWidget *DynPicturesManager::widget() const
{
    return d->mCentralWidget;
}

void DynPicturesManager::installPageGenerator(DPImageServicer *generator)
{
    d->installPageGenerator(generator);
}

void DynPicturesManager::setCellSize(int newSize)
{
    DynPicturesManagerlPrivate::mCellSize = newSize;
}

QSize DynPicturesManager::gridSize()
{
    return QSize(DynPicturesManagerlPrivate::mCellSize, DynPicturesManagerlPrivate::mCellSize);
}

QSize DynPicturesManager::iconSize()
{
    return QSize(DynPicturesManagerlPrivate::mCellSize - 35, DynPicturesManagerlPrivate::mCellSize - 35);
}

QString DynPicturesManager::sizeToString(const QSize &pSize)
{
    return QString("%1x%2").arg(pSize.width()).arg(pSize.height());
}

void DynPicturesManager::cleanMemory()
{

    QTime currentTime = QTime::currentTime();

    for (int i = 0; i < d->pageList.count(); i++) {
        bool intersect = false;
        QVector<QModelIndex> cachIndexes(d->mView->cachedIndexes());
        foreach(QModelIndex curIndex, cachIndexes)
        {
            if (curIndex.row() == i) {
                intersect = true;
                break;
            }
        }
        if (!intersect) {
            Page *curPage = d->pageList.at(i);
            delete curPage->pix;
            curPage->pix = 0;
        }
    }

    qDebug() << "Cleaning memory finished. Elapsed time" << currentTime.msecsTo(QTime::currentTime());
}

void DynPicturesManager::startCleaningTimer()
{
    if (d->mCleaningMemTimer) {
        d->mCleaningMemTimer->start(Globals::cleanerTimerInterval);
    }
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
    case Globals::pageRole :
        return QVariant::fromValue(d->pageList->at(index.row()));

    case Qt::TextAlignmentRole :
        return Qt::AlignCenter;
        break;
    case Qt::DisplayRole :
        return QString("Page %1").arg(index.row());
        break;
    case Qt::DecorationRole :
        QImage *&curPix = d->pageList->at(index.row())->pix;
//        if (curPix) {
//            delete curPix;
//            curPix = 0;
//        }
//        if (curPix) {
//            return *curPix;
//        } else {
//            Q_ASSERT(d->emptyImagePatterns && d->emptyImagePatterns->values().first());
//            return *d->emptyImagePatterns->values().first();
//        }
//        QPixmap newPix("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236/page002.thumbnail.jpg");
//        return QIcon(newPix);
        QString fileName = QString("page%1.thumbnail.jpg").arg(index.row(), 3, 10, QLatin1Char('0'));
        QString filePath = "/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236/" + fileName;
        Q_ASSERT(QFileInfo(filePath).exists());

//        return QIcon(QPixmap(filePath));
        return *curPix;
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
        curPage->setImage(reply.image);
        QModelIndex curIndex  = index(indexRow);
        emit dataChanged(curIndex, curIndex);
    }
}

DPListView::DPListView(QWidget *parent)
    : QListView(parent)
{
    setViewMode(IconMode);
    setGridSize(DynPicturesManager::gridSize());
    setIconSize(DynPicturesManager::iconSize());
    setResizeMode(Adjust);
    setSelectionMode(ContiguousSelection);
}

QVector<QModelIndex> DPListView::visibleInArea(const QRect &pArea)
{
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


QVector<QModelIndex> DPListView::cachedIndexes()
{
    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex>visibleIndexesVector(visibleInArea(viewport()->geometry()));
    if (!visibleIndexesVector.count()) {
        return visibleIndexesVector;
    }

    int numIndexes = visibleIndexesVector.count();
    int minIndex = visibleIndexesVector.first().row();
    int maxIndex = visibleIndexesVector.first().row();
    foreach (QModelIndex index, visibleIndexesVector) {
        int indexRow = index.row();
        if (indexRow < minIndex) {
            minIndex = indexRow;
        } else if (indexRow > maxIndex) {
            maxIndex = indexRow;
        }
    }
    int numIndexesToCache = numIndexes * Globals::saveMemoryMultipler;
    //filling indexes above the visible
    int startIndex = qMax(0, minIndex - numIndexesToCache);
    for (int i = minIndex - 1; i >= startIndex; i--) {
        QModelIndex curIndex = model()->index(i, 0);
        visibleIndexesVector.prepend(curIndex);
    }
    //filling indexes under the visible
    int endIndex = qMin(model()->rowCount() - 1, maxIndex + numIndexesToCache);
    for (int i = maxIndex + 1; i <= endIndex; i++) {
        QModelIndex curIndex = model()->index(i, 0);
        visibleIndexesVector.append(curIndex);
    }
    qDebug() << "cachedIndexes() function time" << currentTime.msecsTo(QTime::currentTime());

    return visibleIndexesVector;
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

void DPListView::setNewGridSize(int newSize)
{
    DynPicturesManager::setCellSize(newSize);
    setGridSize(DynPicturesManager::gridSize());
    setIconSize(DynPicturesManager::iconSize());
//    qDebug() << "scroller value" << newSize;
}

QListViewPrivate* DPListView::privPtr()
{
    return reinterpret_cast<QListViewPrivate*>(qGetPtrHelper(d_ptr));
}

void DPListView::updateEmptyPagesData()
{
    QTime currentTime = QTime::currentTime();
    QVector<QModelIndex>visibleIndexesVector(visibleInArea(viewport()->geometry()));
    if (!visibleIndexesVector.count()) {
        return;
    }

    int numIndexes = visibleIndexesVector.count();
    int minIndex = visibleIndexesVector.first().row();
    int maxIndex = visibleIndexesVector.first().row();
    foreach (QModelIndex index, visibleIndexesVector) {
        int indexRow = index.row();
        if (indexRow < minIndex) {
            minIndex = indexRow;
        } else if (indexRow > maxIndex) {
            maxIndex = indexRow;
        }
        Page *curPage = index.data(Globals::pageRole).value<Page*>();
        if (!curPage->pix) {
            DPImageRequest request(DynPicturesManager::iconSize().width(), DynPicturesManager::iconSize().height(), index.row(), 0);
            emit sendRequest(request);
        }
    }
    int numIndexesToCache = numIndexes * Globals::saveMemoryMultipler;
    //caching rect above the visible
    int startIndex = qMax(0, minIndex - numIndexesToCache);
    for (int i = startIndex; i < minIndex; i++) {
        Page *curPage = model()->index(i, 0).data(Globals::pageRole).value<Page*>();
        if (!curPage->pix) {
            DPImageRequest request(DynPicturesManager::iconSize().width(), DynPicturesManager::iconSize().height(), i, 0, DPImageRequest::veryLow);
            emit sendRequest(request);
        }
    }
    //caching rect under the visible
    int endIndex = qMin(model()->rowCount() - 1, maxIndex + numIndexesToCache);
    for (int i = maxIndex + 1; i <= endIndex; i++) {
        Page *curPage = model()->index(i, 0).data(Globals::pageRole).value<Page*>();
        if (!curPage->pix) {
            DPImageRequest request(DynPicturesManager::iconSize().width(), DynPicturesManager::iconSize().height(), i, 0, DPImageRequest::veryLow);
            emit sendRequest(request);
        }
    }
    qDebug() << "numIndexes" << numIndexes << "numIndexesToCache" << numIndexesToCache
             << "start above cache from" << startIndex << "endCache with" << minIndex << endl
             << "visible index from " << minIndex << "to" << maxIndex << endl
             << "start bottom cache from" << maxIndex + 1 << "end cache with" << endIndex << endl;

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

        if (image || image->size() != QSize(newWidth, newHeight)) {
            QImage scaled = image->scaled(newWidth, newHeight, Qt::KeepAspectRatio);
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

    void run()
    {
        forever {
            mMutex.lock();
            DPImageRequest curReq = requests.isEmpty() ? DPImageRequest() : requests.takeFirst();
            mMutex.unlock();

            if (abort) {
                return;
            }
            if (curReq.isValid()) {
//                mMutex.lock();
                QImage *image = q->imageForindex(curReq.pageNo);
//                mMutex.unlock();
                if (image) {
                    if (!image->isNull() && image->size() != QSize(curReq.w, curReq.h)) {
                        QImage scaled = image->scaled(curReq.w, curReq.h, Qt::KeepAspectRatio, Globals::defTRansformationMode);
                        delete image;
                        image = new QImage(scaled);
                    } else {
                        delete image;
                        image = 0;
                    }
                }

                emit q->sendReply(DPImageReply(image->width(), image->height(), curReq.pageNo, 0, image));
            }

            mMutex.lock();
            if (requests.isEmpty())
                mCondition.wait(&mMutex);
            mMutex.unlock();
        }
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

QImage *DPImageServicer::imageForindex(int index) const
{
    Q_UNUSED(index);
    return 0;
}

void DPImageServicer::run()
{
    d->run();
}

void DPImageServicer::replyOnRequest(DPImageRequest request)
{
    addRequest(request);
}

//#include "dynpicturesmodel.moc" //add include if going to implement QObject subclasses
