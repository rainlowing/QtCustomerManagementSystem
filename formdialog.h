#ifndef FORMDIALOG_H
#define FORMDIALOG_H

#include <QDialog>
#include <QList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>

#include "namespace.h"

class FormDialog : public QDialog {
public:
    FormDialog(const QList<FieldConfig> &fields, QWidget *parent = nullptr);

private:
    void buildForm();
    QWidget *createInputWidget(const FieldConfig &field);

    QList<FieldConfig>                          m_fields;
};

#endif // FORMDIALOG_H
