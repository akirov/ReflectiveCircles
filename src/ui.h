/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#ifndef UI_H
#define UI_H

#include <QMainWindow>
#include "qglobal.h"
#if QT_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif  // QT_VERSION

#include "renderer.h"


namespace circles
{

typedef enum {
    DM_POINTA,
    DM_POINTB,
    DM_CIRCLE
} DrawingMode;


class ReflectiveCirclesUI : public QMainWindow 
{
    Q_OBJECT

  public:
    ReflectiveCirclesUI(QWidget *parent = 0);
    ~ReflectiveCirclesUI();

    DrawingMode GetDrawingMode() const;
    int GetK() const;
    void SetK(int k);
    int GetMinR() const;
    int GetRenderHeight() const;
    int GetRenderWidth() const;

  private slots:
    void on_mRenderButton_clicked();
    void on_mStopButton_clicked();
    void on_mClearButton_clicked();
    void LoadScene();
    void SaveScene();

  private:
    void SetupUi();
    void CreateActions();
    void CreateMenus();

  private:
    QWidget        *mCentralWidget;
    RenderingFrame *mRenderFrame;
    QLabel         *mDrawLabel;
    QRadioButton   *mRBPointA;
    QRadioButton   *mRBPointB;
    QRadioButton   *mRBCircle;
    QButtonGroup   *mRBGroup;
    QSpinBox       *mKSpinBox;
    QLabel         *mKLabel;
    QPushButton    *mRenderButton;
    QPushButton    *mStopButton;
    QPushButton    *mClearButton;
    QLabel         *mOptionsLabel;
    QSpinBox       *mMinRSpinBox;
    QLabel         *mMinRLabel;
    QMenuBar       *mMenuBar;
    QMenu          *mFileMenu;
    QAction        *mFileOpen;
    QAction        *mFileSave;
};

}  // namespace

#endif // UI_H
