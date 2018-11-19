/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "renderer.h"
#include "ui.h"


namespace circles
{

#undef DEBUG
#define MAX_REFLECTIONS  500  // Debug parameter.

const float MIN_TARGET_SIZE      = 2.0f;
const float MAX_TARGET_SIZE      = 4.0f;
const float INC_TARGET_SIZE      = 1.0f;
unsigned long MAX_NUM_RAYS       = 2000000;  // TODO: Make it configurable


RenderingFrame::RenderingFrame(ReflectiveCirclesUI *ui, QWidget *parent):
        QFrame(parent),
        mUI(ui),
        mMousePressed(false),
        mMousePressPos(0,0),
        mMoseEditFig(NULL),
        mRenderingInProgress(false),
        mA(NULL),
        mB(NULL),
        mScene(),
        mRays()
{
}


RenderingFrame::~RenderingFrame()
{
    for ( std::vector<Figure*>::iterator fig=mScene.begin();
          fig != mScene.end(); ++fig )
    {
        delete *fig;
    }
}


bool RenderingFrame::CheckInput() const
{
    if ( (NULL == mA) || (NULL == mB) )
    {
        QMessageBox::warning(mUI, "ERROR", "Point A or B is missing.");
        return false;
    }

#if 0  // We don't need this condition.
    if ( mUI->GetK() > ((int)mScene.size() - 2) )
    {
        QMessageBox::warning(mUI, "ERROR", "K is bigger than the number of circles.");
        return false;
    }
#endif // 0

    // No need to check overlapping again. It is checked during the input.

    return true;
}


// Remove leading spaces
inline void ltrim( std::string & s )
{
    while ( isspace(s[0]) )
        s.erase(0,1);
}


// TODO: Break in several sub-functions
void RenderingFrame::LoadScene(const char *fileName)
{
    if ( RenderingInProgress() )
    {
        QMessageBox::warning(this, "ERROR", "Rendering is in progress");
        return;
    }

    std::ifstream inFile;
    std::stringstream errSStr;

    inFile.open(fileName, std::ios::in);
    if ( ! inFile.is_open() )
    {
        errSStr << "Unable to open the input file '" << fileName << "'";
        QMessageBox::warning(mUI, "ERROR", errSStr.str().c_str());
        return;
    }

    Reset();  // Delete the old scene first. Do we need to do this?

    const std::string CirclesBeginDelim="CirclesBegin";
    const std::string CirclesEndDelim = "CirclesEnd";

    float minX = INF_DIST;
    float minY = INF_DIST;
    float maxX = -INF_DIST;
    float maxY = -INF_DIST;

    std::string s;
    float x, y, r;
    Point pt(0,0);
    Circle cr(0,0,0);
    bool doScale=false;

    while ( std::getline(inFile, s) )
    {
        ltrim(s);
        if ( ('\0' == s[0]) || ('#' == s[0]) )
            continue;

        // TODO: generic point section?
        if ( s.substr(0,2) == "A=" )
        {
            int res = sscanf((s.substr(2)).c_str(), "%f %f", &x, &y);
            if ( res != 2 )
            {
                errSStr << "Ignored invalid A on this row : " << s 
                        << std::endl;
                continue;
            }

            pt.x = x;
            pt.y = y;

            if ( NULL != FindCollision(&pt) )
            {
                errSStr << "Ignored overlapping A on this row : " << s 
                        << std::endl;
                continue;
            }

            if ( NULL == mA )
            {
                mA = new Point(pt);
                mScene.push_back(mA);
            }
            else
            {
                *mA = pt;
            }

            if ( x < minX ) minX = x;
            if ( y < minY ) minY = y;
            if ( x > maxX ) maxX = x;
            if ( y > maxY ) maxY = y;

            continue;
        }

        else if ( s.substr(0,2) == "B=" )
        {
            int res = sscanf((s.substr(2)).c_str(), "%f %f", &x, &y);
            if ( res != 2 )
            {
                errSStr << "Ignored invalid B on this row : " << s 
                        << std::endl;
                continue;
            }

            pt.x = x;
            pt.y = y;

            if ( NULL != FindCollision(&pt) )
            {
                errSStr << "Ignored overlapping B on this row : " << s 
                        << std::endl;
                continue;
            }

            if ( NULL == mB )
            {
                mB = new Point(pt);
                mScene.push_back(mB);
            }
            else
            {
                *mB = pt;
            }

            if ( x < minX ) minX = x;
            if ( y < minY ) minY = y;
            if ( x > maxX ) maxX = x;
            if ( y > maxY ) maxY = y;

            continue;
        }

        else if ( s.substr(0,CirclesBeginDelim.length()) == CirclesBeginDelim )
        {
            while (std::getline(inFile, s))
            {
                ltrim(s);
                if ( ('\0' == s[0]) || ('#' == s[0]) )
                    continue;
                if ( s.substr(0,CirclesEndDelim.length()) == CirclesEndDelim )
                    break;

                int res = sscanf(s.c_str(), "%f %f %f",  &x, &y, &r);
                if ( res != 3 )
                {
                    errSStr << "Ignored invalid circle on this row : " << s 
                            << std::endl;
                    continue;
                }

                if ( r < 0 )
                    r = 0.0f;

                cr.C.x = x;
                cr.C.y = y;
                cr.R = r;

                if ( NULL != FindCollision(&cr) )
                {
                    errSStr << "Ignored overlapping circle on this row : " 
                            << s << std::endl;
                    continue;
                }

                Circle* crp = new Circle(cr);
                mScene.push_back(crp);

                if ( x-r < minX ) minX = x-r;
                if ( y-r < minY ) minY = y-r;
                if ( x+r > maxX ) maxX = x+r;
                if ( y+r > maxY ) maxY = y+r;

                continue;
            }
        }

        else if ( s.substr(0,2) == "K=" )
        {
            int K;
            int res = sscanf((s.substr(2)).c_str(), "%d", &K);
            if ( (res != 1) || (K < 1) )
            {
                errSStr << "Ignored invalid K on this row : " << s 
                        << std::endl;
                continue;
            }

            mUI->SetK(K);
            continue;
        }

        else if ( s.substr(0,6) == "Scale=" )
        {
            if ( s.find("true", 6) != std::string::npos )
                doScale = true;
            else
                doScale = false;
        }
    }

    if ( doScale )
    {
        // Scale the scene to fit in the rendering frame
        int width = mUI->GetRenderWidth();
        int height = mUI->GetRenderHeight();
        int margin = width / 100; // 1% margin
        width -= 2*margin;
        height -= 2*margin;
        float xScale=1.0f, yScale=1.0f, scale;

        if ( minX < maxX )
            xScale = width / (maxX - minX);

        if ( minY < maxY )
            yScale = height / (maxY - minY);

        scale = (xScale < yScale)? xScale : yScale;

        for ( std::vector<Figure*>::iterator fig=mScene.begin();
              fig != mScene.end(); ++fig )
        {
            Point *ptp = dynamic_cast<Point*>(*fig);
            if ( NULL != ptp )
            {
                ptp->x -= minX;
                ptp->x *= scale;
                ptp->y -= minY;
                ptp->y *= scale;
                ptp->x += margin;
                ptp->y += margin;
                continue;
            }

            Circle *crp = dynamic_cast<Circle*>(*fig);
            if ( NULL !=  crp )
            {
                crp->C.x -= minX;
                crp->C.x *= scale;
                crp->C.y -= minY;
                crp->C.y *= scale;
                crp->C.x += margin;
                crp->C.y += margin;
                crp->R *= scale;
            }
        }
    }

    if (errSStr.str() != "")
        QMessageBox::warning(mUI, "WARNING", errSStr.str().c_str());

    inFile.close();

    update();
}


void RenderingFrame::SaveScene(const char *fileName) const
{
    std::ofstream outFile;
    std::stringstream errSStr;

    outFile.open(fileName, std::ios::out);
    if ( ! outFile.is_open() )
    {
        errSStr << "Unable to open the output file '" << fileName << "'";
        QMessageBox::warning(mUI, "ERROR", errSStr.str().c_str());
        return;
    }

    outFile << std::fixed << std::setprecision(6);

    for ( std::vector<Figure*>::const_iterator fig=mScene.begin();
          fig != mScene.end(); ++fig )
    {
        const Point *pt = dynamic_cast<const Point*>(*fig);
        if ( NULL != pt )
        {
            outFile << std::endl;

            if ( pt == mA )
                outFile << "A= " << pt->x << " " << pt->y << std::endl;
            else if ( pt == mB )
                outFile << "B= " << pt->x << " " << pt->y << std::endl;
            else
                errSStr << "Unknown point in the scene" << std::endl;
        }
    }

    outFile << std::endl << "CirclesBegin" << std::endl;
    for ( std::vector<Figure*>::const_iterator fig=mScene.begin();
          fig != mScene.end(); ++fig )
    {
        const Circle *cr = dynamic_cast<const Circle*>(*fig);
        if ( NULL !=  cr )
        {
            outFile << cr->C.x << " " << cr->C.y << " " << cr->R << std::endl;
        }
    }
    outFile << "CirclesEnd" << std::endl;

    outFile << std::endl << "K=" << mUI->GetK() << std::endl;

    outFile << std::endl << "Scale=false" << std::endl;

    if (errSStr.str() != "")
        QMessageBox::warning(mUI, "WARNING", errSStr.str().c_str());

    outFile.close();
}


void RenderingFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);  // Store it in the class?

    painter.setRenderHint(QPainter::Antialiasing, true);  // Is this safe?

    // Do we have to re-paint everything?

    for ( std::vector<Figure*>::const_iterator fig=mScene.begin();
          fig != mScene.end(); ++fig )
    {
        (*fig)->Draw(&painter);
    }

    for ( std::vector<Ray>::const_iterator iray=mRays.begin();
          iray != mRays.end(); ++iray )
    {
        iray->Draw(&painter);
    }

    QFrame::paintEvent(e);
}


void RenderingFrame::DelFigure(Figure* fig)
{
    if ( NULL == fig )
        return;

    std::vector<Figure*>::iterator ci = std::find( mScene.begin(), mScene.end(),
                                                   fig );
    if ( ci != mScene.end() )
        mScene.erase(ci);
    else
        QMessageBox::warning(mUI, "ERROR", "Figure not found in DelFigure()");  // or throw?

    delete fig;

//  update();
}


Figure* RenderingFrame::FindCollision(const Figure* fig) const
{
    for ( std::vector<Figure*>::const_iterator f=mScene.begin();
          f != mScene.end(); ++f )
    {
        if ( fig == *f )
            continue;

        if ( fig->Distance(*f) <= 0.0f )
            return *f;
    }
    return NULL;
}


void RenderingFrame::mousePressEvent(QMouseEvent * e)
{
    if ( RenderingInProgress() )
    {
        QMessageBox::warning(this, "ERROR", "Rendering is in progress");
        return;
    }

    mMousePressed = true;
    mMousePressPos = e->pos();

    if ( NULL != FindCollision(&mMousePressPos) )
    {
        QMessageBox::warning(mUI, "ERROR", "Fugures shall not overlap!");
        return;
    }

    DrawingMode dMode = mUI->GetDrawingMode();

    switch ( dMode )
    {
        case DM_POINTA:
        case DM_POINTB:
        {
            Point* newPt = new Point(mMousePressPos);  // If this throws nothing changes.
            Point* oldPt = (dMode == DM_POINTA)? mA : mB;

            if ( NULL != oldPt )
                DelFigure(oldPt);

            if ( dMode == DM_POINTA )
                mA = newPt;
            else
                mB = newPt;

            mScene.push_back(newPt);
            break;
        }

        case DM_CIRCLE:
        {
            Circle* cr = new Circle(mMousePressPos, 0);
            mMoseEditFig = cr;
            mScene.push_back(cr);
            break;
        }

        default:
            break;
    }

    mRays.clear();
    update();
}


void RenderingFrame::mouseMoveEvent(QMouseEvent * e)
{
    if ( RenderingInProgress() || (! mMousePressed) )
        return;

    Point mousePos = e->pos();

    switch ( mUI->GetDrawingMode() )
    {
        case DM_POINTA:
        case DM_POINTB:
            break;

        case DM_CIRCLE:
        {
            Circle* cp = dynamic_cast<Circle*>(mMoseEditFig);
            if ( NULL != cp )
            {
                cp->R = mousePos.Distance(&cp->C);
                if ( NULL != FindCollision(cp) )
                {
                    QMessageBox::warning(mUI, "ERROR", "Overlapping figures!");
                    DelFigure(cp);
                    return;
                }
            }
            break;
        }

        default:
            break;
    }

    update();
}


void RenderingFrame::mouseReleaseEvent(QMouseEvent * e)
{
    if ( RenderingInProgress() || (! mMousePressed) )
        return;

    QPoint mouseReleasePos = e->pos();

    switch ( mUI->GetDrawingMode() )
    {
        case DM_POINTA:
        case DM_POINTB:
            break;

        case DM_CIRCLE:
        {
            Circle* cp = dynamic_cast<Circle*>(mMoseEditFig);
            if ( NULL != cp )
            {
                if ( cp->R < mUI->GetMinR() )  // Delete too small circles.
                {
                    if ( cp->R > 0 )
                        QMessageBox::warning(mUI, "ERROR", "Circle is too small");
                    DelFigure(cp);
                }
            }
            mMoseEditFig = NULL;
            break;
        }

        default:
            break;
    }

    mMousePressed = false;

    update();
}


void RenderingFrame::Reset()
{
    mMousePressed = false;
    mMousePressPos = QPoint(0,0);
    mMoseEditFig = NULL;
    mA = mB = NULL;

    for ( std::vector<Figure*>::iterator fig=mScene.begin();
          fig != mScene.end(); ++fig )
    {
        delete *fig;
    }
    mScene.clear();
    mRays.clear();
}


void RenderingFrame::Render()
{
    // Check if mRThread is still alive (which if we are here SHOULD not be the case)?

    mRenderingInProgress = true;

    mRays.clear();  // Delete the previous solutions.
    update();

    int K = mUI->GetK();
    mRThread = new RenderingThread(mA, mB, mScene, K);
    qRegisterMetaType<Ray>("Ray");
    connect(mRThread, SIGNAL(sendRayInResults(Ray)), this, SLOT(addRayInResults(Ray)), Qt::QueuedConnection);
    connect(mRThread, SIGNAL(sendRenderFinished(bool)), this, SLOT(noteRenderFinished(bool)), Qt::QueuedConnection);
    connect(mRThread, &RenderingThread::finished, mRThread, &QObject::deleteLater);  // auto-delete
    mRThread->start();
}


void RenderingFrame::addRayInResults(const Ray& r)
{
    mRays.push_back(r);
#if 0  // Doesn't work
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    r.Draw(&painter);
#else
    update();
#endif // 0
}


void RenderingFrame::noteRenderFinished(bool result)
{
    mRenderingInProgress = false;

    if( !result )
        QMessageBox::warning(mUI, "Info", "No solutions found");

    update();
    // Where it is safe to set mRThread to NULL?
}


void RenderingFrame::StopRendering()
{
    if( NULL != mRThread ) mRThread->requestInterruption();
}


/***************************** RenderingThread ********************************/


void RenderingThread::run()
{
    // TODO: Parse the scene and determine visible surfaces (from point A) then cast rays only to them?

    Circle* target=NULL;
    bool foundSolution=false;

#ifdef DEBUG
    try
#endif // DEBUG
    {

        if ( mK == 0 )
        {
            Ray r( *mA, *mB );  // Ray r( *mA, Vector(*mA, *mB) );

            if ( (foundSolution = RayTrace(&r, mB, mK)) )
            {
                Q_EMIT sendRayInResults(r);
            }
#ifdef DEBUG
            else
            {
                Q_EMIT sendRayInResults(r);
            }
#endif // DEBUG
            Q_EMIT sendRenderFinished(foundSolution);
            return;
        }

#if 0  // This is a waste of time in most cases.
        // First try to find an exact solution - hit point B directly
        for ( unsigned int i=0; i<MAX_NUM_RAYS; i++ )
        {
            int x = ::rand()-RAND_MAX/2;
            int y = ::rand()-RAND_MAX/2;
            if ( (x == 0) && (y == 0) )
                continue;

            Ray r( *mA, Point(x, y) );

            if ( (foundSolution = RayTrace(&r, mB, K)) )
                Q_EMIT sendRayInResults(r);
        }

        // If no exact solution is found try to find approximate solutions.
        if ( ! foundSolution )
#endif // 0
        {
            // Put mB in a circle (a target). The radius will be the precision.
            // It must be less than the minimum distance to all other figures.
            // If a ray hits this circle we consider it an approximate solution.

            float minTargetSize, maxTargetSize=INF_DIST;

            for ( std::vector<Figure*>::const_iterator fig = mScene.begin();
                  fig != mScene.end(); ++fig )
            {
                if ( *fig == mB )
                    continue;

                float dist = mB->Distance(*fig);

                if ( dist < maxTargetSize )
                    maxTargetSize = dist;
            }

            if ( maxTargetSize > MAX_TARGET_SIZE )
            {
                minTargetSize = MIN_TARGET_SIZE;
                maxTargetSize = MAX_TARGET_SIZE;
            }
            else if ( (maxTargetSize < MAX_TARGET_SIZE) &&
                      (maxTargetSize > MIN_TARGET_SIZE) )
            {
                minTargetSize = MIN_TARGET_SIZE;
            }
            else
            {
                minTargetSize = maxTargetSize;
            }

            target = new Circle(*mB, minTargetSize);
            mScene.push_back(target);

            while( (! foundSolution) && (! isInterruptionRequested())
                   && (target->R <= maxTargetSize) )
            {
                // Cast rays from A to various directions and trace them.
                // Remember the rays hitting the target with K reflections.
                // TODO: cast rays only to the figures.

//              ::srand( ::time(NULL) );
                for ( unsigned int i=0; i<MAX_NUM_RAYS; i++ )
                {
                    if( 0 == i%100 )
                    {
                        if( isInterruptionRequested() ) break;
                    }
#if 1  // Random ray.
                    int x = ::rand()-RAND_MAX/2;
                    int y = ::rand()-RAND_MAX/2;
                    if ( (x == 0) && (y == 0) )
                        continue;
                    Ray r( *mA, Point(x, y) );
#else  // Circulate in steps.
                    float angle = 2.0f * M_PI * i / MAX_NUM_RAYS;
                    Ray r( *mA, Vector(cos(angle), sin(angle)) );
#endif // 0
                    if ( RayTrace(&r, target, mK) )  // TODO: Do calculations in a pool of threads?
                    {
                        Q_EMIT sendRayInResults(r);
                        foundSolution = true;
                    }
                }

                target->R += INC_TARGET_SIZE;  // Bigger target is easier to hit.
            }

            // Delete the target circle.
            mScene.pop_back();  // Don't really need this - mScene is a copy.
            delete target;
            target = NULL;

            if ( foundSolution )
            {
                ;  // TODO: Optimize the closest rays to hit exactly mB?
            }
        }

    }
#ifdef DEBUG
    catch(std::runtime_error& e)
    {
        QMessageBox::warning(mUI, "ERROR", e.what());
    }
    catch(...)
    {
        QMessageBox::warning(mUI, "ERROR", "An exception occured");
    }
#endif // DEBUG

    if ( NULL != target )
        delete target;

    Q_EMIT sendRenderFinished(foundSolution);
}


bool RenderingThread::RayTrace( Ray *ray, const Figure* const target, int K )
{
    float dist, minDist=INF_DIST;
    Figure* firstHit=NULL;

    for ( std::vector<Figure*>::const_iterator fig = mScene.begin();
          fig != mScene.end(); ++fig )
    {
        if ( *fig == mA )
            continue;

        if ( *fig == ray->OnFig() )
            continue;  // Skip the figure containing the source.

        if ( (*fig)->Intersect(ray, &dist) )
        {
            if ( dist < minDist )
            {
                minDist = dist;  // TODO: pass minDist to Reflect()
                firstHit = (*fig);
            }
        }
    }

    if ( NULL == firstHit )
    {
#ifdef DEBUG
        ray->Propagate(ray->GetPointAt(100));
#endif // DEBUG
        return false;
    }

    if ( firstHit == target )
    {
#ifdef DEBUG
        ray->Propagate(ray->GetPointAt(minDist));
        return true;
#else
        if ( ray->GetNumberOfReflections() == K )
        {
            ray->Propagate(ray->GetPointAt(minDist));
            return true;
        }
        else
        {
            return false;  // Don't allow repeated hits of the target.
        }
#endif // DEBUG
    }

#ifdef DEBUG
    if ( ray->GetNumberOfReflections() >= MAX_REFLECTIONS )
        return false;
#else
    if ( ray->GetNumberOfReflections() >= K )
        return false;
#endif // DEBUG

    firstHit->Reflect(ray);

    return RayTrace(ray, target, K);  // Recurse...
}

}  // namespace
