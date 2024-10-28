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
    void setMaterial(const QVector<SMaterial>& arrMat);
    void setAnimTile(const QVector<SAnimTile>& arrAnimTile);
    const QVector<SAnimTile>& animTile() {return m_arrAnimTile;};
    const QVector<SMaterial>& material() {return m_arrMaterial;};
private:
    void resizeTable(float tilePercentage);
    QPixmap tileWithRot(int index, int rot);
    QPixmap tileWithRot(int index) {return tileWithRot(index, m_tileRot);};

signals:
    void onSelect(QPixmap);

private slots:
    void on_tileScaleSlider_sliderMoved(int position);
    void onCellClicked(int row, int column);
    void onSelectMaterial(int index);
    void onSelectAnimTile(int index);

    void on_toolButtonAddAnimTile_clicked();

    void on_toolButtonAddMaterial_clicked();

private:
    Ui::CTileForm *ui;
    QList<QIcon> m_icoList;
    int m_nTilePerRow;
    int m_nTextureAtlas;
    int m_originalTilesize;
    QVector<ETileType> m_tileTypes;
    int m_tileRot;
    QVector<SMaterial> m_arrMaterial;
    QVector<SAnimTile> m_arrAnimTile;
};

#endif // TILE_FORM_H
