#ifndef FILEPICKER_H
#define FILEPICKER_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

class FilePicker : public QWidget
{
    Q_OBJECT
public:
    explicit FilePicker(const QString &text, QWidget *parent = 0);
    ~FilePicker();

    QString file() const;

signals:
    void fileChanged(const QString &file);

public slots:
    void setFile(const QString &file);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private slots:
    void selectFile();
    void onEnterPressed();

private:
    QLabel *m_label;
    QLineEdit *m_edit;
    QPushButton *m_btn;
};

#endif // FILEPICKER_H
