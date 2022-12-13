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
#include "resourcemanager.h"
#include "landscape.h"
#include "log.h"
#include "preview.h"
#include "undo.h"
#include "tree_view.h"
#include <QJsonArray>

CCreateObjectForm::CCreateObjectForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CCreateObjectForm)
  ,m_pNode(nullptr)
  ,m_pView(nullptr)
  ,m_pPreview(nullptr)
  ,m_pUndoStack(nullptr)
{
    ui->setupUi(this);
    initViewWidget();
    QObject::connect(this, SIGNAL(sendNewBbbox(CBox)), m_pPreview, SLOT(refreshCam(CBox)));
    m_tableManager.reset(new CTableManager(ui->tableParameters));
    QObject::connect(m_tableManager.get(), SIGNAL(changeParamSignal(SParam&)), this, SLOT(onParamChange(SParam&)));

    setWindowTitle("Creating object dialog");
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);


    m_objType[eWorldObject] = "World object";
    m_objType[eUnit] = "Unit";
    m_objType[eTorch] = "Torch";
    m_objType[eMagicTrap] = "Magic trap";
    m_objType[eLever] = "Lever";
    m_objType[eLight] = "Light source";
    m_objType[eSound] = "Sound source";
    m_objType[eParticle] = "Particle source";
    ui->comboObjectType->initComboItem("<choose object type>", m_objType.values());

    QObject::connect(ui->comboObjectType, SIGNAL(currentIndexChangedOut(QString&)), this, SLOT(onObjectChoose(QString&)));
}

CCreateObjectForm::~CCreateObjectForm()
{
    //delete m_pPreview; // it will remove as children of Form (inherited behaviour from parent widget)
    delete ui;
}

void CCreateObjectForm::attach(CView *pView, QUndoStack *pStack)
{
     m_pView = pView;
     m_pUndoStack = pStack;
}

void CCreateObjectForm::initViewWidget()
{
    m_pPreview = new CPreview(this);
    m_pPreview->setObjectName(QString::fromUtf8("openGLWidget"));

    ui->horizontalLayout_4->addWidget(m_pPreview);
}

void CCreateObjectForm::updateTable()
{
    m_tableManager->reset();
    QMap<EObjParam, QString> aParam;
    m_pNode->collectParams(aParam, m_pNode->nodeType());
    //filter parameters
    aParam.remove(eObjParam_NID);
    aParam.remove(eObjParam_POSITION);
    aParam.remove(eObjParam_ROTATION);

    auto type = m_pNode->nodeType();
    switch (type)
    {
    case ENodeType::eTorch:
    {
        aParam.remove(eObjParam_PLAYER);
        break;
    }
    case ENodeType::eLight:
    case ENodeType::eSound:
    case ENodeType::eParticle:
    {
        aParam.remove(eObjParam_PRIM_TXTR);
        break;
    }
    case ENodeType::eMagicTrap:
    {
        aParam.remove(eObjParam_PRIM_TXTR);
        break;
    }
    default:
        break;
    }

    m_tableManager->setNewData(aParam);
    m_pPreview->updateGL();
}

CPreview *CCreateObjectForm::viewWidget()
{
    return m_pPreview;
}

void CCreateObjectForm::closeEvent(QCloseEvent *event)
{ //ignore close event bcs id does something wrong and closing app has assert when CPreview destroying
    event->ignore();
    hide();
}

void CCreateObjectForm::onObjectChoose(QString& object)
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
    m_pPreview->attachNode(m_pNode);
    updateTable();
}

void CCreateObjectForm::onParamChange(SParam& param)
{
    m_pNode->applyParam(param.param, param.value);
    if(param.param == eObjParam_TEMPLATE)
    {
        emit sendNewBbbox(m_pNode->getBBox());
    }
    updateTable();
}

void CCreateObjectForm::on_buttonCancel_clicked()
{
    hide();
}


void CCreateObjectForm::on_buttonCreate_clicked()
{
    Q_ASSERT(m_pView);
    if (nullptr == m_pView->currentMob())
    {
        ei::log(eLogWarning, "cant create pre-defined object without active mob file");
        return;
    }
    auto pMob = m_pView->currentMob();
    pMob->clearSelect();
    auto pos = QWidget::mapFromGlobal(QCursor::pos());
    auto posOnLand = m_pView->getLandPos(pos.x(), pos.y());
    ENodeType type = m_pNode->nodeType();
    switch (type)
    {
    case eWorldObject:
    case eUnit:
    case eTorch:
    case eMagicTrap:
    case eLever:
    {
        posOnLand.setZ(0.0f);
        break;
    }
    default:
        break;
    }

    QVector3D posBackup(0,0,0);
    m_pNode->updatePos(posOnLand);
    CLandscape::getInstance()->projectPosition(m_pNode);

    CCreateNodeCommand* pUndo = new CCreateNodeCommand(m_pView, m_pNode->toJson());
    QObject::connect(pUndo, SIGNAL(addNodeSignal(CNode*)), m_pView->objectTree(), SLOT(addNodeToTree(CNode*)));
    QObject::connect(pUndo, SIGNAL(undo_addNodeSignal(uint)), m_pView->objectTree(), SLOT(onNodeDelete(uint)));
    m_pUndoStack->push(pUndo);
    //restore node position after projection. it is temporary solution for correct drawing object in preview window
    m_pNode->setPos(posBackup);
    m_pNode->setDrawPosition(posBackup);
    if(type == eUnit)
    {
        dynamic_cast<CUnit*>(m_pNode)->resetLogic();
    }

    m_pView->changeOperation(EButtonOpMove);
    hide();
}

