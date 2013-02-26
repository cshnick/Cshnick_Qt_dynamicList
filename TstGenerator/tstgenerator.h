#ifndef TSTGENERATOR_H
#define TSTGENERATOR_H

#include "TstGenerator_global.h"
#include "dynpicturesmodel.h"

class TSTGENERATORSHARED_EXPORT TstGenerator : public DPImageGenerator
{
public:
    TstGenerator();

    qint64 imageCount();
    QImage *imageForindex(int index);

};

#endif // TSTGENERATOR_H
