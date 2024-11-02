#include <QStyledItemDelegate>
#include <QPainter>
#include <QKeyEvent>

#include "tile_form.h"
#include "ui_tile_form.h"
#include "resourcemanager.h"
#include "log.h"

#include "layout_components/tablemanager.h"

CTileForm::CTileForm(QWidget *parent) :
    QWidget(parent)
   ,ui(new Ui::CTileForm)
  ,m_nTilePerRow(8)
  ,m_nTextureAtlas(0)
  ,m_tileRot(0)
{
    ui->setupUi(this);
    ui->tileScaleSlider->setSliderPosition(100); // set 100% tile scaling by default. TODO: get from option
    ui->comboTileType->addItems(CResourceStringList::getInstance()->tileTypes().values());
    connect(ui->tableTile, SIGNAL(cellClicked(int,int)), this, SLOT(onCellClicked(int,int)));
    connect(ui->comboMaterial, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectMaterial(int)));
    connect(ui->comboAnimTile, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectAnimTile(int)));
    KeyPressEventFilter *filter = new KeyPressEventFilter(ui->tableTile);
    connect(filter, SIGNAL(setQuick(int,int,int)), this, SLOT(onSetQuick(int,int,int)));
    ui->tableTile->installEventFilter(filter);
    ui->tableTile->verticalHeader()->setVisible(false);
    ui->tableQuick->verticalHeader()->setVisible(false);
    ui->tableQuick->setRowCount(1);
    ui->tableQuick->setColumnCount(8);
    ui->tabWidget->setCurrentIndex(0);

    QSharedPointer<IPropertyBase> prop;
    prop.reset(new prop3D(EObjParam::eObjParamUnknow));
    m_pColorButton.reset(new CColorButtonItem(prop));
    connect(m_pColorButton.get(), SIGNAL(onColorChange(QSharedPointer<IPropertyBase>)), this, SLOT(onColorSelect(QSharedPointer<IPropertyBase>)));

    ui->formLayout->setWidget(2, QFormLayout::FieldRole, m_pColorButton.get());

    prop.reset(new propUint(EObjParam::eObjParamUnknow));
    m_pIndexText.reset(new CValueItem(prop));
    ui->formLayout_2->setWidget(1, QFormLayout::FieldRole, m_pIndexText.get());
    connect(m_pIndexText.get(), SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onIndexSet(QSharedPointer<IPropertyBase>)));
    m_pPhaseNumText.reset(new CValueItem(prop));
    ui->formLayout_2->setWidget(2, QFormLayout::FieldRole, m_pPhaseNumText.get());
    connect(m_pPhaseNumText.get(), SIGNAL(onParamChange(QSharedPointer<IPropertyBase>)), this, SLOT(onPhaseNumSet(QSharedPointer<IPropertyBase>)));
}


CTileForm::~CTileForm()
{
    delete ui;
}

void CTileForm::resizeTable(float tilePercentage)
{
    int iIco(0);
    int scaledSize = m_originalTilesize*tilePercentage;
    int nRow = m_nTilePerRow * m_nTextureAtlas;
    for(int row(0); row<nRow; ++row)
    {
        ui->tableTile->setRowHeight(row, scaledSize);
        for(int col(0); col<m_nTilePerRow; ++col)
        {
            ui->tableTile->setColumnWidth(col, scaledSize);
            QTableWidgetItem *item = ui->tableTile->item(row, col);
            QIcon scaledIco = m_icoList[iIco].pixmap(QSize(m_originalTilesize, m_originalTilesize)).scaled(scaledSize, scaledSize, Qt::KeepAspectRatio);
            item->setIcon(scaledIco);
            ++iIco;
        }
    }

    ui->tableQuick->setRowHeight(0, scaledSize);
    for(int col(0); col<m_nTilePerRow; ++col)
       ui->tableQuick->setColumnWidth(col, scaledSize);

    auto setNewTableSize = [](QTableWidget* pTable)
    {
        //pTable->resizeColumnsToContents();
        //pTable->resizeRowsToContents();
        int width = pTable->verticalHeader()->width() + pTable->frameWidth() * 2;
        for (int i = 0; i < pTable->columnCount(); ++i) {
            width += pTable->columnWidth(i);
        }

        int height = pTable->horizontalHeader()->height() + pTable->frameWidth() * 2;
        for (int i = 0; i < pTable->rowCount(); ++i) {
            height += pTable->rowHeight(i);
        }

        pTable->setFixedSize(width, height);

    };

    //setNewTableSize(ui->tableTile);
    //setNewTableSize(ui->tableQuick);
}

QPixmap CTileForm::tileWithRot(int index, int rot)
{
    QTransform transform;
    transform.rotate(90*rot);
    return m_icoList[index].pixmap(QSize(32, 32)).transformed(transform);
}

void CTileForm::updateMaterialData()
{
    ui->comboMaterialType->blockSignals(true); // block backward updating material data from ui

    ui->comboMaterial->clear();
    ui->comboActiveLiquidMaterial->clear();
    for(int i(0); i<m_arrMaterial.size(); ++i)
    {
        ui->comboMaterial->addItem(QString::number(i));
        ui->comboActiveLiquidMaterial->addItem(QString::number(i));
    }
    ui->comboActiveLiquidMaterial->insertItem(0, CResourceStringList::getInstance()->noLiquidIndexName());
    if(ui->comboActiveLiquidMaterial->count() > 1)
        ui->comboActiveLiquidMaterial->setCurrentText("0");

    ui->comboMaterialType->blockSignals(false);
}

void CTileForm::updateAnimTileData()
{
    ui->comboAnimTile->clear();
    for(int i(0); i<m_arrAnimTile.size(); ++i)
    {
        ui->comboAnimTile->addItem(QString::number(i));
    }
}

bool CTileForm::isGetMaterial(SMaterial& mat)
{
    if(ui->comboMaterial->count() == 0)
        return false;

    mat = m_arrMaterial[ui->comboMaterial->currentIndex()];
    return true;
}

// Кастомный делегат для отображения иконки во всю ячейку
class IconDelegate : public QStyledItemDelegate
{
public:
    ~IconDelegate()
    {
    }
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    // Переопределяем метод для рисования иконки
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        if (!icon.isNull()) {
            // Получаем размер ячейки и рисуем иконку во всю ее область
            QRect rect = option.rect;
            QPixmap pixmap = icon.pixmap(rect.size());
            painter->drawPixmap(rect, pixmap);
        }
        if (option.state & QStyle::State_Selected) {
            // Настройка кисти и пера для рисования рамки
            QPen pen(Qt::green, 2);  // Зеленая рамка толщиной 4 пикселя
            painter->setPen(pen);
            painter->drawRect(option.rect);
        }
    }
};

// map name (-> generate texture name)
// texture number (zone1000->zone1007)
// tile number per atlas -> calc tile size
void CTileForm::fillTable(QString mapName, int textureAtlasNumber)
{

    QStringList arrTexName;
    for(int i(0); i<textureAtlasNumber; ++i)
    {
        arrTexName.append(mapName + "00" + QString::number(i) + ".mmp");
    }
    QVector<QImage> arrImage;
    CTextureList::getInstance()->extractMmpToDxt1(arrImage, arrTexName);
    for(const auto& img: arrImage)
    {
        if(img.width() != img.height())
        {
            ei::log(eLogFatal, "Texture has incorrect aspect");
            return;
        }
    }


    m_originalTilesize = arrImage.front().width()/m_nTilePerRow;
    m_nTextureAtlas = textureAtlasNumber;
    int nRow = m_nTilePerRow * m_nTextureAtlas;

    ui->tableTile->setRowCount(nRow); // Задаем количество строк (например, 5 строк)
    ui->tableTile->setColumnCount(m_nTilePerRow); // Одна колонка для иконок


    for(int i(0); i<textureAtlasNumber; ++i)
    {
        for(int col(0); col<m_nTilePerRow; ++col)
        {
            for(int row(0); row<m_nTilePerRow; ++row)
            {
                QImage croppedImage = arrImage[i].copy(row*m_originalTilesize, col*m_originalTilesize, m_originalTilesize, m_originalTilesize);
                croppedImage = croppedImage.mirrored(false, true);
                QPixmap croppedPixmap = QPixmap::fromImage(croppedImage);
                m_icoList.append(QIcon(croppedPixmap));
            }
        }
    }

    //int scaledSize = m_originalTilesize*ui->tileScaleSlider->value()/100.0f;
    int iIco(0);
    for(int row(0); row<nRow; ++row)
    {
        //ui->tableTile->setRowHeight(row, scaledSize);
        for(int col(0); col<m_nTilePerRow; ++col)
        {
            //ui->tableTile->setColumnWidth(col, scaledSize);
            QTableWidgetItem *item = new QTableWidgetItem;
            //QIcon scaledIco = m_icoList[iIco].pixmap(QSize(m_originalTilesize, m_originalTilesize)).scaled(scaledSize, scaledSize, Qt::KeepAspectRatio);
            //item->setIcon(scaledIco);
            item->setIcon(m_icoList[iIco]);
            item->setText(""); // Очищаем текст
            item->setTextAlignment(Qt::AlignCenter); // Центрируем иконку
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableTile->setItem(row, col, item);
            ++iIco;
        }
    }

    // Применяем кастомный делегат для столбца с иконками
    //IconDelegate *delegate = new IconDelegate(ui->tableTile);
    for(int i(0); i<m_nTilePerRow; ++i)
    {
        ui->tableTile->setItemDelegateForColumn(i, new IconDelegate(ui->tableTile));
        ui->tableQuick->setItemDelegateForColumn(i, new IconDelegate(ui->tableQuick)); // table of quick items
    }
    // Автоматическое изменение размера ячеек по содержимому
    //m_testTable->resizeColumnsToContents();
    //m_testTable->resizeRowsToContents();

    // Политика изменения размеров таблицы при изменении размеров окна
    //ui->tableTile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resizeTable(float(ui->tileScaleSlider->value())/100.0f);
}

void CTileForm::selectTile(int index)
{
    int row = index/m_nTilePerRow;
    int column = index%m_nTilePerRow;
    ui->tableTile->setCurrentCell(row, column);
    ui->comboTileType->setCurrentIndex(m_tileTypes[index]);
    emit onSelect(tileWithRot(index));
}

int CTileForm::activeMaterialindex()
{
    QString index = ui->comboActiveLiquidMaterial->currentText();
    if(index.contains("-1"))
        return -1;
    return index.toInt();
}

void CTileForm::setActiveMatIndex(int index)
{
    ui->comboActiveLiquidMaterial->setCurrentText(index == -1 ? (CResourceStringList::getInstance()->noLiquidIndexName()) : QString::number(index));
}

void CTileForm::getSelectedTile(QVector<int>& arrSelIndex, int& rotNum)
{
    arrSelIndex.clear();
    for(const auto& index: ui->tableTile->selectionModel()->selectedIndexes())
    {
        arrSelIndex.append(index.row()*8+index.column());
    }
    rotNum = m_tileRot;
}

void CTileForm::setTileRotation(ushort rot)
{
    m_tileRot = rot;
    QVector<int> arrSelIndex;
    for(const auto& index: ui->tableTile->selectionModel()->selectedIndexes())
    {
        arrSelIndex.append(index.row()*8+index.column());
    }
    if(arrSelIndex.isEmpty())
        return;
    emit onSelect(tileWithRot(arrSelIndex.back()));
}

void CTileForm::setMaterial(const QVector<SMaterial>& arrMat)
{
    m_arrMaterial = arrMat;
    updateMaterialData();
}

void CTileForm::setAnimTile(const QVector<SAnimTile>& arrAnimTile)
{
    m_arrAnimTile = arrAnimTile;
    updateAnimTileData();
}

void CTileForm::on_tileScaleSlider_sliderMoved(int position)
{
    resizeTable(float(position)/100.0f);
}

void CTileForm::onCellClicked(int row, int column)
{
    int ind = column + m_nTilePerRow * row;
    ui->comboTileType->setCurrentIndex(m_tileTypes[ind]);
    emit onSelect(tileWithRot(ind));
    //QCursor curs(m_icoList[ind].pixmap(QSize(32, 32)));
}

void CTileForm::onSelectMaterial(int index)
{
    if(index == -1)
        return; // avoid select invalid material after cleaning combobox

    const SMaterial& mat = m_arrMaterial[index];
    ui->sliderOpacity->setValue(mat.A*100); // set opacity in percentage (0->100%)
    ui->sliderIllumination->setValue(mat.selfIllumination*100);
    ui->sliderWaveMultiplier->setValue(mat.waveMultiplier*100);
    ui->sliderWarpSpeed->setValue(mat.warpSpeed*100);
    ui->comboMaterialType->blockSignals(true);
    ui->comboMaterialType->clear();
    ui->comboMaterialType->addItems(CResourceStringList::getInstance()->materialType().values());
    ui->comboMaterialType->setCurrentText(CResourceStringList::getInstance()->materialType()[mat.type]);
    QVector3D clr(mat.R, mat.G, mat.B);
    m_pColorButton->renewColor(clr);
    ui->comboMaterialType->blockSignals(false);
}

void CTileForm::onSelectAnimTile(int index)
{
    if(index == -1)
        return; // avoid select invalid anim tile after cleaning combobox

    const SAnimTile& anmTile = m_arrAnimTile[index];
    m_pIndexText->renewValue(QString::number(anmTile.tileIndex));
    m_pPhaseNumText->renewValue(QString::number(anmTile.nPhase));
}

void CTileForm::onSetQuick(int ind, int row, int col)
{
    int textureInd = col + m_nTilePerRow * row;
    qDebug() << textureInd;
}

void CTileForm::onSelectMaterialType(int index)
{
    Q_UNUSED(index);
    qDebug() << ui->comboMaterialType->currentText();
}

void CTileForm::onColorSelect(QSharedPointer<IPropertyBase> prop)
{
    if(ui->comboMaterial->count() == 0)
        return;

    SMaterial& mat = m_arrMaterial[ui->comboMaterial->currentIndex()];
    const auto& clr = dynamic_cast<prop3D*>(prop.get())->value();
    mat.R = clr.x();
    mat.G = clr.y();
    mat.B = clr.z();
}

void CTileForm::onIndexSet(QSharedPointer<IPropertyBase> prop)
{
    if(ui->comboAnimTile->count() == 0)
        return;

    SAnimTile& tile = m_arrAnimTile[ui->comboAnimTile->currentIndex()];
    const auto& value = dynamic_cast<propUint*>(prop.get())->value();
    tile.tileIndex = value;
}

void CTileForm::onPhaseNumSet(QSharedPointer<IPropertyBase> prop)
{
    if(ui->comboAnimTile->count() == 0)
        return;

    SAnimTile& tile = m_arrAnimTile[ui->comboAnimTile->currentIndex()];
    const auto& value = dynamic_cast<propUint*>(prop.get())->value();
    tile.nPhase = value;
}


void CTileForm::on_toolButtonAddAnimTile_clicked()
{
    m_arrAnimTile.append(SAnimTile{0, 0});
    updateAnimTileData();
    ui->comboAnimTile->setCurrentIndex(ui->comboAnimTile->count()-1);
}


void CTileForm::on_toolButtonAddMaterial_clicked()
{
    m_arrMaterial.append(SMaterial{ETerrainType::eTerrainWater, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f});
    updateMaterialData();
    ui->comboMaterial->setCurrentIndex(ui->comboMaterial->count()-1);
}


void CTileForm::on_buttonOnAnimTileShow_clicked()
{
    int index = dynamic_cast<const propUint*>(m_pIndexText->value().get())->value();
    int count = dynamic_cast<const propUint*>(m_pPhaseNumText->value().get())->value();
    if(count == 0)
        return;
    int row = index/m_nTilePerRow;
    int col = index%m_nTilePerRow;

    ui->tabWidget->setCurrentIndex(0);
    ui->tableTile->clearSelection();
    for(int i(0); i<count; ++i)
    {
        int cRow = (index+i)/m_nTilePerRow;
        int cCol = (index+i)%m_nTilePerRow;
        ui->tableTile->item(cRow, cCol)->setSelected(true);
    }
    ui->tableTile->scrollToItem(ui->tableTile->item(row, col));
}


void CTileForm::on_toolButtonDelMaterial_clicked()
{
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial.removeAt(ui->comboMaterial->currentIndex());
    updateMaterialData();
}


void CTileForm::on_toolButtonDelAnimTile_clicked()
{
    if(ui->comboAnimTile->count() == 0)
        return;

    m_arrAnimTile.removeAt(ui->comboAnimTile->currentIndex());
    updateAnimTileData();
}


void CTileForm::on_comboMaterialType_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial[ui->comboMaterial->currentIndex()].type = CResourceStringList::getInstance()->materialType().key(ui->comboMaterialType->currentText());
}


void CTileForm::on_sliderOpacity_sliderReleased()
{
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial[ui->comboMaterial->currentIndex()].A = ui->sliderOpacity->value()/100.0f;
}


void CTileForm::on_sliderIllumination_sliderReleased()
{
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial[ui->comboMaterial->currentIndex()].selfIllumination = ui->sliderIllumination->value()/100.0f;
}


void CTileForm::on_sliderWaveMultiplier_sliderReleased()
{
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial[ui->comboMaterial->currentIndex()].waveMultiplier = ui->sliderWaveMultiplier->value()/100.0f;
}


void CTileForm::on_sliderWarpSpeed_sliderReleased()
{
    if(ui->comboMaterial->count() == 0)
        return;

    m_arrMaterial[ui->comboMaterial->currentIndex()].warpSpeed = ui->sliderWarpSpeed->value()/100.0f;
}


void CTileForm::on_buttonApply_clicked()
{
    emit applyChangesSignal();
    close();
}


void CTileForm::on_buttonCancel_clicked()
{
    close();
}

