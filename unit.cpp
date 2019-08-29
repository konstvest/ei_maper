#include "unit.h"
#include "view.h"
#include "objectlist.h"
#include "texturelist.h"
#include "landscape.h"

CUnit::CUnit():
    m_mob(nullptr)
{
}

CUnit::~CUnit()
{
    for(auto& logic: m_aLogic)
        delete logic;
}

void CUnit::draw(QOpenGLShaderProgram* program)
{
    CObjectBase::draw(program);
    for(auto& logic: m_aLogic)
    {
        logic->draw(program);
    }
}

void CUnit::drawSelect(QOpenGLShaderProgram* program)
{
    CObjectBase::drawSelect(program);
    for(auto& logic: m_aLogic)
    {
        logic->drawSelect(program);
    }
}

void CUnit::updateFigure(ei::CFigure* fig)
{
    QString pointName("ppoint.mod");
    CObjectBase::updateFigure(fig);
    for(auto& logic: m_aLogic)
    {
        logic->updatePointFigure(m_mob->view()->objList()->getFigure(pointName));
    }
}

void CUnit::setTexture(QOpenGLTexture* texture)
{
    CObjectBase::setTexture(texture);
    for(auto& logic: m_aLogic)
        logic->setPointTexture(m_mob->view()->texList()->textureDefault());
}

uint CUnit::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("UNIT_R"))
        {
            Q_ASSERT("UNIT_R" && false);
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_type);
        }
        else if(parser.isNextTag("UNIT_PROTOTYPE"))
        {
            readByte += parser.readHeader();
            readByte += parser.readString(m_prototypeName, parser.nodeLen());
        }
        else if(parser.isNextTag("UNIT_ITEMS"))
        {
            Q_ASSERT("UNIT_ITEMS" && false);
        }
        else if(parser.isNextTag("UNIT_STATS"))
        {
            readByte += parser.readHeader();
            readByte += parser.readUnitStats(m_stat, parser.nodeLen());
        }
        else if(parser.isNextTag("UNIT_QUEST_ITEMS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aQuestItem);
        }
        else if(parser.isNextTag("UNIT_QUICK_ITEMS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aQuickItem);
        }
        else if(parser.isNextTag("UNIT_SPELLS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aSpell);
        }
        else if(parser.isNextTag("UNIT_WEAPONS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aWeapon);
        }
        else if(parser.isNextTag("UNIT_ARMORS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readStringArray(m_aArmor);
        }
        else if(parser.isNextTag("UNIT_NEED_IMPORT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bImport);
        }
        else if(parser.isNextTag("UNIT_LOGIC"))
        {
            readByte += parser.skipHeader();
            CLogic* logic = new CLogic(this);
            logic->attachMob(m_mob);
            readByte += logic->deserialize(parser);
            m_aLogic.append(logic);
        }
        else
        {
            uint baseByte = CWorldObj::deserialize(parser);
            if(baseByte > 0)
                readByte += baseByte;
            else
            {
                auto a = parser.nextTag();
                break;
            }
        }
        //"UNIT_R", eNull};
        //"UNIT_ITEMS", eNull};
    }
    Q_ASSERT(m_aLogic.size() == 5);

    return readByte;
}

CLogic::CLogic(CUnit* unit):
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
    ,m_use(false)
    ,m_mob(nullptr)
    ,m_parent(unit)
{
}

CLogic::~CLogic()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
    for(auto& pp: m_aPatrolPt)
        delete pp;
}

void CLogic::draw(QOpenGLShaderProgram* program)
{
    if(!m_use || m_aDrawPoint.empty())
        return;

    //todo: draw help radius
    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);

    program->setUniformValue("u_bUseColor", true);
    int offset(0);
    // Tell OpenGL which VBOs to use
    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

//    offset+=int(sizeof(QVector3D)); // size of vertex position
//    int normLocation = program->attributeLocation("a_normal");
//    program->enableAttributeArray(normLocation);
//    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

//    offset+=int(sizeof(QVector3D)); // size of normal
//    int textureLocation = program->attributeLocation("a_texture");
//    program->enableAttributeArray(textureLocation);
//    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_LINE_STRIP, m_aDrawPoint.count(), GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("u_bUseColor", false);
    //todo: choose guard behavior
    for(auto& pp : m_aPatrolPt)
        pp->draw(program);
}

void CLogic::drawSelect(QOpenGLShaderProgram* program)
{
    if(!m_use)
        return;

    //todo: choose guard behavior
    for(auto& pp : m_aPatrolPt)
        pp->drawSelect(program);
}

void CLogic::updatePointFigure(ei::CFigure* fig)
{
    for(auto& pp: m_aPatrolPt)
        pp->updateFigure(fig);
}

void CLogic::setPointTexture(QOpenGLTexture* pTexture)
{
    for(auto& pp: m_aPatrolPt)
        pp->setTexture(pTexture);
}

void CLogic::update()
{
    m_aDrawPoint.clear();
    switch (m_model) {
    case EBehaviourType::eRadius:
    {
        util::getCirclePoint(m_aDrawPoint, m_guardPlacement, double(m_guardRadius), 40);
        m_mob->view()->land()->projectPt(m_aDrawPoint);
        break;
    }
    case EBehaviourType::ePath:
    {
        Q_ASSERT(m_parent);
        QVector3D pos(m_parent->position());
        pos.setZ(0.0f);
        m_mob->view()->land()->projectPt(pos);
        m_aDrawPoint.append(pos);
        for (auto& pt: m_aPatrolPt)
        {
            pos = pt->position();
            pos.setZ(0.0f);
            m_mob->view()->land()->projectPt(pos);
            pt->setDrawPosition(pos);
            m_aDrawPoint.append(pos);
            pt->update();
        }
        util::splitByLen(m_aDrawPoint, 2.0f);
        break;
    }
    case EBehaviourType::ePlace:
    {
        return; //todo
        //break;
    }
    case EBehaviourType::eBriffing:
    {
        return;// todo
        //break;
    }
    default:
        Q_ASSERT("unknown behaviour type" && false);
        return; //break;
    }
    // todo: draw lines over the landscape

    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aDrawPoint.data(), m_aDrawPoint.count() * int(sizeof(QVector3D)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    for (ushort i(0); i<m_aDrawPoint.size(); ++i)
        aInd.append(i);

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

uint CLogic::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("UNIT_LOGIC_CYCLIC"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readBool(m_bCyclic);
        }
        else if(parser.isNextTag("UNIT_LOGIC_MODEL"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_model);
        }
        else if(parser.isNextTag("UNIT_LOGIC_GUARD_R"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_guardRadius);
        }
        else if(parser.isNextTag("UNIT_LOGIC_GUARD_PT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_guardPlacement);
        }
        else if(parser.isNextTag("UNIT_LOGIC_NALARM"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_numAlarm);
        }
        else if(parser.isNextTag("UNIT_LOGIC_USE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_use);
        }
        else if(parser.isNextTag("UNIT_LOGIC_WAIT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_wait);
        }
        else if(parser.isNextTag("UNIT_LOGIC_ALARM_CONDITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_alarmCondition);
        }
        else if(parser.isNextTag("UNIT_LOGIC_HELP"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readFloat(m_help);
        }
        else if(parser.isNextTag("UNIT_LOGIC_ALWAYS_ACTIVE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_alwaysActive);
        }
        else if(parser.isNextTag("UNIT_LOGIC_AGRESSION_MODE"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_agressionMode);
        }
        else if(parser.isNextTag("GUARD_PT"))
        {
            readByte += parser.skipHeader();
            CPatrolPoint* place = new CPatrolPoint(); //need unit parent?
            place->attachMob(m_mob);
            readByte += place->deserialize(parser);
            m_aPatrolPt.append(place);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    update();
    return readByte;
}

CPatrolPoint::CPatrolPoint():
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
    ,m_mob(nullptr)
{
    m_modelName = "ppoint.mod";
}

CPatrolPoint::CPatrolPoint(CNode* node):
    CObjectBase (node)
    ,m_indexBuf(QOpenGLBuffer::IndexBuffer)
    ,m_mob(nullptr)
{

}

CPatrolPoint::~CPatrolPoint()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
    for(auto& lp : m_aLookPt)
        delete lp;
}

void CPatrolPoint::draw(QOpenGLShaderProgram* program)
{
    CObjectBase::draw(program);

    if(m_aDrawingLine.size() == 0) // do not draw looking point if absent
        return;

    QMatrix4x4 matrix;
    matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", matrix);

    program->setUniformValue("u_bUseColor", true);
    int offset(0);
    // Tell OpenGL which VBOs to use

    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

//    offset+=int(sizeof(QVector3D)); // size of vertex position
//    int normLocation = program->attributeLocation("a_normal");
//    program->enableAttributeArray(normLocation);
//    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

//    offset+=int(sizeof(QVector3D)); // size of normal
//    int textureLocation = program->attributeLocation("a_texture");
//    program->enableAttributeArray(textureLocation);
//    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_LINES, (m_aDrawingLine.count()-1)*2, GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("u_bUseColor", false);

    for(auto& look: m_aLookPt)
        look->draw(program);
}

void CPatrolPoint::drawSelect(QOpenGLShaderProgram* program)
{
    CObjectBase::drawSelect(program);
    for(auto& look: m_aLookPt)
        look->drawSelect(program);
}

void CPatrolPoint::updateFigure(ei::CFigure* fig)
{
    CObjectBase::updateFigure(fig);
    for(auto& lp : m_aLookPt)
        lp->updateFigure(fig);
}

void CPatrolPoint::setTexture(QOpenGLTexture* texture)
{
    CObjectBase::setTexture(texture);
    for(auto& lp : m_aLookPt)
        lp->setTexture(texture);
}

void CPatrolPoint::update()
{
    m_aDrawingLine.clear();
    if(m_aLookPt.empty())
        return;

    QVector3D pos(position());
    pos.setZ(0.0f);
    m_aDrawingLine.append(pos); // self position
    for(auto& lp: m_aLookPt)
    {
        pos=lp->position();
        pos.setZ(0.0f);
        m_aDrawingLine.append(pos);   //drawing position?!
    }
    m_mob->view()->land()->projectPt(m_aDrawingLine);

    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aDrawingLine.data(), m_aDrawingLine.count() * int(sizeof(QVector3D)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    for(int i(0); i<m_aLookPt.size(); ++i)
    {
        aInd.append(0);
        aInd.append(ushort(i+1));
    }

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

uint CPatrolPoint::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("GUARD_PT_POSITION"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_position);
            m_drawPosition = m_position;
        }
        else if(parser.isNextTag("GUARD_PT_ACTION"))
        {
            Q_ASSERT("GUARD_PT_ACTION" && false);
        }
        else if(parser.isNextTag("ACTION_PT"))
        {
            readByte += parser.skipHeader();
            CLookPoint* act = new CLookPoint();
            readByte += act->deserialize(parser);
            m_aLookPt.append(act);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    return readByte;
}

CLookPoint::CLookPoint()
{
    m_modelName = "ppoint.mod";
}

CLookPoint::CLookPoint(CNode* node):
    CObjectBase(node)
{
    m_modelName = "ppoint.mod";
}

//void CLookPoint::draw(QOpenGLShaderProgram* program)
//{
//    CObjectBase::draw(program);
//}

//void CLookPoint::drawSelect(QOpenGLShaderProgram* program)
//{
//    CObjectBase::drawSelect(program);
//}

uint CLookPoint::deserialize(util::CMobParser& parser)
{
    uint readByte(0);
    while(true)
    {
        if(parser.isNextTag("ACTION_PT_LOOK_PT"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readPlot(m_position);
            m_drawPosition = m_position;
        }
        else if(parser.isNextTag("ACTION_PT_WAIT_SEG"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_wait);
        }
        else if(parser.isNextTag("ACTION_PT_TURN_SPEED"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readDword(m_turnSpeed);
        }
        else if(parser.isNextTag("ACTION_PT_FLAGS"))
        {
            readByte += parser.skipHeader();
            readByte += parser.readByte(m_flag);
        }
        else
        {
            auto a = parser.nextTag();
            break;
        }
    }
    return readByte;

}
