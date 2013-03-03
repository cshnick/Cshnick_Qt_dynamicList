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

class DynPicturesManagerlPrivate;
class DPListModelPrivate;
class Page;
class DPListView; 
class DPImageGenerator;
class DPImageServicer;
struct DPImageRequest;
struct DPImageReply;

namespace Globals {
    const int defaultCellSize = 150;
    const int maxCellSize = 200;
    const int pageRole = Qt::UserRole + 1;
    const Qt::TransformationMode defTRansformationMode = Qt::SmoothTransformation;
}

class PICTURESMODELVIEWSHARED_EXPORT DynPicturesManager : public QObject
{
    Q_OBJECT

public:
    typedef QMap<QString, QImage*> EmptyPatterns;

    DynPicturesManager(const QUrl &dataUrl, QObject *parent = 0);
    ~DynPicturesManager();

    void setVisible(bool pVisible);
    DPListView *view() const;
    QWidget *widget() const;
    void installPageGenerator(DPImageServicer *generator);

    static void setCellSize(int newSize);
    static QSize gridSize();
    static QSize iconSize();

    static QString sizeToString(const QSize &pSize);

private slots:
    void cleanMemory();

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
    DPListView (QWidget *parent = 0);
    QVector<QModelIndex> visibleInArea(const QRect &pArea);

protected:
    void resizeEvent(QResizeEvent *);
    void scrollContentsBy(int dx, int dy);

signals:
    void sendRequest(DPImageRequest pRequest);

private slots:
    void setNewGridSize(int newSize);

private:
    QListViewPrivate *privPtr();
    void updateEmptyPagesData();
};

class DPItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DPItemDelegate(QObject *parent = 0);
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
    DPImageReply() : DPImageRequest(), image(0) {;}
    DPImageReply(int pw, int ph, int ppn, DPImageGenerator *pGenerator,QImage *pImage)
        : DPImageRequest(pw, ph, ppn, pGenerator), image(pImage) {;}
    DPImageReply(DPImageRequest req, QImage *pImage)
        : DPImageRequest(req), image(pImage) {;}

    QImage *image;
};

class DPImageGeneratorPrivate;

class PICTURESMODELVIEWSHARED_EXPORT DPImageGenerator : public QObject
{
    Q_OBJECT

public:
    DPImageGenerator();
    virtual ~DPImageGenerator();

    virtual QImage *imageForindex(int index);
    virtual qint64 imageCount() = 0;

signals:
    void sendReply(DPImageReply reply);

private slots:
    void replyOnRequest(DPImageRequest request);

private:
    DPImageGeneratorPrivate *d;
    QMutex mutex;

    friend class DPImageGeneratorPrivate;

};

class DPImageServicerPrivate;
class PICTURESMODELVIEWSHARED_EXPORT DPImageServicer : public QThread
{
    Q_OBJECT

public:
    DPImageServicer(QObject *parent = 0);
    virtual ~DPImageServicer();
    void addRequest(DPImageRequest req);

    virtual QImage *imageForindex(int index) const;
    virtual qint64 imageCount() const = 0;

protected:
    void run();

signals:
    void sendReply(DPImageReply reply);

private slots:
    void replyOnRequest(DPImageRequest request);

private:
    DPImageServicerPrivate *d;

    friend class DPImageServicerPrivate;
};

#endif // DYNPICTURESMANAGER_H
