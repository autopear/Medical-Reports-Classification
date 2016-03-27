#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStringList>
#include "newtagdialog.h"
#include "taglistwidget.h"

TagListWidget::TagListWidget(QWidget *parent) :
    QListWidget(parent),
    m_editingItem(0),
    m_editingTag(QString())
{
    m_actionDelete = new QAction(tr("&Delete"), this);
    m_actionDelete->setShortcut(QKeySequence::Delete);
    connect(m_actionDelete, SIGNAL(triggered(bool)),
            this, SLOT(deleteTag()));

    m_actionNew = new QAction(tr("&New"), this);
    m_actionNew->setShortcut(QKeySequence::New);
    connect(m_actionNew, SIGNAL(triggered(bool)),
            this, SLOT(newTag()));

    m_actionRename = new QAction(tr("&Rename"), this);
    m_actionRename->setShortcut(QKeySequence::Replace);
    connect(m_actionRename, SIGNAL(triggered(bool)),
            this, SLOT(renameTag()));

    m_actionClear = new QAction(tr("&Clear"), this);
    connect(m_actionClear, SIGNAL(triggered(bool)),
            this, SIGNAL(requestClear()));

    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setTextElideMode(Qt::ElideRight);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

TagListWidget::~TagListWidget()
{
    delete m_actionDelete;
    delete m_actionNew;
    delete m_actionRename;
    delete m_actionClear;
}

QString TagListWidget::currentTag() const
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
        return items.first()->text();
    else
        return QString();
}

QStringList TagListWidget::tags() const
{
    return m_tags;
}

void TagListWidget::clearAllTags()
{
    clear();
    m_tags.clear();
}

void TagListWidget::addNewTag(const QString &tag)
{
    QString newTag = tag.trimmed().toLower();
    if (newTag.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Empty Tag"),
                             tr("The tag must not be empty."));
        return;
    }
    if (m_tags.contains(newTag))
    {
        QMessageBox::warning(this,
                             tr("Duplicate Tag"),
                             tr("Tag \"%1\" already exists.").arg(newTag));
        return;
    }

    m_tags.append(newTag);
    m_tags.sort();

    QListWidgetItem *newItem = new QListWidgetItem(newTag);
    newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    newItem->setToolTip(newTag);

    insertItem(m_tags.indexOf(newTag), newItem);

    setCurrentItem(newItem, QItemSelectionModel::ClearAndSelect);
}

void TagListWidget::addNewTags(const QStringList &tags)
{
    QStringList toAdd, invalids;
    bool hasEmpty = false;
    foreach (QString tag, tags)
    {
        tag = tag.trimmed().toLower();
        if (tag.isEmpty())
            hasEmpty = true;
        else if (m_tags.contains(tag))
            invalids.append(tag);
        else
            toAdd.append(tag);
    }

    toAdd.removeDuplicates();
    toAdd.sort();

    invalids.removeDuplicates();
    invalids.sort();

    if (invalids.size() == 1)
    {
        QMessageBox::warning(this,
                             tr("Duplicate Tag"),
                             tr("Tag \"%1\" already exists.").arg(invalids.first()));
    }
    else if (invalids.size() > 1)
    {
        QMessageBox::warning(this,
                             tr("Duplicate Tags"),
                             tr("Tags \"%1\" already exist.").arg(invalids.join("\", \"")));
    }
    else if (hasEmpty)
    {
        QMessageBox::warning(this,
                             tr("Empty Tag"),
                             tr("All tags must not be empty."));
    }
    else
    {

    }

    if (toAdd.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Nothing to Add"),
                             tr("No tag can be added."));
        return;
    }

    m_tags.append(toAdd);
    m_tags.sort();

    foreach (QString newTag, toAdd)
    {
        QListWidgetItem *newItem = new QListWidgetItem(newTag);
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        newItem->setToolTip(newTag);

        insertItem(m_tags.indexOf(newTag), newItem);
    }

    setCurrentRow(m_tags.indexOf(toAdd.last()), QItemSelectionModel::ClearAndSelect);
}

void TagListWidget::deleteTag()
{
    QModelIndexList indexes = selectedIndexes();
    if (indexes.size() == 1)
    {
        int row = indexes.first().row();
        QListWidgetItem *item = takeItem(row);

        QString tag = item->text();
        delete item;

        m_tags.removeOne(tag);

        setCurrentRow(qMin(row, count() - 1), QItemSelectionModel::ClearAndSelect);

        emit tagDeleted(tag);

        emit currentTextChanged(tag);
    }
}

void TagListWidget::renameTag()
{
    QList<QListWidgetItem *> items = selectedItems();
    if (items.size() == 1)
    {
        m_editingItem = items.first();
        m_editingTag = m_editingItem->text();
        editItem(m_editingItem);
    }
}

void TagListWidget::newTag()
{
    NewTagDialog *dlg = new NewTagDialog(this);
    if (dlg->exec() == QDialog::Accepted)
    {
        QString tag = dlg->tag();

        if (m_tags.contains(tag))
        {
            QMessageBox::warning(this,
                                 tr("Duplicate Tag"),
                                 tr("Tag \"%1\" already exists.").arg(tag));
            delete dlg;
            return;
        }

        m_tags.append(tag);
        m_tags.sort();

        QListWidgetItem *newItem = new QListWidgetItem(tag);
        newItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        newItem->setToolTip(tag);

        insertItem(m_tags.indexOf(tag), newItem);

        setCurrentItem(newItem, QItemSelectionModel::ClearAndSelect);

        emit tagAdded(tag);

        emit currentTextChanged(tag);
    }
    delete dlg;
}

void TagListWidget::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    QListWidget::closeEditor(editor, hint);
    if (m_editingItem)
    {
        QString newTag = m_editingItem->text().trimmed().toLower();
        if (newTag.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Rename Tag"),
                                 tr("The new tag must not be empty."));
            m_editingItem->setText(m_editingTag);
            m_editingItem->setToolTip(m_editingTag);
        }
        else if (newTag.compare(m_editingTag) == 0)
        {
            //Do nothing
        }
        else if (m_tags.contains(newTag))
        {
            QMessageBox::warning(this,
                                 tr("Rename Tag"),
                                 tr("Tag \"%1\" already exists.").arg(newTag));
            m_editingItem->setText(m_editingTag);
            m_editingItem->setToolTip(m_editingTag);
        }
        else
        {
            m_tags.removeOne(m_editingTag);
            m_tags.append(newTag);
            m_tags.sort();

            for (int i=0; i<count(); i++)
            {
                QListWidgetItem *listItem = item(i);
                QString tag = m_tags.at(i);
                if (listItem->text().compare(tag) != 0)
                    listItem->setText(tag);

                listItem->setToolTip(tag);

                if (tag.compare(newTag) == 0)
                    setCurrentRow(i, QItemSelectionModel::ClearAndSelect);
            }

            emit tagChanged(m_editingTag, newTag);

            emit currentTextChanged(newTag);
        }

        m_editingItem = 0;
        m_editingTag = QString();
    }
}

void TagListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction(m_actionNew);

    if (selectedItems().size() == 1)
    {
        menu->addSeparator();
        menu->addAction(m_actionRename);
        menu->addSeparator();
        menu->addAction(m_actionDelete);
    }

    if (count() > 0)
    {
        menu->addSeparator();
        menu->addAction(m_actionClear);
    }

    menu->exec(event->globalPos());
    event->accept();
}

void TagListWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete) || (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Backspace))
    {
        event->accept();
        m_actionDelete->trigger();
    }
    else if (event->matches(QKeySequence::New))
    {
        event->accept();
        m_actionNew->trigger();
    }
    else if (event->matches(QKeySequence::Replace))
    {
        event->accept();
        m_actionRename->trigger();
    }
    else
        QListWidget::keyPressEvent(event);
}

void TagListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::NoModifier && event->button() == Qt::LeftButton)
    {
        QListWidgetItem *item = itemAt(event->pos());
        if (item)
        {
            m_editingItem = item;
            m_editingTag = m_editingItem->text();
            editItem(m_editingItem);
        }
        else
            m_actionNew->trigger();
    }
    QListWidget::mouseDoubleClickEvent(event);
}
