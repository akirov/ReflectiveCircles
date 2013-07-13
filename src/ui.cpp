/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#include "ui.h"


namespace circles
{

ReflectiveCirclesUI::ReflectiveCirclesUI(QWidget *parent) :
        QMainWindow(parent)
{
    SetupUi();
    CreateActions();
    CreateMenus();
}


ReflectiveCirclesUI::~ReflectiveCirclesUI()
{
    // The parent will delete his children automatically
}


void ReflectiveCirclesUI::SetupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ReflectiveCirclesUI"));
    this->resize(1000, 640);
    this->setWindowTitle(QString::fromUtf8("ReflectiveCircles"));

    mCentralWidget = new QWidget(this);
    mCentralWidget->setObjectName(QString::fromUtf8("mCentralWidget"));

    mRenderFrame = new RenderingFrame(this, mCentralWidget);
    mRenderFrame->setObjectName(QString::fromUtf8("mRenderFrame"));
    mRenderFrame->setGeometry(QRect(190, 8, 800, 600));
    mRenderFrame->setFrameShape(QFrame::StyledPanel);
    mRenderFrame->setFrameShadow(QFrame::Sunken);

    mDrawLabel = new QLabel(mCentralWidget);
    mDrawLabel->setObjectName(QString::fromUtf8("mDrawLabel"));
    mDrawLabel->setGeometry(QRect(20, 30, 160, 16));
    mDrawLabel->setText(QString::fromUtf8("Draw with the mouse"));

    mRBPointA = new QRadioButton(mCentralWidget);
    mRBPointA->setObjectName(QString::fromUtf8("mRBPointA"));
    mRBPointA->setGeometry(QRect(20, 55, 95, 21));
    mRBPointA->setText(QString::fromUtf8("Point A"));
    mRBPointA->setChecked(true);

    mRBPointB = new QRadioButton(mCentralWidget);
    mRBPointB->setObjectName(QString::fromUtf8("mRBPointB"));
    mRBPointB->setGeometry(QRect(20, 80, 95, 21));
    mRBPointB->setText(QString::fromUtf8("Point B"));

    mRBCircle = new QRadioButton(mCentralWidget);
    mRBCircle->setObjectName(QString::fromUtf8("mRBCircle"));
    mRBCircle->setGeometry(QRect(20, 105, 160, 21));
    mRBCircle->setText(QString::fromUtf8("Circle (press and drag)"));
    
    mRBGroup = new QButtonGroup(this);
    mRBGroup->setObjectName(QString::fromUtf8("mRBGroup"));
    mRBGroup->addButton(mRBPointA);
    mRBGroup->addButton(mRBPointB);
    mRBGroup->addButton(mRBCircle);

    mKSpinBox = new QSpinBox(this);
    mKSpinBox->setObjectName(QString::fromUtf8("mKSpinBox"));
    mKSpinBox->setGeometry(QRect(20, 200, 51, 21));
    mKSpinBox->setMinimum(1);
    mKSpinBox->setValue(1);

    mKLabel = new QLabel(mCentralWidget);
    mKLabel->setObjectName(QString::fromUtf8("mKLabel"));
    mKLabel->setGeometry(QRect(85, 182, 60, 16));
    mKLabel->setText(QString::fromUtf8("Reflections"));

    mRenderButton = new QPushButton(mCentralWidget);
    mRenderButton->setObjectName(QString::fromUtf8("mRenderButton"));
    mRenderButton->setGeometry(QRect(20, 270, 93, 28));
    mRenderButton->setText(QString::fromUtf8("Find Path"));

    mClearButton = new QPushButton(mCentralWidget);
    mClearButton->setObjectName(QString::fromUtf8("mClearButton"));
    mClearButton->setGeometry(QRect(20, 320, 93, 28));
    mClearButton->setText(QString::fromUtf8("Reset"));

    mOptionsLabel = new QLabel(mCentralWidget);
    mOptionsLabel->setObjectName(QString::fromUtf8("mOptionsLabel"));
    mOptionsLabel->setGeometry(QRect(20, 450, 50, 16));
    mOptionsLabel->setText(QString::fromUtf8("Options"));

    mMinRSpinBox = new QSpinBox(this);
    mMinRSpinBox->setObjectName(QString::fromUtf8("mMinRSpinBox"));
    mMinRSpinBox->setGeometry(QRect(20, 500, 51, 21));
    mMinRSpinBox->setMinimum(1);
    mMinRSpinBox->setValue(10);

    mMinRLabel = new QLabel(mCentralWidget);
    mMinRLabel->setObjectName(QString::fromUtf8("mMinRLabel"));
    mMinRLabel->setGeometry(QRect(85, 482, 160, 16));
    mMinRLabel->setText(QString::fromUtf8("Min circle radius"));

    this->setCentralWidget(mCentralWidget);

    mMenuBar = new QMenuBar(this);
    mMenuBar->setObjectName(QString::fromUtf8("mMenuBar"));
    mMenuBar->setGeometry(QRect(0, 0, 1000, 22));
    this->setMenuBar(mMenuBar);

    QMetaObject::connectSlotsByName(this);
}


void ReflectiveCirclesUI::CreateActions()
{
    mFileOpen = new QAction(tr("&Open"), this);
    connect(mFileOpen, SIGNAL(triggered()), this, SLOT(LoadScene()));

    mFileSave = new QAction(tr("&Save"), this);
    connect(mFileSave, SIGNAL(triggered()), this, SLOT(SaveScene()));
}


void ReflectiveCirclesUI::CreateMenus()
{
    mFileMenu = new QMenu(tr("&File"), this);

    mFileMenu->addAction(mFileOpen);
    mFileMenu->addAction(mFileSave);

    menuBar()->addMenu(mFileMenu);
}


void ReflectiveCirclesUI::LoadScene()
{
    if ( mRenderFrame->RenderingInProgress() )
    {
        QMessageBox::warning(this, "ERROR", "Rendering is in progress");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load scene",
        QDir::currentPath(),
        tr("Scenes (*.txt)") );

    if ( ! fileName.isNull() )
    {
        mRenderFrame->LoadScene(fileName.toStdString().c_str());
    }
}


void ReflectiveCirclesUI::SaveScene()
{
    QString fileName = QFileDialog::getSaveFileName( 
        this, 
        tr("Save Scene"), 
        QDir::currentPath(), 
        tr("Scenes (*.txt)") );

    if ( ! fileName.isNull() )
    {
        mRenderFrame->SaveScene(fileName.toStdString().c_str());
    }
}


void ReflectiveCirclesUI::on_mRenderButton_clicked()
{
    if ( mRenderFrame->RenderingInProgress() )
    {
        QMessageBox::warning(this, "ERROR", "Rendering is in progress");
        return;
    }

    // Check the input
    if ( ! mRenderFrame->CheckInput() )
        return;

    // Render scene. TODO: Start rendering in a separate thread.
    mRenderFrame->Render();

    mRenderFrame->update();
}


void ReflectiveCirclesUI::on_mClearButton_clicked()
{
    if ( mRenderFrame->RenderingInProgress() )
    {
        QMessageBox::warning(this, "ERROR", "Rendering is in progress");
        return;
    }

    mRenderFrame->Reset();

    mRenderFrame->update();
}


DrawingMode ReflectiveCirclesUI::GetDrawingMode() const
{
    if ( mRBPointA->isChecked() )
        return DM_POINTA;
    else if ( mRBPointB->isChecked() )
        return DM_POINTB;
    else
        return DM_CIRCLE;
}


int ReflectiveCirclesUI::GetK() const
{
    return mKSpinBox->value();
}


void ReflectiveCirclesUI::SetK(int k)
{
    mKSpinBox->setValue(k);
}


int ReflectiveCirclesUI::GetMinR() const
{
    return mMinRSpinBox->value();
}


int ReflectiveCirclesUI::GetRenderHeight() const
{
    return mRenderFrame->height();
}


int ReflectiveCirclesUI::GetRenderWidth() const
{
    return mRenderFrame->width();
}

}  // namespace
