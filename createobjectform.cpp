#include "createobjectform.h"
#include "ui_createobjectform.h"
#include "objects\lever.h"
#include "objects\light.h"
#include "objects\magictrap.h"
#include "objects\particle.h"
#include "objects\sound.h"
#include "objects\torch.h"
#include "objects\worldobj.h"
#include "objects\unit.h"
#include "objectlist.h"
#include "texturelist.h"

CCreateObjectForm::CCreateObjectForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CCreateObjectForm)
  ,m_pNode(nullptr)
{
    ui->setupUi(this);
    m_tableManager.reset(new CTableManager(ui->tableParameters));
    QObject::connect(m_tableManager.get(), SIGNAL(changeParamSignal(SParam&)), this, SLOT(onParamChange(SParam&)));

    setWindowTitle("Creating object dialog");

    //setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    m_objType[eWorldObject] = "World object";
    m_objType[eUnit] = "Unit";
    m_objType[eTorch] = "Torch";
    m_objType[eMagicTrap] = "Magic trap";
    m_objType[eLever] = "Lever";
    m_objType[eLight] = "Light source";
    m_objType[eSound] = "Sound source";
    m_objType[eParticle] = "Particle source";


    for (const auto& pair : m_objType.toStdMap())
        ui->comboObjectType->addItem(pair.second);
        //ui->comboObjectType->insertItem(ui->comboObjectType->count(), pair.second);
    //onObjectChoose(m_objType.first());

    QObject::connect(ui->comboObjectType, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onObjectChoose(const QString&)));
    onObjectChoose(ui->comboObjectType->currentText());

}

CCreateObjectForm::~CCreateObjectForm()
{
    delete ui;
}

void CCreateObjectForm::updateTable()
{
    m_tableManager->reset();
    QMap<EObjParam, QString> aParam;
    m_pNode->collectParams(aParam, m_pNode->nodeType());
    m_tableManager->setNewData(aParam);
}

void CCreateObjectForm::onObjectChoose(const QString& object)
{
    ENodeType objType = m_objType.key(object);

    if (m_pNode)
        delete m_pNode;

    switch (objType)
    {
    case ENodeType::eUnit:
    {
        m_pNode = new CUnit();
        break;
    }
    case ENodeType::eTorch:
    {
        m_pNode = new CTorch();
        break;
    }
    case ENodeType::eMagicTrap:
    {
        m_pNode = new CMagicTrap();
        break;
    }
    case ENodeType::eLever:
    {
        m_pNode = new CLever();
        break;
    }
    case ENodeType::eLight:
    {
        m_pNode = new CLight();
        break;
    }
    case ENodeType::eSound:
    {
        m_pNode = new CSound();
        break;
    }
    case ENodeType::eParticle:
    {
        m_pNode = new CParticle();
        break;
    }
    case ENodeType::eWorldObject:
    {
        m_pNode = new CWorldObj();
        break;
    }
    default:
    {
        Q_ASSERT("unknown node type" && false);
        break;
    }
    }

    if(nullptr == m_pNode)
    {
        Q_ASSERT("Failed to create new node" && false);
    }
    updateTable();
//    pNode->attachMob(this);
//    addNode(pNode);
//    pNode->loadFigure();
//    pNode->loadTexture();

}

void CCreateObjectForm::onParamChange(SParam& param)
{
    //TODO special case for changing model, texture
    m_pNode->applyParam(param.param, param.value);
    updateTable();
}
