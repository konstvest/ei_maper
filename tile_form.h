#ifndef TILE_FORM_H
#define TILE_FORM_H

#include <QWidget>
#include <QTableWidget>
#include <QKeyEvent>
#include "types.h"

namespace Ui {
class CTileForm;
}

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
    void setActiveMatIndex(int index);
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
    void onSetQuick(int ind, int row, int col);

    void on_toolButtonAddAnimTile_clicked();

    void on_toolButtonAddMaterial_clicked();

    void on_toolButton_clicked();

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
