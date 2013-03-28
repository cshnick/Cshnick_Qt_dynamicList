#include "IDocumentGenerator.h"

namespace Docs {
IDocumentGenerator::IDocumentGenerator(QObject *parent)
    : QObject(parent)
{
}
IDocumentGenerator::~IDocumentGenerator()
{
}

} //namespace Docs
