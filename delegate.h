#ifndef DELEGATE_H
#define DELEGATE_H
#include <QStringList>
#include <QStyledItemDelegate>
#include "combobox.h"
//! [0]
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboBoxDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private slots:
    void emitDoubleClicked(QModelIndex index);

    void fillEditors(ComboBox *editor);
signals:
    void doubleClickedComboBox(QModelIndex);
    void fillList(QAbstractItemModel*)const;
    void fillEditor(ComboBox*)const;

private:
    QList<ComboBox*> lista_editores;
};
//! [0]

#endif
