#include "main_window.h"
#include "ui_main_window.h"

#include <QWidget>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QUndoView>

#include "resourcemanager.h"
#include "node.h"
#include "settings.h"
#include "select_window.h"
#include "createobjectform.h"
#include "mob.h"
#include "undo.h"
#include "mobparameters.h"
#include "ui_connectors.h"
#include "log.h"


#include <QImageReader>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    CIconManager::getInstance()->init();

    m_settings.reset(new CSettings());
    m_selector.reset(new CSelector());
    m_mobParams.reset(new CMobParameters());
    m_createDialog.reset(new CCreateObjectForm());
    m_settings->attachMainWindow(this);
    m_ui->setupUi(this); //init CView core also

#ifndef QT_DEBUG
  m_ui->toolButton_2->hide();
#endif

    m_selector->attachParents(this, m_ui->myGLWidget);
    m_undoStack = new QUndoStack(this);
    createUndoView();
    CStatusConnector::getInstance()->attach(m_ui->statusIco, m_ui->statusBar);
    connectUiButtons();
    m_ui->myGLWidget->attach(m_settings.get(), m_ui->tableWidget, m_undoStack, m_ui->progressBar, m_ui->mousePosText);
    initShortcuts();
    QObject::connect(m_ui->myGLWidget, SIGNAL(mobLoad(bool)), this, SLOT(updateMobListInParam(bool)));
//    m_ui->progressBar->setValue(0);
    m_ui->progressBar->reset();
//    m_ui->progressBar->setVisible(false);

    m_ui->mousePosText->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
    CTextureList::getInstance()->attachSettings(m_settings.get());
    CObjectList::getInstance()->attachSettings(m_settings.get());
}

MainWindow::~MainWindow()
{
    delete m_ui;
    CIconManager::getInstance()->~CIconManager();
}

void MainWindow::createUndoView()
{
    m_undoView = new QUndoView(m_undoStack);
    m_undoView->setWindowTitle(tr("History"));
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MainWindow::initShortcuts()
{
    m_ui->actionSelect_by->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    m_ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    m_ui->actionSave_as->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    m_ui->actionSelect_All->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    m_ui->actionClose_all->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    m_ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    m_ui->actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    m_ui->actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

}

void MainWindow::connectUiButtons()
{
    CButtonConnector::getInstance()->attach(m_ui->myGLWidget);
    CButtonConnector::getInstance()->addButton(EButtonOpSelect, m_ui->selectButton);
    m_ui->selectButton->setEnabled(false);
    CButtonConnector::getInstance()->addButton(EButtonOpMove, m_ui->moveButton);
    m_ui->moveButton->setEnabled(false);
    CButtonConnector::getInstance()->addButton(EButtonOpRotate, m_ui->rotateButton);
    m_ui->rotateButton->setEnabled(false);
    CButtonConnector::getInstance()->addButton(EButtonOpScale, m_ui->scaleButton);
    m_ui->scaleButton->setEnabled(false);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionOpen_triggered()
{
    COptString* opt = nullptr;

    opt = dynamic_cast<COptString*>(m_settings->opt(eOptSetGeneral, "lastVisitedFolder"));
    Q_ASSERT(opt);

    QFileInfo fileName;
    if(m_ui->myGLWidget->isLandLoaded())
        fileName = QFileDialog::getOpenFileName(this, "Open mob", opt->value(), tr("MOB (*.mob)"));
    else
    {
        fileName = QFileDialog::getOpenFileName(this, "Open mpr", opt->value(), tr("MPR (*.mpr)"));
    }

    if(!fileName.path().isEmpty())
        opt->setValue(fileName.dir().path());

    if(fileName.fileName().toLower().endsWith(".mpr"))
    {
        //m_ui->myGLWidget->loadLandscape(fileName);
        QUndoCommand* loadMpr = new COpenCommand(m_ui->myGLWidget, fileName, this);
        m_undoStack->push(loadMpr);
    }
    else if(fileName.fileName().toLower().endsWith(".mob"))
    {
        if(!m_ui->myGLWidget->isLandLoaded())
        {
            QMessageBox::warning(this, "Warning","Landscape must be loaded (*.mpr)");
            return;
        }

        QUndoCommand* loadMob = new COpenCommand(m_ui->myGLWidget, fileName, this);
        m_undoStack->push(loadMob);
    }
}

void MainWindow::on_actionSettings_triggered()
{
    m_settings->onShow(eOptSetResource);
    setEnabled(false);
}

void MainWindow::on_actionSave_as_triggered()
{
    m_ui->myGLWidget->saveMobAs();
}


void MainWindow::on_actionSelect_All_triggered()
{
    m_selector->selectAll();
}

void MainWindow::on_actionSelect_by_triggered()
{
    m_selector->show();
}

void MainWindow::on_actionShow_undo_redo_triggered()
{
    m_undoView->show();
}

void MainWindow::on_actionClose_all_triggered()
{
    m_ui->myGLWidget->unloadMob("");
    m_ui->myGLWidget->unloadLand();
}

void MainWindow::on_actionSave_triggered()
{
    m_ui->myGLWidget->saveAllMob();
}

void MainWindow::on_action_Mob_parameters_triggered()
{
    updateMobListInParam(false);
    m_mobParams->show();
}

void MainWindow::updateMobListInParam(bool bReset)
{
    if(bReset)
        m_mobParams->reset();
    m_mobParams->initMobList(m_ui->myGLWidget->mobs());
}

void MainWindow::on_actionUndo_triggered()
{
    m_undoStack->undo();
}

void MainWindow::on_toolButton_2_clicked()
{
    ei::log(eLogDebug, "btn test start");
    m_createDialog->show();
    ei::log(eLogDebug, "btn test end");
}

void MainWindow::on_selectButton_clicked()
{
    qDebug() << "hello Select";
    CButtonConnector::getInstance()->clickButton(EButtonOpSelect);
}

void MainWindow::on_moveButton_clicked()
{
    qDebug() << "TODO: insert gizmo to Move";
    CButtonConnector::getInstance()->clickButton(EButtonOpMove);
}

void MainWindow::on_actionRedo_triggered()
{
    m_undoStack->redo();
}

