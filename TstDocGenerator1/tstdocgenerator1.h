#ifndef TSTDOCGENERATOR1_H
#define TSTDOCGENERATOR1_H

#include "TstDocGenerator1_global.h"

#include <QUrl>
#include "DocumentManager.h"
#include "Node.h"

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
    void setMetadata(const QDomDocument &data);

private:
    DocumentNodePrivate *d;

    friend class DocumentNodePrivate;

};

class TstDocGenerator1Private;
class TSTDOCGENERATOR1SHARED_EXPORT TstDocGenerator1 : public Docs::DocumentGenerator
{
    Q_OBJECT

public:
    TstDocGenerator1(QObject *parent = 0);

    QString displayText() const;
    QIcon icon() const;

    void createNodeTree();
    Docs::GeneratorNode *rootNode() const;


private:
    TstDocGenerator1Private *d;

    friend class TstDocGenerator1Private;
};

#endif // TSTDOCGENERATOR1_H