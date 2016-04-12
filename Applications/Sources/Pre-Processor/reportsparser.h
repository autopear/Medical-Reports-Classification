#ifndef REPORTSPARSER_H
#define REPORTSPARSER_H

#include <QThread>

class ReportsParser : public QThread
{
    Q_OBJECT
public:
    explicit ReportsParser(const QString &dir,
                           const QStringList &ignores,
                           const QStringList &suffixes,
                           QObject *parent = 0);
    ~ReportsParser();

signals:
    void reportsListed(const QStringList &reports);
    void reportStemmed(const QString &report, const QStringList &stems);

protected:
    void run();

private:
    static void findAllReports(const QString &root, const QString &dir, QStringList *reports);
    QString m_dir;
    QStringList m_ignores;
    QStringList m_suffixes;
};

#endif // REPORTSPARSER_H
