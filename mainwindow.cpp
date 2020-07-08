#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWidget>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "objectlist.h"
#include "texturelist.h"
#include "node.h"
#include "settings.h"
#include "mob.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    CNode::s_freeId = 0;
    m_settings.reset(new CSettings());
    m_settings->attachMainWindow(this);
    m_ui->myGLWidget->attachLogWindow(m_ui->logWindow);
    m_ui->myGLWidget->attachSettings(m_settings.get());
    QObject::connect(m_ui->myGLWidget, SIGNAL(updateMsg(QString)), m_ui->logWindow, SLOT(log(QString)));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(event);
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
        fileName = QFileDialog::getOpenFileName(this, "Open mpr", opt->value(), tr("MPR (*.mpr)"));

    if(!fileName.path().isEmpty())
        opt->setValue(fileName.dir().path());

    if(fileName.fileName().contains(".mpr"))
    {
        m_ui->myGLWidget->loadLandscape(fileName);
    }
    else if(fileName.fileName().contains(".mob"))
    {
        if(!m_ui->myGLWidget->isLandLoaded())
        {
            QMessageBox::warning(this, "Warning","Landscape must be loaded (*.mpr)");
            return;
        }

        QVector<QFileInfo> aFile;
        aFile.append(QFileInfo(dynamic_cast<COptString*>(m_settings->opt(eOptSetResource, "figPath1"))->value()));
        m_ui->myGLWidget->objList()->addResourceFile(aFile);
        m_ui->myGLWidget->loadMob(fileName);
    }
}

void MainWindow::on_pushButton_clicked()
{
    statusBar()->showMessage(tr("status bar here"));
}

void MainWindow::on_actionSettings_triggered()
{
    m_settings->onShow(eOptSetResource);
    setEnabled(false);
}

void MainWindow::on_actionSave_as_triggered()
{
    QFileInfo fileName;
    fileName = QFileDialog::getSaveFileName(this, "Save as... ", "" , tr("Map landscape(*.mpr);;Map objects (*.mob);;Mob as JSON(*.json)"));
    if (fileName.fileName().endsWith(".json"))
    {
        m_ui->myGLWidget->serializeMob(fileName);
    }
}

