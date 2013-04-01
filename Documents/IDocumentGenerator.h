#ifndef IDOCUMENTGENERATOR_H
#define IDOCUMENTGENERATOR_H

#include "DocumentManager_global.h"
#include <QObject>
#include <QIcon>
#include "ThumbnailManager.h"

#include "Node.h"

namespace Docs {

class DOCUMENTSSHARED_EXPORT IDocumentGenerator : public QObject
{
    Q_OBJECT

public:
    IDocumentGenerator(QObject *parent = 0);
    virtual ~IDocumentGenerator();
    virtual QString displayText() const = 0;
    virtual QIcon icon() const = 0;

    virtual void createNodeTree() = 0;
    virtual GeneratorNode *rootNode() const = 0;
    virtual DPImageServicer *thumbServicer() = 0;

};

} //namespace Documents

Q_DECLARE_INTERFACE(Docs::IDocumentGenerator,
                    "org.openSankore.IDocumentGenerator")


#endif // IDOCUMENTGENERATOR_H
