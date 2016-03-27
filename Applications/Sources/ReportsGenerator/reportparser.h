#ifndef REPORTPARSER_H
#define REPORTPARSER_H

#include <QMap>
#include <QThread>

class ReportParser : public QThread
{
    Q_OBJECT
public:
    explicit ReportParser(const QStringList &reports,
                          const QMap<QString, QString> &tags,
                          const QString &dir,
                          const QString &xml,
                          QObject *parent = 0);
    ~ReportParser();

signals:
    void reportParsed(int index);
    void errorOccurred(const QString &error);

protected:
    void run();

private:
    QStringList m_reports;
    QString m_dir;
    QString m_xml;
    QMap<QString, QString> m_tags;
};

#endif // REPORTPARSER_H
