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

class DocumentGenerator;
class CatalogNode;
class GeneratorNode;

class DocumentManagerPrivate;
class DOCUMENTMANAGERSHARED_EXPORT DocumentManager : public QObject
{
    Q_OBJECT

public:
    DocumentManager(QObject *parent = 0);
    ~DocumentManager();

    void registerGenerator(DocumentGenerator *pGenerator);
    void print();
    void setVisible(bool pVisible);
    QWidget *topWidget() const;

private slots:
    void loadPlugins();
    void actionMenuChecked(bool checked);

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
    virtual QString displayText() const = 0;
    virtual QIcon icon() const = 0;

    virtual void createNodeTree() = 0;
    virtual GeneratorNode *rootNode() const = 0;
    virtual QAction *associatedAction() const = 0;

};

} //namespace Documents

Q_DECLARE_INTERFACE(Docs::DocumentGenerator,
                    "org.openSankore.DocumentGenerator")

#endif // DOCUMENTMANAGER_H
