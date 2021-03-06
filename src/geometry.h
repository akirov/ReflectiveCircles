/******************************************************************************
 * Copyright: Assen Kirov                                                     *
 ******************************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <vector>
#include <stdexcept>
#include <QPoint>
#include <QPainter>


namespace circles
{

extern const float EPSILON;
extern const float INF_DIST;


inline bool areEqual( const float a, const float b )
{
    return ( fabs(a-b) < ((fabs(a) + fabs(b) + 1.0f)*EPSILON) );
}


class Ray;


/****************************** Figure interface ******************************/

struct Figure
{
    virtual ~Figure() {}

    virtual void Draw( QPainter *painter ) const = 0;

    // This is used to check for ovelapping.
    virtual float Distance( const Figure* other ) const = 0;

    // Checks if the ray hits the Figure, returns true if yes, returns the distance.
    virtual bool Intersect( const Ray* ray, float* distance ) const = 0;

    // Reflects an intersecting ray.
    virtual void Reflect( Ray* ray /*, float distance */ ) const = 0;

    // Add Scale(minX, minY, scale, margin) method?

    // Add Serialize(ostream) method?

    // Add enum Type and GetType() method?
};


/*********************************** Point ************************************/

/* A point is described with a radius (or position) vector, different from the
 * free vector below. We don't want to add points... */
struct Point : public Figure
{
    Point( float x_, float y_ ) : x(x_), y(y_) {}
    Point( QPoint qp ) : x(qp.x()), y(qp.y()) {}
    // Default copy constructor and assignment operator.
    ~Point() {}

    operator QPointF() const { return QPointF(x, y); }
//  operator QPoint() const { return QPoint(static_cast<int>(x), static_cast<int>(y)); }

    void Draw( QPainter *painter ) const ;
    float Distance( const Figure* other ) const;
    bool Intersect( const Ray* ray, float* distance ) const;
    void Reflect( Ray* ray ) const;
 
    float x;
    float y;
    // Add color?
    // Add string label?
};


inline bool operator==(const Point& lhs, const Point& rhs)
{
    return ( areEqual(lhs.x, rhs.x) && areEqual(lhs.y, rhs.y) );
}


inline bool operator!=(const Point& lhs, const Point& rhs)
{
    return (! operator==(lhs,rhs));
}


/*********************************** Circle ***********************************/

struct Circle : public Figure
{
    Circle( float cx_, float cy_, float r_ ) : C(cx_, cy_), R(r_) {}
    Circle( Point c_, float r_ ) : C(c_), R(r_) {}
    // Default copy constructor and assignment operator.
    ~Circle() {}

    void Draw( QPainter *painter ) const;
    float Distance( const Figure* other ) const;
    bool Intersect( const Ray* ray, float* distance ) const;
    void Reflect( Ray* ray ) const;

    Point C;  // Center
    float  R;  // Radius
    // Add color?
};


/*********************************** Vector ***********************************/

inline float Module( float x, float y )
{
    return sqrt( x*x + y*y );
}


/* This class describes a free (or direction) vector. We don't want to draw it. */
class Vector
{
  public:
    explicit Vector( float x_, float y_ ) : x(x_), y(y_) {}
    explicit Vector( const Point& p ) : x(p.x), y(p.y) {}  // From a radius-vector.
    explicit Vector( const Point& pa, const Point& pb) : x(pb.x - pa.x), y(pb.y - pa.y) {}  // From point A to B.
    Vector( const Vector& other ) : x(other.x), y(other.y) {}  // As the default.
    ~Vector() {}

    Vector& operator=( const Vector& rhs )
    {
        if ( this != &rhs )
        {
            x = rhs.x;
            y = rhs.y;
        }
        return *this;
    }

    Vector& operator +=(const Vector& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector& operator -=(const Vector& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector& operator *=(float c)
    {
        x *= c;
        y *= c;
        return *this;
    }

    double Norm() const
    {
        return Module(x, y);
    }

    float GetX() const { return x; }
    float GetY() const { return y; }

    void Normalize()
    {
        float len = Module(x, y);
        if ( len != 0.0 )  // Or less than the epsilon?
        {
            x /= len;
            y /= len;
        }
    }

    float ScalarProduct( const Vector& other ) const
    {
        return (x * other.x  +  y * other.y);
    }

  private:
    float x;
    float y;
};


inline const Vector operator +( Vector lhs, const Vector& rhs )
{
    lhs += rhs;
    return lhs;
}


inline const Vector operator -( Vector lhs, const Vector& rhs )
{
    lhs -= rhs;
    return lhs;
}


inline const Vector operator *( Vector lhs, float c )
{
    lhs *= c;
    return lhs;
}


inline const Vector operator *( float c, Vector rhs )
{
    rhs *= c;
    return rhs;
}


/************************************* Ray ************************************/

class Ray
{
  public:
    /* Default values are only to satisfy Qt's requirement for signal arguments
     * to have default constructor! */
    Ray ( Point s=Point(0,0), Vector d=Vector(1,0) ) : src(s), dir(d), onFig(NULL), trace()
    {
        if ( dir.Norm() < EPSILON )
            throw std::runtime_error("Ray with no direction!");

        dir.Normalize();
    }

    Ray ( Point s, Point d ) : src(s), dir(Vector(s,d)), onFig(NULL), trace()  // From point S to D.
    {
        if ( dir.Norm() < EPSILON )
            throw std::runtime_error("Ray with no direction!");

        dir.Normalize();
    }

    Ray ( const Ray& other ) : 
            src(other.src), 
            dir(other.dir), 
            onFig(other.onFig),
            trace(other.trace)
    {
    }

    ~Ray() {}

    Ray& operator=( const Ray& rhs )
    {
        if ( this != &rhs )
        {
            src = rhs.src;
            dir = rhs.dir;  // Normalize?
            onFig = rhs.onFig;
            trace = rhs.trace;
        }
        return *this;
    }

    Point GetSrc() const { return src; }
    Vector GetDir() const { return dir; }
    void SetDir( const Vector& d ) { dir = d; }
    const Figure* OnFig() { return onFig; }
    void SetOnFig( const Figure* fig ) { onFig = fig; }
    int GetNumberOfReflections() const { return trace.size(); }

    Point GetPointAt( float t ) const
    {
//      Vector v = Vector(src) + t*dir; return Point(v.x, v.y);
        return Point(src.x + t*dir.GetX(), src.y + t*dir.GetY());
    }

    void Draw(QPainter *painter) const;

    // Doesn't change the direction, just go forward (pt should be on the ray).
    void Propagate ( const Point& pt )
    {
        trace.push_back(src);
        src = pt;
    }

  private:
    Point               src;   // Current source
    Vector              dir;   // Current direction
    const Figure*       onFig; // The last reflection figure (src is on it now)
    std::vector<Point>  trace; // Initial source and the next reflection points
//  const Vector        origDir; // Initial direction
};

}  // namespace

#endif // GEOMETRY_H
