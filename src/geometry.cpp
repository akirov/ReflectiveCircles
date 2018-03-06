/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#include <limits>
#include "geometry.h"


namespace circles
{

const float EPSILON  = 0.01f;
const float INF_DIST = std::numeric_limits<float>::max();


/*********************************** Point ************************************/

void Point::Draw(QPainter *painter) const
{
#if 0
    painter->setPen(Qt::black);
    painter->drawPoint(x, y);
#else
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
    painter->drawEllipse(QPointF(x, y), 2, 2);
#endif // 0
}


float Point::Distance( const Figure* other ) const
{
    const Point *pt = dynamic_cast<const Point*>(other);
    if ( NULL != pt )
        return Module( (x - pt->x), (y - pt->y) );

    const Circle *cr = dynamic_cast<const Circle*>(other);
    if ( NULL !=  cr )
        return (Module((x - cr->C.x), (y - cr->C.y)) - cr->R);

    return INF_DIST;
}


bool Point::Intersect( const Ray* ray, float* distance ) const
{
    if ( ray->GetSrc() == *this )
    {
        *distance = 0.0f;
        return true;
    }

#if 1
    // normv = (P - S) - ((P - S).dir)*dir
    Vector pms(ray->GetSrc(), *this);
    float pmsdir = pms.ScalarProduct(ray->GetDir());

    if ( pmsdir < 0.0f )  // The oposite direction of the ray
    {
        *distance = INF_DIST;
        return false;
    }
    else
    {
        Vector normv = pms - pmsdir * ray->GetDir();
        if ( normv.Norm() < EPSILON )
        {
            *distance = pms.Norm();
            return true;
        }
        else
        {
            *distance = INF_DIST;
            return false;
        }
    }
#else
    if ( ray->dir.x == 0.0f )  // Or fabs less than epsilon?
    {
        
    }
    else if ( ray->dir.y == 0.0f )
    {
        
    }
    else
    {
        float tx = (x - ray->src.x) / ray->dir.x;
        float ty = (y - ray->src.y) / ray->dir.y;
        if ( (fabs(tx - ty) < EPSILON) && (tx > 0) )  // The second condition is for the direction.
        {
            *distance = tx /* * ray->dir.Norm() */;  // Direction is normalized!
            return true;
        }
        else
        {
            *distance = INF_DIST;
            return false;
        }
    }
#endif // 0
}


void Point::Reflect( Ray* ray ) const
{
    ray->Propagate(*this);
    // The direction is not changed.
    ray->SetOnFig(this);
}


/*********************************** Circle ***********************************/

void Circle::Draw(QPainter *painter) const
{
    painter->setPen(Qt::blue);
    painter->drawPoint(C);
    painter->setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    painter->setBrush(QBrush());  // Or fill it?
    if ( R > 0 )
    {
#if 0
        painter->drawArc(C.x-R, C.y-R, 2*R, 2*R, 0, 16*360);
#else
        painter->drawEllipse(QPointF(C.x, C.y), R, R);
#endif // 0
    }
}


float Circle::Distance( const Figure* other ) const
{
    const Point *pt = dynamic_cast<const Point*>(other);
    if ( NULL != pt )
        return (Module((pt->x - C.x), (pt->y - C.y)) - R);

    const Circle *cr = dynamic_cast<const Circle*>(other);
    if ( NULL !=  cr )
        return (Module((C.x - cr->C.x), (C.y - cr->C.y)) - R - cr->R);

    return INF_DIST;
}


bool Circle::Intersect( const Ray* ray, float* distance ) const
{
    // In the intersection point P = Src + t*Dir we have : (P-C).(P-C) = R^2.
    // We will find the t parameter by solving a quadratic equation.

    float a = 1.0f; // Direction is normalized! Otherwise : ray->dir.ScalarProduct( ray->dir );
    Vector smc(C, ray->GetSrc());  // (Src - C)
    float b = 2 * ray->GetDir().ScalarProduct(smc);  // 2 Dir.(Src-C)
    float c = smc.ScalarProduct(smc) - R*R;  // (Src-C).(Src-C) - r^2
    float D = b*b - 4*a*c;  // Discriminant

    if ( D < 0 )  // No solutions
    {
        *distance = INF_DIST;
        return false;
    }
    else
    {
        if( D < EPSILON )  // One solution. The ray is tangent to the circle.
        {
            float t = - b/(2*a);
            if ( t < 0 )  // The oposite direction of the ray
            {
                *distance = INF_DIST;
                return false;
            }
            else
            {
                *distance = t; // Direction is normalized!
                return true;
            }
        }
        else  // Two solutions. Take the smaller one.
        {
            D = sqrt(D);
            float t1 = (-b + D)/(2*a);
            float t2 = (-b - D)/(2*a);

            if ( (t1 > 0) && (t2 > 0) )
            {
                *distance = (t1 < t2)? t1 : t2; // Direction is normalized!
                return true;
            }
            else
            {
                // Wrong direction or invalid ray source. What about 0?
#ifdef DEBUG
                if ( ((t1 < 0) && (t2 > 0)) || ((t1 > 0) && (t2 < 0)) )
                    QMessageBox::warning(0, "ERROR", "Ray source inside of a circle!");
#endif // DEBUG
                *distance = INF_DIST;
                return false;
            }
        }
    }
}


void Circle::Reflect( Ray* ray ) const
{
    float dist;
    if ( this->Intersect(ray, &dist) )  // TODO: Don't do this again.
    {
        Point P = ray->GetPointAt(dist);
#ifdef DEBUG
        if ( R  > Module(P.x-C.x, P.y-C.y) )
            QMessageBox::warning(0, "ERROR", "Reflection point inside a circle!");
#endif // DEBUG

        ray->Propagate(P);

        // Reflect the direction
        Vector n(C, P);  // Normal vector
        n.Normalize();

        // r = Dir - 2(n.Dir)n , should be normalized.
        ray->SetDir( ray->GetDir() - ((2*n.ScalarProduct(ray->GetDir())) * n) );

        ray->SetOnFig(this);
    }
}


/************************************* Ray ************************************/

void Ray::Draw(QPainter *painter) const
{
    if ( trace.size() == 0 )
        return;

    painter->setPen(QPen(Qt::darkYellow, 1, Qt::SolidLine));

    unsigned int i;
    for ( i=1; i<trace.size(); ++i )
    {
        painter->drawLine(trace[i-1], trace[i]);
    }
    painter->drawLine(trace[i-1], src);
}

}  // namespace
