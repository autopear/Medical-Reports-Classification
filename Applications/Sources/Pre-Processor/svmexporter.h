#ifndef SVMEXPORTER_H
#define SVMEXPORTER_H

#include <QThread>

class ExportSVMDialog;

class SVMExporter : public QThread
{
    Q_OBJECT
public:
    explicit SVMExporter(ExportSVMDialog *dialog,
                         const QString &file,
                         bool isRank,
                         QObject *parent = 0);
    ~SVMExporter();

signals:
    void progressUpdated();

protected:
    void run();

private:
    ExportSVMDialog *m_dialog;
    QString m_file;
    bool m_rank;
};

#endif // SVMEXPORTER_H
