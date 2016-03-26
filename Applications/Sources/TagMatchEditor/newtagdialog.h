#ifndef NEWTAGDIALOG_H
#define NEWTAGDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class NewTagDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewTagDialog(QWidget *parent = 0);
    ~NewTagDialog();

    QString tag() const;

private slots:
    void onOk();

private:
    QLabel *m_label;
    QLineEdit *m_edit;
    QPushButton *m_btnOk;
    QPushButton *m_btnCancel;
};

#endif // NEWTAGDIALOG_H
