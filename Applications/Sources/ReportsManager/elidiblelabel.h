#ifndef ELIDIBLELABEL_H
#define ELIDIBLELABEL_H

#include <QLabel>

class ElidibleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ElidibleLabel(QWidget *parent = 0);
    ~ElidibleLabel();

    QString displayText() const;
    QString text() const;

public slots:
    void setText(const QString &text);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void updateDisplayText(int w);
};

#endif // ELIDIBLELABEL_H
