#ifndef TSTGENERATOR_H
#define TSTGENERATOR_H

#include "TstGenerator_global.h"
#include "dynpicturesmodel.h"

class TSTGENERATORSHARED_EXPORT TstGenerator : public DPImageServicer
{
    Q_OBJECT

public:
    TstGenerator(QObject *parent = 0);

    qint64 imageCount() const;
    QImage imageForindex(int index) const;

};

#endif // TSTGENERATOR_H
