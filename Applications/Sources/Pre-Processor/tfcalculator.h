#ifndef TFCALCULATOR_H
#define TFCALCULATOR_H

#include <QThread>

class ExportSVMDialog;

class TFCalculator : public QThread
{
    Q_OBJECT
public:
    explicit TFCalculator(ExportSVMDialog *dialog, QObject *parent = 0);
    ~TFCalculator();

protected:
    void run();

private:
    ExportSVMDialog *m_dialog;
};

#endif // TFCALCULATOR_H
