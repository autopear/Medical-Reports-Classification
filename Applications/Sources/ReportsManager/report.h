#ifndef REPORT_H
#define REPORT_H

#include <QDomElement>
#include <QMap>
#include <QString>

class Report : public QMap<QString, QString>
{
public:
    enum State
    {
        None     = -1,
        Unknown  =  0,
        Seizures =  1
    };

    explicit Report();
    explicit Report(const QString &report, State state = Unknown);
    ~Report();

    QString report() const;
    void setReport(const QString &report);

    State patientState() const;
    void setPatientState(State state);

    QDomElement toXml(QDomDocument doc);

private:
    QString m_report;
    State m_state;
};


#endif // REPORT_H
