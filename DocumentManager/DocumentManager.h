#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "DocumentManager_global.h"

//#include "Node.h"
#include <QObject>
#include <QIcon>

namespace Docs {

enum Role {
    CustomRole = 100
};

class DocumentGenerator;
class CatalogNode;

class DocumentManagerPrivate;
class DOCUMENTMANAGERSHARED_EXPORT DocumentManager : public QObject
{
    Q_OBJECT

public:
    DocumentManager(QObject *parent = 0);
    ~DocumentManager();

    void registerGenerator(DocumentGenerator *pGenerator);
    void print();

private:

    DocumentManagerPrivate* d;

    friend class DocumentManagerPrivate;

};

class DOCUMENTMANAGERSHARED_EXPORT DocumentGenerator : public QObject
{
    Q_OBJECT

public:
    DocumentGenerator(QObject *parent = 0);
    virtual ~DocumentGenerator();

    virtual QString displayName() const = 0;
    virtual QIcon visibleIcon() const;

    virtual void createNodeTree();
    virtual CatalogNode *rootNode() const = 0;

};

} //namespace Documents



#endif // DOCUMENTMANAGER_H
