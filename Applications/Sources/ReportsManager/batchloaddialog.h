#ifndef BATCHLOADDIALOG_H
#define BATCHLOADDIALOG_H

#include <QDialog>
#include "report.h"

class ElidibleLabel;
class QLabel;
class QListWidget;
class QPushButton;
class QRadioButton;

class BatchLoadDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BatchLoadDialog(const QStringList &files, QWidget *parent = 0);
    ~BatchLoadDialog();

    Report::State state() const;

    QStringList reports() const;

private:
    QLabel *m_labelFile;
    ElidibleLabel *m_labelSingle;
    QListWidget *m_listMulti;
    QLabel *m_labelReports;
    QListWidget *m_listReports;
    QLabel *m_labelState;
    QRadioButton *m_radioNone;
    QRadioButton *m_radioUnknown;
    QRadioButton *m_radioSeizures;
    QPushButton *m_btnImport;
    QPushButton *m_btnCancel;

    QStringList m_reports;
};

#endif // BATCHLOADDIALOG_H
