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
Classes
***************************************************************************************/

class Shape {
private:
	//Shape location
	float xPos;
	float yPos;
	float zPos;

	//Whether the shape is curently selected by user or not
	bool selected;

	//1 = sphere, 2 = cube, more to come
	int type;

	//Sizes
	float size;

public:
	//Constructors
	Shape(){
		xPos = 0;
		yPos = 0;
		zPos = 0;
		selected = false;
		type = 1;
	}

	void set(float x, float y, float z, float inputSize, int inputType){
		xPos = x;
		yPos = y;
		zPos = z;
		selected = false;
		type = inputType;
		size = inputSize;
	}

	//Getters
	float getXPos(){
		return xPos;
	}

	float getYPos(){
		return yPos;
	}

	float getZPos(){
		return zPos;
	}

	float getType(){
		return type;
	}

	float getSize(){
		return size;
	}

	//Functions
	void draw(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    	glColor3d(1,1,1); 
 
		glPushMatrix();
		glTranslated(xPos,yPos,zPos);

		switch (type){
			case 1:
		    	glutSolidSphere(size, 50, 50); //radius,slices,stacks
		    	break;
		    case 2:
		    	glutSolidCube(size);
		    	break;
		    case 3:
		    	glutSolidTorus(size/2, size, 10, 15); //inner, outer, radial sides, ring divisons
		    	break;
		    case 4:
		    	glutSolidTeapot(size);
		    	break;
		}

		glPopMatrix(); 
	}

	//Move object in specified direction
	void move(float moveX, float moveY, float moveZ){
		xPos += moveX;
		yPos += moveY;
		zPos += moveZ;
	}

	void rotate(){

	}

	void select(){
		selected = true;
	}

	void deselect(){
		selected = false;
	}

};


/***************************************************************************************
GLOBALS
***************************************************************************************/

//Initialize array holding all scene objects (Shapes)
Shape sceneShapes[20];
int activeShapes[20]; //Because I don't know how pointers work

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



void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{	
		//Exit program
		case 'q':
		case 27:
			exit (0);
			break;

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
void drawShapes() {
	for (int i = 0; i < 20; i++){//(sizeof(sceneShapes)/sizeof(*sceneShapes)); i++) {
		if (activeShapes[i] == 1) {
			sceneShapes[i].draw();
		}
	}
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

	drawShapes();
	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	printf("\nWelcome to Joseph's Modelling Assignment!\n\nControls:\nArrow Keys -> Camera movement\n'q' -> Quit\n\n");
	sceneShapes[0].set(1.0, 1.0 , 1.0, 2.0, 2);
	activeShapes[0] = 1;

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