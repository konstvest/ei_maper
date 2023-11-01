#include <QHeaderView>
#include <QToolButton>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

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
    m_aRowName[eObjParam_QUEST_INFO] = "Quest name marker";
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
    m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Key_ID] = "Key ID";
    m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight] = "Hands skill";
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

    m_aRowName[eObjParam_ALWAYS_ACTIVE] = "Is always active?";
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
    //case eObjParam_POSITION:
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

void CTableManager::setNewData(const QList<QSharedPointer<IPropertyBase>>& aProp)
{
    reset();
    int i(0); // row number
    EObjParam type;
    m_pTable->blockSignals(true); //block sending signals

    for(int iParam(0); iParam < EObjParam::eObjParamCount; ++iParam)

    for (const auto& item : aProp)
    {
        type = item->type();
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
        case eObjParam_LIGHT_SHADOW:
        case eObjParam_TYPE:
        case eObjParam_PARTICL_TYPE:
        case eObjParam_AGRESSION_MODE:
        case eObjParam_TRAP_CAST_ONCE:
        case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
        case eObjParam_ALWAYS_ACTIVE:
        {
            m_pTable->insertRow(i);
            //https://doc.qt.io/archives/qt-4.8/qtablewidget.html#setItem
            //The table takes ownership of the item.
            // => table widget take care about memory
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            //todo: collect all created items and delete them before create new (or update). reduce memory leaks
            CComboStItem* pCombo = new CComboStItem(item);
            QObject::connect(pCombo, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            break;
        }
        case eObjParam_PRIM_TXTR:
        case eObjParam_TEMPLATE:
        {
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboDynItem* pCombo = new CComboDynItem(item);
            QObject::connect(pCombo, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
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
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboStItem* pCombo = new CComboStItem(item);
            QObject::connect(pCombo, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;

            if(item->isInit() && (EBehaviourType(dynamic_cast<propUint*>(item.get())->value()) == EBehaviourType::eRadius))
            {
                m_pTable->insertRow(i);
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_GUARD_RADIUS]));
                blockEditWidget(m_pTable->item(i, 0));
                //m_pTable->setItem(i, 1, new CValueItem(aProp.key(eObjParam_GUARD_RADIUS))); // get radius value
                auto* pCellValue = new CValueItem(util::constProp(aProp, eObjParam_GUARD_RADIUS));
                QObject::connect(pCellValue, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
                m_pTable->setCellWidget(i, 1, pCellValue);
                m_pTable->resizeColumnToContents(0);
                ++i;
            }
            break;
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
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CComboStItem* pCombo = new CComboStItem(item);
            QObject::connect(pCombo, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            if(item->isInit() && dynamic_cast<propBool*>(item.get())->value() == true) // is need import unit stats == true
            {
                qDebug() << "todo: unit map stats";
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
            pColorButton = new CColorButtonItem(item);
            QObject::connect(pColorButton, SIGNAL(onColorChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i, 1, pColorButton);
            ++i;
            break;
        }
        case eObjParam_POSITION_Y:
        case eObjParam_POSITION_Z:
        case eObjParam_POSITION:
        {//will be processed below
            break;
        }
        case eObjParam_POSITION_X:
        {
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_POSITION]));
            blockEditWidget(m_pTable->item(i, 0));
            auto* pItem = new C3DItem(util::constProp(aProp, eObjParam_POSITION_X), util::constProp(aProp, eObjParam_POSITION_Y), util::constProp(aProp, eObjParam_POSITION_Z));
            QObject::connect(pItem, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
            m_pTable->setCellWidget(i,1, pItem);
            m_pTable->setRowHeight(i, pItem->height()*3);
            ++i;
            break;
        }
        case eObjParam_ROTATION:
        {
            break;
        }

        default:
        {
            m_pTable->insertRow(i);
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[type]));
            blockEditWidget(m_pTable->item(i, 0));
            CValueItem* pCellValue = new CValueItem(item);
            QObject::connect(pCellValue, SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));
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

CColorButtonItem::CColorButtonItem(const QSharedPointer<IPropertyBase>& prop)
{
    if(prop->isInit())
        m_pValue.reset(prop->clone());
    else
    {
        m_pValue.reset(new prop3D(prop->type(), 0.0f, 0.0f, 0.0f)); //todo: display value dif
    }
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

CComboStItem::CComboStItem(const QSharedPointer<IPropertyBase>& prop)
{
    m_pListModel.reset(new QStringListModel());
    m_pValue.reset(prop->clone());
    if(!CResourceStringList::getInstance()->getPropList(m_valueList, prop->type()))
    {// case for property that has self-named value (without int-string convertion
        return;
    }

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    //lineEdit()->setPlaceholderText("Select item");
    //addItems(m_valueList.values());
    m_pListModel->setStringList(m_valueList.values());
    setModel(m_pListModel.get());
    if(m_pValue->isInit())
        setCurrentText(m_valueList[m_pValue->toString().toUInt()]);
    else
    {
        insertItem(0, "undefined/different");
        setCurrentIndex(0);
    }
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(_onChange(QString))); //reconnect default currentIndexChanged to override
}

void CComboStItem::_onChange(QString str)
{
    QSharedPointer<IPropertyBase> valueNew(m_pValue->clone());
    QString sourceValue = QString::number(m_valueList.key(str));
    valueNew->resetFromString(sourceValue);
    if(m_pValue->isInit() && valueNew->isEqual(m_pValue.get()))
        return;

    m_pValue->resetFromString(sourceValue);
    emit onParamChange(m_pValue);
}


CValueItem::CValueItem(const QSharedPointer<IPropertyBase>& prop)
{
    m_pValue.reset(prop->clone());
    if(false)
    {
        // todo: add validator for each text type (int, uint, float, string)
        QRegExp re("\\d+");
        QRegExpValidator *validator = new QRegExpValidator(re, this);
        setValidator(validator);
    }
    setPlaceholderText("undefined/different");
    //setFrame(false); //with frames look nicer

    if(m_pValue->isInit())
    {
        setText(m_pValue->toString());
    }
    m_filter.reset(new CLineEditEventFilter(this, text())); // create text backuper with old text
    this->installEventFilter(m_filter.get());
    QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(onTextChangeEnd()));
}


CComboDynItem::CComboDynItem(const QSharedPointer<IPropertyBase> &prop)
{
    setFocusPolicy(Qt::FocusPolicy::TabFocus);
    setStyleSheet("combobox-popup: 0;");
    setMaxVisibleItems(20);
    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pListModel.reset(new QStringListModel());
    m_pValue.reset(prop->clone());

    if(m_pValue->isInit())
    {
        insertItem(0, m_pValue->toString());
        setCurrentIndex(0);
    }
    else
    {
        insertItem(0, "undefined/different");
        setCurrentIndex(0);
    }
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(_onChange(QString))); //reconnect default currentIndexChanged to override
}

void CComboDynItem::showPopup()
{
    blockSignals(true);
    clear();
    if(m_pValue->type() == eObjParam_PRIM_TXTR)
    {
        m_pListModel->setStringList(CTextureList::getInstance()->textureList());
    }
    else if(m_pValue->type() == eObjParam_TEMPLATE)
    {
        m_pListModel->setStringList(CObjectList::getInstance()->figureList());
    }
    else
    {
        Q_ASSERT(false && "not defined");
        return;
    }

    setModel(m_pListModel.get());
    if(m_pValue->isInit())
        setCurrentText(m_pValue->toString());
    else
    {
        insertItem(0, "undefined/different");
        setCurrentIndex(0);
    }

    blockSignals(false);
    QComboBox::showPopup();
}

void CComboDynItem::_onChange(QString str)
{
    if(m_pValue->isEqual(str))
        return;

    m_pValue->resetFromString(str);
    emit onParamChange(m_pValue);
}

C3DItem::C3DItem(const QSharedPointer<IPropertyBase>& propX,const QSharedPointer<IPropertyBase>& propY, const QSharedPointer<IPropertyBase>& propZ)
{
    auto* pLayout = new QFormLayout();
    pLayout->setMargin(0);
    const auto setProp = [this, &pLayout](QString rowName, const QSharedPointer<IPropertyBase>& prop)
    {
        auto pLine = new CValueItem(prop);
        QObject::connect(pLine, SIGNAL(onParamChange(const QSharedPointer<IPropertyBase>&)), this, SLOT(_onParamChange(const QSharedPointer<IPropertyBase>&)));
        pLayout->addRow(rowName, pLine);
    };

    setProp("X:", propX);
    setProp("Y:", propY);
    setProp("Z:", propZ);
    setLayout(pLayout);
}

void C3DItem::_onParamChange(const QSharedPointer<IPropertyBase> &prop)
{
    emit onParamChange(prop);
}
