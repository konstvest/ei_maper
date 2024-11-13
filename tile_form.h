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
    bool eventFilter(QObject *obj, QEvent *event) override;

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
    void selectQuickAccessTile(int index);
    int activeMaterialindex();
    void setActiveMatIndex(int index);
    void getSelectedTile(int& index, int& rotNum);
    void getSelectedTiles(QVector<int>& arrSelIndex, int& rotNum);
    void setTileRotation(ushort rot);
    const int& tileRotation() {return m_tileRot;}
    void setMaterial(const QVector<SMaterial>& arrMat);
    void setAnimTile(const QVector<SAnimTile>& arrAnimTile);
    const QVector<SAnimTile>& animTile() {return m_arrAnimTile;};
    const QVector<SMaterial>& material() {return m_arrMaterial;};
    const QVector<ETileType>& tileTypes() {return m_tileTypes;}

private:
    void fitTable();
    QPixmap tileWithRot(int index, int rot);
    QPixmap tileWithRot(int index) {return tileWithRot(index, m_tileRot);};
    void updateMaterialData();
    void updateAnimTileData();
    bool isGetMaterial(SMaterial& mat);
    void updateQuickTable();

signals:
    void onSelect(QPixmap);
    void applyChangesSignal();

private slots:
    void onSelectFinish();
    void onCellClicked(int row, int column);
    void onQuickCellClicked(int row, int column);
    void onSelectMaterial(int index);
    void onSelectAnimTile(int index);
    void onSelectTileType(int index);
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
    QVector<int> m_arrQuickTile;
};

class CCustomSelectionTable : public QTableWidget
{
    Q_OBJECT

public:
    CCustomSelectionTable(QWidget *parent = nullptr)
        : QTableWidget(parent)
    {
        setSelectionMode(QAbstractItemView::ContiguousSelection);
        connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &CCustomSelectionTable::onSelectionChanged);
    }
    const QModelIndex& lastSelectedIndex() const {return m_endIndex;}

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        m_startIndex = indexAt(event->pos());
        m_endIndex = m_startIndex;
        clearSelection();
        QTableWidget::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QModelIndex endIndex = indexAt(event->pos());
        if (m_startIndex.isValid() && endIndex.isValid())
        {
            m_endIndex = endIndex;
            selectToIndex(m_startIndex, endIndex);
        }
    }
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && selectionInProgress)
        {
            emit selectionFinished();
            selectionInProgress = false;
        }
        QTableWidget::mouseReleaseEvent(event);
    }

signals:
    void selectionFinished();

private slots:
    void onSelectionChanged()
    {
        selectionInProgress = true;
    }

private:
    QModelIndex m_startIndex, m_endIndex;
    bool selectionInProgress = false;

    void selectToIndex(const QModelIndex &startIndex, const QModelIndex &endIndex)
    {
        int startRow = startIndex.row();
        int startColumn = startIndex.column();
        int endRow = endIndex.row();
        int endColumn = endIndex.column();

        QItemSelection selection;

        if (startRow < endRow) {
            for (int row = startRow; row <= endRow; ++row) {
                int beginColumn = (row == startRow) ? startColumn : 0;
                int finishColumn = (row == endRow) ? endColumn : columnCount() - 1;

                selection.select(model()->index(row, beginColumn), model()->index(row, finishColumn));
            }
        }
        else if (startRow > endRow) {
            for (int row = startRow; row >= endRow; --row) {
                int beginColumn = (row == startRow) ? startColumn : columnCount() - 1;
                int finishColumn = (row == endRow) ? endColumn : 0;

                selection.select(model()->index(row, finishColumn), model()->index(row, beginColumn));
            }
        }
        else {
            int beginColumn = qMin(startColumn, endColumn);
            int finishColumn = qMax(startColumn, endColumn);
            selection.select(model()->index(startRow, beginColumn), model()->index(startRow, finishColumn));
        }

        selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    }
};

#endif // TILE_FORM_H
