#ifndef COLOR_H
#define COLOR_H

#include <QDataStream>
#include <QDebug>
#include <QVector4D>

struct SColor
{
    SColor(){rgb[0]=0; rgb[1]=0; rgb[2]=0;}
    SColor(uchar R, uchar G, uchar B) {rgb[0]=R; rgb[1]=G; rgb[2]=B; rgb[3]=1;}
    SColor(uchar R, uchar G, uchar B, uchar A) { rgb[0]=R; rgb[1]=G; rgb[2]=B; rgb[3]=A; hasAlpha = true; }

    bool hasAlpha = false;
    uchar rgb[4];

    uchar red() const {return rgb[0];}
    uchar red(uchar val) {rgb[0] = val; return rgb[0];}
    uchar green() const  {return rgb[1];}
    uchar green(uchar val) {rgb[1] = val; return rgb[1];}
    uchar blue() const  {return rgb[2];}
    uchar blue(uchar val) {rgb[2] = val; return rgb[2];}
    uchar alpha() const  { return hasAlpha ? rgb[3] : 0;}
    uchar alpha(uchar val){rgb[3] = val; return hasAlpha ? rgb[3] : 0;}

    bool isBlack() const {return rgb[0]==0 && rgb[1]==0 && rgb[2]==0;}

    //todo: operator=
    bool operator==(SColor& color)
    {
        return hasAlpha ?
                    (red() == color.red() && green() == color.green() && blue() == color.blue() && alpha() == color.alpha()) :
                    (red() == color.red() && green() == color.green() && blue() == color.blue());
    }
    bool operator==(const SColor& color) const
    {
        return hasAlpha ?
                    (red() == color.red() && green() == color.green() && blue() == color.blue() && alpha() == color.alpha()) :
                    (red() == color.red() && green() == color.green() && blue() == color.blue());
    }
    QVector4D toVec4()
    {
        return hasAlpha ?
                    QVector4D(rgb[0]/255.0f, rgb[1]/255.0f, rgb[2]/255.0f, rgb[3]/255.0f)  :
                    QVector4D(rgb[0]/255.0f, rgb[1]/255.0f, rgb[2]/255.0f, 1.0f);
    }
};

struct SMmpColorDetail
{
    uint m_value;
    uint m_mask;
    uint m_shift;

    SMmpColorDetail(){}
    SMmpColorDetail(uint val, uint mas, uint shif){m_value = val; m_mask = mas; m_shift = shif;}
    friend QDataStream& operator>> (QDataStream& data, SMmpColorDetail& col)
    {
        return data >> col.m_value >> col.m_mask >> col.m_shift;
    }
};

struct SMmpColor
{
    SMmpColor(){}
    SMmpColor(SMmpColorDetail r, SMmpColorDetail g, SMmpColorDetail b, SMmpColorDetail a)
    {red = r; green = g; blue = b; alpha = a;}

    SMmpColorDetail red;
    SMmpColorDetail green;
    SMmpColorDetail blue;
    SMmpColorDetail alpha;

    friend QDataStream& operator >> (QDataStream& data, SMmpColor& col)
    {
        return data >> col.alpha >> col.red >> col.green >> col.blue;
    }
};


#endif // COLOR_H
