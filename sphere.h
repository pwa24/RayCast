/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include "vector.h"

struct Material {
  float ambient[3];    // material property used in Phong model
  float diffuse[3];
  float specular[3];
  float shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  float transparency;      // [0,1] how transparent object is. 0 if object not transparent
  float ref_index;         // index of refraction

};

typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  Point center;
  float radius;
  
  struct Material mat;
  struct sphere *next;
} Spheres;   // a list of spheres


// chessboard
// Note: this definition is strictly for chessboards with normal = (0.0, 1.0, 0.0)
typedef struct chessboard {
  //represented as a section of a plane in (x,y,z)
  Point center;
  Vector normal;

  float length;
  float xmax;
  float xmin;
  float zmax;
  float zmin;

  int numDivisions;
  struct Material blackmat;
  struct Material whitemat;

} ChessBoard;

// intersect ray with chessboard
int intersect_chess(Point, Vector, ChessBoard *, Point *);
// intersect ray with sphere
Spheres *intersect_scene(Point, Vector, Spheres *, Point *);
// return the unit normal at a point on sphere
Vector sphere_normal(Point, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, Point, float, float [], float [], float [], float, float, float, float, int);

