#ifndef IDFCALCULATOR_H
#define IDFCALCULATOR_H

#include <QThread>

class ExportSVMDialog;

class IDFCalculator : public QThread
{
    Q_OBJECT
public:
    explicit IDFCalculator(ExportSVMDialog *dialog, QObject *parent = 0);
    ~IDFCalculator();

protected:
    void run();

private:
    ExportSVMDialog *m_dialog;
};

#endif // IDFCALCULATOR_H
