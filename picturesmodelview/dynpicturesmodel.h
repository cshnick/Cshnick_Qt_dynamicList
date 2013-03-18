#ifndef DYNPICTURESMANAGER_H
#define DYNPICTURESMANAGER_H

#include "picturesmodelview_global.h"

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPointer>
#include <QUrl>
#include <QThread>
#include <QMutex>
#include <QFlags>

class DynPicturesManagerlPrivate;
class DPListModelPrivate;
class Page;
class DPListView; 
class DPImageGenerator;
class DPImageServicer;
struct DPImageRequest;
struct DPImageReply;

namespace Globals {
    const int defaultCellSize = 80;
    const int maxCellSize = 250;
    const int pageRole = Qt::UserRole + 1;
    const Qt::TransformationMode defTRansformationMode = Qt::SmoothTransformation;
    const int cleanerTimerInterval = 10000; //10 seconds
    const int saveMemoryMultipler = 1; //Allways save current viewport and cache indexes above and under the visible area;
                                       //save visible QModelIndex count * saveMemoryMultipler on top and on the bottom
    const bool useMemoryCleaner = true;
}

class PICTURESMODELVIEWSHARED_EXPORT DynPicturesManager : public QObject
{
    Q_OBJECT

public:
    typedef QMap<QString, QImage*> EmptyPatterns;

    DynPicturesManager(const QUrl &dataUrl, QObject *parent = 0);
    ~DynPicturesManager();

    void setVisible(bool pVisible);
    DPListView *view(int index = 0) const;
    QWidget *widget() const;
    void installPageGenerator(DPImageServicer *generator);

    static void setCellSize(int newSize);
    static QSize gridSize();
    static QSize iconSize();
    static QSize iconSizeFromGridSize(const QSize &gridSize);
    static int maxIconSize();

    static QString sizeToString(const QSize &pSize);

protected:
    bool eventFilter(QObject *, QEvent *);

signals:
    void requestCleanImageServicerQueue();
    void sliderReleased();

private slots:
    void cleanMemory();
    void startCleaningTimer();
    void pauseCleaningTimer();
    void playCleaningTimer();
    void setMaxIconSize(const QSize &pSz);

private:
    DynPicturesManagerlPrivate* d;

    friend class DynPicturesManagerlPrivate;
};

class DPListModel;

class DPListModel : public QAbstractListModel
{
    Q_OBJECT

public:
     DPListModel(QList<Page*> *pPageList, QObject *parent = 0);
    ~DPListModel();
    void setEmptyImagePatterns(DynPicturesManager::EmptyPatterns *pPatterns);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private slots:
    void reactOnImageReply(DPImageReply reply);

private:
    DPListModelPrivate *d;

    friend class DPListModelPrivate;
};

class DPListView : public QListView
{
    Q_OBJECT

public:
    enum eAdditionalFlag {
        fAutoExpanding = 1
    };
    Q_DECLARE_FLAGS(eAdditionalFlags, eAdditionalFlag)

    DPListView (QWidget *parent = 0);
//    QVector<QModelIndex> visibleInArea(const QRect &rect);
    QVector<QModelIndex> visibleInArea(const QRect &rect);
    QVector<QModelIndex> cachedIndexes();

    void setAdditionalFlags(eAdditionalFlags flags);
    eAdditionalFlags additionalFlags();

protected:
    void resizeEvent(QResizeEvent *);
    void scrollContentsBy(int dx, int dy);

signals:
    void sendRequest(DPImageRequest pRequest);
    void manipulateContentsStarted();
    void manipulateContentsFinished();
    void iconSizeChanged(const QSize &newSize);

private slots:
    void setNewGridSize(int newSize);
    void reactOnSliderReleased();

private:
    QListViewPrivate *privPtr();
    void updateEmptyPagesData();

    eAdditionalFlags mAdditionalFlags;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(DPListView::eAdditionalFlags)

class DPItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DPItemDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;
};

struct DPImageRequest
{
    enum priority {
        veryLow = 0
        , veryHigh
    };

    DPImageRequest()
        : w(0)
        , h(0)
        , pageNo(-1)
        , generator(0)
        , pr(veryLow) {;}
    DPImageRequest(int pw, int ph, int ppn, DPImageGenerator *pGenerator, priority ppr = veryHigh)
        : w(pw)
        , h(ph)
        , pageNo(ppn)
        , generator(pGenerator)
        , pr(ppr) {;}
    bool isValid() {return w && h && pageNo != -1;}
    int w;
    int h;
    qint64 pageNo;
    DPImageGenerator *generator;
    priority pr;
};

struct DPImageReply : public DPImageRequest
{
    DPImageReply() {;}
    DPImageReply(int pw, int ph, int ppn, DPImageGenerator *pGenerator,const QImage &pImage)
        : DPImageRequest(pw, ph, ppn, pGenerator), image(pImage) {;}
    DPImageReply(DPImageRequest req, const QImage &pImage)
        : DPImageRequest(req), image(pImage) {;}

    QImage image;
};

class DPImageServicerPrivate;
class PICTURESMODELVIEWSHARED_EXPORT DPImageServicer : public QThread
{
    Q_OBJECT

public:
    DPImageServicer(QObject *parent = 0);
    virtual ~DPImageServicer();
    void addRequest(DPImageRequest req);

    virtual QImage imageForindex(int index) const;
    virtual qint64 imageCount() const = 0;

protected:
    void run();

signals:
    void sendReply(DPImageReply reply);

private slots:
    void replyOnRequest(DPImageRequest request);
    void replyCleanImageServicerQueue();

private:
    DPImageServicerPrivate *d;

    friend class DPImageServicerPrivate;
};

#endif // DYNPICTURESMANAGER_H
