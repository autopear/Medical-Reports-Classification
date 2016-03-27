#ifndef NEWPROPERTYDIALOG_H
#define NEWPROPERTYDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class NewPropertyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewPropertyDialog(QWidget *parent = 0);
    ~NewPropertyDialog();

    QString newProperty() const;

private slots:
    void onOk();

private:
    QLabel *m_label;
    QLineEdit *m_edit;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;
};

#endif // NEWPROPERTYDIALOG_H
