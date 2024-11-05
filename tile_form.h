#ifndef TILE_FORM_H
#define TILE_FORM_H

#include <QWidget>
#include <QTableWidget>
#include <QKeyEvent>
#include "types.h"
#include "property.h"

namespace Ui {
class CTileForm;
}

class CColorButtonItem;
class CValueItem;

class KeyPressEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyPressEventFilter(QTableWidget* parent): QObject(parent) {m_pTable = parent;};
    virtual ~KeyPressEventFilter(){}

signals:
    void setQuick(int ind, int row, int col);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            {
                qDebug() << "Enter key pressed";
                emit setQuick(0, m_pTable->currentRow(), m_pTable->currentColumn());
                return true; // блокируем дальнейшую обработку
            } else if (keyEvent->key() == Qt::Key_Delete)
            {
                qDebug() << "Delete key pressed";
                return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }
private:
    QTableWidget* m_pTable;
};

class CTileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CTileForm(QWidget *parent = nullptr);
    ~CTileForm();

    void fillTable(QString mapName, int textureAtlasNumber);
    void setTileTypes(const QVector<ETileType>& arrTileType) {m_tileTypes = arrTileType;}
    void selectTile(int index);
    int activeMaterialindex();
    void setActiveMatIndex(int index);
    void getSelectedTile(QVector<int>& arrSelIndex, int& rotNum);
    void setTileRotation(ushort rot);
    const int& tileRotation() {return m_tileRot;}
    void setMaterial(const QVector<SMaterial>& arrMat);
    void setAnimTile(const QVector<SAnimTile>& arrAnimTile);
    const QVector<SAnimTile>& animTile() {return m_arrAnimTile;};
    const QVector<SMaterial>& material() {return m_arrMaterial;};

private:
    void fitTable();
    QPixmap tileWithRot(int index, int rot);
    QPixmap tileWithRot(int index) {return tileWithRot(index, m_tileRot);};
    void updateMaterialData();
    void updateAnimTileData();
    bool isGetMaterial(SMaterial& mat);

signals:
    void onSelect(QPixmap);
    void applyChangesSignal();

private slots:
    void onCellClicked(int row, int column);
    void onSelectMaterial(int index);
    void onSelectAnimTile(int index);
    void onSetQuick(int ind, int row, int col);
    void onSelectMaterialType(int index);
    void onColorSelect(QSharedPointer<IPropertyBase> prop);
    void onIndexSet(QSharedPointer<IPropertyBase> prop);
    void onPhaseNumSet(QSharedPointer<IPropertyBase> prop);

    void on_toolButtonAddAnimTile_clicked();
    void on_toolButtonAddMaterial_clicked();
    void on_buttonOnAnimTileShow_clicked();
    void on_toolButtonDelMaterial_clicked();
    void on_toolButtonDelAnimTile_clicked();
    void on_comboMaterialType_currentIndexChanged(int index);
    void on_sliderOpacity_sliderReleased();
    void on_sliderIllumination_sliderReleased();
    void on_sliderWaveMultiplier_sliderReleased();
    void on_sliderWarpSpeed_sliderReleased();
    void on_buttonApply_clicked();
    void on_buttonCancel_clicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

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
    QSharedPointer<CColorButtonItem> m_pColorButton;
    QSharedPointer<CValueItem> m_pIndexText;
    QSharedPointer<CValueItem> m_pPhaseNumText;
};

#endif // TILE_FORM_H
