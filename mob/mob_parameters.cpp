#include <QTableWidget>
#include <QMessageBox>
#include <QTextBlock>
#include <QMenu>
#include <QTextCodec>

#include "mob_parameters.h"
#include "ui_mob_parameters.h"
#include "range_dialog.h"
#include "utils.h"
#include "view.h"
#include "settings.h"
#include "undo.h"
#include "layout_components\ui_connectors.h"

CMobParameters::CMobParameters(QWidget* parent, CMob* pMob, CView* pView):
    QWidget(parent)
  ,ui(new Ui::CMobParameters)
  ,m_pCurMob(pMob)
  ,m_pHighlighter(nullptr)
  ,m_pView(pView)
  ,m_isDurty(false)
  ,m_pShortcut(nullptr)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    initLineEdit();

    m_pUndoStack.reset(new QUndoStack(this));
    ui->undoViewMob->setStack(m_pUndoStack.get());

    generateTitle();
    ui->listRanges->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_pTable.reset(new QTableWidget(32, 32));
    QObject::connect(m_pTable.get(), SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableItemClicked(int,int)));
    m_pShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(onSaveShortcut()));
    //m_aCell.clear();
    m_aCell.resize(32*32);
    for (int i(0); i < 32; ++i)
    {
        for (int j(0); j < 32; ++j)
        {
            m_aCell[i*32+j].reset(new QTableWidgetItem());
            // low performance
            m_pTable->setItem(i, j, m_aCell[i*32+j].get());
        }
    }
    //connect(m_pTable.get(), SIGNAL(cellClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
    m_pHighlighter = new Highlighter(ui->plainTextEdit->document());
    ui->listRanges->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listRanges, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    updateWindow();
}

CMobParameters::~CMobParameters()
{
    m_aCell.clear();
    m_pTable.clear();
    delete m_pShortcut;
}

void CMobParameters::reset()
{
    paramLine(eWsTypeWindDir)->clear();
    paramLine(eWsTypeWindStr)->clear();
    paramLine(eWsTypeAmbient)->clear();
    paramLine(eWsTypeTime)->clear();
    paramLine(eWsTypeSunLight)->clear();
    ui->listRanges->clear();
    ui->plainTextEdit->clear();
    ui->isPrimaryBox->setChecked(false);
    m_pCurMob = nullptr;
}

void CMobParameters::test()
{
    //ui->diplomacyButton->click();
    show();
}

void CMobParameters::execWsChanges(EWsType paramType, QString &value)
{
    auto pCommand = new CChangeWorldSetCommand(m_pCurMob, paramType, value);
    QObject::connect(pCommand, SIGNAL(changeWsSignal()), this, SLOT(updateMobParamsOnly()));
    m_pUndoStack->push(pCommand);
}

void CMobParameters::setNewRange(SRange& arrRanges, int index)
{
    auto pRangeCommand = new CChangeRangeCommand(m_pCurMob, index, arrRanges);
    QObject::connect(pRangeCommand, SIGNAL(changeRangeSignal()), this, SLOT(updateMobParamsOnly()));
    m_pUndoStack->push(pRangeCommand);
}

void CMobParameters::initLineEdit()
{
    for(int i(0); i<eWsTypeCount; ++i)
    {
        auto pParam = new CParamLineEdit(this, (EWsType)i);
        pParam->setMinimumSize(QSize(0, 0));
        ui->formLayout_2->setWidget(1+i, QFormLayout::FieldRole, pParam); // 0 row - label environment
        m_arrMobParam.append(pParam);
    }
}

CParamLineEdit *CMobParameters::paramLine(EWsType param)
{
    CParamLineEdit* pEdit = nullptr;
    foreach(pEdit, m_arrMobParam)
    {
        if(pEdit->isParam(param))
            return pEdit;
    }
    Q_ASSERT("cant find suitable line edit" && false);
    return pEdit;
}

const QVector<SRange> &CMobParameters::activeRanges()
{
    return m_pCurMob->ranges(!m_pCurMob->isQuestMob());
}

void CMobParameters::setDurty(bool isDurty)
{
    m_isDurty = isDurty;
    generateTitle();
}

void CMobParameters::generateTitle()
{
    QString title("Mob Parameters: " + m_pCurMob->filePath().absoluteFilePath());
    if (m_isDurty)
        title += " *";

    setWindowTitle(title);
}

void colorizeCell(QTableWidgetItem* pCell, int value)
{
    switch (value)
    {
    case 0: // if diplomacy table - friend. if scripts - neutral
        pCell->setBackground(Qt::darkGreen);
        break;
    case 1: // if diplomacy table - neutral. if scripts - friend
        pCell->setBackground(Qt::darkYellow);
        break;
    case 2:
        pCell->setBackground(Qt::darkRed);
        break;
    }
}

void CMobParameters::updateWindow()
{
    Q_ASSERT(m_pCurMob);


    QVector<SRange> range;
    if(m_pCurMob->isQuestMob())
    { // collect only sec rangers for quest mob
        range = m_pCurMob->ranges(false);
        ui->baseMobParamWidget->hide();
    }
    else
    { // collect data for base mob

        CWorldSet worldSet = m_pCurMob->worldSet();
        for(int i(0); i<eWsTypeCount; ++i)
        {
            paramLine((EWsType)i)->setText(worldSet.data((EWsType)i));
            paramLine((EWsType)i)->saveBackupValue();
        }

        range = m_pCurMob->ranges(true);

        ui->baseMobParamWidget->show();
    }

    QString rangeText;
    ui->listRanges->clear();
    const SRange& activeRange = m_pCurMob->activeRange();
    for (const auto& r : range)
    {
        rangeText = QString::number(r.minRange) + "-" + QString::number(r.maxRange);
        if(r == activeRange)
        {
            ui->listRanges->addItem(new QListWidgetItem(*CIconManager::getInstance()->icon("arrow_right"), rangeText));
        }
        else
            ui->listRanges->addItem(rangeText);
//            auto pItem = ui->listRanges->item(ui->listRanges->count()-1);
//            pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
    }

    ui->plainTextEdit->clear();
    QTextCodec* pDefaultTextCodec = QTextCodec::codecForName("Windows-1251");
    QTextDecoder* pDecoder = new QTextDecoder(pDefaultTextCodec);
    QString str = pDecoder->toUnicode(m_pCurMob->script().toLatin1());
    ui->plainTextEdit->setPlainText(str);

    //diplomacy
    QStringList dipName = QStringList::fromVector(m_pCurMob->diplomacyNames());
    QVector<QVector<uint>>& dipTable = m_pCurMob->diplomacyField();
    int val;
    if(!dipName.isEmpty() && !dipTable.isEmpty())
    {
        Q_ASSERT(dipName.size() == 32);
        for (int i(0); i < dipTable.size(); ++i)
        {
            for (int j(0); j < dipTable[i].size(); ++j)
            {
                auto pCell = m_aCell[i*32 + j];
                val = dipTable[i][j];
                pCell->setText(QString::number(val));
                if(i == j)
                    pCell->setBackground(QBrush(Qt::gray));
                else
                {
                    colorizeCell(pCell.get(), val);
                }
                pCell->setTextColor(Qt::white);
                pCell->setFlags(pCell->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
    ui->isPrimaryBox->setChecked(m_pCurMob->isQuestMob());
}


void CMobParameters::on_diplomacyButton_clicked()
{
    if(!m_pCurMob)
    {
        return;
    }

    QStringList dipName = QStringList::fromVector(m_pCurMob->diplomacyNames());
    QVector<QVector<uint>> dipTable = m_pCurMob->diplomacyField();
    if(dipName.isEmpty() || dipTable.isEmpty())
    {
        QMessageBox::information(nullptr, "Info", "This mob has no diplomacy table");
        return;
    }

    //setup view

    HorizontalHeaderView* headerView = new HorizontalHeaderView(Qt::Horizontal); //todo: remove memory leak!

    //QHeaderView* headerView = new QHeaderView(Qt::Horizontal);

    m_pTable->setHorizontalHeader(headerView); //set our custom headerview
    m_pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_pTable->setVerticalHeaderLabels(dipName);
    m_pTable->setHorizontalHeaderLabels(dipName);
    m_pTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_pTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTable->resizeColumnsToContents();
    m_pTable->resizeRowsToContents();
    m_pTable->setFixedSize(m_pTable->horizontalHeader()->length() + m_pTable->verticalHeader()->width() + m_pTable->columnWidth(0), m_pTable->verticalHeader()->length() + m_pTable->rowHeight(0)*3);
    //m_pTable->setFixedSize(m_pTable->horizontalHeader()->length()+m_pTable->verticalHeader()->width(), m_pTable->verticalHeader()->length()+m_pTable->horizontalHeader()->height());
    m_pTable->show();
    //todo: disable mob parameter widget, connect to table close signal to enable main window again
}

void CMobParameters::tableItemClicked(int r, int c)
{
    if (r==c)
        return;

    auto pCommand = new CChangeDiplomacyTableItem(m_pCurMob, r, c);
    QObject::connect(pCommand, SIGNAL(changeDipGroup(int,int)), this, SLOT(updateDiplomacyTable(int,int)));
    m_pUndoStack->push(pCommand);
}

void CMobParameters::on_pushCancel_clicked()
{
    if(m_pUndoStack->count() != 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Closing", "Parameters has local changes.\nDo you want to Apply them?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Yes)
        {
            m_pView->setDurty(m_pCurMob);
        }
        else if(reply == QMessageBox::No)
        {
            for(int i(0); i<m_pUndoStack->count(); ++i)
                m_pUndoStack->undo();
        }
        else if(reply == QMessageBox::Cancel)
        {
            return;
        }

    }
    emit editFinishedSignal(this);
    close();
}

void CMobParameters::on_pushApply_clicked()
{
    QTextCodec* pDefaultTextCodec = QTextCodec::codecForName("Windows-1251");
    auto strArr = pDefaultTextCodec->fromUnicode(ui->plainTextEdit->toPlainText()); //convert from script unicode to win1251 byte array
    QString str = QString::fromLatin1(strArr); //convert byte array to qstring

    m_pCurMob->setScript(str);
    if(m_pUndoStack->count() > 0)
    {
        m_pUndoStack->clear();
        m_pView->setDurty(m_pCurMob);
    }
    emit editFinishedSignal(this);
    close();
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QBrush(QColor(22, 67, 233)));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bGlobalVars\\b" << "\\bDeclareScript\\b" << "\\bScript\\b"
                    << "\\bWorldScript\\b" << "\\bif\\b" << "\\bthen\\b"
                    << "\\bNULL\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //
//    classFormat.setFontWeight(QFont::Bold);
//    classFormat.setForeground(Qt::green);
//    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
//    rule.format = classFormat;
//    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setForeground(QBrush(QColor(196, 62, 229)));
    functionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\b(ActivateTrap|Add|AddLoot|AddMob|AddObject|AddRectToArea|AddRoundToArea|AddUnitToParty|AddUnitToServer|AddUnitUnderControl|AlarmPosX|AlarmPosY|AlarmTime|Any|AttachParticles|AttachParticleSource|Attack|BlockUnit|Cast|CastSpellPoint|CastSpellUnit|ConsoleFloat|ConsoleString|CopyItems|CopyLoot|CopyStats|Crawl|CreateFX|CreateFXSource|CreateLightning|CreateParticleSource|CreateParty|CreatePointLight|CreateRandomizeFXSource|DeleteArea|DeleteFXSource|DeleteLightning|DeleteParticleSource|DeletePointLight|DistanceUnitPoint|DistanceUnitUnit|Div|EnableLever|EraseQuestItem|Every|FixItems|FixWorldTime|Follow|For|ForIf|GetAIClass|GetBSZValue|GetDiplomacy|GetFutureX|GetFutureY|GetLeader|GetLeverState|GetLootItemsCount|GetMercsNumber|GetMoney|GetObject|GetObjectByID|GetObjectByName|GetObjectID|GetPlayer|GetPlayerUnits|GetUnitOfPlayer|GetWorldTime|GetX|GetY|GetZ|GetZValue|GiveDexterity|GiveIntelligence|GiveItem|GiveMoney|GiveQuestItem|GiveSkill|GiveStrength|GiveUnitQuestItem|GiveUnitSpell|GodMode|GroupAdd|GroupCross|GroupHas|GroupSee|GroupSize|GroupSub|GSDelVar|GSGetVar|GSSetVar|GSSetVarMax|Guard|HaveItem|HideObject|HP|Idle|InflictDamage|InvokeAlarm|IsAlarm|IsAlive|IsCameraPlaying|IsDead|IsEnemy|IsEqual|IsEqualString|IsGreater|IsInArea|IsInSquare|IsLess|IsNight|IsPlayerInDanger|IsPlayerInSafety|IsUnitBlocked|IsUnitInWater|IsUnitVisible|KillScript|KillUnit|LeaveToZone|Lie|Mana|MaxHP|MaxMana|MoveParticleSource|MovePointLight|MoveToObject|MoveToPoint|Mul|Not|PlayAnimation|PlayCamera|PlayerSee|PlayFX|PlayMovie|QFinish|QObjArea|QObjGetItem|QObjKillGroup|QObjKillUnit|QObjSeeObject|QObjSeeUnit|QObjUse|QStart|QuestComplete|Random|RecalcMercBriefings|RedeployParty|RemoveObject|RemoveObjectFromServer|RemoveParty|RemoveQuestItem|RemoveUnitFromControl|RemoveUnitFromParty|RemoveUnitFromServer|ResetTarget|Rest|RotateTo|Run|RunWorldTime|SendEvent|SendStringEvent|Sentry|SetCameraOrientation|SetCameraPosition|SetCP|SetCPFast|SetCurrentParty|SetDiplomacy|SetDirectionToObject|SetEnemy|SetParticleSourceSize|SetPlayer|SetPlayerAggression|SetScience|SetSpellAggression|SetSunLight|SetWaterLevel|SetWind|ShowBitmap|ShowCredits|Sleep|SleepUntil|SleepUntilIdle|Stand|StartAnimation|Sub|SwitchLeverState|SwitchLeverStateEx|UMAg|UMAggression|UMClear|UMCorpseWatcher|UMFear|UMFollow|UMGuard|UMGuardEx|UMPatrol|UMPatrolAddPoint|UMPatrolAddPointLook|UMPatrolClear|UMPlayer|UMRevenge|UMSentry|Walk|CreateRandomizedFXSource|PlayMusic|SetBackGroundColor|UMStandard|UMSuspection|UnitInSquare|UnitSee|WaitEndAnimation|WaitSegment|WasLooted|GetPlayerOfUnit|GetUnitExp|GetArmor|GetObjectStringID|FloatToStr|StrToFloat|StrCat|StrLen|Sin|Cos|Pow|DistancePointPoint|NearestUnit|UnitsNearPoint)(?!.*\")\\b");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//    QStringList funcPatterns;
//    keywordPatterns << "\\bGlobalVars\\b" << "\\bDeclareScript\\b" << "\\bScript\\b"
//                    << "\\bWorldScript\\b" << "\\bif\\b" << "\\bthen\\b"
//                    << "\\bNULL\\b";
//    foreach (const QString &pattern, funcPatterns) {
//        rule.pattern = QRegExp(pattern);
//        rule.format = functionFormat;
//        highlightingRules.append(rule);
//    }

//    customFunctionFormat.setFontItalic(true);
//    customFunctionFormat.setForeground(Qt::magenta);
//    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
//    rule.format = customFunctionFormat;
//    highlightingRules.append(rule);

    //
    singleLineCommentFormat.setForeground(Qt::lightGray);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::lightGray);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    typesFormat.setFontWeight(QFont::Bold);
    typesFormat.setForeground(QBrush(QColor(80, 232, 209)));
    rule.pattern = QRegExp("(object|float|group|string)((?=,|\\s+|\\))|$)");
    rule.format = typesFormat;
    highlightingRules.append(rule);

    numberFormat.setFontWeight(QFont::Bold);
    numberFormat.setForeground(QBrush(QColor(224, 217, 26)));
    //rule.pattern = QRegExp("(?<!_)[0-9.]+"); //look behind didnt work with QRegExp. Use QRegularExpression instead. rewrite highlight block
    rule.pattern = QRegExp("\\-?\\b[\\d.]+\\b(?![a-zA-Z ]*\")");
    rule.format = numberFormat;
    highlightingRules.append(rule);
}


void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
            QRegExp expression(rule.pattern);
            int index = expression.indexIn(text);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, rule.format);
                index = expression.indexIn(text, index + length);
            }
        }
    //
    setCurrentBlockState(0);
    //
    int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);
     //
     while (startIndex >= 0) {
          int endIndex = commentEndExpression.indexIn(text, startIndex);
          int commentLength;
          if (endIndex == -1) {
              setCurrentBlockState(1);
              commentLength = text.length() - startIndex;
          } else {
              commentLength = endIndex - startIndex
                              + commentEndExpression.matchedLength();
          }
          setFormat(startIndex, commentLength, multiLineCommentFormat);
          startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
      }
}


void CMobParameters::on_isPrimaryBox_clicked()
{
    if(nullptr == m_pCurMob)
        return;

    //m_pView->switchToQuestMob(m_pCurMob, !ui->isPrimaryBox->isChecked());
    auto pCommand = new CSwitchToQuestMobCommand(m_pCurMob);
    QObject::connect(pCommand, SIGNAL(switchQuestMobSignal()), this, SLOT(updateMobParamsOnly()));
    m_pUndoStack->push(pCommand);

    //auto iii = m_pUndoStack->count();
    //convertIdRange();
    //m_pCurMob->setQuestMob(ui->isPrimaryBox->isChecked());
    //updateWindow();
}

void CMobParameters::on_button_minusRanges_clicked()
{
    int index = ui->listRanges->currentRow();
    if(index < 0)
        return;

    SRange range;
    auto pRangeCommand = new CChangeRangeCommand(m_pCurMob, index, range);
    QObject::connect(pRangeCommand, SIGNAL(changeRangeSignal()), this, SLOT(updateMobParamsOnly()));
    m_pUndoStack->push(pRangeCommand);
}


void CMobParameters::on_button_plusRanges_clicked()
{
    CRangeDialog* rangeD = new CRangeDialog(this);
    rangeD->setAttribute(Qt::WA_DeleteOnClose);
    rangeD->initRanges(activeRanges(), ui->listRanges->count());
    rangeD->exec();
}

void CMobParameters::onMobUnload(CMob *pMob)
{
    if(pMob == m_pCurMob)
        close();
}


CParamLineEdit::CParamLineEdit(QWidget *pParent, EWsType param):
    QLineEdit(pParent)
  ,m_mobParam(param)
{
    m_pParent = dynamic_cast<CMobParameters*>(pParent);
    QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(editingFinishedOverried()));
}

void CParamLineEdit::saveBackupValue()
{
    m_storedValue = text();
}

void CParamLineEdit::editingFinishedOverried()
{
    auto enteredText = text();
    if(enteredText == m_storedValue)
        return;

    if(isValidValue(m_mobParam, enteredText))
    {
        m_storedValue = enteredText;
        m_pParent->execWsChanges(m_mobParam, enteredText);
    }
    else
    { // return old value
        //todo: show user warn message
        this->blockSignals(true);
        setText(m_storedValue);
        this->blockSignals(false);
    }

}

bool CParamLineEdit::isValidValue(EWsType paramType, const QString& str)
{
    bool bRes = false;
    if(paramType == eWsTypeWindDir)
    {// vector3d. checked value: (111.66,94.46,-0.25) ( 0.21, 0.13, 1.20)
        QRegExp rx("\\((\\s*-?\\d+(\\.\\d+)?\\,){2}(\\s*-?\\d+(\\.\\d+)?)\\)");
        bRes = rx.exactMatch(str);
    }
    else
    { //float;
        QRegExp rx("^\\s*-?\\d+(\\.\\d+)?");
        bRes = rx.exactMatch(str);
    }

    return bRes;
}

void CMobParameters::on_listRanges_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    auto rangeD = new CRangeDialog(this);
    rangeD->setAttribute(Qt::WA_DeleteOnClose);
    rangeD->initRanges(activeRanges(), ui->listRanges->currentRow());
    rangeD->exec();
}

void CMobParameters::on_pushButtonOpenExtEditor_clicked()
{

}

void CMobParameters::updateMobParamsOnly()
{
    Q_ASSERT(m_pCurMob);

    QVector<SRange> arrRange;
    if(m_pCurMob->isQuestMob())
    { // collect only sec rangers for quest mob
        arrRange = m_pCurMob->ranges(false);
        ui->baseMobParamWidget->hide();
    }
    else
    { // collect data for base mob
        CWorldSet worldSet = m_pCurMob->worldSet();
        for(int i(0); i<eWsTypeCount; ++i)
        {
            paramLine((EWsType)i)->setText(worldSet.data((EWsType)i));
            paramLine((EWsType)i)->saveBackupValue();
        }

        arrRange = m_pCurMob->ranges(true);

        ui->baseMobParamWidget->show();
    }

    QString rangeText;
    ui->listRanges->clear();
    SRange range;
    foreach(range, arrRange)
    {
        rangeText = QString::number(range.minRange) + "-" + QString::number(range.maxRange);
        ui->listRanges->addItem(rangeText);
    }

    ui->isPrimaryBox->setChecked(m_pCurMob->isQuestMob());
    setDurty(true);
}

void CMobParameters::updateDiplomacyTable()
{
    //diplomacy
    QStringList dipName = QStringList::fromVector(m_pCurMob->diplomacyNames());
    const QVector<QVector<uint>>& dipTable = m_pCurMob->diplomacyField();
    if(!dipName.isEmpty() && !dipTable.isEmpty())
    {
        Q_ASSERT(dipName.size() == 32);
        for (int i(0); i < dipTable.size(); ++i)
        {
            for (int j(0); j < dipTable[i].size(); ++j)
            {
                auto pCell = m_aCell[i*32 + j];
                pCell->setText(QString::number(dipTable[i][j]));
                //pCell->setFlags(pCell->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
}

void CMobParameters::updateDiplomacyTable(int row, int column)
{
    //diplomacy
    QStringList dipName = QStringList::fromVector(m_pCurMob->diplomacyNames());
    const QVector<QVector<uint>>& dipTable = m_pCurMob->diplomacyField();
    if(!dipName.isEmpty() && !dipTable.isEmpty())
    {
        Q_ASSERT(dipName.size() == 32);
        auto pCell = m_aCell[row*32 + column];
        int value = dipTable[row][column];
        pCell->setText(QString::number(value));
        colorizeCell(pCell.get(), value);
    }
}


void CMobParameters::on_pushCheck_clicked()
{

}

void CMobParameters::onSaveShortcut()
{
    ui->plainTextEdit->document()->clearUndoRedoStacks();
    m_pUndoStack->clear();
    setDurty(false);
}

void CMobParameters::showContextMenu(QPoint pos)
{
    // Handle global position
        QPoint globalPos = ui->listRanges->mapToGlobal(pos);

        // Create menu and insert some actions
        QMenu myMenu;
        myMenu.addAction("mark as range for auto-generate IDs", this, SLOT(markRange()));
        //myMenu.addAction("Erase",  this, SLOT(eraseItem()));

        // Show context menu at handling position
        myMenu.exec(globalPos);
}

void CMobParameters::markRange()
{
    const QVector<SRange>& arrRange = m_pCurMob->ranges(!m_pCurMob->isQuestMob());
    m_pCurMob->setActiveRange(arrRange[ui->listRanges->currentRow()]);
    // If multiple selection is on, we need to erase all selected items
        for (int i = 0; i < ui->listRanges->selectedItems().size(); ++i)
        {
            qDebug() << ui->listRanges->item(ui->listRanges->currentRow())->text();
            // Get curent item on selected row
            //QListWidgetItem *item = ui->listRanges->takeItem(ui->listRanges->currentRow());
            // And remove it
            //delete item;
        }
        updateRangeList();
}

void CMobParameters::updateRangeList()
{
    ui->listRanges->clear();
    QVector<SRange> arrRange = m_pCurMob->ranges(!m_pCurMob->isQuestMob());
    QString rangeText;
    ui->listRanges->clear();
    const SRange& activeRange = m_pCurMob->activeRange();
    for (const auto& r : arrRange)
    {
        rangeText = QString::number(r.minRange) + "-" + QString::number(r.maxRange);
        if(r == activeRange)
            ui->listRanges->addItem(new QListWidgetItem(*CIconManager::getInstance()->icon("arrow_right"), rangeText));
        else
            ui->listRanges->addItem(rangeText);
    }
}

