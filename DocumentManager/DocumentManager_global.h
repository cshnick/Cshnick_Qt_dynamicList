#ifndef DOCUMENTMANAGER_GLOBAL_H
#define DOCUMENTMANAGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DOCUMENTMANAGER_LIBRARY)
#  define DOCUMENTMANAGERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DOCUMENTMANAGERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DOCUMENTMANAGER_GLOBAL_H
