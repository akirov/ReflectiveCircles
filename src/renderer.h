/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <stdexcept>
#include <QtGui>
#include "geometry.h"


namespace circles
{

extern const float MIN_TARGET_SIZE;
extern const float MAX_TARGET_SIZE;
extern const float INC_TARGET_SIZE;
extern const unsigned long MAX_NUM_RAYS;


/******************************* Locking class ********************************/

class Locker
{
  public:
    explicit Locker( bool& flag ) :
            mFlag(flag)
    {
        if ( mFlag )
            throw std::runtime_error("Rendering is in progress");

        mFlag = true;  // "Lock".
    }

    ~Locker()
    {
        mFlag = false;  // "Unlock".
    }

  private:
    Locker( const Locker& );  // Disable copying.
    Locker& operator=( const Locker& );  // Disable assignment.

    //  Disable creation on the heap by making these private
    void *operator new( size_t ); // throw() { return 0; }
    void operator delete( void * ); // {}
    void *operator new[]( size_t );
    void operator delete[]( void * );

    bool& mFlag;
};


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
    std::vector<Figure*> mScene;
    std::vector<Ray> mRays;
};

}  // namespace

#endif // RENDERER_H
