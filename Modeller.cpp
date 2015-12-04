#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/***************************************************************************************
GLOBALS
***************************************************************************************/

//Terrain heightmap
float terrain[300][300];

//Camera stuff
float pos[] = {0,0,0};
float rot[] = {0, 1, 0};
float headRot[] = {0, 0, 0};
float camPos[] = {7, 7, 7};
float angle = 0.0f;

//light stuff
float lightpos[4] = {0, 2, 0, 0};
float amb[4] = {1, 1, 1, 1};
float diff[4] = {2, 2,2, 2};
float spec[4] = {0, 0, 1, 1};
bool useLight = true;

//selected quad
int selX = 0;
int selZ = 0;

//Randomly generates terrain and applies circles algorithm
void generateTerrain(){	
}

void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{	
		//Exit program
		case 'q':
		case 27:
			exit (0);
			break;

		// //WASD: Move light source
		// case 'a':
		// case 'A':
		// 	if(lightpos[0] > -15)
		// 		lightpos[0]-=1;
		// 	break;

		// case 'w':
		// case 'W':
		// 	if(lightpos[2] > -15)
		// 		lightpos[2] -= 1;
		// 	break;
		// case 'd':
		// case 'D':
		// 	if(lightpos[0] < 15)
		// 		lightpos[0]+=1;
		// 	break;

		// case 's':
		// case 'S':
		// 	if(lightpos[2] < 15)
		// 		lightpos[2] += 1;
		// 	//printf('hi %f\n', lightpos[1]);
		// 	break;

		//Toggle light
		case 'f':
		case 'F':
			if (useLight) {
				glDisable(GL_LIGHTING);
				useLight = false;
			}
				
			else {
				glEnable(GL_LIGHTING);
				useLight = true;
			}
			
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	//Move camera
	switch(key)
	{
		case GLUT_KEY_LEFT:
			camPos[2]-=0.1;
			break;

		case GLUT_KEY_RIGHT:
			camPos[2]+=0.1;
			break;

		case GLUT_KEY_UP:
			camPos[0] += 0.1;
			break;

		case GLUT_KEY_DOWN:
			camPos[0] -= 0.1;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 0.1;
			break;

		case GLUT_KEY_END:
			camPos[1] -= 0.1;
			break;



	}
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	//Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);

	glLightfv(GL_LIGHT1, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, 1, 1, 100);

}

//Draws and colours terrain depending on height
void drawTerrain(float* pos, float* rot) {

}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{

	//
	float m_amb[] = {0.33, 0.22, 0.03, 1.0};
	float m_dif[] = {0.78, 0.57, 0.11, 1.0};
	float m_spec[] = {0.99, 0.91, 0.81, 1.0};
	float shiny = 27;
	//
	float origin[3] = {0,0,0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{

	printf("\nWelcome to Joseph's Modelling Assignment!\n\nControls:\nArrow Keys -> Camera movement\n'q' -> Quit\n\n");


	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("A3:Modelling   manaloja/1304227");	//creates the window
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}