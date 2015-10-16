//
// this provide functions to set up the scene
//
#include "sphere.h"
#include <stdio.h>

extern Point light1;
extern float light1_ambient[3];
extern float light1_diffuse[3];
extern float light1_specular[3];

extern float global_ambient[3];
extern Spheres *scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern Point eye_pos;

extern ChessBoard CB;

//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE //added transparency variables
 *******************************************/
void set_up_default_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // setup global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // setup light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_ambient[0] = light1_ambient[1] = light1_ambient[2] = 0.1;
  light1_diffuse[0] = light1_diffuse[1] = light1_diffuse[2] = 1.0;
  light1_specular[0] = light1_specular[1] = light1_specular[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  Point sphere1_ctr = {1.5, -0.2, -3.2};
  float sphere1_rad = 1.23;
  float sphere1_ambient[] = {0.7, 0.7, 0.7};
  float sphere1_diffuse[] = {0.1, 0.5, 0.8};
  float sphere1_specular[] = {1.0, 1.0, 1.0};
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transparency = 0.0;
  float sphere1_ref_index = 1.0;
  scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
		     sphere1_reflectance, sphere1_transparency, sphere1_ref_index, 1);

  // sphere 2
  Point sphere2_ctr = {-1.5, 0.0, -3.5};
  float sphere2_rad = 1.5;
  float sphere2_ambient[] = {0.6, 0.6, 0.6};
  float sphere2_diffuse[] = {1.0, 0.0, 0.25};
  float sphere2_specular[] = {1.0, 1.0, 1.0};
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transparency = 0.0;
  float sphere2_ref_index  = 1.0;
  scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
		     sphere2_reflectance, sphere2_transparency, sphere2_ref_index, 2);

  // sphere 3
  Point sphere3_ctr = {-0.35, 1.75, -2.25};
  float sphere3_rad = 0.5;
  float sphere3_ambient[] = {0.2, 0.2, 0.2};
  float sphere3_diffuse[] = {0.0, 1.0, 0.25};
  float sphere3_specular[] = {0.0, 1.0, 0.0};
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transparency = 0.0;
  float sphere3_ref_index  = 1.0;
  scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
             sphere3_diffuse, sphere3_specular, sphere3_shineness,
		     sphere3_reflectance, sphere3_transparency, sphere3_ref_index, 3);

  // chessboard setup
  CB.center = {0.0, -2.0, -4.0};
  CB.normal = {0.0, 1.0, 0.0};
  CB.length = 5.0;
  CB.xmax = CB.center.x + (CB.length/2.0);
  CB.xmin = CB.center.x - (CB.length/2.0);
  CB.zmax = CB.center.z + (CB.length/2.0);
  CB.zmin = CB.center.z - (CB.length/2.0);
  CB.numDivisions = 8;

    //material properties
    CB.blackmat.ambient[0] = 0.0; CB.blackmat.ambient[1] = 0.0; CB.blackmat.ambient[2] = 0.0;
    CB.blackmat.diffuse[0] = 0.0; CB.blackmat.diffuse[1] = 0.0; CB.blackmat.diffuse[2] = 0.0;
    CB.blackmat.specular[0] = 1.0; CB.blackmat.specular[1] = 1.0; CB.blackmat.specular[2] = 1.0;
    CB.blackmat.shineness = 50;
    CB.blackmat.reflectance = 0.5;
    CB.blackmat.transparency = 0.0;
    CB.blackmat.ref_index = 0.0;

    CB.whitemat.ambient[0] = 0.0; CB.whitemat.ambient[1] = 0.0; CB.whitemat.ambient[2] = 0.0;
    CB.whitemat.diffuse[0] = 1.0; CB.whitemat.diffuse[1] = 1.0; CB.whitemat.diffuse[2] = 1.0;
    CB.whitemat.specular[0] = 1.0; CB.whitemat.specular[1] = 1.0; CB.whitemat.specular[2] = 1.0;
    CB.whitemat.shineness = 50;
    CB.whitemat.reflectance = 0.5;
    CB.whitemat.transparency = 0.0;
    CB.whitemat.ref_index = 0.0;

}

/***************************************
 * My Transluscent/Refraction Scene with 3 Balls
 ***************************************/
void set_up_user_scene() {
  // set background color
  background_clr.r = 0.5;
  background_clr.g = 0.05;
  background_clr.b = 0.8;

  // setup global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // setup light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_ambient[0] = light1_ambient[1] = light1_ambient[2] = 0.1;
  light1_diffuse[0] = light1_diffuse[1] = light1_diffuse[2] = 1.0;
  light1_specular[0] = light1_specular[1] = light1_specular[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  Point sphere1_ctr = {-0.7, -0.2, -3.0};
  float sphere1_rad = 1.4;
  float sphere1_ambient[] = {0.7, 0.7, 0.7};
  float sphere1_diffuse[] = {0.1, 0.5, 0.8};
  float sphere1_specular[] = {1.0, 1.0, 1.0};
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transparency = 0.8;
  float sphere1_ref_index  = 1.01;
  scene = add_sphere(scene, sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
         sphere1_reflectance, sphere1_transparency, sphere1_ref_index, 1);

  // sphere 2
  Point sphere2_ctr = {-0.2, -1.0, -5.5};
  float sphere2_rad = 1.0;
  float sphere2_ambient[] = {0.6, 0.6, 0.6};
  float sphere2_diffuse[] = {1.0, 0.0, 0.25};
  float sphere2_specular[] = {1.0, 1.0, 1.0};
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transparency = 0.8;
  float sphere2_ref_index  = 1.01;
  scene = add_sphere(scene, sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
         sphere2_reflectance, sphere2_transparency, sphere2_ref_index, 2);

  // sphere 3
  Point sphere3_ctr = {-0.5, 3.0, -8.5};
  float sphere3_rad = 2.0;
  float sphere3_ambient[] = {0.2, 0.2, 0.2};
  float sphere3_diffuse[] = {0.0, 1.0, 0.25};
  float sphere3_specular[] = {0.0, 1.0, 0.0};
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transparency = 0.8;
  float sphere3_ref_index  = 1.01;
  scene = add_sphere(scene, sphere3_ctr, sphere3_rad, sphere3_ambient,
             sphere3_diffuse, sphere3_specular, sphere3_shineness,
        sphere3_reflectance, sphere3_transparency, sphere3_ref_index, 3);

  // chessboard setup
  CB.center = {0.0, -2.0, -4.0};
  CB.normal = {0.0, 1.0, 0.0};
  CB.length = 5.0;
  CB.xmax = CB.center.x + (CB.length/2.0);
  CB.xmin = CB.center.x - (CB.length/2.0);
  CB.zmax = CB.center.z + (CB.length/2.0);
  CB.zmin = CB.center.z - (CB.length/2.0);
  CB.numDivisions = 8;

    //material properties
    CB.blackmat.ambient[0] = 0.0; CB.blackmat.ambient[1] = 0.0; CB.blackmat.ambient[2] = 0.0;
    CB.blackmat.diffuse[0] = 0.0; CB.blackmat.diffuse[1] = 0.0; CB.blackmat.diffuse[2] = 0.0;//not needed
    CB.blackmat.specular[0] = 1.0; CB.blackmat.specular[1] = 1.0; CB.blackmat.specular[2] = 1.0; //not needed
    CB.blackmat.shineness = 50;  //not needed
    CB.blackmat.reflectance = 0.5;
    CB.blackmat.transparency = 0.0;
    CB.blackmat.ref_index = 0.0;

    CB.whitemat.ambient[0] = 0.0; CB.whitemat.ambient[1] = 0.0; CB.whitemat.ambient[2] = 0.0;
    CB.whitemat.diffuse[0] = 1.0; CB.whitemat.diffuse[1] = 1.0; CB.whitemat.diffuse[2] = 1.0;//not needed
    CB.whitemat.specular[0] = 1.0; CB.whitemat.specular[1] = 1.0; CB.whitemat.specular[2] = 1.0; //not needed
    CB.whitemat.shineness = 50;  //not needed
    CB.whitemat.reflectance = 0.5;
    CB.whitemat.transparency = 0.0;
    CB.whitemat.ref_index = 0.0;

  eye_pos = {0.0, 0.5, 0.0};    //I only changed this to show refractive interaction between sphere + chessboard
}
