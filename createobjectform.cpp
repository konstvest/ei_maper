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
#include "layout_components/tree_view.h"
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
    QObject::connect(m_tableManager.get(), SIGNAL(onUpdateProperty(QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(QSharedPointer<IPropertyBase>)));

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
    QList<QSharedPointer<IPropertyBase>> aProp;
    m_pNode->collectParams(aProp, m_pNode->nodeType());
    //filter parameters
    util::removeProp(aProp, eObjParam_NID);
    util::removeProp(aProp, eObjParam_POSITION);
    util::removeProp(aProp, eObjParam_POSITION_X);
    util::removeProp(aProp, eObjParam_POSITION_Y);
    util::removeProp(aProp, eObjParam_POSITION_Z);
    util::removeProp(aProp, eObjParam_ROTATION);
    util::removeProp(aProp, eObjParam_ROTATION_X);
    util::removeProp(aProp, eObjParam_ROTATION_Y);
    util::removeProp(aProp, eObjParam_ROTATION_Z);
    util::removeProp(aProp, eObjParam_COMPLECTION);

    auto type = m_pNode->nodeType();
    switch (type)
    {
    case ENodeType::eTorch:
    {
        util::removeProp(aProp, eObjParam_PLAYER);
        break;
    }
    case ENodeType::eLight:
    case ENodeType::eSound:
    case ENodeType::eParticle:
    {
        util::removeProp(aProp, eObjParam_PRIM_TXTR);
        break;
    }
    case ENodeType::eMagicTrap:
    {
        util::removeProp(aProp, eObjParam_PRIM_TXTR);
        break;
    }
    default:
        break;
    }

    m_tableManager->setNewData(aProp);
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
    QList<CNode*> arrNode = m_pView->selectedNodes();
    if(arrNode.size() == 1)
    {
        //copy data from selected node
        auto pSourceNode = arrNode.first();
        QList<QSharedPointer<IPropertyBase>> aProp;
        ENodeType type = pSourceNode->nodeType();
        pSourceNode->collectParams(aProp, type);
        for(const auto& param: aProp)
        {
            const EObjParam& tp = param->type();
            if(tp == eObjParam_POSITION_X
                    || tp == eObjParam_POSITION_Y
                    || tp == eObjParam_POSITION_Z)
                continue;
            if(objType == eMagicTrap && param->type() == eObjParam_NAME)
                continue;
//            switch (param.first) {
//            case eObjParam_ROTATION:
//            case eObjParam_POSITION:
//            {
//                continue;
//            }
//            default:
//                break;
//            }
            m_pNode->applyParam(param);
        }
    }
    m_pPreview->attachNode(m_pNode);
    updateTable();
}

void CCreateObjectForm::onParamChange(const QSharedPointer<IPropertyBase> prop)
{
    m_pNode->applyParam(prop);
    if(prop->type() == eObjParam_TEMPLATE)
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
    auto posOnLand = m_pView->getTerrainPos(pos.x(), pos.y());
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

void CCreateObjectForm::showEvent(QShowEvent *pEvent)
{
    QString text = ui->comboObjectType->currentText();
    if(text != "<choose object type>")
    {
        onObjectChoose(text);
    }
    QWidget::showEvent(pEvent);
}

