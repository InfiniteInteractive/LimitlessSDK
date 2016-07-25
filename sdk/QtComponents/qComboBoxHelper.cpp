#include "qComboBoxHelper.h"
#include <algorithm>

namespace Limitless
{

void updateComboBox(QComboBox *comboBox, const std::vector<std::string> &items)
{
    std::vector<std::string> localItems=items;

    for(size_t i=0; i<comboBox->count();)
    {
        std::string comboBoxItem=comboBox->itemText((int)i).toStdString();
        std::vector<std::string>::iterator iter=std::find(localItems.begin(), localItems.end(), comboBoxItem);

        if(iter==localItems.end())
        {
            comboBox->removeItem((int)i);
        }
        else
        {
            localItems.erase(iter);
            i++;
        }
    }

    for(auto &item:localItems)
    {
        comboBox->addItem(QString::fromStdString(item));
    }
}

}

