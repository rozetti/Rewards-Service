#ifndef REWARDSSERVICE_GLOBAL_H
#define REWARDSSERVICE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(REWARDSSERVICE_LIBRARY)
#  define REWARDSSERVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define REWARDSSERVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // REWARDSSERVICE_GLOBAL_H
