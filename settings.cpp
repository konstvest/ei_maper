#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QCheckBox>

#include "settings.h"
#include "ui_settings.h"
#include "main_window.h"
#include "utils.h"

CSettings::CSettings(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CSettings)
    ,m_fileOpt("options.json")
{
    ui->setupUi(this);
    m_aOptCategory.resize(eOptSetCount);
    initOptions();
    readOptions();
}

CSettings::~CSettings()
{
    delete ui;
    saveOptions(); //todo:what reason for saving options here? last folder path?
}

COpt* CSettings::opt(QString name)
{
    for(auto& cat : m_aOptCategory)
        for (auto& opt: cat)
        {
            if (opt->name() == name)
                return opt.get();
        }
    return nullptr;
}

COpt* CSettings::opt(EOptSet optSet, QString& name)
{
    for(const auto& opt: m_aOptCategory[optSet])
        if(opt->name() == name)
            return opt.get();

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
    saveOptions();
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
    updateOptUi();
    close();
}

void CSettings::resetOptions()
{
    if(m_fileOpt.exists())
        m_fileOpt.remove();
}

void CSettings::updateOptUi()
{
    for (auto& cat: m_aOptCategory)
    {
        for(const auto& opt: cat)
        {
            if (COptString* pOpt = dynamic_cast<COptString*>(opt.get()))
            {
                //find label, text edit
                QLineEdit* pLineEdit = ui->tabWidget->findChild<QLineEdit*>(pOpt->name());
                if (pLineEdit)
                {
                    pLineEdit->setText(pOpt->value());
                }
            }
            else if(COptBool* pOpt = dynamic_cast<COptBool*>(opt.get()))
            {
                // find checkbox
                QCheckBox* pCheckBox = ui->tabWidget->findChild<QCheckBox*>(pOpt->name());
                if (pCheckBox)
                    pCheckBox->setChecked(pOpt->value());
            }
            else if(COptDouble* pOpt = dynamic_cast<COptDouble*>(opt.get()))
            {
                //todo
                Q_UNUSED(pOpt);
            }
            else if(COptInt* pOpt = dynamic_cast<COptInt*>(opt.get()))
            {
                QSlider* pSlider = ui->tabWidget->findChild<QSlider*>(pOpt->name());
                if (pSlider)
                    pSlider->setValue(pOpt->value());
            }
        }
    }
}

void CSettings::updateOptFromUi()
{
    for (auto& cat: m_aOptCategory)
    {
        for(const auto& opt: cat)
        {
            if (COptString* pOpt = dynamic_cast<COptString*>(opt.get()))
            {
                //find label, text edit
                QLineEdit* pLineEdit = ui->tabWidget->findChild<QLineEdit*>(pOpt->name());
                if (pLineEdit)
                {
                    pOpt->setValue(pLineEdit->text());
                }
            }
            else if(COptBool* pOpt = dynamic_cast<COptBool*>(opt.get()))
            {
                // find checkbox
                QCheckBox* pCheckBox = ui->tabWidget->findChild<QCheckBox*>(pOpt->name());
                if (pCheckBox)
                    pOpt->setValue(pCheckBox->isChecked());
            }
            else if(COptDouble* pOpt = dynamic_cast<COptDouble*>(opt.get()))
            {
                //todo
                Q_UNUSED(pOpt);
            }
            else if(COptInt* pOpt = dynamic_cast<COptInt*>(opt.get()))
            {
                QSlider* pSlider = ui->tabWidget->findChild<QSlider*>(pOpt->name());
                if (pSlider)
                    pOpt->setValue(pSlider->value());
            }
        }
    }
}

void CSettings::initOptions()
{
    QList<QSharedPointer<COpt>> aOpt;

    //init default options for strings
    aOpt.append(QSharedPointer<COpt>(new COptString("figPath1", "")));
    aOpt.append(QSharedPointer<COpt>(new COptString("figPath2", "")));
    aOpt.append(QSharedPointer<COpt>(new COptString("texPath1", "")));
    aOpt.append(QSharedPointer<COpt>(new COptString("texPath2", "")));
    aOpt.append(QSharedPointer<COpt>(new COptString("lastVisitedFolder", "")));

    //init default options for bools
    aOpt.append(QSharedPointer<COpt>(new COptBool("drawLogic", true)));
    aOpt.append(QSharedPointer<COpt>(new COptBool("drawWater", true)));
    aOpt.append(QSharedPointer<COpt>(new COptBool("freeCamera", false)));
    aOpt.append(QSharedPointer<COpt>(new COptBool("saveSelectedOnly", false)));
    aOpt.append(QSharedPointer<COpt>(new COptBool("dipEditSymmetric", true)));
    aOpt.append(QSharedPointer<COpt>(new COptBool("detailedLog", false)));


    //init default options for digits
    aOpt.append(QSharedPointer<COpt>(new COptDouble("version", 1.0)));
    aOpt.append(QSharedPointer<COpt>(new COptInt("mouseSenseX", 25)));
    aOpt.append(QSharedPointer<COpt>(new COptInt("mouseSenseY", 25)));

    //split options into different category
    QFile inputFile(":/optSet.txt");
    QStringList aOptCategoryList;
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          aOptCategoryList.append(line);
       }
       inputFile.close();
    }

    EOptSet set = eOptSetGeneral;
    for (auto& line : aOptCategoryList)
    {
        if (line.isEmpty())
            continue;

        if (line.contains("Option set name"))
        {
            QString section = line.split(":").last();
            section.remove(QRegExp("[\\n\\t\\r]"));
            if (section == "General")
                set = eOptSetGeneral;
            else if (section == "Render")
                set = eOptSetRender;
            else if (section == "Resource")
                set = eOptSetResource;
            continue;
        }

        QString optName(line);
        optName.remove(QRegExp("[\\n\\t\\r]"));
        for (auto& opt : aOpt)
        {
            if (opt.get()->name() == optName)
            {
                m_aOptCategory[set].append(opt);
                break;
            }
        }

    }
}

void CSettings::updatePathOpt(const char* name, EOptSet optSet)
{
    COptString* val = dynamic_cast<COptString*>(opt(optSet, name));
    QLineEdit* le = ui->tabWidget->findChild<QLineEdit*>(name);
    if(le && val)
    {
        val->setValue(le->text());
    }
    COptBool* valB = dynamic_cast<COptBool*>(opt(optSet, name));
    QCheckBox* chB = ui->tabWidget->findChild<QCheckBox*>(name);
    if(chB && valB)
    {
        valB->setValue(chB->isChecked());
    }
}

void CSettings::readOptions()
{
    if(!m_fileOpt.exists())
    {
        updateOptUi();
        return;
    }

    QString json;
    if(!m_fileOpt.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Q_ASSERT("can't open option file" && false);
        return;
    }
    json = m_fileOpt.readAll();
    m_fileOpt.close();


    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject aOptObject = doc.object();


    auto readOptionTab = [&aOptObject, this](QString tabName, EOptSet optSet)
    {
        QJsonValue val = aOptObject.value(tabName);
        if (val.isObject())
        {
            QJsonObject obj =  val.toObject(); // General
            for(auto& opt : m_aOptCategory[optSet])
            {
                QJsonValue jVal = obj.value(opt.get()->name()); // figPath1
                if (jVal == QJsonValue::Null || jVal.isUndefined())
                    continue;

                if (COptString* pOpt = dynamic_cast<COptString*>(opt.get()))
                {
                    pOpt->setValue(jVal.toString());
                }
                else if(COptBool* pOpt = dynamic_cast<COptBool*>(opt.get()))
                {
                    pOpt->setValue(jVal.toBool());
                }
                else if(COptDouble* pOpt = dynamic_cast<COptDouble*>(opt.get()))
                {
                    pOpt->setValue(jVal.toDouble());
                }
                else if(COptInt* pOpt = dynamic_cast<COptInt*>(opt.get()))
                {
                    pOpt->setValue(jVal.toInt());
                }
            }
        }
    };
    readOptionTab("General", eOptSetGeneral);
    readOptionTab("Render", eOptSetRender);
    readOptionTab("Resource", eOptSetResource);
    readOptionTab("KeyBinding", eOptSetKeyBinding);

    updateOptUi();
}

void CSettings::saveOptions()
{
    QJsonObject aOpt;

    auto optToObj = [&aOpt, this](QString tabName, EOptSet optSet)
    {
        QJsonObject tabObj;
        for(auto& opt : m_aOptCategory[optSet])
        {

            if (COptString* pOpt = dynamic_cast<COptString*>(opt.get()))
            {
                tabObj.insert(opt.get()->name(), pOpt->value());
            }
            else if(COptBool* pOpt = dynamic_cast<COptBool*>(opt.get()))
            {
                tabObj.insert(opt.get()->name(), pOpt->value());
            }
            else if(COptDouble* pOpt = dynamic_cast<COptDouble*>(opt.get()))
            {
                tabObj.insert(opt.get()->name(), pOpt->value());
            }
            else if(COptInt* pOpt = dynamic_cast<COptInt*>(opt.get()))
            {
                tabObj.insert(opt.get()->name(), pOpt->value());
            }
        }
        aOpt.insert(tabName, tabObj);
    };

    optToObj("General", eOptSetGeneral);
    optToObj("Render", eOptSetRender);
    optToObj("Resource", eOptSetResource);
    optToObj("KeyBinding", eOptSetKeyBinding);

    QJsonDocument doc(aOpt);
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
    updateOptFromUi();
    onClose();
}

void CSettings::on_FigurePath_1_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "lastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Choose path to mod Figures", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->figPath1->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_FigurePath_2_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "lastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Choose path to original Figures", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->figPath2->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_TexturePath_1_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "lastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Choose path to mod Texture", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->texPath1->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}

void CSettings::on_TexturePath_2_open_clicked()
{
    COptString* option = dynamic_cast<COptString*>(opt(eOptSetGeneral, "lastVisitedFolder"));
    QFileInfo fileName = QFileDialog::getOpenFileName(this, "Choose path to original Texture", option->value(), tr("RES (*.res)"));
    if(!fileName.path().isEmpty())
    {
        ui->texPath2->setText(fileName.filePath());
        option->setValue(fileName.path());
    }
}
