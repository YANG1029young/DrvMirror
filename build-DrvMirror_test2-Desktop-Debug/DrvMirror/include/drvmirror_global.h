#ifndef DRVMIRROR_GLOBAL_H
#define DRVMIRROR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DRVMIRROR_LIBRARY)
#  define DRVMIRRORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DRVMIRRORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DRVMIRROR_GLOBAL_H
