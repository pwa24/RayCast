#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "global.h"
#include "sphere.h"

#include <iostream>
using namespace std;

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;
extern ChessBoard CB;

// light 1 position and color
extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

// maximum level of recursion
extern int step_max;

// optional flags
extern bool shadow_on;
extern bool reflection_on;
extern bool refraction_on;
extern bool chess_pattern_on;
extern bool diffuse_rend_on;
extern bool super_samp_on;

const double PI = 3.14159265358979;

/////////////////////////////////////////////////////////////////////

//generates a uniformly random vector within the hemisphere around the vector n
Vector random_ray(Vector n) {

  float u = ((float) rand() / (RAND_MAX)) * 2 - 1;    //generate uniform rnd # in [-1,1]
  float t = ((float) rand() / (RAND_MAX)) * PI;       //generate uniform rnd # in [0,2pi]
  float v = ((float) rand() / (RAND_MAX));            //generate uniform rnd # in [0,1]

  float x = sqrt(1-u*u)*cos(u);
  float y = sqrt(1-u*u)*sin(t);
  float z = v;

  Point p = {x,y,z};
  //create orthonormal basis from n
  Vector a;
  if (abs(n.z) < 0.0001 && abs(n.y) < 0.0001) {
    a = {-n.z, 0.0, n.x};
  } else {
    a = {0.0, -n.z, n.y};
  }

  Vector b = vec_cross(n,a);
  normalize(&a);
  normalize(&b);
  normalize(&n);
  //change of basis from (x,y,z) to (a,b,n)
  Vector ret;
  ret.x = a.x*p.x + b.x*p.y + n.x*p.z;
  ret.y = a.y*p.x + b.y*p.y + n.y*p.z;
  ret.z = a.z*p.x + b.z*p.y + n.z*p.z;
  return ret;
}


//returns the refracted ray of ray v hitting a point with surface normal n
//index of refraction of incoming ray v = nl (1 for air)
//index of refraction of outgoing ray t = nt
Vector get_refract(Vector n, Vector v, float nl, float nt) {
  v = vec_scale (v, -1.0);
  normalize(&n);
  normalize(&v);
  float nr = nl/nt;
  Vector t;

  //solving snell's equation in terms of t yields:
  //t = ( nr*(n.v) - sqrt(1 - nr^2*(1-(n.v)^2)) ) * n - nr*v
  float n_dot_v = vec_dot(n, v);
  float root_part = sqrtf( 1 - nr*nr*( 1 - n_dot_v*n_dot_v ) );
  
  if (root_part >= 0.0) {
    t = vec_minus( vec_scale(n, nr*n_dot_v - root_part), vec_scale(v, nr) );
  } else {
    cout << " negative root" << endl;
  }

  return t;
}

//returns the reflected ray of ray v hitting a point with surface normal n
Vector get_reflect(Vector n, Vector v) {
  v = vec_scale(v, -1.0);
  return vec_minus( vec_scale( n, 2*(vec_dot(n,v)) ) , v ); //r=2(n*v)n-v
}

/*********************************************************************
 * Phong illumination - you need to implement this!
   q = point on surface
   n = normal vector to q
   sph = sphere 
 *********************************************************************/
RGB_float phong(Point q, Vector n, struct Material *mat) {
	RGB_float color;

  Vector l = get_vec(q, light1);      //q to light source
  Vector v = get_vec(q, eye_pos);     //q to eye
  Vector r = get_reflect(n, vec_scale(l, -1.0));    //reflected ray of light
  float d = vec_len(l);   //distance from q to light source
  int d_exist;    //existance of diffuse light

  //normalize vectors
  normalize(&l);
  normalize(&n);
  normalize(&v);
  normalize(&r);

  if (vec_dot(n,l) > 0) {   //there exists diffuse light
    d_exist = 1;
  } else {    //there is no diffuse light
    d_exist = 0;
  }

  float f_att = 1 / ( decay_a + decay_b*d + decay_c*d*d );  //attenuation factor

  RGB_float I_ga = {global_ambient[0], global_ambient[1], global_ambient[2]};
  float k_ga = mat->reflectance;
  RGB_float I_a = {light1_ambient[0], light1_ambient[1], light1_ambient[2]};
  RGB_float k_a = {mat->ambient[0], mat->ambient[1], mat->ambient[2]};
  RGB_float I_d = {light1_diffuse[0], light1_diffuse[1], light1_diffuse[2]};
  RGB_float k_d = {mat->diffuse[0], mat->diffuse[1], mat->diffuse[2]};
  RGB_float I_s = {light1_specular[0], light1_specular[1], light1_specular[2]};
  RGB_float k_s = {mat->specular[0], mat->specular[1], mat->specular[2]};
  float N = mat->shineness;

  //Calculation

  color.r = (I_ga.r * k_ga) + (I_a.r * k_a.r) + f_att * (d_exist * I_d.r * k_d.r *vec_dot(n,l) + I_s.r * k_s.r * pow(vec_dot(r,v), N) );
  color.g = (I_ga.g * k_ga) + (I_a.g * k_a.g) + f_att * (d_exist * I_d.g * k_d.g *vec_dot(n,l) + I_s.g * k_s.g * pow(vec_dot(r,v), N) );
  color.b = (I_ga.b * k_ga) + (I_a.b * k_a.b) + f_att * (d_exist * I_d.b * k_d.b *vec_dot(n,l) + I_s.b * k_s.b * pow(vec_dot(r,v), N) );

	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point p, Vector ray, int step) {

  RGB_float Clocal, Creflected, Ctransmitted, Cdiff;
  Creflected = {0.0, 0.0, 0.0};
  Ctransmitted = {0.0, 0.0, 0.0};
  Cdiff = {0.0, 0.0, 0.0};
  Vector n,l,r,t;         //normal vector, shadow ray, reflect ray, refract ray
  Point q;                //point of intersection (if any)
  struct Material q_mat;  //material properies of q
  bool isInShadow = 0;    //is q part of a shadow
  bool intersect = 0;     //ray intersects sphere or chessboard
  int chessTile = 0;      //0 = no intersection of chessboard, 1 = intersect black tiles, 2 = intersect white tiles

  if (step > step_max) return background_clr;

  //intersect with spheres
  Spheres * sph = intersect_scene(p, ray, scene, &q);

  if (sph!= NULL) {                   //intersection at a sphere
    
    n = sphere_normal(q, sph);        //q normal vector
    l = get_vec( q, light1);          //q to light, shadow ray
    q_mat = sph->mat;                 //set material of q
    intersect = 1;

  } else if (chess_pattern_on && sph == NULL) {     //try intersect with chessboard if no sphere intersection
    chessTile = intersect_chess(p, ray, &CB, &q);
    if (chessTile != 0) {                           //intersection at chessboard

      n = CB.normal;                           //q normal vector
      l = get_vec( q, light1);                 //q to light, shadow ray
      if (chessTile == 1) q_mat = CB.blackmat;  //set black chess tile material for q
      if (chessTile == 2) q_mat = CB.whitemat;  //set white chess tile material for q
      intersect = 1;

    }
  }

  if (intersect == 1) { //there is an intersection
    
    if (shadow_on) {  //note: we can cheat here and dont worry about shadow rays intersecting chessboard since 
                      //shadow rays don't intersect with chessboard (it is furthest away from light source)
      Point qq; //dummy point
      if (intersect_scene( pt_nudge(q,n) , l, scene, &qq) != NULL)
        isInShadow = 1;
    }

    if (isInShadow) { //cast shadow (color used is ambiance only)
      RGB_float I_ga = {global_ambient[0], global_ambient[1], global_ambient[2]};
      float k_ga = q_mat.reflectance;
      RGB_float I_a = {light1_ambient[0], light1_ambient[1], light1_ambient[2]};
      RGB_float k_a = {q_mat.ambient[0], q_mat.ambient[1], q_mat.ambient[2]};
      Clocal.r = (I_ga.r * k_ga) + (I_a.r * k_a.r);
      Clocal.g = (I_ga.g * k_ga) + (I_a.g * k_a.g);
      Clocal.b = (I_ga.b * k_ga) + (I_a.b * k_a.b);
    } else {          //use phong shading for LIM at q
      Clocal = phong(q, n, &q_mat);
    }

    if (reflection_on) {  //&& !isInShadow
      r = get_reflect( n, ray );     //perfect reflection vector
      Creflected = clr_scale( recursive_ray_trace( pt_nudge(q,r) , r, step + 1) , q_mat.reflectance );
    }

    //stochastic ray generation for diffuse effects
    if (diffuse_rend_on) {
      for (int i=0; i<NUM_DIFF_RAYS; i++) {
        Vector d_ray = random_ray(n);
        RGB_float d_color = clr_scale( recursive_ray_trace( pt_nudge(q,n) , d_ray, step + 1) , q_mat.reflectance );
        Cdiff = clr_scale(clr_add(Cdiff, d_color), 1.0/NUM_DIFF_RAYS);
      }
    }

    if (refraction_on && (q_mat.transparency > 0.0)) {  //refract if transparent object

      //first refract ray going in sphere
      if (vec_dot(n, ray) < 0.0) {                      //ray going inside object
        t = get_refract( n, ray, 1.0, q_mat.ref_index);
      } else {                                          //ray coming out object
        t = get_refract( vec_scale(n, -1.0), ray, q_mat.ref_index, 1.0);
        cout << "refraction inside out error";
      }

        Point qq;
        ray = t;
        Spheres * sph2 = intersect_scene( pt_nudge(q,ray), ray, scene, &qq );

        //second refract ray going out sphere
        if (sph2 != NULL) {
          n = sphere_normal(qq, sph2);
          q_mat = sph2->mat;
          if (vec_dot(n, ray) < 0.0) {                      //ray going inside object
            t = get_refract( n, ray, 1.0, q_mat.ref_index);
            cout << "refraction inside out error";
          } else {                                          //ray coming out object
            t = get_refract( vec_scale(n, -1.0), ray, q_mat.ref_index, 1.0);
          }
          Ctransmitted = clr_scale( recursive_ray_trace( pt_nudge(qq,t) , t, step + 1) , q_mat.transparency);
        } else {
          cout << "refraction ray calculation error" << endl;
          return background_clr;
        }
    }

  } else {

    return background_clr;
  }

  RGB_float color;
  color.r = Clocal.r + Creflected.r + Ctransmitted.r + Cdiff.r;
  color.g = Clocal.g + Creflected.g + Ctransmitted.g + Cdiff.g;
  color.b = Clocal.b + Creflected.b + Ctransmitted.b + Cdiff.b;

	return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {

  srand(time(0));
  int i, j, k;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  int numSamples = 5;
  Vector ray[numSamples];
  float x,y,z,xl,xr,yu,yd;

  if (!super_samp_on) {
    numSamples = 1;
  }

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {

      x = cur_pixel_pos.x;
      y = cur_pixel_pos.y;
      z = cur_pixel_pos.z;
      
      ray[0] = get_vec(eye_pos, {x,y,z});

      if (super_samp_on) {
        xl = cur_pixel_pos.x - 0.25*x_grid_size;
        xr = cur_pixel_pos.x + 0.25*x_grid_size;
        yu = cur_pixel_pos.y - 0.25*y_grid_size;
        yd = cur_pixel_pos.y + 0.25*y_grid_size;
        ray[1] = get_vec(eye_pos, {xl,yu,z});
        ray[2] = get_vec(eye_pos, {xl,yd,z});
        ray[3] = get_vec(eye_pos, {xr,yu,z});
        ray[4] = get_vec(eye_pos, {xr,yd,z});
      }
      
      ret_color = {0.0,0.0,0.0};
      for (k=0; k<numSamples; k++) {
        ret_color = clr_add( ret_color, recursive_ray_trace(eye_pos, ray[k], 0) );
      }      
      ret_color = clr_scale( ret_color, 1.0/numSamples );

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
