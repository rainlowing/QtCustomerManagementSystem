#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <QMetaEnum>
#include <QVariant>
#include <QString>

namespace Log {
    Q_NAMESPACE

    enum Category {
        APPLICATION = 0,
        DATABASE,
        NETWORK,
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

    enum class WidgetType {
        LineEDit,
        ComboBox,
        PlainTextEdit
    };

    struct FieldConfig {
        QString         display;            // 表单项
        QString         name;               // 控件名
        WidgetType      type;               // 控件类型
        bool            readonly = true;    // 只读
    };

#endif // NAMESPACE_H
