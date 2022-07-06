#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDir>

#include <QClipboard>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonArray>

#include "view.h"
#include "key_manager.h"
#include "mob.h"
#include "camera.h"
#include "landscape.h"
#include "operationmanager.h"

void CView::keyPressEvent(QKeyEvent* event)
{
    QApplication* application = static_cast<QApplication *>(QApplication::instance());
    application->inputMethod()->reset();
    QLocale a = application->inputMethod()->locale();
    QString lang = a.languageToString(a.language());

//    if (a.language() != QLocale::English)
//        QMessageBox::warning(this, "Warning", "Please set locale to english");

    switch (event->key()) {
    case Qt::Key_M:
        unloadLand();
        break;
    case Qt::Key_U:
        unloadMob("");
        break;
    case Qt::Key_P:
    {
        CNode* pNode;
        for(auto& mob: m_aMob)
            foreach(pNode, mob->nodes())
            {
                if (pNode->nodeState() & ENodeState::eSelect)
                    m_landscape->projectPosition(pNode);
            }
        break;
    }
    case Qt::Key_C:
        m_cam->reset();
        break;
    case Qt::Key_O:
    {
        viewParameters();
        break;
    }
    case Qt::Key_I:
    {
        CNode* pNode;

        QJsonArray arrObj;
        for(auto& mob: m_aMob)
            foreach(pNode, mob->nodes())
                if (pNode->nodeState() & ENodeState::eSelect)
                    arrObj.append(pNode->toJson());

//                    QClipboard *clipboard = QGuiApplication::clipboard();
//                    clipboard->setText(QString(doc.toJson(QJsonDocument::JsonFormat::Indented)));
        QJsonDocument doc(arrObj);
        QFile clipboard_buffer_file(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("copy_paste_buffer.json"));
        if (!clipboard_buffer_file.open(QIODevice::WriteOnly))
        {
            Q_ASSERT("Couldn't open option file." && false);
            break;
        }
        clipboard_buffer_file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
        break;
    }
    default:
    {
        m_pOp->keyPress(event);
        break;
    }
    }
}

void CView::keyReleaseEvent(QKeyEvent* event)
{
    m_pOp->keyRelease(event);
}
