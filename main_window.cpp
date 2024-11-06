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
#include "landscape.h"
#include "node.h"
#include "settings.h"
#include "select_window.h"
#include "createobjectform.h"
#include "randomize_form.h"
#include "undo.h"
#include "mob\mob.h"
#include "mob\mob_parameters.h"
#include "layout_components/connectors_ui.h"
#include "preview.h"
#include "log.h"
#include "scene.h"

void testFunc()
{
    ei::log(eLogDebug, "test func start");
    //auto aaa = CResourceManager::getInstance()->tempFolder();
    ei::log(eLogDebug, "test func end");
}


void MainWindow::on_toolButton_2_clicked()
{
    ei::log(eLogDebug, "btn test start");

    ei::log(eLogDebug, "btn test end");
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
#ifdef QT_DEBUG
    testFunc();
#endif
    CIconManager::getInstance()->init();

    m_settings.reset(new CSettings());
    m_selector.reset(new CSelectForm(this));
    m_createDialog.reset(new CCreateObjectForm());
    m_randomizeForm.reset(new CRandomizeForm());
    m_settings->attachMainWindow(this);
    m_ui->setupUi(this); //init CView core also

    m_pView = new CView(m_ui->centralWidget, m_createDialog.get()->viewWidget());
    m_createDialog->viewWidget()->attachView(m_pView);
    m_createDialog->viewWidget()->attachSettings(m_settings.get());

    m_pView->setObjectName(QString::fromUtf8("myGLWidget"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_pView->sizePolicy().hasHeightForWidth());
    m_pView->setSizePolicy(sizePolicy);
    m_pView->setMinimumSize(QSize(200, 200));
    m_pView->setMouseTracking(true);
    m_ui->workSpaceLayout->addWidget(m_pView);

#ifndef QT_DEBUG
  m_ui->toolButton_2->hide();
#endif

    m_selector->attachParents(m_pView);
    m_randomizeForm->attachView(m_pView);
    m_undoStack = new QUndoStack(this);
    createUndoView();
    CStatusConnector::getInstance()->attach(m_ui->statusIco, m_ui->statusBar);
    connectUiButtons();
    m_pView->attach(m_settings.get(), m_ui->tableWidget, m_undoStack, m_ui->progressBar, m_ui->mousePosText, m_ui->treeWidget);
    initShortcuts();
    QObject::connect(m_pView, SIGNAL(updateMainWindowTitle(eTitleTypeData,QString)), this, SLOT(updateWindowTitle(eTitleTypeData,QString)));
    QObject::connect(CScene::getInstance(), SIGNAL(modeChanged()), m_pView, SLOT(viewParameters()));
    QObject::connect(m_pView, SIGNAL(showOutlinerSignal(bool)), this, SLOT(showOutliner(bool)));


//    m_ui->progressBar->setValue(0);
    m_ui->progressBar->reset();
//    m_ui->progressBar->setVisible(false);

    m_ui->mousePosText->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
    CTextureList::getInstance()->attachSettings(m_settings.get());
    CObjectList::getInstance()->attachSettings(m_settings.get());
    m_createDialog.get()->attach(m_pView, m_undoStack);
//    if(m_pView->isRecentAvailable())
//        m_ui->actionOpen_recent->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete m_pView;
    delete m_ui;
    CIconManager::getInstance()->~CIconManager();
}

void MainWindow::testFunc1()
{
    ei::log(eLogDebug, "test func 1 started");
    QMap<QString, QString> mapName;
    QFile inputFile("c:\\konst\\unit_rename.txt");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList list = line.split(";");
          mapName.insert(list[0], list[1]);
       }
       inputFile.close();
    }


    QDir mobFolder("c:\\konst\\Проклятые Земли (Дополнение)\\Mods\\ferneo_mod\\Maps");
    QFileInfo land(mobFolder.absolutePath() + "\\zone1g.mpr");
    m_pView->loadLandscape(land);
    QStringList mobFilter;
    mobFilter << "*.mob";
    QFileInfoList mobFiles = mobFolder.entryInfoList(mobFilter);
    int n=0;
    foreach(QFileInfo filename, mobFiles) {
        m_pView->loadMob(filename);
        m_pView->roundActiveMob();
        n+=m_pView->renameActiveMobUnits(mapName);
        m_pView->saveActiveMob();
        m_pView->unloadActiveMob();
    }
    ei::log(eLogInfo, "renamed:" + QString::number(n) + "units");
    ei::log(eLogDebug, "test func 1 ended");
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

    m_ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + eKey_S));
    //m_ui->actionSave_all_MOB_s->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + eKey_S));
    //m_ui->actionSave_as->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + eKey_S));
    m_ui->actionSave_landscape_MPR->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + eKey_S));

    m_ui->actionSelect_All->setShortcut(QKeySequence(Qt::CTRL + eKey_A));
    m_ui->actionClose_all->setShortcut(QKeySequence(Qt::CTRL + eKey_Q));
    m_ui->actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    m_ui->actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    m_ui->actionCreate_new_object->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    m_ui->actionReset_cam_position->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_ui->actionChange_mod_e->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    //m_ui->actionSwitch_active_MOB->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    m_ui->action_Mob_parameters->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    m_ui->actionReset_logic_paths->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
}

void MainWindow::connectUiButtons()
{
    CButtonConnector::getInstance()->attach(m_pView);
    CButtonConnector::getInstance()->addButton(EButtonOpSelect, m_ui->selectButton);
    CButtonConnector::getInstance()->addButton(EButtonOpMove, m_ui->moveButton);
    CButtonConnector::getInstance()->addButton(EButtonOpRotate, m_ui->rotateButton);
    CButtonConnector::getInstance()->addButton(EButtonOpScale, m_ui->scaleButton);
    CButtonConnector::getInstance()->addButton(EButtonOpTilebrush, m_ui->tileBrushButton);
    //disable buttons bcs operations works bad for mouse'moove' action without start point. op's starts work frommouse button position
    m_ui->selectButton->setEnabled(false);
    m_ui->moveButton->setEnabled(false);
    m_ui->rotateButton->setEnabled(false);
    m_ui->scaleButton->setEnabled(false);
    m_ui->tileBrushButton->setEnabled(false);
}

bool MainWindow::isExitAllowed()
{
    CMob* pMob = nullptr;
    bool bCloseAllowed = true;
    foreach(pMob, m_pView->mobs())
    {
        if(!pMob->isDurty())
            continue;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit", pMob->mobName() + " has unsaved changes.\nDo you want to save changes?", QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Save)
            pMob->save();
        else if(reply == QMessageBox::Cancel)
        {
            bCloseAllowed = false;
            break;
        }
    }
    return bCloseAllowed;
}

void MainWindow::closeAll()
{
    if(isExitAllowed())
    {
        m_pView->unloadMob("");
        m_pView->unloadLand();
        m_undoStack->clear();
    }
}

void MainWindow::showOutliner(bool bShow)
{
    if(bShow)
    {
        m_ui->treeWidget->show();
        m_ui->tableWidget->show();
    }
    else
    {
        m_ui->treeWidget->hide();
        m_ui->tableWidget->hide();
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(isExitAllowed())
        close();
}

void MainWindow::on_actionOpen_triggered()
{
    COptString* opt = dynamic_cast<COptString*>(m_settings->opt(eOptSetGeneral, "lastVisitedFolder"));
    Q_ASSERT(opt);

    QFileInfo fileName;
    if(CLandscape::getInstance()->isMprLoad())
        fileName = QFileDialog::getOpenFileName(this, "Open MOB", opt->value(), tr("MOB (*.mob)"));
    else
    {
        fileName = QFileDialog::getOpenFileName(this, "Open MPR", opt->value(), tr("MPR (*.mpr)"));
    }

    if(!fileName.path().isEmpty())
        opt->setValue(fileName.dir().path());

    if(fileName.fileName().toLower().endsWith(".mpr"))
    {
        QUndoCommand* loadMpr = new COpenCommand(m_pView, fileName);
        m_undoStack->push(loadMpr);
        m_pView->saveRecent();
    }
    else if(fileName.fileName().toLower().endsWith(".mob"))
    {
        if(!CLandscape::getInstance()->isMprLoad())
        {
            QMessageBox::warning(this, "Warning","Landscape must be loaded first (*.mpr)");
            return;
        }

        COpenCommand* pLoadCommand = new COpenCommand(m_pView, fileName);
        m_undoStack->push(pLoadCommand);
        CRoundMobCommand* pRound = new CRoundMobCommand(m_pView);
        m_undoStack->push(pRound);
        m_pView->saveRecent();
    }
}

void MainWindow::on_actionSettings_triggered()
{

}

void MainWindow::on_actionSave_as_triggered()
{
    m_pView->saveMobAs();
}


void MainWindow::on_actionSelect_All_triggered()
{
    m_selector->selectAll();
}

void MainWindow::on_actionSelect_by_triggered()
{
    m_selector->show();
    m_selector->activateWindow();
}

void MainWindow::on_actionShow_undo_redo_triggered()
{
    m_undoView->show();
    m_undoView->activateWindow();
}

void MainWindow::on_actionClose_all_triggered()
{
    closeAll();
}

void MainWindow::on_actionSave_triggered()
{
    m_pView->saveActiveMob();
}

void MainWindow::on_action_Mob_parameters_triggered()
{
    m_pView->openActiveMobEditParams();
}


void MainWindow::on_actionUndo_triggered()
{
    m_undoStack->undo();
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


void MainWindow::on_actionCreate_new_object_triggered()
{
    m_createDialog->show();
    m_createDialog->activateWindow();
}

void MainWindow::updateWindowTitle(eTitleTypeData type, QString data)
{
    switch (type) {
        case eTitleTypeData::eTitleTypeDataActiveMob:
    {
        m_sWindowTitle.activeMob = data;
        break;
    }
    case eTitleTypeData::eTitleTypeDataMpr:
    {
        m_sWindowTitle.mpr = data;
        break;
    }
    case eTitleTypeData::eTitleTypeDataDurtyFlag:
    {
        m_sWindowTitle.durty = !data.isEmpty();
        break;
    }
    default:
        Q_ASSERT(false);
        break;
    }
    QString title = "ei_maper";
    if(!m_sWindowTitle.mpr.isEmpty())
        title += QString(" MPR: (%1)").arg(m_sWindowTitle.mpr);

    if(!m_sWindowTitle.activeMob.isEmpty())
    {
        title += QString(" Active MOB: (%1)").arg(m_sWindowTitle.activeMob);
        if(m_sWindowTitle.durty) //show durty flag only if mob is loaded. if it will be possible to edit mpr, move flag outside this block
            title += " *";
    }


    setWindowTitle(title);
}


void MainWindow::on_actionSave_all_MOB_s_triggered()
{
    m_pView->saveAllMob();
}


void MainWindow::on_rotateButton_clicked()
{
    CButtonConnector::getInstance()->clickButton(EButtonOpRotate);
}


void MainWindow::on_scaleButton_clicked()
{
    CButtonConnector::getInstance()->clickButton(EButtonOpScale);
}


void MainWindow::on_actionReset_cam_position_triggered()
{
    m_pView->resetCamPosition();
}


void MainWindow::on_actionChange_mod_e_triggered()
{
    CScene::getInstance()->changeMode(CScene::getInstance()->getMode() == eEditModeObjects ? eEditModeLogic : eEditModeObjects);
}


void MainWindow::on_actionCopy_IDs_to_clipboard_triggered()
{
    m_pView->copySelectedIDsToClipboard();
}


void MainWindow::on_actionOpen_2_triggered()
{
    m_settings->onShow(eOptSetResource);
    setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(isExitAllowed())
    {
        //m_pView->~CView();
        close();
    }
    else
        e->ignore();
}

void MainWindow::on_actionOpen_recent_triggered()
{
    closeAll();
    m_pView->openRecent();
}


void MainWindow::on_actionReset_logic_paths_triggered()
{
    m_pView->resetUnitLogicPaths();
}


void MainWindow::on_actionRandomize_parameter_triggered()
{
    m_randomizeForm->show();
    m_randomizeForm->activateWindow();
}


void MainWindow::on_actionReset_selected_IDs_triggered()
{
    m_pView->resetSelectedId();
}


void MainWindow::on_action_About_triggered()
{
    QMessageBox::information(this, "About","Map editor for Evil Islands game v0.3\n\nAuthor: konstvest\nHome page: https://github.com/konstvest/ei_maper\nLicense: GNU GPL-3.0");
}


void MainWindow::on_actionSave_landscape_MPR_as_triggered()
{
    m_pView->saveLandAs();
}


void MainWindow::on_actionMap_parameters_triggered()
{
    m_pView->openMapParameters();
}


void MainWindow::on_tileBrushButton_clicked()
{
    CButtonConnector::getInstance()->clickButton(EButtonOpTilebrush);
}


void MainWindow::on_actionSave_landscape_MPR_triggered()
{
    m_pView->saveLand();
}

