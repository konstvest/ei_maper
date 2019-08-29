#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"

CSettings::CSettings(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CSettings)
    ,m_fileOpt("options.json")
{
    ui->setupUi(this);
    m_aOptCategory.resize(eOptSetCount);
    readOptions();
}

CSettings::~CSettings()
{
    delete ui;
    saveOptions();
}

COpt* CSettings::opt(EOptSet optSet, QString& name)
{
    for(const auto& opt: m_aOptCategory[optSet])
    {
        if(opt->name() == name)
        {
            return opt.get();
        }
    }
    return nullptr;
}

COpt* CSettings::opt(EOptSet optSet, const char* name)
{
    QString optName(name);
    return opt(optSet, optName);
}

void CSettings::onShow(EOptSet optSet)
{
    for(auto& optCategory: m_aOptCategory)
    {
        for(auto& option: optCategory)
        {
            //todo: choose tab index to find option
            QLineEdit* le = ui->tabWidget->findChild<QLineEdit*>(option->name());
            if(le != nullptr)
            {
                le->setText(dynamic_cast<COptString*>(option.get())->value());
            }
        }
    }
    show();
    switch (optSet) {
    case eOptSetGeneral:
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabGeneral));
        break;
    case eOptSetRender:
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabRender));
        break;
    case eOptSetKeyBinding:
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabKeyBindings));
        break;
    case eOptSetResource:
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tabResource));
        break;
    default:
        Q_ASSERT("incorrect option set" && false);
        break;
    }
}

bool CSettings::onClose()
{
    m_mainWindow->setEnabled(true);
    return QWidget::close();
}

void CSettings::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);
    onClose();
}

bool CSettings::close()
{
    return onClose();
}

void CSettings::on_buttonCancel_clicked()
{
    close();
}

void CSettings::resetOptions()
{
    if(m_fileOpt.exists())
        m_fileOpt.remove();
}

void CSettings::initOptions()
{
    QJsonObject resource;
    resource.insert("FigurePath_1", QJsonValue::fromVariant(""));
    resource.insert("FigurePath_2", QJsonValue::fromVariant(""));
    resource.insert("TexturePath_1", QJsonValue::fromVariant(""));
    resource.insert("TexturePath_2", QJsonValue::fromVariant(""));
//    resource.insert("DatabasePath_1", QJsonValue::fromVariant(""));
//    resource.insert("DatabasePath_2", QJsonValue::fromVariant(""));
//    resource.insert("TextPath_1", QJsonValue::fromVariant(""));
//    resource.insert("TextPath_2", QJsonValue::fromVariant(""));

    QJsonObject general;
    general.insert("LastVisitedFolder", "");
    general.insert("opt1", false);
    general.insert("opt2", true);

    QJsonObject optSection;
    optSection.insert("Version", 0.1);
    optSection.insert("General", general);
    //todo: other options set
    optSection.insert("Resource", resource);
    QJsonDocument doc(optSection);
    if (!m_fileOpt.open(QIODevice::WriteOnly)) {
        Q_ASSERT("Couldn't open option file." && false);
        return;
    }
    m_fileOpt.write(doc.toJson());
    m_fileOpt.close();
}

void CSettings::updatePathOpt(const char* name, EOptSet optSet)
{
    COptString* val = dynamic_cast<COptString*>(opt(optSet, name));
    QLineEdit* le = ui->tabWidget->findChild<QLineEdit*>(name);
    if(le && val)
    {
        val->setValue(le->text());
    }
}

void CSettings::readOptions()
{
    if(!m_fileOpt.exists())
        initOptions();

    QString json;
    if(!m_fileOpt.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Q_ASSERT("can't open option file" && false);
        return;
    }
    json = m_fileOpt.readAll();
    m_fileOpt.close();


    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject optSection = doc.object();
    m_version = optSection.value(QString("Version")).toDouble(); //todo: check version

    // #general tab
    QJsonObject jsonObj = optSection.value(QString("General")).toObject();
    EOptSet section = eOptSetGeneral;
    auto readOptPath = [&section, &jsonObj](QVector<QVector<QSharedPointer<COpt>>>& optCategory, const char* name)
    {
        QString val = jsonObj[name].toString();
        optCategory[section].append(QSharedPointer<COpt>(new COptString(name, val)));
    };
    readOptPath(m_aOptCategory, "LastVisitedFolder");

    // #resource tab
    jsonObj = optSection.value(QString("Resource")).toObject();
    section = eOptSetResource;
    readOptPath(m_aOptCategory, "FigurePath_1" );
    readOptPath(m_aOptCategory, "FigurePath_2" );
    readOptPath(m_aOptCategory, "TexturePath_1");
    readOptPath(m_aOptCategory, "TexturePath_2");

}

void CSettings::saveOptions()
{
    QJsonObject resource;
    for(auto& option: m_aOptCategory[eOptSetResource])
    {
        resource.insert(option->name(), dynamic_cast<COptString*>(option.get())->value());
    }
//    resource.insert("FigurePath_1",   dynamic_cast<COptString*>(opt(eOptSetResource, "FigurePath_1"))->value());
//    resource.insert("FigurePath_2",   dynamic_cast<COptString*>(opt(eOptSetResource, "FigurePath_2"))->value());
//    resource.insert("TexturePath_1",  dynamic_cast<COptString*>(opt(eOptSetResource, "TexturePath_1"))->value());
//    resource.insert("TexturePath_2",  dynamic_cast<COptString*>(opt(eOptSetResource, "TexturePath_2"))->value());

//    resource.insert("DatabasePath_1", dynamic_cast<COptString*>(opt(eOptSetResource, "DatabasePath_1"))->value());
//    resource.insert("DatabasePath_2", dynamic_cast<COptString*>(opt(eOptSetResource, "DatabasePath_2"))->value());
//    resource.insert("TextPath_1",     dynamic_cast<COptString*>(opt(eOptSetResource, "TextPath_1"))->value());
//    resource.insert("TextPath_2",     dynamic_cast<COptString*>(opt(eOptSetResource, "TextPath_2"))->value());

    QJsonObject general;
    general.insert("LastVisitedFolder", dynamic_cast<COptString*>(opt(eOptSetGeneral, "LastVisitedFolder"))->value());

    QJsonObject optSection;
    optSection.insert("Version", m_version);
    optSection.insert("General", general);
    //todo: other options set
    optSection.insert("Resource", resource);
    QJsonDocument doc(optSection);
    if (!m_fileOpt.open(QIODevice::WriteOnly)) {
        Q_ASSERT("Couldn't open option file." && false);
        //todo: suggest choosing a folder to save
        return;
    }
    m_fileOpt.write(doc.toJson());
    m_fileOpt.close();
}

void CSettings::on_buttonApply_clicked()
{
    // #resource tab
    updatePathOpt("FigurePath_1", eOptSetResource);
    updatePathOpt("FigurePath_2", eOptSetResource);
    updatePathOpt("TexturePath_1", eOptSetResource);
    updatePathOpt("TexturePath_2", eOptSetResource);
    onClose();
}

void CSettings::on_FigurePath_1_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "LastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Open mob", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->FigurePath_1->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_FigurePath_2_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "LastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Open mob", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->FigurePath_2->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_TexturePath_1_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "LastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Open mob", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->TexturePath_1->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_TexturePath_2_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "LastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Open mob", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->TexturePath_2->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}
