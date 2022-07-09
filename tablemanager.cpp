#include <QHeaderView>

#include "tablemanager.h"
#include "table_item.h"

CTableManager::CTableManager(QTableWidget* pTable):
    m_pTable(pTable)
{
    initRowName();
    reset();
    QObject::connect(m_pTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onParamChange(QTableWidgetItem*)));
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
    m_aRowName[eObjParam_PLAYER] = "Player(group)";
    m_aRowName[eObjParam_NID] = "Map ID";
    m_aRowName[eObjParam_TYPE] = "Type";
    m_aRowName[eObjParam_NAME] = "Map name";
    m_aRowName[eObjParam_TEMPLATE] = "Model name";
    m_aRowName[eObjParam_PARENT_TEMPLATE] = "Template";
    m_aRowName[eObjParam_PRIM_TXTR] = "Texture";
    m_aRowName[eObjParam_SEC_TXTR] = "Secondary texture";
    m_aRowName[eObjParam_COMMENTS] = "Comments";
    m_aRowName[eObjParam_POSITION] = "Position";
    m_aRowName[eObjParam_ROTATION] = "Rotation";
    m_aRowName[eObjParam_USE_IN_SCRIPT] = "Use in script?";
    m_aRowName[eObjParam_IS_SHADOW] = "Is shadow?";
    m_aRowName[eObjParam_PARENT_ID] = "Parent ID";
    m_aRowName[eObjParam_QUEST_INFO] = "Quest name(marker)";
    m_aRowName[eObjParam_COMPLECTION] = "Complection";
    m_aRowName[eObjParam_TORCH_PTLINK] = "Torch point";
    m_aRowName[eObjParam_TORCH_STRENGHT] = "Torch power";
    m_aRowName[eObjParam_TORCH_SOUND] = "Torch sound";
    m_aRowName[eObjParam_RANGE] = "Range";
    m_aRowName[eObjParam_SOUND_RANGE] = "Sound distance";
    m_aRowName[eObjParam_SOUND_MIN] = "Sound min";
    m_aRowName[eObjParam_SOUND_MAX] = "Sound max";
    m_aRowName[eObjParam_SOUND_RESNAME] = "Sound resource name";
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
    m_aRowName[eObjParam_TRAP_SPELL] = "Trap spells";
    m_aRowName[eObjParam_TRAP_AREAS] = "Trap Areas";
    m_aRowName[eObjParam_TRAP_TARGETS] = "Trap targets";
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
}

void CTableManager::onParamChange(CComboBoxItem *pItem)
{
    Q_ASSERT(pItem);
    QString val;
    pItem->getValue(val);
    SParam param{pItem->param(), val};
    emit changeParamSignal(param);
}

void CTableManager::onParamChange(QTableWidgetItem *pItem)
{
    const auto pParamItem  = dynamic_cast<CStringItem*>(pItem);
    Q_ASSERT(pParamItem);
    SParam param{pParamItem->param(), pItem->text()};
    emit changeParamSignal(param);
}

void CTableManager::setNewData(QMap<EObjParam, QString> &aParam)
{
    reset();
    int i(0);
    m_pTable->blockSignals(true); //block sending signals
    for (const auto& item : aParam.toStdMap())
    {
        switch (item.first) {
            case eObjParam_PLAYER:
            case eObjParam_IS_SHADOW:
            case eObjParam_LEVER_IS_CYCLED:
            case eObjParam_LEVER_IS_DOOR:
            case eObjParam_USE_IN_SCRIPT:
            case eObjParam_LEVER_RECALC_GRAPH:
            case eObjParam_UNIT_NEED_IMPORT:
            case eObjParam_SOUND_AMBIENT:
            case eObjParam_SOUND_IS_MUSIC:
            {
                m_pTable->insertRow(i);
                //https://doc.qt.io/archives/qt-4.8/qtablewidget.html#setItem
                //The table takes ownership of the item.
                // => table widget take care about memory
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
                m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
                //todo: collect all created items and delete them before create new (or update). reduce memory leaks
                CComboBoxItem* pCombo = new CComboBoxItem(item.second, item.first);
                QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
                m_pTable->setCellWidget(i, 1, pCombo);
                ++i;
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
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
                m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
                CComboBoxItem* pCombo = new CComboBoxItem(item.second, eObjParam_LEVER_SCIENCE_STATS_Type_Open);
                QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
                m_pTable->setCellWidget(i, 1, pCombo);
                ++i;
                if (!item.second.isEmpty())
                {
                    uint type = item.second.toUInt();
                    switch (type) {
                    case 0:
                    case 1:
                    {
                        break;
                    }
                    case 5:
                    {
                        m_pTable->insertRow(i);
                        m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight]));
                        m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
                        m_pTable->setItem(i, 1, new CStringItem(aParam[eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight], eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight));
                        m_pTable->resizeColumnToContents(0);
                        ++i;
                        break;
                    }
                    case 8:
                    {
                        m_pTable->insertRow(i);
                        m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_LEVER_SCIENCE_STATS_Key_ID]));
                        m_pTable->setItem(i, 1, new CStringItem(aParam[eObjParam_LEVER_SCIENCE_STATS_Key_ID], eObjParam_LEVER_SCIENCE_STATS_Key_ID));
                        m_pTable->resizeColumnToContents(0);
                        ++i;
                        break;
                    }
                    default:
                        Q_ASSERT("go away" && false);
                    }
                }
                break; //eObjParam_LEVER_SCIENCE_STATS_Type_Open
            }
            default:
            {
                m_pTable->insertRow(i);
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
                m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
                m_pTable->setItem(i, 1, new CStringItem(item.second, item.first));
                m_pTable->resizeColumnToContents(0);
                ++i;
                break;
            }
        } //switch
    }
    m_pTable->blockSignals(false);
}
