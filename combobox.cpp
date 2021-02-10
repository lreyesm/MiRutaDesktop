#include "combobox.h"

ComboBox::ComboBox(QWidget *parent):
    QComboBox (parent)
{

}
void ComboBox::setList(QAbstractItemModel* m){
    this->setModel(m);
}
