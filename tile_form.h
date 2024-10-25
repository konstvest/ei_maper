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
    ~CTileForm();

    void fillTable(QString mapName, int textureAtlasNumber);
    void setTileTypes(const QVector<ETileType>& arrTileType) {m_tileTypes = arrTileType;}
    void selectTile(int index);
    void getSelectedTile(QVector<int>& arrSelIndex, int& rotNum);
    void setTileRotation(ushort rot);
    const int& tileRotation() {return m_tileRot;}
private:
    void resizeTable(float tilePercentage);
    QPixmap tileWithRot(int index, int rot);
    QPixmap tileWithRot(int index) {return tileWithRot(index, m_tileRot);};

signals:
    void onSelect(QPixmap);

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
    int m_tileRot;
};

#endif // TILE_FORM_H
