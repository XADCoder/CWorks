/*
 * Paralleize the mandelbrot using OpenMP.
 * Render the Image in OpenGL.
 * Support real time zoom in:
    Press "u" to zoom in and "o" to zoom out.
    Press "i" and "k" to shift vertically and
    "j" and "l" horizontally.

    by axu
 */

#include <GL/glut.h>      /* use for Macs */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "bmp.h"

#define WIDTH 500
#define HEIGHT 500

#define ESC 27


int hxres = 500;    /* horizonal resolution   */
int hyres = 500;    /* vertical resolution    */
double magnify=1.0;   /* no magnification   */
double shiftHr = 0;   /* no horizontal shift*/
double shiftVr = 0;   /* no vertical shift*/
RGBTRIPLE **pixels;

 double mandelbrot(double cx, double cy){
  double x,xx,y;
  // double xx;
  int itermax = 100;    /* how many iterations to do  */
  int iteration;
  x = 0.0; y = 0.0;

  for (iteration=1;iteration<itermax;iteration++)  {
    xx = x*x-y*y+cx;
    y = 2.0*x*y+cy;
    x = xx;
    if (x*x+y*y>100.0)  iteration = 999999;
  }
  if (iteration<99999){
    return 25;
  }else 
    return 255;

}
/**Set color to the image*/
void myRender()
{
  double cx,cy;
  int color;
  
  pixels = malloc(hyres*sizeof(RGBTRIPLE*));
  for(int i=0; i < hyres; i++)
    pixels[i] = malloc(hxres*sizeof(RGBTRIPLE));

  #pragma omp parallel for private(cx, cy, color) schedule(dynamic)
  for (int hy = 0; hy < hyres; hy++)  {
    for (int hx = 0; hx < hxres; hx++)  {
      cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7 + shiftHr;
      cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0 + shiftVr;

      color = mandelbrot(cx,cy);
      
      pixels[hx][hy].rgbtBlue = color*3;
      pixels[hx][hy].rgbtGreen = color*10;
      pixels[hx][hy].rgbtRed = color*3;
    }
  }
}

/*
 * initialize the OpenGL state machine
 */
 void init (void)
 {
	glClearColor(1.0, 1.0, 1.0, 0.0);	// set display-window color white
	glColor3f(0.0f, 0.0f, 1.0f);		// set drawing color to blue
	glPointSize(8.0);					// a dot is 8 x 8 pixel
	glMatrixMode(GL_PROJECTION);		// set projection parameters
	glLoadIdentity();					// load the identity matrix

	/* set the orthogonal projection and
	 * the world coordinate bounaries
	 */
  gluOrtho2D(0.0, 500.0, 0.0, 500.0);
}

/*
 * the display callback function
 */
 void display (void)
 {
	/* clear the screen to the background color */
   glClear( GL_COLOR_BUFFER_BIT );

   for(int i=0; i<hxres; i++){		
    for(int j=0; j<hyres; j++){
			glColor3b(pixels[i][j].rgbtRed/2, pixels[i][j].rgbtGreen/2, pixels[i][j].rgbtBlue/2);		// set drawing color 
			glBegin(GL_POINTS);
      glVertex2i(i, j);
      glEnd();
    }
  }

  glFlush();
}

/*
 * the keyboard callback function.
 * all it does is allow us to exit cleanly
 * when we hit the escape key.
 */
 void keyboard(unsigned char key, int x, int y)
 {
   switch (key) {
    case ESC:
    exit(0);
    break;
    case 'u':
    glScaled(1.2, 1.2, 1.2);
    magnify*=1.2;
    myRender();
    glutPostRedisplay();
    break;
    case 'o':
    glScaled(1/1.2, 1/1.2, 1/1.2);
    magnify/=1.2;
    myRender();
    glutPostRedisplay();
    break;
    case 'i':
    // glTranslated(0, 5, 0);
    shiftVr +=0.1;
    myRender();
    glutPostRedisplay();
    break;
    case 'k':
    // glTranslated(0, -5, 0);
    shiftVr -=0.1;
    myRender();
    glutPostRedisplay();
    break;
    case 'j':
    // glTranslated(5, 0, 0);
    shiftHr +=0.1;
    myRender();
    glutPostRedisplay();
    break;
    case 'l':
    // glTranslated(-5, 0, 0);
    shiftHr -=0.1;
    myRender();
    glutPostRedisplay();
    break;
  }
}

int main (int argc, char** argv)
{
    /* initialize GLUT and set up the window */
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition( 50, 100);
	glutInitWindowSize( WIDTH, HEIGHT );
	glutCreateWindow("Fractals!");

    /* set the display callback functions */
  myRender();
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );

	/* initialize OpenGL */
  init();

	/* start the main GLUT execution loop */
  glutMainLoop();

	/* unnecessary as the glutMainLoop never exits
	 * but what the heck.
	 */
  return 0;
}

