#ifndef PROPERTYLISTWIDGET_H
#define PROPERTYLISTWIDGET_H

#include <QListWidget>

class QAction;

class PropertyListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PropertyListWidget(QWidget *parent = 0);
    ~PropertyListWidget();

    QString currentProperty() const;

    QStringList properties() const;

signals:
    void propertyChanged(const QString &oldProp, const QString &newProp);
    void propertyDeleted(const QString &prop);
    void propertyAdded(const QString &prop);

public slots:
    void clearAllproperties();
    void addNewProperty(const QString &prop);
    void addNewProperties(const QStringList &props);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

protected Q_SLOTS:
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

private slots:
    void deleteProperty();
    void renameProperty();
    void newProperty();

private:
    QAction *m_actionRename;
    QAction *m_actionDelete;
    QAction *m_actionNew;

    QStringList m_props;

    QListWidgetItem *m_editingItem;
    QString m_editingProp;
};

#endif // PROPERTYLISTWIDGET_H
