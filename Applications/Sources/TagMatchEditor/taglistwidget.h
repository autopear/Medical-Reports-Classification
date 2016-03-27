#ifndef TAGLISTWIDGET_H
#define TAGLISTWIDGET_H

#include <QListWidget>

class QAction;

class TagListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit TagListWidget(QWidget *parent = 0);
    ~TagListWidget();

    QString currentTag() const;

    QStringList tags() const;

signals:
    void tagChanged(const QString &oldTag, const QString &newTag);
    void tagDeleted(const QString &tag);
    void tagAdded(const QString &tag);
    void requestClear();

public slots:
    void clearAllTags();
    void addNewTag(const QString &tag);
    void addNewTags(const QStringList &tags);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

protected Q_SLOTS:
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

private slots:
    void deleteTag();
    void renameTag();
    void newTag();

private:
    QAction *m_actionRename;
    QAction *m_actionDelete;
    QAction *m_actionNew;
    QAction *m_actionClear;

    QStringList m_tags;

    QListWidgetItem *m_editingItem;
    QString m_editingTag;
};

#endif // TAGLISTWIDGET_H
