#ifndef VARIANTLISTWIDGET_H
#define VARIANTLISTWIDGET_H

#include <QListWidget>

class QAction;

class VariantListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit VariantListWidget(QWidget *parent = 0);
    ~VariantListWidget();

    QStringList variants() const;

signals:
    void variantsChanged(const QStringList &vars);
    void requestClear();

public slots:
    void setVariants(const QString &tag, const QStringList &vars);
    void clearAllVariants();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

protected Q_SLOTS:
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

private slots:
    void deleteVariant();
    void renameVariant();
    void newVariant();

private:
    QAction *m_actionRename;
    QAction *m_actionDelete;
    QAction *m_actionNew;
    QAction *m_actionClear;

    QStringList m_vars;
    QString m_tag;

    QListWidgetItem *m_editingItem;
    QString m_editingVar;
};

#endif // VARIANTLISTWIDGET_H
