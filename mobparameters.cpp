#include <QTableWidget>
#include <QMessageBox>
#include <QTextBlock>

#include "mobparameters.h"
#include "ui_mobParameters.h"
#include "utils.h"
#include "view.h"
#include "settings.h"

CMobParameters::CMobParameters(QWidget* parent):
    QWidget(parent)
    ,ui(new Ui::CMobParameters)
    ,m_pCurMob(nullptr)
  ,highlighter(nullptr)
{
    ui->setupUi(this);
    ui->mainRangesList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->secRangesList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QObject::connect(ui->chooseMob, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onChooseMob(const QString&)));
    m_pTable.reset(new QTableWidget(32, 32));
    QObject::connect(m_pTable.get(), SIGNAL(cellDoubleClicked(int,int)), this, SLOT(tableItemClicked(int,int)));
    m_aCell.clear();
    m_aCell.resize(32*32);
    for (int i(0); i < 32; ++i)
    {
        for (int j(0); j < 32; ++j)
        {
            m_aCell[i*32+j].reset(new QTableWidgetItem);
            //auto pCell = new QTableWidgetItem; //memory leaks here, again. and low performance
            m_pTable->setItem(i, j, m_aCell[i*32+j].get());
        }
    }
    //connect(m_pTable.get(), SIGNAL(cellClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
    highlighter = new Highlighter(ui->plainTextEdit->document());
}

CMobParameters::~CMobParameters()
{
    m_aCell.clear();
    m_pTable.clear();
}

void CMobParameters::initMobList(const QVector<CMob*> &mob)
{
    m_aMob = mob;
    ui->chooseMob->clear();
    for(const auto& mob : m_aMob)
    {
        ui->chooseMob->insertItem(ui->chooseMob->count(), mob->mobName());
    }
}

void CMobParameters::reset()
{
    setWindowTitle(mainFormName());
    ui->windDirEdit->clear();
    ui->windStrEdit->clear();
    ui->ambientEdit->clear();
    ui->timeEdit->clear();
    ui->sunLightEdit->clear();
    ui->mainRangesList->clear();
    ui->secRangesList->clear();
    ui->plainTextEdit->clear();
    ui->isPrimaryBox->setChecked(false);
    m_pCurMob = nullptr;
}

void CMobParameters::test()
{
    //ui->diplomacyButton->click();
    show();
}

QString CMobParameters::mainFormName()
{
    return "Mob Parameters";
}

void CMobParameters::updateWindow()
{
    if(!m_pCurMob)
        return;

    setWindowTitle(mainFormName() + " (" + m_pCurMob->filePath().absoluteFilePath() + ")");
    QString nd("not defined");
    auto setUndefined = [&nd](QLineEdit* pEdit)
    {
        pEdit->setText(nd);
    };

    SWorldSet worldSet = m_pCurMob->worldSet();
    if (worldSet.bInit)
    {
        ui->windDirEdit->setText(util::makeString(worldSet.m_windDirection));
        ui->windStrEdit->setText(QString::number(worldSet.m_windStrength));
        ui->ambientEdit->setText(QString::number(worldSet.m_ambient));
        ui->timeEdit->setText(QString::number(worldSet.m_time));
        ui->sunLightEdit->setText(QString::number(worldSet.m_sunLight));
    }
    else
    {
        setUndefined(ui->windDirEdit);
        setUndefined(ui->windStrEdit);
        setUndefined(ui->timeEdit);
        setUndefined(ui->ambientEdit);
        setUndefined(ui->sunLightEdit);
    }

    QVector<SRange> range = m_pCurMob->mainRanges();
    QString rangeText;
    ui->mainRangesList->clear();
    for (const auto& r : range)
    {
        rangeText = QString::number(r.minRange) + "-" + QString::number(r.maxRange);
        ui->mainRangesList->addItem(rangeText);
    }
    range = m_pCurMob->secRanges();
    ui->secRangesList->clear();
    for (const auto& r : range)
    {
        rangeText = QString::number(r.minRange) + "-" + QString::number(r.maxRange);
        ui->secRangesList->addItem(rangeText);
    }
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(m_pCurMob->script());

    //diplomacy
    QStringList dipName = QStringList::fromVector(m_pCurMob->diplomacyNames());
    QVector<QVector<uint>> dipTable = m_pCurMob->diplomacyField();
    if(!dipName.isEmpty() && !dipTable.isEmpty())
    {
        Q_ASSERT(dipName.size() == 32);
        for (int i(0); i < dipTable.size(); ++i)
        {
            for (int j(0); j < dipTable[i].size(); ++j)
            {
                auto pCell = m_aCell[i*32 + j];
                pCell->setText(QString::number(dipTable[i][j]));
                pCell->setFlags(pCell->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
    ui->isPrimaryBox->setChecked(m_pCurMob->isPrimaryMob());
}

void CMobParameters::onChooseMob(const QString &name)
{
    if(name.isEmpty())
        return;

    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
        if (pMob->mobName().contains(name))
        {
            m_pCurMob = pMob;
            break;
        }

    if (!m_pCurMob)
    {
        Q_ASSERT(m_pCurMob);
        return;
    }
    updateWindow();
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

    QTableWidgetItem* item = m_pTable->item(r, c);
    int i = item->text().toInt();
    ++i;
    i = i%3;
    item->setText(QString::number(i));
    COptBool* pOpt = dynamic_cast<COptBool*>(m_pCurMob->view()->settings()->opt("dipEditSymmetric"));
    if(pOpt && pOpt->value())
    {
        item = m_pTable->item(c, r);
        item->setText(QString::number(i));
    }
}

void CMobParameters::on_pushCancel_clicked()
{
    close();
}

void CMobParameters::on_pushApply_clicked()
{
    if(nullptr == m_pCurMob)
        return;

    QString nd("not defined");
    auto isValidText =[&nd](QLineEdit* pEdit)
    {
        if (pEdit->text().isEmpty() || pEdit->text().contains(nd))
            return false;
        return true;
    };

    if(isValidText(ui->windDirEdit)
        && isValidText(ui->windStrEdit)
        && isValidText(ui->timeEdit)
        && isValidText(ui->ambientEdit)
        && isValidText(ui->sunLightEdit)
        )
    {
        SWorldSet ws
        {
             util::vec3FromString(ui->windDirEdit->text()),
             ui->windStrEdit->text().toFloat(),
             ui->timeEdit->text().toFloat(),
             ui->ambientEdit->text().toFloat(),
             ui->sunLightEdit->text().toFloat(),
            true
        };
        m_pCurMob->setWorldSet(ws);
    }
    if(!m_pCurMob->diplomacyField().isEmpty())
    {
        auto& df = m_pCurMob->diplomacyField();
        for(int i(0); i < 32; ++i)
            for(int j(0); j < 32;++j)
                df[i][j] = m_aCell[i*32+j]->text().toUInt();
    }
    //main, sec ranges
    QVector<SRange> aRange;
    for(int i(0); i < ui->mainRangesList->count(); ++i)
    {
        auto a = util::vec2FromString(ui->mainRangesList->item(i)->text());
        SRange r(a[0], a[1]);
        aRange.append(r);
    }
    m_pCurMob->setMainRanges(aRange);
    aRange.clear();
    for(int i(0); i < ui->secRangesList->count(); ++i)
    {
        auto a = util::vec2FromString(ui->secRangesList->item(i)->text());
        SRange r(a[0], a[1]);
        aRange.append(r);
    }
    m_pCurMob->setSecRanges(aRange);
    m_pCurMob->setScript(ui->plainTextEdit-> toPlainText());
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
    rule.pattern = QRegExp("(ActivateTrap|Add|AddLoot|AddMob|AddObject|AddRectToArea|AddRoundToArea|AddUnitToParty|AddUnitToServer|AddUnitUnderControl|AlarmPosX|AlarmPosY|AlarmTime|Any|AttachParticles|AttachParticleSource|Attack|BlockUnit|Cast|CastSpellPoint|CastSpellUnit|ConsoleFloat|ConsoleString|CopyItems|CopyLoot|CopyStats|Crawl|CreateFX|CreateFXSource|CreateLightning|CreateParticleSource|CreateParty|CreatePointLight|CreateRandomizeFXSource|DeleteArea|DeleteFXSource|DeleteLightning|DeleteParticleSource|DeletePointLight|DistanceUnitPoint|DistanceUnitUnit|Div|EnableLever|EraseQuestItem|Every|FixItems|FixWorldTime|Follow|For|ForIf|GetAIClass|GetBSZValue|GetDiplomacy|GetFutureX|GetFutureY|GetLeader|GetLeverState|GetLootItemsCount|GetMercsNumber|GetMoney|GetObject|GetObjectByID|GetObjectByName|GetObjectID|GetPlayer|GetPlayerUnits|GetUnitOfPlayer|GetWorldTime|GetX|GetY|GetZ|GetZValue|GiveDexterity|GiveIntelligence|GiveItem|GiveMoney|GiveQuestItem|GiveSkill|GiveStrength|GiveUnitQuestItem|GiveUnitSpell|GodMode|GroupAdd|GroupCross|GroupHas|GroupSee|GroupSize|GroupSub|GSDelVar|GSGetVar|GSSetVar|GSSetVarMax|Guard|HaveItem|HideObject|HP|Idle|InflictDamage|InvokeAlarm|IsAlarm|IsAlive|IsCameraPlaying|IsDead|IsEnemy|IsEqual|IsEqualString|IsGreater|IsInArea|IsInSquare|IsLess|IsNight|IsPlayerInDanger|IsPlayerInSafety|IsUnitBlocked|IsUnitInWater|IsUnitVisible|KillScript|KillUnit|LeaveToZone|Lie|Mana|MaxHP|MaxMana|MoveParticleSource|MovePointLight|MoveToObject|MoveToPoint|Mul|Not|PlayAnimation|PlayCamera|PlayerSee|PlayFX|PlayMovie|QFinish|QObjArea|QObjGetItem|QObjKillGroup|QObjKillUnit|QObjSeeObject|QObjSeeUnit|QObjUse|QStart|QuestComplete|Random|RecalcMercBriefings|RedeployParty|RemoveObject|RemoveObjectFromServer|RemoveParty|RemoveQuestItem|RemoveUnitFromControl|RemoveUnitFromParty|RemoveUnitFromServer|ResetTarget|Rest|RotateTo|Run|RunWorldTime|SendEvent|SendStringEvent|Sentry|SetCameraOrientation|SetCameraPosition|SetCP|SetCPFast|SetCurrentParty|SetDiplomacy|SetDirectionToObject|SetEnemy|SetParticleSourceSize|SetPlayer|SetPlayerAggression|SetScience|SetSpellAggression|SetSunLight|SetWaterLevel|SetWind|ShowBitmap|ShowCredits|Sleep|SleepUntil|SleepUntilIdle|Stand|StartAnimation|Sub|SwitchLeverState|SwitchLeverStateEx|UMAg|UMAggression|UMClear|UMCorpseWatcher|UMFear|UMFollow|UMGuard|UMGuardEx|UMPatrol|UMPatrolAddPoint|UMPatrolAddPointLook|UMPatrolClear|UMPlayer|UMRevenge|UMSentry|Walk|CreateRandomizedFXSource|PlayMusic|SetBackGroundColor|UMStandard|UMSuspection|UnitInSquare|UnitSee|WaitEndAnimation|WaitSegment|WasLooted|GetPlayerOfUnit|GetUnitExp|GetArmor|GetObjectStringID|FloatToStr|StrToFloat|StrCat|StrLen|Sin|Cos|Pow|DistancePointPoint|NearestUnit|UnitsNearPoint)");
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
    rule.pattern = QRegExp("(^|[^a-zA-Z_\\[])(\\b[0-9.]+\\b)(?!\")"); //todo: exclude whitespace from matching
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

    m_pCurMob->setPrimaryMob(ui->isPrimaryBox->isChecked());
}

