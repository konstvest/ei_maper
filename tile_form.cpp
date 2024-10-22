#include <QStyledItemDelegate>
#include <QPainter>

#include "tile_form.h"
#include "ui_tile_form.h"
#include "resourcemanager.h"
#include "log.h"

CTileForm::CTileForm(QWidget *parent) :
    QWidget(parent)
   ,ui(new Ui::CTileForm)
  ,m_nTilePerRow(8)
  ,m_nTextureAtlas(0)
{
    ui->setupUi(this);
    ui->tileScaleSlider->setSliderPosition(100); // set 100% tile scaling by default. TODO: get from option
    ui->comboTileType->addItems(CResourceStringList::getInstance()->tileTypes().values());
    connect(ui->tableTile, SIGNAL(cellClicked(int,int)), this, SLOT(onCellClicked(int,int)));
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
}

// Кастомный делегат для отображения иконки во всю ячейку
class IconDelegate : public QStyledItemDelegate {
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
                QPixmap croppedPixmap = QPixmap::fromImage(croppedImage);
                m_icoList.append(QIcon(croppedPixmap));
            }
        }
    }

    int scaledSize = m_originalTilesize*ui->tileScaleSlider->value()/100.0f;
    int iIco(0);
    for(int row(0); row<nRow; ++row)
    {
        ui->tableTile->setRowHeight(row, scaledSize);
        for(int col(0); col<m_nTilePerRow; ++col)
        {
            ui->tableTile->setColumnWidth(col, scaledSize);
            QTableWidgetItem *item = new QTableWidgetItem;
            QIcon scaledIco = m_icoList[iIco].pixmap(QSize(m_originalTilesize, m_originalTilesize)).scaled(scaledSize, scaledSize, Qt::KeepAspectRatio);
            item->setIcon(scaledIco);
            item->setText(""); // Очищаем текст
            item->setTextAlignment(Qt::AlignCenter); // Центрируем иконку
            ui->tableTile->setItem(row, col, item);
            ++iIco;
        }
    }

    // Применяем кастомный делегат для столбца с иконками
    IconDelegate *delegate = new IconDelegate(ui->tableTile);
    for(int i(0); i<m_nTilePerRow; ++i)
        ui->tableTile->setItemDelegateForColumn(i, delegate);
    // Автоматическое изменение размера ячеек по содержимому
    //m_testTable->resizeColumnsToContents();
    //m_testTable->resizeRowsToContents();

    // Политика изменения размеров таблицы при изменении размеров окна
    ui->tableTile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



void CTileForm::on_tileScaleSlider_sliderMoved(int position)
{
    resizeTable(float(position)/100.0f);
}

void CTileForm::onCellClicked(int row, int column)
{
    int ind = column + m_nTilePerRow * row;
    ui->comboTileType->setCurrentIndex(m_tileTypes[ind]);
}

