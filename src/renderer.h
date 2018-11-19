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
#include <QThread>

#include "geometry.h"


namespace circles
{

extern const float MIN_TARGET_SIZE;
extern const float MAX_TARGET_SIZE;
extern const float INC_TARGET_SIZE;
extern unsigned long MAX_NUM_RAYS;


/******************************* RenderingFrame *******************************/

class ReflectiveCirclesUI;
class RenderingThread;

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
    void StopRendering();

  public slots:
    void addRayInResults(const Ray& r);
    void noteRenderFinished(bool result);

  protected:
    void paintEvent(QPaintEvent*);
    Figure* FindCollision(const Figure* f) const;

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

    Point* mA;
    Point* mB;
    std::vector<Figure*> mScene;  // Or a volume tree? Use smart pointers?
    std::vector<Ray> mRays;

    RenderingThread* mRThread;
};


class RenderingThread : public QThread
{
    Q_OBJECT

  public:
    RenderingThread(const Point* const pA, const Point* const pB,
                    const std::vector<Figure*>& scene, int K) :
        mA(pA), mB(pB), mScene(scene), mK(K)
    {
    }

    ~RenderingThread()
    {
    }

    void run();

    // Returns true if the ray hits the target after K reflections
    bool RayTrace(Ray *ray, const Figure* const target, int K);

  Q_SIGNALS:
    void sendRayInResults(const Ray& ray);
    void sendRenderFinished(bool result);

  private:
    const Point* const mA;
    const Point* const mB;
    std::vector<Figure*> mScene;  // A copy of the scene. Will be modified.
    const int mK;
};

}  // namespace

#endif // RENDERER_H
