#include "formdialog.h"

FormDialog::FormDialog(const QList<FieldConfig> &fields, QWidget *parent)
    : QDialog(parent), m_fields(fields) {

    buildForm();
}


// 构建表单结构
void FormDialog::buildForm() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout;

    foreach (const FieldConfig &field, m_fields) {
        QWidget *inputWidget = createInputWidget(field);
        formLayout->addRow(field.display, inputWidget);
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnConfirm = new QPushButton("确认");
    QPushButton *btnCancel = new QPushButton("取消");
    btnConfirm->setFixedWidth(80);
    btnCancel->setFixedWidth(80);
    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);
    connect(btnConfirm, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
}


// 根据 field 中保存的枚举值创建表单控件
QWidget *FormDialog::createInputWidget(const FieldConfig &field) {
    QWidget *widget = nullptr;
    switch (field.type) {
    case WidgetType::LineEDit : {
        auto line = new QLineEdit();
        line->setReadOnly(field.readonly);
        widget = line;
        break;
    }
    case WidgetType::ComboBox : {
        auto combo = new QComboBox();
        combo->setEditable(!field.readonly);
        widget = combo;
        break;
    }
    case WidgetType::PlainTextEdit : {
        auto plain = new QPlainTextEdit();
        plain->setReadOnly(field.readonly);
        widget = plain;
        break;
    }
    default:
        widget = new QLineEdit;
    }

    widget->setObjectName(field.name);
    return widget;
}
