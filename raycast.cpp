/***********************************************************
 * CMPT 361, Summer 2012
 *
 *  raycast.cpp
 *      
 *  Render a simple scene using ray tracing
 * 
 *  NAME: Patrick Wang
 *  SFU ID: 301160793
 *
 *  Template code for drawing a scene using raycasting.
 *  Some portions of the code was originally written by 
 *  M. vandePanne - and then modified by R. Zhang & H. Li
***********************************************************/

#include "include/Angel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "global.h"
#include "sphere.h"
#include "image_util.h"
#include "scene.h"

//
// Global variables
//
// Here we avoid dynamic memory allocation as a convenience. You can
// change the resolution of your rendered image by changing the values
// of WIN_X_SIZE and WIN_Y_SIZE in "global.h", along with other
// global variables
//

int win_width = WIN_WIDTH;
int win_height = WIN_HEIGHT;

GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];   
// array for the final image 
// This gets displayed in glut window via texture mapping, 
// you can also save a copy as bitmap by pressing 's'

float image_width = IMAGE_WIDTH;
float image_height = (float(WIN_HEIGHT) / float(WIN_WIDTH)) * IMAGE_WIDTH;

// some colors
RGB_float background_clr; // background color
RGB_float null_clr = {0.0, 0.0, 0.0};   // NULL color

//
// these view parameters should be fixed
//
Point eye_pos = {0.0, 0.0, 0.0};  // eye position
float image_plane = -1.5;           // image plane position

// list of spheres in the scene
Spheres *scene = NULL;

// chessboard
ChessBoard CB;

// light 1 position and color
Point light1;
float light1_ambient[3];
float light1_diffuse[3];
float light1_specular[3];

// global ambient term
float global_ambient[3];

// light decay parameters
float decay_a;
float decay_b;
float decay_c;

// maximum level of recursions; you can use to control whether reflection
// is implemented and for how many levels
int step_max = 1;

// You can put your flags here
bool shadow_on = 0;			//inclusion of shadows
bool reflection_on = 0; 		//inclusion of reflection
bool refraction_on = 0; 		//inclusion of refraction
bool chess_pattern_on = 0; 	//inclusion of chess board pattern
bool diffuse_rend_on = 0; 	//enable diffuse rendering using stochastic ray generations
bool super_samp_on = 0; 		//enable super sampling


// OpenGL
const int NumPoints = 6;

//----------------------------------------------------------------------------

void init()
{
	// Vertices of a square
	double ext = 1.0;
	vec4 points[NumPoints] = {
		vec4( -ext, -ext,  0, 1.0 ), //v1
		vec4(  ext, -ext,  0, 1.0 ), //v2
		vec4( -ext,  ext,  0, 1.0 ), //v3	
		vec4( -ext,  ext,  0, 1.0 ), //v3	
		vec4(  ext, -ext,  0, 1.0 ), //v2
		vec4(  ext,  ext,  0, 1.0 )  //v4
	};

	// Texture coordinates
	vec2 tex_coords[NumPoints] = {
		vec2( 0.0, 0.0 ),
		vec2( 1.0, 0.0 ),
		vec2( 0.0, 1.0 ),
		vec2( 0.0, 1.0 ),
		vec2( 1.0, 0.0 ),
		vec2( 1.0, 1.0 )
	};

	// Initialize texture objects
	GLuint texture;
	glGenTextures( 1, &texture );

	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0,
		GL_RGB, GL_FLOAT, frame );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glActiveTexture( GL_TEXTURE0 );

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(tex_coords), NULL, GL_STATIC_DRAW );
	GLintptr offset = 0;
	glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(points), points );
	offset += sizeof(points);
	glBufferSubData( GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords );

	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );

	// set up vertex arrays
	offset = 0;
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset) );

	offset += sizeof(points);
	GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" ); 
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset) );

	glUniform1i( glGetUniformLocation(program, "texture"), 0 );

	glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

/*********************************************************
 * This is the OpenGL display function. It is called by
 * the event handler to draw the scene after you have
 * rendered the image using ray tracing. Remember that
 * the pointer to the image memory is stored in 'frame'.
 *
 * There is no need to change this.
 **********************************************************/

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	glDrawArrays( GL_TRIANGLES, 0, NumPoints );

	glutSwapBuffers();
}

/*********************************************************
 * This function handles keypresses
 *
 *   s - save image
 *   q - quit
 *
 * DO NOT CHANGE
 *********************************************************/
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':case 'Q':
		free(scene);
		exit(0);
		break;
	case 's':case 'S':
		save_image();
		glutPostRedisplay();
		break;
	default:
		break;
	}
}



//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
	// Parse the arguments
	if (argc < 3) {
		printf("Missing arguments ... use:\n");
		printf("./raycast [-u | -d] step_max <options>\n");
		return -1;
	}
	
	if (strcmp(argv[1], "-u") == 0) {  // user defined scene
		set_up_user_scene();
	} else { // default scene
		set_up_default_scene();
	}

	step_max = atoi(argv[2]); // maximum level of recursions

	// Optional arguments
	for(int i = 3; i < argc; i++)
	{
		if (strcmp(argv[i], "+s") == 0)	shadow_on = 1;
		if (strcmp(argv[i], "+l") == 0)	reflection_on = 1;
		if (strcmp(argv[i], "+r") == 0)	refraction_on = 1;
		if (strcmp(argv[i], "+c") == 0)	chess_pattern_on = 1;
		if (strcmp(argv[i], "+f") == 0)	diffuse_rend_on = 1;
		if (strcmp(argv[i], "+p") == 0)	super_samp_on = 1;		
	}

	//
	// ray trace the scene now
	//
	// we have used so many global variables and this function is
	// happy to carry no parameters
	//
	printf("Rendering scene using my fantastic ray tracer ...\n");
	ray_trace();

	// we want to make sure that intensity values are normalized
	histogram_normalization();

	// Show the result in glut via texture mapping
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
	glutCreateWindow( "Ray tracing" );
	glewInit();
	init();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMainLoop();
	return 0;
}
