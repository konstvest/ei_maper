#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include "resfile.h"
#include <string.h>
#include "scene.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect (&timerProperty, SIGNAL(timeout()), this, SLOT(updateProperty()));
    //timerProperty.start(100);

    m_scene = new CScene;
    m_scene->setName(QString("init scene"));
    this->ui->widget->setScene(m_scene);
}

MainWindow::~MainWindow()
{
    delete m_scene;
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()  //"brick" -> load res file
{
    m_scene->setName(QString("button scene"));
    m_scene->loadResFile(QString("f:\\temp\\figures.res"));
    //m_scene->loadResFile(QString("f:\\temp\\menus.res"));

}

void MainWindow::on_pushButton_3_clicked()
{
//    this->ui->widget->name = "big cube";
//    this->ui->widget->width = 2;
//    this->ui->widget->length = 2;
//    this->ui->widget->height = 2;
//    this->ui->widget->update();
//    this->ui->horizontalSlider->setVisible(false);
//    this->ui->horizontalSlider_2->setVisible(false);
//    this->ui->horizontalSlider_3->setVisible(false);
}

void MainWindow::on_pushButton_4_clicked()
{
//    this->ui->widget->name = "column";
//    this->ui->widget->width = 0.3;
//    this->ui->widget->length = 0.3;
//    this->ui->widget->height = 1.4;
//    this->ui->widget->update();
//    this->ui->horizontalSlider->setVisible(false);
//    this->ui->horizontalSlider_2->setVisible(false);
//    this->ui->horizontalSlider_3->setVisible(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    QMessageBox msg;
    msg.setText("Input\nRight Mouse Button - rotate object\n"
                "Mouse wheel - zoom object\n"
                "W,S - move depth camera\n"
                "A,D - move side camera\n"
                "Q,E - move height camera\n\n"
                "Note: check keyboard layout and click on viewport if key buttons don't work");
    msg.exec();
}

void MainWindow::updateProperty(){
//    this->ui->label->setText("Object "+this->ui->widget->name+
//                                " has "+QString::number(this->ui->widget->triangles)+" triangle(s).\n"
//                             "Property: length="+QString::number(this->ui->widget->length)+
//                                " width="+QString::number(this->ui->widget->width)+
//                                " height="+QString::number(this->ui->widget->height)+"." );
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
//    this->ui->widget->width = (float)value/10;
//    this->ui->widget->update();
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
//    this->ui->widget->length = (float)value/10;
//    this->ui->widget->update();
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
//    this->ui->widget->height = (float)value/10;
//    this->ui->widget->update();
}

void MainWindow::on_pushButton_6_clicked()
{
//    this->ui->widget->name = "custom box";
//    this->ui->horizontalSlider->setVisible(true);
//    this->ui->horizontalSlider_2->setVisible(true);
//    this->ui->horizontalSlider_3->setVisible(true);
//    this->ui->widget->width = (float)this->ui->horizontalSlider->value()/10;
//    this->ui->widget->length = (float)this->ui->horizontalSlider_2->value()/10;
//    this->ui->widget->height = (float)this->ui->horizontalSlider_3->value()/10;
//    this->ui->widget->update();
}


void MainWindow::on_pushButton_7_clicked()
{
//    QFileDialog fldlg;
//    fldlg.setFileMode(QFileDialog::AnyFile);
//    figure figa;
//    figa.loadFromFile(fldlg.getOpenFileName(this, tr("Open File"), "f:\\temp\\",
//                                           tr("Figure file (*.fig);;"
//                                              "Link file (*.lnk);;"
//                                              "Bone file (*.bon)")
//                                               ));
//    figa.recalcConstitution(0,0,0);
//    figa.convertToGLIndices();
//    this->ui->widget->fig.push_back(figa);


    //this->ui->widget->open_file = true;



}

void MainWindow::on_btnLoadmp_clicked()
{
//    QFileDialog fldlg;
//        fldlg.setFileMode(QFileDialog::AnyFile);
//        figure figa;
//        figa.loadFromFile(fldlg.getOpenFileName(this, tr("Open File"), "f:\\temp\\",
//                                               tr("Map header file (*.fig);;"
//                                                  "All files (*.*)")
//                                                   ));
    //QString path = "";
}
