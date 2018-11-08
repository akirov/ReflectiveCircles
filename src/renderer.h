/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <stdexcept>

#include "qglobal.h"
#if QT_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif  // QT_VERSION

#include "geometry.h"


namespace circles
{

extern const float MIN_TARGET_SIZE;
extern const float MAX_TARGET_SIZE;
extern const float INC_TARGET_SIZE;
extern const unsigned long MAX_NUM_RAYS;


/******************************* RenderingFrame *******************************/

class ReflectiveCirclesUI;

class RenderingFrame : public QFrame
{
    Q_OBJECT

  public:
    RenderingFrame(ReflectiveCirclesUI *ui, QWidget *parent = 0);
    virtual ~RenderingFrame();

    bool CheckInput() const;
    void LoadScene(const char *fileName);
    void SaveScene(const char *fileName) const;
    void Render();
    void Reset();
    void AddFugure(Figure* fig) { mScene.push_back(fig); }
    void DelFigure(Figure* fig);
    bool RenderingInProgress() const { return mRenderingInProgress; }

  protected:
    void paintEvent(QPaintEvent*);
    Figure* FindCollision(const Figure* f) const;

    // Returns true if the ray hits the target after K reflections
    bool RayTrace(Ray *ray, Figure* target, int K);

  private slots:
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
//  void mouseDoubleClickEvent(QMouseEvent * e);  // Use it for deleting figures?

  private:
    ReflectiveCirclesUI* mUI;
    bool mMousePressed;
    Point mMousePressPos;
    Figure* mMoseEditFig;
    bool mRenderingInProgress;
    bool mStopRendering;  // TODO: Add "Stop" button to set it.

    Point* mA;
    Point* mB;
    std::vector<Figure*> mScene;  // Or a volume tree?
    std::vector<Ray> mRays;
};

}  // namespace

#endif // RENDERER_H
