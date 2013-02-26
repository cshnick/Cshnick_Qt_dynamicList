#ifndef DYNPICTURESMANAGER_H
#define DYNPICTURESMANAGER_H

#include "picturesmodelview_global.h"

#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPointer>
#include <QUrl>

class DynPicturesManagerlPrivate;
class DPListModelPrivate;
class Page;
class DPListView; 
class DPImageGenerator;

class PICTURESMODELVIEWSHARED_EXPORT DynPicturesManager : public QObject
{
    Q_OBJECT

public:
    DynPicturesManager(const QUrl &dataUrl, QObject *parent = 0);
    ~DynPicturesManager();

    void setVisible(bool pVisible);
    DPListView *view() const;
    void installPageGenerator(DPImageGenerator *generator);

    static QString sizeToString(const QSize &pSize);

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

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

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

private:
    QListViewPrivate *privPtr();
};

class DPItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DPItemDelegate(QObject *parent = 0);
};

struct DPImageRequest
{
    int w;
    int h;
    int pageNo;
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

private slots:
    void processRequestStack();

private:
    DPImageGeneratorPrivate *d;

    friend class DPImageGeneratorPrivate;

};

#endif // DYNPICTURESMANAGER_H
