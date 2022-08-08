#include <QHeaderView>
#include "tablemanager.h"
#include "resourcemanager.h"

CStringItem::CStringItem(QString value, EObjParam param):
    QTableWidgetItem(value)
    ,m_parameter(param)
{

}

void initComboStr(QMap<uint, QString>& aStr, const EObjParam param)
{
    switch (param) {


    case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
    {
        aStr[0] = "Not active";
        aStr[1] = "Free";
        aStr[5] = "Hands skill";
        aStr[8] = "Key ID";
        break;
    }
    case eObjParam_PLAYER:
    {
        for(int i(0); i < 32; ++i)
            aStr[i] = "Player-" + QString::number(i);

        break;
    }
    case eObjParam_IS_SHADOW:
    case eObjParam_LEVER_IS_CYCLED:
    case eObjParam_LEVER_IS_DOOR:
    case eObjParam_USE_IN_SCRIPT:
    case eObjParam_LEVER_RECALC_GRAPH:
    case eObjParam_UNIT_NEED_IMPORT:
    case eObjParam_SOUND_AMBIENT:
    case eObjParam_SOUND_IS_MUSIC:
    case eObjParam_LIGHT_SHADOW:
    case eObjParam_TRAP_CAST_ONCE:
    {
        aStr[0] = "false";
        aStr[1] = "true";
        break;
    }
    case eObjParam_PRIM_TXTR:
    {
        aStr.clear();
        break;
    }
    case eObjParam_TEMPLATE:
    {
        aStr.clear();
        break;
    }
    case eObjParam_TYPE:
    {
        aStr[50] = "Human";
        aStr[51] = "Animal";
        aStr[52] = "Monstr";
        break;
    }
    case eObjParam_PARTICL_TYPE:
    {
      aStr[8192] = "FIREBALL";
      aStr[8193] = "CAMPFIRE";
      aStr[8194] = "FIREBLAST";
      aStr[8195] = "FIRE";
      aStr[8196] = "SMOKE";
      aStr[8197] = "VULCANSMOKE";
      aStr[8198] = "HEALING";
      aStr[8199] = "POISONFOG";
      aStr[8200] = "AGGRESSIONFOG";
      aStr[8201] = "GEYSER";
      aStr[8202] = "TORNADO";
      aStr[8203] = "CASTING";
      aStr[8204] = "NUKE";
      aStr[8205] = "BANSHEECASTING";
      aStr[8206] = "MUSHROOM";
      aStr[8207] = "BLOOD1";
      aStr[8208] = "FIREWALL";
      aStr[8209] = "FIREARROW";
      aStr[8210] = "ACIDRAY";
      aStr[8211] = "BLUEGAS";
      aStr[8212] = "LINK";
      aStr[8213] = "SPHEREACID";
      aStr[8214] = "SPHEREELECTRICITY";
      aStr[8215] = "SPHEREFIRE";
      aStr[8216] = "CLAYRING";
      aStr[8217] = "TELEPORT";
      aStr[8218] = "ANTIMAGIC";
      aStr[8219] = "MODIFIER1";
      aStr[8220] = "MODIFIER2";
      aStr[8221] = "MODIFIER3";
      aStr[8222] = "MODIFIER4";
      aStr[8223] = "MODIFIER5";
      aStr[8224] = "MODIFIER6";
      aStr[8225] = "MODIFIER7";
      aStr[8226] = "MODIFIER8";
      aStr[8227] = "MODIFIER9";
      aStr[8228] = "MODIFIER10";
      aStr[8229] = "MODIFIER11";
      aStr[8230] = "MODIFIER12";
      aStr[8231] = "CASTINGFIRE";
      aStr[8232] = "CASTINGELECTRICITY";
      aStr[8233] = "CASTINGACID";
      aStr[8234] = "CASTINGDIVINATION";
      aStr[8235] = "CASTINGILLUSION";
      aStr[8236] = "CASTINGDOMINATION";
      aStr[8237] = "CASTINGENCHANTMENT";
      aStr[8238] = "CASTINGHEALING";
      aStr[8239] = "CASTINGFAILED";
      aStr[8240] = "LIGHTNINGBLAST";
      aStr[8241] = "BLOOD2";
      aStr[8242] = "BLOOD3";
      aStr[8243] = "BLOOD4";
      aStr[8244] = "A4BLOODRED";
      aStr[8245] = "A4BLOODGREEN";
      aStr[8246] = "A4BLOODBLUE";
      aStr[8247] = "A4BLOODBLACK";
      aStr[8248] = "ZONEEXIT";
      aStr[8249] = "PATH";
      aStr[8250] = "PATHDESTINATION";
      aStr[8251] = "PATHFAILED";
      aStr[8252] = "MOSHKA";
      aStr[8253] = "PORTALSTAR";
      aStr[8254] = "PORTAL";
      aStr[8255] = "CYLINDER1";
      aStr[8256] = "CYLINDER2";
      aStr[8257] = "FIRESTAR";
      aStr[8258] = "ACIDSTAR";
      aStr[8259] = "SPARKS";
      aStr[8260] = "VISIONSTAR1";
      aStr[8261] = "VISIONSTAR2";
      aStr[8262] = "VISIONSTAR3";
      aStr[8263] = "REGENERATION";
      aStr[8264] = "SILENCE";
      aStr[8265] = "FEEBLEMIND";
      aStr[8266] = "FEETCLOUD1";
      aStr[8267] = "FEETCLOUD2";
      aStr[8268] = "VISIONSTAR4";
      aStr[8269] = "BALLOFSTARS";
      aStr[8270] = "RICKARROW";
      aStr[8271] = "CURSESTARS";
      aStr[8272] = "CURSEHOLDER";
      aStr[8273] = "STARTTRANS";
      aStr[8274] = "TRANSFORM";
        break;
    }
    case eObjParam_LOGIC_BEHAVIOUR:
    {
        aStr[0] = "Idle";
        aStr[1] = "Guard radius";
        aStr[2] = "Path";
        aStr[3] = "Place";
        aStr[4] = "Briffing";
        aStr[5] = "Guard Alaram";
        break;
    }
    case eObjParam_AGRESSION_MODE:
    {
        aStr[0] = "Attack";
        aStr[1] = "Revenge";
        aStr[2] = "Fear";
        aStr[3] = "Fear player";
        break;
    }
    default:
        break;
    }
}



CComboBoxItem::CComboBoxItem(const QString& currentValue, EObjParam param)
    :m_parameter(param)
{

    switch(m_parameter)
    {
    case eObjParam_PRIM_TXTR:
    case eObjParam_TEMPLATE:
    {
        break;
    }
    default:
        break;
    }

    initComboStr(m_aComboString, param);
    int i = 0;
    for (const auto& item : m_aComboString.toStdMap())
    {
        insertItem(i, item.second);
        ++i;
    }

    if(isDIfferent(currentValue))
    {
        insertItem(i, valueDifferent());
        setCurrentText(valueDifferent());
    }
    else if(currentValue.isEmpty())
    {
        insertItem(0, "<choose>");
        setCurrentIndex(0);
    }
    else
    {
        switch(param)
        {
        case eObjParam_PRIM_TXTR:
        case eObjParam_TEMPLATE:
        {
            insertItem(0, currentValue);
            break;
        }
        default:
            setCurrentText(m_aComboString[currentValue.toUInt()]);
            break;
        }

    }
    setMaxVisibleItems(20);
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChangedOver(QString))); //reconnect default currentIndexChanged to override
}

void CComboBoxItem::getKey(QString &val)
{
    val = QString::number(m_aComboString.key(currentText()));
}

void CComboBoxItem::getValue(QString &val)
{
    val = currentText();
}

void CComboBoxItem::showPopup()
{
    switch(m_parameter)
    {
    case eObjParam_PRIM_TXTR:
    {
        blockSignals(true);
        auto curText = currentText();
        clear();
        insertItems(0, CTextureList::getInstance()->textureList());
        if(curText != "<choose>")
            setCurrentText(curText);
        else
        {
            insertItem(0, "<choose>");
            setCurrentIndex(0);
        }
        blockSignals(false);
        break;
    }
    case eObjParam_TEMPLATE:
    {
        blockSignals(true);
        auto curText = currentText();
        clear();
        insertItems(0, CObjectList::getInstance()->figureList());
        if(curText != "<choose>")
            setCurrentText(curText);
        else
        {
            insertItem(0, "<choose>");
            setCurrentIndex(0);
        }
        blockSignals(false);
        break;
    }
    default:
        break;
    }

    QComboBox::showPopup();
}

void CComboBoxItem::currentIndexChangedOver(QString str)
{
    if(isDIfferent(str))
        return; //skip <different value>
    emit updateValueOver(this);
}


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

void CTableManager::updateParam(EObjParam param, QString newValue)
{
    m_pTable->blockSignals(true); //block sending signals
    for(int i(0); i<m_pTable->rowCount(); ++i)
    {
        auto tableItem = m_pTable->item(i, 1);
        auto it = dynamic_cast<CStringItem*>(tableItem);
        if (nullptr != it)
            if(it->param() == param)
            {
                it->setText(newValue);
            }
    }
    m_pTable->blockSignals(false); //unblock sending signals
}

void CTableManager::initRowName()
{
    m_aRowName[eObjParam_BODYPARTS] = "Model parts";
    m_aRowName[eObjParam_PLAYER] = "Player (group)";
    m_aRowName[eObjParam_NID] = "Map ID";
    m_aRowName[eObjParam_TYPE] = "Unit subtype";
    m_aRowName[eObjParam_NAME] = "Map(script) name";
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

void CTableManager::onParamChange(CComboBoxItem *pItem)
{
    Q_ASSERT(pItem);
    QString val;
    EObjParam currParam = pItem->param();
    switch(currParam)
    {
    case eObjParam_PRIM_TXTR:
    case eObjParam_TEMPLATE:
    {
        pItem->getValue(val);
        break;
    }
    default:
        pItem->getKey(val);
        break;
    }

    SParam param{currParam, val};
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
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
            m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
            //todo: collect all created items and delete them before create new (or update). reduce memory leaks
            CComboBoxItem* pCombo = new CComboBoxItem(item.second, item.first);
            QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
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
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
            m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
            CComboBoxItem* pCombo = new CComboBoxItem(item.second, eObjParam_LOGIC_BEHAVIOUR);
            QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;

            EBehaviourType type = (EBehaviourType)item.second.toUInt();
            if(type == EBehaviourType::eRadius)
            {
                m_pTable->insertRow(i);
                m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[eObjParam_GUARD_RADIUS]));
                m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
                m_pTable->setItem(i, 1, new CStringItem(aParam[eObjParam_GUARD_RADIUS], eObjParam_GUARD_RADIUS));
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
            m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[item.first]));
            m_pTable->item(i, 0)->setFlags(m_pTable->item(i, 0)->flags() & ~Qt::ItemIsEditable);
            CComboBoxItem* pCombo = new CComboBoxItem(item.second, item.first);
            QObject::connect(pCombo, SIGNAL(updateValueOver(CComboBoxItem*)), this, SLOT(onParamChange(CComboBoxItem*)));
            m_pTable->setCellWidget(i, 1, pCombo);
            ++i;
            if(item.second == "1") // is need import unit stats == true
            {
                QVector<EObjParam> importStats;
                importStats.resize(6);
                importStats[0] = eObjParam_UNIT_STATS;
                importStats[1] = eObjParam_UNIT_WEAPONS;
                importStats[2] = eObjParam_UNIT_ARMORS;
                importStats[3] = eObjParam_UNIT_SPELLS;
                importStats[4] = eObjParam_UNIT_QUICK_ITEMS;
                importStats[5] = eObjParam_UNIT_QUEST_ITEMS;
                for(int j(0); j<importStats.size();++j)
                {
                    m_pTable->insertRow(i);
                    m_pTable->setItem(i, 0, new QTableWidgetItem(m_aRowName[importStats[j]]));
                    m_pTable->setItem(i, 1, new CStringItem(aParam[importStats[j]], importStats[j]));
                    m_pTable->resizeColumnToContents(0);
                    ++i;
                }
            }
            break;
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
