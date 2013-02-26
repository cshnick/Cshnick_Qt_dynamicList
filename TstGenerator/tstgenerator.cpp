#include "tstgenerator.h"
#include <QtGui>

static const QUrl storageUrl = QUrl::fromLocalFile("/home/ilia/.local/share/data/Sankore/document/Sankore Document 2013-02-21 18-58-39.236");


TstGenerator::TstGenerator()
{
}

qint64 TstGenerator::imageCount()
{
    QDir dir(storageUrl.toLocalFile());
    return dir.entryInfoList(QStringList() << "*.thumbnail.jpg", QDir::Files | QDir::NoDotAndDotDot, QDir::Name).count();
}

QImage *TstGenerator::imageForindex(int index)
{
    QString fileName = QString("page%1.thumbnail.jpg").arg(index, 3, 10, QLatin1Char('0'));
    QString filePath = storageUrl.toLocalFile() + "/" + fileName;
    Q_ASSERT(QFileInfo(filePath).exists());

    QImage *img = new QImage(QSize(), QImage::Format_RGB888);
    if (!img->load(filePath)) {
        delete img;
        img = 0;
    }

    return img;
}

