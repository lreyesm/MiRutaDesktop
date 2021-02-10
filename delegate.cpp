#include "delegate.h"
#include <QComboBox>
#include "qpersonalizebutton.h"

//! [0]
ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}
//! [0]

void ComboBoxDelegate::fillEditors(ComboBox *editor){
    lista_editores.append(editor);
}
void ComboBoxDelegate::emitDoubleClicked(QModelIndex index){
    emit doubleClickedComboBox(index);
}
//! [1]
QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(option);
    if(index.column() == 1){
        ComboBox *editor = new ComboBox(parent);
        editor->setFrame(false);
        editor->setIndex(index);
        connect(editor, SIGNAL(doubleClickedComboBox(QModelIndex)), this, SLOT(emitDoubleClicked(QModelIndex)));
        connect(this, SIGNAL(fillList(QAbstractItemModel*)), editor, SLOT(setList(QAbstractItemModel*)));
        return editor;
    }
    return nullptr;
}
//! [1]
//! [2]
void ComboBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();

    if(index.column() == 1){
        ComboBox *comboBox = static_cast<ComboBox*>(editor);
        if(comboBox->count() == 0){
            QStringList info_list_local = value.split("\n");
            comboBox->addItems(info_list_local);
            comboBox->showPopup();
        }else{
            comboBox->showPopup();
        }
    }

}
//! [2]
//! [3]
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    if(index.column() == 1){
        ComboBox *comboBox = static_cast<ComboBox*>(editor);
        if(comboBox->count() == 0){
            QString value = comboBox->currentText();
            model->setData(index, value, Qt::EditRole);
        }
    }
}
//! [3]
//! [4]
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if(index.column() == 1){
        editor->setGeometry(option.rect);
    }
}
//! [4]
