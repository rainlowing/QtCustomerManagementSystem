#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <QMetaEnum>

namespace Log {
    Q_NAMESPACE

    enum Category {
        LOG_APPLICATION = 0,
        LOG_DATABASE,
        LOG_NETWORK,
    };
    Q_ENUM_NS(Category)

    enum Level {
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
    };
    Q_ENUM_NS(Level)
}


#endif // NAMESPACE_H
