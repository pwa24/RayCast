#include "sphere.h"
#include <stdlib.h>
#include <math.h>

#include <iostream>
using namespace std;

//Intersect chessboard with ray at point p
//return 0 - no intersection
//return 1 - white square
//return 2 - black square
int intersect_chess(Point p, Vector ray, ChessBoard * cb, Point *hit) {
  Point cp = cb->center;
  Vector cn = cb->normal;
  float t = 0;;

  normalize(&ray);
  normalize(&cn);

  Point ihit;

  float d = vec_dot(ray, cn);
  if (d > 1e-4) {   //no intersection with plane
    return 0;
  } else {          //intersection with plane
    t = vec_dot( get_vec(p, cp), cn) / d;
    ihit = get_point(p, vec_scale(ray, t));

    bool c1 = (ihit.x < cb->xmax);
    bool c2 = (ihit.x > cb->xmin);
    bool c3 = (ihit.z < cb->zmax);
    bool c4 = (ihit.z > cb->zmin);

    if (c1 && c2 && c3 && c4) { //ihit is within bounds of chessboard
      *hit = ihit;
      
      //determine black or white tile
      float x = ihit.x - cb->xmin;
      float z = ihit.z - cb->zmin;
      
      int xx = (int) ((x/cb->length)*(cb->numDivisions)) + 1; //1 <= xx <= 8
      int zz = (int) ((z/cb->length)*(cb->numDivisions)) + 1; //1 <= zz <= 8

      int type = (xx + zz) % 2 + 1;

      return type;
    } else {
      return 0;
    }

  }
  
}


/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {

  float depth = -1.0;
  normalize(&u);  //u is a unit vector

  Point c = sph->center;
  float r = sph->radius;

  Vector oc = get_vec(o,c);  //vector between o and center of sphere

  if (vec_dot(oc, u) < 0) {   //center of sphere is behind o
    
    if (vec_len(oc) < r) {    //o is inside the sphere 
      cout << "behind\n";
      Vector p_oc_u = vec_scale( u, vec_dot(oc,u)/vec_len(u) );  //projection of vector oc on vector u
      Point pc = get_point(o, p_oc_u);  //o + p_oc_u
                  
      float a = vec_len(get_vec(pc,c));   //|pc-c|
      float b = vec_len(get_vec(pc,o));   //|pc-o|
      float dist = sqrt(r*r - a*a);  //distance from pc to first intersection
      depth = dist - b;
      *hit = get_point(o , vec_scale(u,depth));

    } else if (vec_len(oc) == r) { //p0 lies on edge of sphere
      cout << "exact\n";
      *hit = o;
    }

  } else {  //center of sphere is in front of o

    Vector p_oc_u = vec_scale( u, vec_dot(oc,u)/vec_len(u) );  //projection of vector oc on vector u
    Point pc = get_point(o, p_oc_u);  //point on u normal to c

    float a = vec_len(get_vec(pc,c));   //|pc-c|
    if (a < r) { //intersection

      float dist = sqrt(r*r - a*a);  //distance from pc to first intersection

      if (vec_len(oc) > r) {    //origin is outside sphere, ray going in
        depth = vec_len(get_vec(pc,o)) - dist;
      } else {                  //origin is inside sphere, ray going out
        depth = vec_len(get_vec(pc,o)) + dist;
      }

      *hit = get_point(o , vec_scale(u,depth));
    }
  }

  return depth;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point p, Vector ray, Spheres *scene, Point *hit) {

  float depth, minDepth = 100000.0;
  Spheres * firstSphere = NULL;
  Point ihit;

  while(scene != NULL) {
      depth = intersect_sphere(p, ray, scene,&ihit);
      if (depth >= 0.0 && depth < minDepth) {
        minDepth = depth;
        firstSphere = scene;
        *hit = ihit;
      }

      scene = scene->next;

  }

	return firstSphere;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, float trans, float ref_index, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat.ambient)[0] = amb[0];
  (new_sphere->mat.ambient)[1] = amb[1];
  (new_sphere->mat.ambient)[2] = amb[2];
  (new_sphere->mat.diffuse)[0] = dif[0];
  (new_sphere->mat.diffuse)[1] = dif[1];
  (new_sphere->mat.diffuse)[2] = dif[2];
  (new_sphere->mat.specular)[0] = spe[0];
  (new_sphere->mat.specular)[1] = spe[1];
  (new_sphere->mat.specular)[2] = spe[2];
  new_sphere->mat.shineness = shine;
  new_sphere->mat.reflectance = refl;
  new_sphere->mat.transparency = trans;
  new_sphere->mat.ref_index = ref_index;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}