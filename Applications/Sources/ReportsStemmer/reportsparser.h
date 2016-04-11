#ifndef REPORTSPARSER_H
#define REPORTSPARSER_H

#include <QThread>

class ReportsParser : public QThread
{
    Q_OBJECT
public:
    explicit ReportsParser(const QString &dir, QObject *parent = 0);
    ~ReportsParser();

signals:
    void reportsListed(const QStringList &reports);
    void reportStemmed(const QString &report, const QStringList &stems);

protected:
    void run();

private:
    static void findAllReports(const QString &root, const QString &dir, QStringList *reports);
    QString m_dir;
};

#endif // REPORTSPARSER_H
