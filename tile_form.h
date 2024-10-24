#ifndef TILE_FORM_H
#define TILE_FORM_H

#include <QWidget>
#include "types.h"

namespace Ui {
class CTileForm;
}

class CTileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CTileForm(QWidget *parent = nullptr);
    void fillTable(QString mapName, int textureAtlasNumber);
    void setTileTypes(const QVector<ETileType>& arrTileType) {m_tileTypes = arrTileType;}
    void selectTile(ushort index);
    ~CTileForm();
private:
    void resizeTable(float tilePercentage);

private slots:
    void on_tileScaleSlider_sliderMoved(int position);
    void onCellClicked(int row, int column);

private:
    Ui::CTileForm *ui;
    QList<QIcon> m_icoList;
    int m_nTilePerRow;
    int m_nTextureAtlas;
    int m_originalTilesize;
    QVector<ETileType> m_tileTypes;
};

#endif // TILE_FORM_H
