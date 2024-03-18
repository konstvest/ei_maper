#ifndef CPROGRESSVIEW_H
#define CPROGRESSVIEW_H

#include <QProgressBar>

///
/// \brief The CProgressView class controls the behavior of the progress bar.
///
class CProgressView : public QProgressBar
{
    Q_OBJECT
public:
    explicit CProgressView(QWidget* parent = nullptr);
    void reset();
    void setStageCount(int value);
    void update(double value);

private:
    double m_value;
};

#endif // CPROGRESSVIEW_H
