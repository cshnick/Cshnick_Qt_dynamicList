#ifndef TSTDOCGENERATOR1_H
#define TSTDOCGENERATOR1_H

#include "TstDocGenerator1_global.h"

#include <QUrl>
//#include "DocumentManager.h"
#include "IDocumentGenerator.h"
#include "Node.h"
#include "ICommonInterface.h"
#include "DocumentManager.h"

class QDomDocument;

enum {
    DocumentNodeType = Docs::UserType
};

class DocumentNodePrivate;
class DocumentNode : public Docs::Node
{
    Q_OBJECT

public:
    DocumentNode(QObject *parent = 0);
    ~DocumentNode();
    int type() const;

    QString displayName() const;
    QIcon displayIcon() const;
    bool isCatalog() const;
    void setMetadata(const QDomDocument &data);

private:
    DocumentNodePrivate *d;

    friend class DocumentNodePrivate;

};

class TstDocGenerator1Private;
class TSTDOCGENERATOR1SHARED_EXPORT TstDocGenerator1 : public Docs::IDocumentGenerator, public Plugins::ICommonInterface
{
    Q_OBJECT
    Q_INTERFACES(Docs::IDocumentGenerator Plugins::ICommonInterface)

public:
    TstDocGenerator1(QObject *parent = 0);

    QString displayText() const;
    QIcon icon() const;

    void createNodeTree();
    Docs::GeneratorNode *rootNode() const;
    QAction *associatedAction() const;
    Plugins::PInfoHandler pluginMeta() const;

private:
    TstDocGenerator1Private *d;

    friend class TstDocGenerator1Private;
};

#endif // TSTDOCGENERATOR1_H
