#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "DocumentManager_global.h"

#include <QObject>
#include <QIcon>
#include <QtPlugin>
#include <QAction>

namespace Docs {

enum Role {
    internalNameRole = 0
    , displayNameRole
    , displayIconRole
    , CustomRole = 100
};

class IDocumentGenerator;
class CatalogNode;
class GeneratorNode;

class DocumentManagerPrivate;
class DOCUMENTSSHARED_EXPORT DocumentManager : public QObject
{
    Q_OBJECT

public:
    DocumentManager(QObject *parent = 0);
    ~DocumentManager();

    void registerGenerator(IDocumentGenerator *pGenerator);
    void print();
    void setVisible(bool pVisible);
    QWidget *topWidget() const;

private slots:
    void actionMenuChecked(bool checked);

private:

    DocumentManagerPrivate* d;

    friend class DocumentManagerPrivate;
};

} //namespace Docs
#endif // DOCUMENTMANAGER_H
