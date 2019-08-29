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

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    CNode::s_freeId = 0;
    m_settings.reset(new CSettings());
    m_settings->attachMainWindow(this);
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
    COptString* opt = dynamic_cast<COptString*>(m_settings->opt(eOptSetResource, "FigurePath_1"));
    if(opt->value().isEmpty())
    {
        QMessageBox::warning(this, "Warning","Choose path to figures.res");
        m_settings->onShow(eOptSetResource);
        return;
    }
    opt = dynamic_cast<COptString*>(m_settings->opt(eOptSetResource, "TexturePath_1"));
    if(opt->value().isEmpty())
    {
        QMessageBox::warning(this, "Warning","Choose path to textures.res");
        m_settings->onShow(eOptSetResource);
        return;
    }

    QString lastFolder = ("c:\\");
    opt = dynamic_cast<COptString*>(m_settings->opt(eOptSetGeneral, "LastVisitedFolder"));
    Q_ASSERT(opt);

    QFileInfo fileName;
    if(m_ui->myGLWidget->isLandLoaded())
        fileName = QFileDialog::getOpenFileName(this, "Open mob", opt->value(), tr("MOB (*.mob)"));
    else
        fileName = QFileDialog::getOpenFileName(this, "Open mpr", opt->value(), tr("MPR (*.mpr)"));

    opt->setValue(fileName.dir().path());

    if(fileName.fileName().contains(".mpr"))
    {
        QFileInfo texturePath(dynamic_cast<COptString*>(m_settings->opt(eOptSetResource, "TexturePath_1"))->value());
        m_ui->myGLWidget->texList()->initResourceFile(texturePath);
        m_ui->myGLWidget->loadLandscape(fileName);
    }
    else if(fileName.fileName().contains(".mob"))
    {
        QVector<QFileInfo> aFile;
        aFile.append(QFileInfo(dynamic_cast<COptString*>(m_settings->opt(eOptSetResource, "FigurePath_1"))->value()));
        m_ui->myGLWidget->objList()->initResourceFile(aFile);
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
