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
class DPListModel;

class DynPicturesManagerlPrivate : public QObject
{
    Q_OBJECT

public:
    DynPicturesManagerlPrivate(const QUrl &dataUrl);
    ~DynPicturesManagerlPrivate();

    void createPages();

private:
    QUrl storageUrl;
    QList<Page*> pageList;
    DPListView *mView;
    DPListModel *mModel;

    friend class DynPicturesManager;
};


class PICTURESMODELVIEWSHARED_EXPORT DynPicturesManager : public QObject
{
    Q_OBJECT

public:
    DynPicturesManager(const QUrl &dataUrl, QObject *parent = 0);
    ~DynPicturesManager();

    void setVisible(bool pVisible);
    DPListView *view() const;

    static QString sizeToString(const QSize &pSize);

private:
    DynPicturesManagerlPrivate* d;

    friend class DynPicturesManagerlPrivate;
};

class DPListModelPrivate : public QObject
{
    Q_OBJECT

public:
    DPListModelPrivate(QList<Page*> *pPageList);
    ~DPListModelPrivate();

private:
    QList<Page*> *pageList;
    QMap<QString, QPixmap*> emptyImagePatterns;

    friend class DPListModel;
};

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
};

class DPItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DPItemDelegate(QObject *parent = 0);
};

#endif // DYNPICTURESMANAGER_H
