#include <QMouseEvent>
#include "view.h"
#include "mob.h"
#include "node.h"
#include "key_manager.h"
#include "camera.h"

void CView::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::RightButton)
    {   // process object selection
        for(auto& mob: m_aMob)
        {
            if (CNode* node = pickObject(mob->node(), mob->nodeSelected(), event->pos().x(), event->pos().y()))
            {
                bool bFound = false;
                int ind(-1);
                for (auto& selNode: mob->nodeSelected())
                {
                    if ((selNode == node) && m_keyManager->isPressed(Qt::Key_Shift))
                    {   // deselect selected node if we press SHIFT
                        mob->node().append(selNode);
                        mob->nodeSelected().removeOne(selNode);
                        bFound = true;
                        break;
                    }
                    else if ((selNode != node) && !m_keyManager->isPressed(Qt::Key_Shift))
                    {   // deselect all nodes for single selection (without SHIFT)
                        mob->node().append(selNode);
                        mob->nodeSelected().removeOne(selNode);
                    }
                }
                if (!bFound && (ind = mob->node().indexOf(node)) > -1)
                {
                    mob->nodeSelected().append(mob->node().at(ind));
                    mob->node().removeAt(ind);
                }
            }
            else if (!m_keyManager->isPressed(Qt::Key_Shift)) // clear selection buffer if we click out of objects in single selection mode
                for (auto& selNode: mob->nodeSelected())
                {
                    mob->node().append(selNode);
                    mob->nodeSelected().removeOne(selNode);
                }
        }
    }
    m_lastPos = event->pos();
}

void CView::mouseMoveEvent(QMouseEvent* event)
{
    const int dx = event->x() - m_lastPos.x();
    const int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::MiddleButton)
    {
//        if (m_keyManager->isPressed(Qt::Key_Shift))
//        { // move side + updown
//            if (dx<0) m_cam->strafeRight();
//            if (dx>0) m_cam->strafeLeft();
//            if (dy>0) m_cam->strafeUp();
//            if (dy<0) m_cam->strafeDown();
//        }
//        else
        { // rotate around pivot
            float angle = float(dy)/2.5f;
            m_cam->xRotate(angle);
            angle = float(dx)/2.5f;
            m_cam->zRotate(angle);
        }
        m_lastPos = event->pos();
    }
}

void CView::wheelEvent(QWheelEvent* event)
{
    m_cam->enlarge(event->delta() > 0);
}

// draw objects without light and textures, only colored triangles; find suitable object
// x,y - mouse position
CNode* CView::pickObject(QList<CNode*>& aNode, QList<CNode*>& aNodeSelected, int x, int y)
{
    // Bind shader pipeline for use
    if (!m_selectProgram.bind())
        close();

    m_selectProgram.setUniformValue("u_projMmatrix", m_projection);
    m_selectProgram.setUniformValue("u_viewMmatrix", m_cam->update());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto node: aNode)
    {
        //glColor3ub(node->color()[0], node->color()[1], node->color()[2]);
        node->drawSelect(&m_selectProgram);
    }

    for (auto node: aNodeSelected)
    {
        //glColor3ub(node->color()[0], node->color()[1], node->color()[2]);
        node->drawSelect(&m_selectProgram);
    }

    GLubyte pixel[3];
    glReadPixels(x, m_height-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    SColor pickedColor(pixel[0], pixel[1], pixel[2]);

    for (auto& node: aNodeSelected)
    {
        if (node->isColorSuitable(pickedColor))
            return node;
    }
    for (auto& node: aNode)
    {
        if (node->isColorSuitable(pickedColor))
            return node;
    }
    return nullptr;
}
