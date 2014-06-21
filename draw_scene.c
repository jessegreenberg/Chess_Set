/*
 * HOMEWORK ASSIGNMENT 2
 * Jesse Greenberg
 * CSCI 4229
 * 
 * Render a 3D scene with the ability to toggle projection mode
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* State Variables */
int th = 0; // Azimuthal angle
int ph = 0; // Elevation of View
double dim = 30; // dimension of orthogonal box
int mode = 0; // View mode; ORTHOGONAL = 0
int fov = 55; // Field of view for perspective mode
double asp = 1; // Aspect ratio
int white = 1; // Variable for piece color
int black = 0; // Variable for piece color


// Define macros for sin and cos in degrees
// Imprecision in pi allows for view at angles 0 and 90
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1416927/180*(th))

/*
 * Convenience routine to output raster text
 * This code is taken from the provided course material
 * */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set the Projection Mode
 * */
static void Project()
{
	// Tell OpenGL we want to work with the projection matrix
	glMatrixMode(GL_PROJECTION);
	// Undo previous transformations
	glLoadIdentity();
	// Set the Perspective transformation
	if (mode == 1) {
		gluPerspective(fov, asp, dim/40, 40*dim);
	}
	else {
		glOrtho(-asp*dim, asp*dim, -dim, dim, -dim, dim);
	}
	// Revert back to the model matrix
	glMatrixMode(GL_MODELVIEW);
	// Clear the transformations 
	glLoadIdentity();
}

/* CHESS PIECE BUILDING BLOCKS */

/*
 * Draw a Vertex in polar coordinates
 * */
static void polarVertex(double th, double ph) {
	glVertex3d(Sin(th)*Cos(ph), Sin(ph), Cos(th)*Cos(ph));
}
\
/* Draw a pyramid at (x, y, z)
 * */
static void drawPyramid(double x, double y, double z) {
	glPushMatrix();
	glTranslated(x, y, z);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 2, 0);
	glVertex3f(-1, 0, 1);
	glVertex3f(-1, 0, -1);
	glVertex3f(1, 0, -1);
	glVertex3f(1, 0, 1);
	glVertex3f(-1, 0, 1);
	glEnd();
	glPopMatrix();
}

/*
 * Draw a Rectangular Box at (x,y,z) with dimensions (dx,dy,dz)
 * */
static void drawCube(double x, double y, double z, double dx, double dy, double dz)
{
	// Save the transformation
	glPushMatrix();
	// Apply the offset
	glTranslated(x, y, z);
	glScaled(dx, dy, dz);
	// Draw the rectangular prism
	glBegin(GL_QUADS);
	// Set color (currently off-white)
	//glColor3f(1, 0.9411, 0.5764);
	// Front
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	// Back
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	// Right
	glVertex3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	// Left
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, 1, -1);
	// Top
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);
	// Botton
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);
	// End
	glEnd();
	// Undo transformations
	glPopMatrix();
}

/*
 *  Draw a sphere at (x, y, z) with radius (r)
 */
static void drawSphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         polarVertex(th,ph);
         polarVertex(th,ph+d);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}
/*
 *  Draw a hemisphere at (x, y, z) with radius (r)
 */
static void drawHemisphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glRotated(270, 0, 0, 1);
   glScaled(r,r,r);
   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=180;th+=d)
      {
         polarVertex(th,ph);
         polarVertex(th,ph+d);
      }
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}

/* Draw a Cylinder of radius r and height y */
void drawCylinder(double r, double y) {
	int i; // iteration variable
	for(i = 0; i < 360; i++){
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3d(0, y/2, 0);
		glVertex3d(r*Cos(i), y/2, r*Sin(i));
		glVertex3d(r*Cos(i+1), y/2, r*Sin(i+1));
		// Bottom of the cylinder
		glVertex3d(r*Cos(i), -y/2, r*Sin(i));
		glVertex3d(r*Cos(i+1), -y/2, r*Sin(i+1));
		glVertex3d(0, -y/2, 0);
		glEnd();
	}
}	

/*
 * Draw a curved cone (hat-like shape)
 * eccentricity (ecc) varies the severity of the curve
 * height (height) varies the height of the base
 * This will act as a base for many pieces
 * */
void drawCurvedBase(double ecc, double height){
	// Save the transformation
	glPushMatrix();
	glRotated(180, 1, 0, 0);
	// Apply the offset
	double i; // iteration variable
	/* Time Step */
	double dt = 0.1;
	/* Draw several cylinders with radii of the function
	 * (ecc^x) to create a curved survace */
	for(i = dt; i < height; i = i+dt){
		glTranslated(0, .1, 0);
		double radius = pow(ecc, i);
		drawCylinder(radius, .1);
	}
	glPopMatrix();
	glutPostRedisplay();
}

/* BUILDING THE CHESS PIECES*/
/*
 * Draw a pawn at location (x, y, z)
 * */
void drawPawn(double x, double y, double z) {
	// Save the transformation matrix
	glPushMatrix();
	glTranslated(x, y, z);

	glPushMatrix();
	glScaled(1, 4, 1);
	/* Translate the piece up by its height to set the base at 0 */
	glTranslated(0, 2, 0); 	
	// Draw the base of the pawn
	drawCurvedBase(2, 2);
	glPopMatrix();
	/* Draw the Head of the Pawn */
	/* Translate the head up to the height * the yscale */
	glTranslated(0, 8, 0);
	drawSphere(0, 0, 0, 1.7);
	glPopMatrix();
}
/* Draw a Rook at location (x, y, z)
 * */
void drawRook(double x, double y, double z) {
	glPushMatrix();
	glTranslated(x, y, z);
	glScaled(2.2, 4, 2.2);
	// Draw the base of the rook
	/* Translate the base up to 0 */
	glTranslated(0, 2.5, 0);
	drawCurvedBase(1.1, 2.5);
	// Draw the top
	drawCylinder(1.5, 0.6);
	// draw the battlements
	int i;
	for(i = 0; i < 360; i = i+60){
		drawCube(1.5*Cos(i), 0.4, 1.5*Sin(i), .3, .4, .3);
	}
	glPopMatrix();
}
/* Draw a Bishop at location (x, y, z) {
 * */
void drawBishop(double x, double y, double z) {
	glPushMatrix();
	glTranslated(x, y, z);
	glScaled(0.6, 3, 0.6);
	/* Translate the base up to 0 */
	glTranslated(0, 2.6, 0);
	// Draw the base of the bishop
	drawCurvedBase(2, 2.6);
	// Draw the stand of the bishop
	glTranslated(0, -1.8, 0);
	drawCylinder(1.3, 0.7);
	// Draw the collar of the bishop
	glTranslated(0, 2, 0);
	drawCylinder(2, 0.2);
	glScaled(2.8, 0.8, 2.8);
	drawSphere(0, 1, 0, 0.8);
	drawSphere(0, 1.8, 0, 0.4);
	glPopMatrix();
}

/* Draw a Queen at location (x, y, z)
 * */
void drawQueen(double x, double y, double z) {
	glPushMatrix();
	glTranslated(x, y, z);
	
	glScaled(1, 4, 1);
	/* Translate the base up to 0 */
	glTranslated(0, 3, 0);
	// Draw the base of the queen
	drawCurvedBase(1.3, 3);

	
	// Draw the collar of the queen
	drawCylinder(2, 0.2);

	// Draw the Queen's crown
	glScaled(1, 0.4, 1);
	double i; // iteration variable
	for(i = 0.2; i < 1.5; i = i+0.05){
		glTranslated(0, 0.05, 0);
		drawCylinder(i, 0.05);
	}
	for(i = 1.5; i > 0; i = i-0.05){
		glTranslated(0, 0.02, 0);
		drawCylinder(i, 0.05);
	}
	drawSphere(0, 0.15, 0, 0.25);
	for(i = 0; i < 360; i = i+60){
		drawSphere(1.5*Cos(i), 0, 1.5*Sin(i), 0.15);
	}
	glPopMatrix();
}
/* Draw a King at location (x, y, z)
 * */
void drawKing(double x, double y, double z) {
	glPushMatrix();
		glTranslated(x, y, z);
		/* Translate the base up to 0 */
		glTranslated(0, 12.8, 0);
		glScaled(1.3, 4, 1.3);
		// Draw the base of the King
		drawCurvedBase(1.2, 3.2);
		// Draw the king's collar
		drawCylinder(1.5, 0.3);
		// Draw the King's crown
		double i; // iteration variable
		for(i = 0.8; i < 1; i = i+0.02){
			glTranslated(0, 0.1, 0);
			drawCylinder(i, 0.1);
		}
		for(i = 1; i > 0; i = i-0.05){
			glTranslated(0, 0.02, 0);
			drawCylinder(i, 0.05);
		}
		// Draw a cross on top of the crown
		glScaled(1, 0.4, 1);
		drawCube(0, 0.5, 0, 0.1, 0.5, 0.1);
		drawCube(0, 0.6, 0, 0.3, 0.1, 0.1);
	glPopMatrix();
}
/* Draw a knight at the location (x, y, z)
 * */
void drawKnight(double x, double y, double z) {
	glPushMatrix();
		glTranslated(x, y, z);
		glTranslated(0, 6.75, 0);
		glRotated(180, 0, 1, 0);
		glPushMatrix();
			// Draw the Nose
			glTranslated(0, 3, 0);
			glRotated(60, 1, 0, 0);
			glScaled(1.95, 4, 1.95);
			drawHemisphere(0, 0, 0, 0.5);
		glPopMatrix();
		glPushMatrix();
			// Draw the head
			glTranslated(0, 3, 0);
			drawSphere(0, 0, 0, 1);
			// Draw the ears of the horse
		glPopMatrix();
		glPushMatrix();
			glTranslated(0, 3, 0);
			glRotated(-30, 1, 0, 0);
			glScaled(0.2, 0.6, 0.2);
			drawPyramid(2.4, 0.8, 0.4);
			drawPyramid(-2.4, 0.8, 0.4);
		glPopMatrix();
		// draw the body of the horse
		double neck = 2.5; // iteration variables
		int ang = 50;
		glPushMatrix();
			glTranslated(0, -6, 0);
			glRotated(-90, 0, 1, 0);
			glScaled(0.5, 0.5, 0.5);
			// Draw the base of the knight
			/* Curvature of the neck follows Sin */
			drawCylinder(7, 1.5);
			while(ang < 300){
				glTranslated(Sin(ang)/15, 0.15, 0);
				ang = ang + 2;
				drawCylinder(neck+Sin(ang), .5);
			}
		glPopMatrix();
	glPopMatrix();
}


 
/*
 * Glut will call this to display the view, 
 * */
void display()
{
	// Clear the window and set the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Undo previous transformations
	glLoadIdentity();
	// If perspective mode:
	if (mode == 1) {
		double Ex = -2*dim*Sin(th)*Cos(ph);
		double Ey = 2*dim*Sin(ph);
		double Ez = 2*dim*Cos(th)*Cos(ph);
		gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
	}
	// If Orthogonal view mode, initialize orientation
	else {
		glRotatef(ph, 1, 0, 0);
		glRotatef(th, 0, 1, 0);
		glScaled(0.2, 0.2, 0.2);
	}

	/* Set up a row of pawns */
	
	int i;
	for(i = 0; i <8; i++){ 
		drawPawn((i-4)*9, 0, 0);
	}

	/* Set the rooks */
	
	drawRook(-36, 0, -8);
	drawRook(27, 0, -8);
	
	/* Set the knights */
	
	drawKnight(-27, 0, -8);
	drawKnight(18, 0, -8);
	
	/* Set the bishops*/
	
	drawBishop(-18, 0, -8);
	drawBishop(9, 0, -8);
	
	/* Set the King and Queen */
	drawQueen(-9, 0, -8);
	drawKing(0, 0, -8);
	// Render the scene and make it visible
	glFlush();
	glutSwapBuffers();
}

// Handles key presses
void key(unsigned char ch, int x, int y)
{
	// Conveniently exit on escape
	if (ch == 27) {
		exit(0);
	}
	// Easily reset view angle
	else if (ch == '0') {
		th = ph = 0;
	}
	// Switch projection mode
	else if(ch == 'm' || ch == 'M'){
		mode = 1-mode;
	}
	// Increase the field of view (zoom in)
	else if (ch == '+') {
		fov++;
	}
	// Decrease the field of view (zoom out)
	else if (ch == '-') {
		fov--;
	}
	// Update the projection
	Project();
	// Tell GLUT to update the view
	glutPostRedisplay();
}

// Handles arrow presses to change the view angle
// Mostly taken from example 6 in course material
void special(int key, int x, int y)
{
	// Right arrow key - increase azimuth by 5 degrees
	if (key == GLUT_KEY_RIGHT){
		th += 5;
	}
	else if(key == GLUT_KEY_LEFT) {
		th -= 5;
	}
	else if(key == GLUT_KEY_UP) {
		ph += 5;
	}
	else if (key == GLUT_KEY_DOWN) {
		ph -= 5;
	}
	// Keep angles bounded by 360 in magnitude
	th %= 360;
	ph %= 360;
	// Update the projection
	Project();
	// Tell GLUT to update the screen
	glutPostRedisplay();
}

/*
 * GLUT will call this to handle window resizing
 * */
 void reshape(int width, int height) {
	// Ratio of the width to the height of the window
	asp = (height>0) ? (double)width/height : 1;
	// Set the viewport to the entire window
	glViewport(0, 0, width, height);
	// Reset the projection
	Project();
}

/*
 * Start up GLUT and tell it to handle all displays
 * */
 
int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	// Request a double buffered, true color window
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	// Create the window
	glutCreateWindow("Jesse Greenberg Homework 2");
	// Register function to display the scene
	glutDisplayFunc(display);
	// Tell GLUT to handle window resizing
	glutReshapeFunc(reshape);
	// Tell GLUT to handle key input
	glutKeyboardFunc(key);
	// Tell GLUT to handle input from special keys
	glutSpecialFunc(special);
	// Pass control to GLUT for any events
	glutMainLoop();
	// Return to OS
	return 0;
}
	
	
	
	
	
