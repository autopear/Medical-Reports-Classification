#ifndef NEWVARIANTDIALOG_H
#define NEWVARIANTDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class NewVariantDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewVariantDialog(QWidget *parent = 0);
    ~NewVariantDialog();

    QString variant() const;

private slots:
    void onOk();

private:
    QLabel *m_label;
    QLineEdit *m_edit;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;
};

#endif // NEWVARIANTDIALOG_H
