#include <QHeaderView>
#include <QToolButton>
#include <QColorDialog>

#include "tablemanager.h"
#include "resourcemanager.h"
#include "utils.h"

CTableManager::CTableManager(QTableWidget* pTable):
    m_pTable(pTable)
{
    initRowName();
    reset();
    //QObject::connect(m_pTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onCellEdit(QTableWidgetItem*)));
}

void CTableManager::reset()
{
    m_pTable->clear();
    m_pTable->setColumnCount(2);
    m_pTable->setRowCount(0);
    QStringList header;
    header.append("Parameter");
    header.append("Value");
    m_pTable->setHorizontalHeaderLabels(header);
    m_pTable->horizontalHeader()->setVisible(true);
    m_pTable->verticalHeader()->hide();
    //m_pTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents); //adjust whole table. now both columns fill qtablewidget but it occurs blink and offset for main window
}


void CTableManager::initRowName()
{
    m_aRowName[eObjParam_BODYPARTS] = "Model parts";
    m_aRowName[eObjParam_PLAYER] = "Player (group)";
    m_aRowName[eObjParam_NID] = "Map ID";
    m_aRowName[eObjParam_TYPE] = "Unit subtype";
    m_aRowName[eObjParam_NAME] = "Inner(map) name";
    m_aRowName[eObjParam_TEMPLATE] = "Model name";
    m_aRowName[eObjParam_PARENT_TEMPLATE] = "Template";
    m_aRowName[eObjParam_PRIM_TXTR] = "Texture";
    m_aRowName[eObjParam_SEC_TXTR] = "Secondary texture";
    m_aRowName[eObjParam_COMMENTS] = "Comments";
    m_aRowName[eObjParam_POSITION] = "Position";
    m_aRowName[eObjParam_ROTATION] = "Rotation";
    m_aRowName[eObjParam_USE_IN_SCRIPT] = "Is use in script?";
    m_aRowName[eObjParam_IS_SHADOW] = "Is shadow?";
    m_aRowName[eObjParam_PARENT_ID] = "Parent ID";
    m_aRowName[eObjParam_QUEST_INFO] = "Quest marker";
    m_aRowName[eObjParam_COMPLECTION] = "Complection";
    m_aRowName[eObjParam_TORCH_PTLINK] = "Torch point";
    m_aRowName[eObjParam_TORCH_STRENGHT] = "Torch power";
    m_aRowName[eObjParam_TORCH_SOUND] = "Torch sound";
    m_aRowName[eObjParam_RANGE] = "Range";
    m_aRowName[eObjParam_SOUND_RANGE] = "Sound distance";
    m_aRowName[eObjParam_SOUND_MIN] = "Sound min";
    m_aRowName[eObjParam_SOUND_MAX] = "Sound max";
    m_aRowName[eObjParam_SOUND_RESNAME] = "Sound filepath";
    m_aRowName[eObjParam_SOUND_AMBIENT] = "Is ambient?";
    m_aRowName[eObjParam_SOUND_IS_MUSIC] = "Is music?";
    m_aRowName[eObjParam_LIGHT_SHADOW] = "Is shadow?";
    m_aRowName[eObjParam_LIGHT_COLOR] = "Color";
    m_aRowName[eObjParam_PARTICL_TYPE] = "Particle type";
    m_aRowName[eObjParam_PARTICL_SCALE] = "Particle scale";
    //m_aRowName[eObjParam_LEVER_SCIENCE_STATS_NEW] = "Stats new (wtf?)";
    m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Type_Open] = "Type open";
    m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Key_ID] = "> Key ID";
    m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight] = "> Hands skill";
    //sciene stats new
    m_aRowName[eObjParam_LEVER_CUR_STATE] = "Current state";
    m_aRowName[eObjParam_LEVER_TOTAL_STATE] = "Total state";
    m_aRowName[eObjParam_LEVER_IS_CYCLED] = "Is cycled?";
    m_aRowName[eObjParam_LEVER_IS_DOOR] = "Is door?";
    m_aRowName[eObjParam_LEVER_RECALC_GRAPH] = "Is recalc graph?";
    m_aRowName[eObjParam_TRAP_DIPLOMACY] = "Trap group";
    m_aRowName[eObjParam_TRAP_SPELL] = "Trap spell";
    //m_aRowName[eObjParam_TRAP_AREAS] = "Trap Areas";
    //m_aRowName[eObjParam_TRAP_TARGETS] = "Trap targets";
    m_aRowName[eObjParam_TRAP_AREA_RADIUS] = "Zone radius";
    m_aRowName[eObjParam_TRAP_CAST_INTERVAL] = "Cast interval";
    m_aRowName[eObjParam_TRAP_CAST_ONCE] = "Is cast once?";
    m_aRowName[eObjParam_UNIT_NEED_IMPORT] = "Is need import?";
    m_aRowName[eObjParam_UNIT_PROTOTYPE] = "Database name";
    m_aRowName[eObjParam_UNIT_ARMORS] = "Armors";
    m_aRowName[eObjParam_UNIT_WEAPONS] = "Weapons";
    m_aRowName[eObjParam_UNIT_SPELLS] = "Spells";
    m_aRowName[eObjParam_UNIT_QUICK_ITEMS] = "Quick items";
    m_aRowName[eObjParam_UNIT_QUEST_ITEMS] = "Quest items";
    m_aRowName[eObjParam_UNIT_STATS] = "Unit parameters";

    m_aRowName[eObjParam_GUARD_PLACE] = "Guard place";
    m_aRowName[eObjParam_GUARD_RADIUS] = "Guard radius";
    m_aRowName[eObjParam_GUARD_ALARM] = "Guard help";
    m_aRowName[eObjParam_LOGIC_BEHAVIOUR] = "Behaviour";
    m_aRowName[eObjParam_AGRESSION_MODE] = "Agression mode";
    m_aRowName[eObjParam_VIEW_WAIT] = "Watch time";
    m_aRowName[eObjParam_VIEW_TURN_SPEED] = "Turn speed";
}

bool CTableManager::isValidValue(const EObjParam param, const QString &value)
{
    bool bRes = false;
    switch (param) {
    case eObjParam_NID:
    case eObjParam_RANGE:
    case eObjParam_SOUND_RANGE:
    case eObjParam_SOUND_MIN:
    case eObjParam_SOUND_MAX:
    case eObjParam_LEVER_SCIENCE_STATS_Key_ID:
    case eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight:
    case eObjParam_LEVER_TOTAL_STATE:
    case eObjParam_LEVER_CUR_STATE:
    case eObjParam_VIEW_WAIT:
    case eObjParam_VIEW_TURN_SPEED:
    case eObjParam_TRAP_CAST_INTERVAL:
    { // uint
        QRegExp rx("[0-9]+");
        bRes = rx.exactMatch(value);
        break;
    }
    case eObjParam_PARENT_ID:
    case eObjParam_TRAP_DIPLOMACY:
    { // int
        QRegExp rx("-?[0-9]+");
        bRes = rx.exactMatch(value);
        break;
    }
    case eObjParam_GUARD_RADIUS:
    case eObjParam_GUARD_ALARM:
    case eObjParam_TORCH_STRENGHT:
    case eObjParam_PARTICL_SCALE:
    case eObjParam_TRAP_AREA_RADIUS:
    { // float
        QRegExp rx("^\\s*-?\\d+(\\.\\d+)?");
        bRes = rx.exactMatch(value);
        break;
    }
    case eObjParam_GUARD_PLACE:
    case eObjParam_TORCH_PTLINK:
    case eObjParam_LIGHT_COLOR: //todo: use custom palette choose
    case eObjParam_COMPLECTION:
    case eObjParam_POSITION:
    case eObjParam_ROTATION:
    {// vector3d. checked value: (111.66,94.46,-0.25) ( 0.21, 0.13, 1.20)
        QRegExp rx("\\((\\s*-?\\d+(\\.\\d+)?\\,){2}(\\s*-?\\d+(\\.\\d+)?)\\)");
        bRes = rx.exactMatch(value);
        break;
    }
    case eObjParam_UNIT_PROTOTYPE:
    { // allow any not-empty string
        bRes = !value.isEmpty();
        break;
    }
    case eObjParam_BODYPARTS: //todo: use custom popup instead string value
    case eObjParam_SEC_TXTR:
    case eObjParam_NAME:
    case eObjParam_QUEST_INFO:
    case eObjParam_TORCH_SOUND:
    case eObjParam_PARENT_TEMPLATE:
    case eObjParam_SOUND_RESNAME:
    case eObjParam_COMMENTS:
    { // allow any string
        bRes = true;
        break;
    }
    default:
    {
        bRes = true;
        break;
    }

    //case eObjParam_TRAP_SPELL
//    case eObjParam_UNIT_STATS
//    case eObjParam_UNIT_WEAPONS
//    case eObjParam_UNIT_ARMORS
//    case eObjParam_UNIT_SPELLS
//    case eObjParam_UNIT_QUICK_ITEMS
//    case eObjParam_UNIT_QUEST_ITEMS

    }

    return bRes;
}

void CTableManager::onParamChange(const QSharedPointer<IPropertyBase>& pProp)
{
    emit onUpdateProperty(pProp);
}

///
/// \brief CTableManager::onCellEdit
/// Manually connect to cell widget and get new value if possible. QTableWidget cant use signal-slot system
/// \param pItem
///
void CTableManager::onCellEdit(QTableWidgetItem* pItem)
{
    CValueItem* pValue = dynamic_cast<CValueItem*>(pItem);
    if(nullptr == pValue)
        return;
}

void blockEditWidget(QTableWidgetItem* pItem)
{
    pItem->setFlags(pItem->flags() & ~Qt::ItemIsEditable);
}

// function to sort prop in table
bool variantLessThan(const std::pair<QSharedPointer<IPropertyBase>, bool> &v1, const std::pair<QSharedPointer<IPropertyBase>, bool> &v2)
{
    return v1.first->type() < v2.first->type();
}

void CTableManager::setNewData(const QMap<QSharedPointer<IPropertyBase>, bool>& aProp)
{
    reset();
    int i(0); // row number
    EObjParam type;
    m_pTable->blockSignals(true); //block sending signals

    for(int iParam(0); iParam < EObjParam::eObjParamCount; ++iParam)

    for (const auto& item : aProp.toStdMap())
    {
        type = item.first->type();
        if(type != iParam)
            continue; // fcking sort of properties

        switch (type)
        {
        //dropdown list next
        case eObjParam_PLAYER:
        case eObjParam_IS_SHADOW:
        case eObjParam_LEVER_IS_CYCLED:
        case eObjParam_LEVER_IS_DOOR:
        case eObjParam_USE_IN_SCRIPT:
        case eObjParam_LEVER_RECALC_GRAPH:
        case eObjParam_SOUND_AMBIENT:
        case eObjParam_SOUND_IS_MUSIC:
        case eObjParam_PRIM_TXTR:
        case eObjParam_TEMPLATE:
        case eObjParam_LIGHT_SHADOW:
        case eObjParam_TYPE:
        case eObjParam_PARTICL_TYPE:
        case eObjParam_AGRESSION_MODE:
        case eObjParam_TRAP_CAST_ONCE:
        {
            m_pTable->insertRow(i);
            //https://doc.qt.io/archives/qt-4.8/qtablewidget.html#setItem
            //The table takes ownership of the item.
            // => table widget take care about memory
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            //todo: collect all created items and delete them before create new (or update). reduce memory leaks
            CComboItem* pCombo = new CComboItem(item.first);
            QObject::connect(pCombo, SIGNAL(onValueChange(const QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(const QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            break;
        }
//        case eObjParam_TRAP_CAST_INTERVAL:
//        {// will be processed below
//            break;
//        }
//        case eObjParam_TRAP_CAST_ONCE:
//        {
//            m_pTable->insertRow(i);
//            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
//            m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
//            CComboBoxItem* pCombo = new CComboBoxItem(item.second, item.first);
//            QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
//            m_pTable->setCellWidget(i, 1, pCombo);
//            ++i;
//            if(item.second == "0") // cast once == false
//            {
//                m_pTable->insertRow(i);
//                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_TRAP_CAST_INTERVAL]));
//                m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
//                m_pTable->setItem(i, 1, new CStringItem(aParam[eObjParam_TRAP_CAST_INTERVAL], eObjParam_TRAP_CAST_INTERVAL));
//                m_pTable->resizeColumnToContents(0);
//                ++i;
//            }
//            break;
//        }
        case eObjParam_GUARD_RADIUS:
        { //this case process below
            break;
        }
        case eObjParam_LOGIC_BEHAVIOUR:
        {
            //insert combobox
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboItem* pCombo = new CComboItem(item.first);
            //QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;

            //EBehaviourType type = dynamic_cast<propBehaviour*>(item.first)->value();
            EBehaviourType type = EBehaviourType(dynamic_cast<propUint*>(item.first.get())->value());
            if(type == EBehaviourType::eRadius)
            {
                m_pTable->insertRow(i);
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_GUARD_RADIUS]));
                blockEditWidget(m_pTable->item(i, 0));
                //m_pTable->setItem(i, 1, new CValueItem(aProp.key(eObjParam_GUARD_RADIUS))); // get radius value
                auto* pCellValue = new CValueItem(aProp.key(eObjParam_GUARD_RADIUS));
                m_pTable->setCellWidget(i, 1, pCellValue);
                m_pTable->resizeColumnToContents(0);
                ++i;
            }
            break;
        }
        case eObjParam_LEVER_SCIENCE_STATS_Key_ID:
        case eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight:
        {// this cases processed below
            break;
        }
        case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
        {
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboItem* pCombo = new CComboItem(item.first);
            //QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            if (item.second == true)
            {
                //todo: show lever parameters
            }
            break; //eObjParam_LEVER_SCIENCE_STATS_Type_Open
        }
        case eObjParam_UNIT_STATS:
        case eObjParam_UNIT_WEAPONS:
        case eObjParam_UNIT_ARMORS:
        case eObjParam_UNIT_SPELLS:
        case eObjParam_UNIT_QUICK_ITEMS:
        case eObjParam_UNIT_QUEST_ITEMS:
        {//will be processed below
            break;
        }
        case eObjParam_UNIT_NEED_IMPORT:
        {
            //insert combobox
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboItem* pCombo = new CComboItem(item.first);
            //QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            if(true) // is need import unit stats == true
            {
                //todo: collect unit stats data
//                QVector<EObjParam> importStats;
//                importStats.resize(6);
//                importStats[0] = eObjParam_UNIT_STATS;
//                importStats[1] = eObjParam_UNIT_WEAPONS;
//                importStats[2] = eObjParam_UNIT_ARMORS;
//                importStats[3] = eObjParam_UNIT_SPELLS;
//                importStats[4] = eObjParam_UNIT_QUICK_ITEMS;
//                importStats[5] = eObjParam_UNIT_QUEST_ITEMS;
//                for(int j(0); j<importStats.size();++j)
//                {
//                    m_pTable->insertRow(i);
//                    m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[importStats[j]]));
//                    m_pTable->setItem(i, 1, new CStringItem(aParam[importStats[j]], importStats[j]));
//                    m_pTable->resizeColumnToContents(0);
//                    ++i;
//                }
            }
            break;
        }
        case eObjParam_LIGHT_COLOR:
        {// for color create tool button with color
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CColorButtonItem* pColorButton = nullptr;
            if(item.second)
                pColorButton = new CColorButtonItem(item.first);
            else
                pColorButton = new CColorButtonItem(type);
            QObject::connect(pColorButton, SIGNAL(onColorChange(const QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(const QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pColorButton);
            ++i;
            break;
        }
        default:
        {
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CValueItem* pCellValue = nullptr;
            if(item.second == true) // the same value
                pCellValue = new CValueItem(item.first);
            else
                // todo: create value with <different>
                pCellValue = new CValueItem(item.first);
            QObject::connect(pCellValue, SIGNAL(onParamChange(const QSharedPointer<IPropertyBase>&)), this, SLOT(onParamChange(const QSharedPointer<IPropertyBase>&)));
            m_pTable->setCellWidget(i, 1, pCellValue);
            m_pTable->resizeColumnToContents(0);
            ++i;
            break;
        }
        } //switch
    }
    m_pTable->blockSignals(false);
}

void CColorButtonItem::applyColor()
{
    QColor color = QColorDialog::getColor(Qt::black);
    if(color.isValid())
    {
        updateColor(color);
        dynamic_cast<prop3D*>(m_pValue.get())->setValue(color.red(), color.green(), color.blue());
        emit onColorChange(m_pValue);
    }
}

CColorButtonItem::CColorButtonItem(const EObjParam param):
    m_pValue(new prop3D(param, 0.0f, 0.0f, 0.0f))
{
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(applyColor()));
}

CColorButtonItem::CColorButtonItem(const QSharedPointer<IPropertyBase>& prop)
{
    m_pValue.reset(prop->clone());
    const auto& clr = dynamic_cast<prop3D*>(m_pValue.get())->value();
    QColor color(clr.x(), clr.y(), clr.z());
    updateColor(color);
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(applyColor()));
}

void CColorButtonItem::updateColor(const QColor &color)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Button, color);
    setAutoFillBackground(true);
    setPalette(pal);
    update();
}

bool CValueItem::applyChanges(const QString& text)
{
    if(m_pValue->toString() != text)
    {
        m_pValue->resetFromString(text);
        qDebug() << m_pValue->toString();
        return true;
    }
    //todo: validate data. return value if checks failed
    return false;
}

void CValueItem::onTextChangeEnd()
{
    const QString& val = text();
    if(m_pValue->toString() == val)
        return;

    qDebug() << val;
    m_pValue->resetFromString(val);
    m_filter->updateValue(val);
    emit onParamChange(m_pValue);
}

//CDataItem::CDataItem(IPropertyBase *pProp)
//{
//    QObject::connect(this, SIGNAL(clicked()), this, SLOT(hello()));
//}

//void CDataItem::hello()
//{
//    qDebug() << m_pValue->toString();
//}

CComboItem::CComboItem(const QSharedPointer<IPropertyBase>& prop)
{
    m_pValue.reset(prop->clone());
    if(!CResourceStringList::getInstance()->getPropList(m_valueList, prop->type()))
    {// case for property that has self-named value (without int-string convertion
        return;
    }

    addItems(m_valueList.values());
    setCurrentText(m_valueList[m_pValue->toString().toUInt()]);
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(_onChange(QString))); //reconnect default currentIndexChanged to override
}

void CComboItem::_onChange(QString str)
{
    QSharedPointer<IPropertyBase> valueNew(m_pValue->clone());
    valueNew->resetFromString(str);
    if(valueNew->isEqual(m_pValue.get()))
        return;

    m_pValue->resetFromString(str);
    emit onValueChange(m_pValue);
}

CValueItem::CValueItem(const QSharedPointer<IPropertyBase>& prop):
    QLineEdit(prop->toString())
{
    m_pValue.reset(prop->clone());
    if(false)
    {
        // todo: add validator for each text type (int, uint, float, string)
        QRegExp re("\\d+");
        QRegExpValidator *validator = new QRegExpValidator(re, this);
        setValidator(validator);
    }
    setFrame(false);
    m_filter.reset(new CLineEditEventFilter(this, m_pValue->toString()));
    this->installEventFilter(m_filter.get());
    QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(onTextChangeEnd()));
}

