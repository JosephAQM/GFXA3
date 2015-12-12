/*
Joseph Manalo
1304227
manaloja

SE 3GC3 Assignment 3
Saturday, December 5, 2015
*/

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

	//Shape rotation
	float xRot;
	float yRot;
	float zRot;

	//Whether the shape is curently selected by user or not
	bool selected;

	//1 = sphere, 2 = cube, 3 = torus, 4 = teapot
	int type;

	//Sizes
	float size;

	//Hitbox side length
	float hitBoxSize;

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
		xRot = 0;
		yRot = 0;
		zRot = 0;
		selected = false;
		type = inputType;
		size = inputSize;
		generateHitBox();
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

	bool isSelected(){
		return selected;
	}

	//Functions
	void draw(){
		glPushMatrix();

		glTranslated(xPos,yPos,zPos);
		glRotatef(xRot, 1.0, 0.0, 0.0);
		glRotatef(yRot, 0.0, 1.0, 0.0);
		glRotatef(zRot, 0.0, 0.0, 1.0);
		
		if (!selected)
    		glColor3d(0.1 , 0.1, 0.1); 
 		else
 			glColor3d(1, 0, 0);

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

	void drawHitBox(){
		generateHitBox();
		glPushMatrix();
		glColor3d(0, 1, 0);
		glTranslated(xPos,yPos,zPos);
		glutWireCube(hitBoxSize);
		glPopMatrix(); 
	}

	//Checks if the given point is within the shape's hitbox
	bool isInHitBox(float x, float y, float z){
		if ((x <= xPos+(hitBoxSize/2)) && (x >= xPos-(hitBoxSize/2))){
			if ((y <= yPos+(hitBoxSize/2)) && (y >= yPos-(hitBoxSize/2))){
				if ((z <= zPos+(hitBoxSize/2)) && (z >= zPos-(hitBoxSize/2))){
					return true;
				}
			}	
		}
			
		return false;
	}

	void generateHitBox(){
		if (type == 1) //sphere
			hitBoxSize = size * 1.8;

		else if (type == 2) //cube
			hitBoxSize = size * 1;
		
		else if (type == 3) //torus
			hitBoxSize = size * 2.3;
		
		else if (type == 4) //teapot
			hitBoxSize = size * 2;

	}

	//Move object in specified direction
	void move(float moveX, float moveY, float moveZ){
		xPos += moveX;
		yPos += moveY;
		zPos += moveZ;
	}

	void rotate(float rotateX, float rotateY, float rotateZ){
		xRot += rotateX;
		yRot += rotateY;
		zRot += rotateZ;
	}

	void increaseSize(float changeAmount){
		size += changeAmount;
		hitBoxSize += changeAmount;
	}

	void decreaseSize(float changeAmount){
		size -= changeAmount;
		hitBoxSize -= changeAmount;
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
int numberOfShapes = 20;
Shape sceneShapes[20];
bool activeShapes[20]; //Because I don't know how pointers work

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

//Ray casting stuff

//Coordinates of mouse click in the world
float worldX;
float worldY;
float worldZ;

//camPos coords when click occurs
float cX;
float cY;
float cZ;

//Mouse ray slopes (d for delta)
float deltaY;
float deltaX;
float deltaZ;

bool drawHitBoxes;


/***************************************************************************************
GENERAL FUNCTIONS
***************************************************************************************/

//Cycle select through objects
void cycleSelect(){
	int previousSelection;
	//Find selected object, deselect, remember which one it is
	for (int i = 0; i < 20; i++){
		//If active and selected, deselect
		if (activeShapes[i] && sceneShapes[i].isSelected()){
			sceneShapes[i].deselect();
			previousSelection = i;
			break;
		}
	}

	//Find the next object to select
	for (int j = 0; j < 20; j++){

		int nextToSelect = previousSelection+j + 1;

		if (nextToSelect >= 20)
			nextToSelect = j;

		if (activeShapes[nextToSelect] && !sceneShapes[nextToSelect].isSelected()){
			sceneShapes[nextToSelect].select();
			break;
		}

		if (j == 19)
			sceneShapes[0].select();
	}
}

void unSelectAll(){
	for (int i = 0; i < 20; i++){
		if (activeShapes[i]){
			sceneShapes[i].deselect();
		}
	}
}

void mouseToWorld(int mouseX, int mouseY, int mouseZ){
    GLint viewport[4];
    GLdouble modelMatrix[16];   
    GLdouble projectionMatrix[16];  

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);

    //float winZ;
    float winY = float(viewport[3] - mouseY);
    //glReadPixels(mouseX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    double x, y, z;
    gluUnProject((double)mouseX, winY, mouseZ, 
        modelMatrix, projectionMatrix, viewport,
        &x, &y, &z);
    
    worldX = x;
    worldY = y;
    worldZ = z;
    printf("World coordinates are %i %i %i\n", worldX, worldY, worldZ);
    //return (x, y, z);
}

//Cycles through all the points in a line, checks if each object on the list interects with it
int rayCasting(int mouseX, int mouseY){
	mouseToWorld(mouseX, mouseY, 1);

	//Calculate slopes for ray
	deltaX = (worldX - camPos[0]);
	deltaY = (worldY - camPos[1]);
	deltaZ = (worldZ - camPos[2]);

	//Save camera position for ray drawing
	cX = camPos[0];
	cY = camPos[1];
	cZ = camPos[2];

	//Go through points in the ray
	for (int i = 0; i < 100000; i++){ // lol overkill resolution
		float linePointX = cX + (worldX + deltaX)*(float)i*0.00001;
		float linePointY = cY + (worldY + deltaY)*(float)i*0.00001;
		float linePointZ = cZ + (worldZ + deltaZ)*(float)i*0.00001;

		//Loop through every active shape in the array
		for (int j = 0; j < 20; j++){
			if (activeShapes[j]){
				if(sceneShapes[j].isInHitBox(linePointX, linePointY, linePointZ)){
					printf("Selected: %i", j);
					return j;
				}
			}
		}
	}

	return -1;
}

void mouse(int btn, int state, int x, int y){
	switch(btn){
		case GLUT_LEFT_BUTTON:
			if(state==GLUT_DOWN){
				int toSelect = rayCasting(x, y);

				if (toSelect != -1){
					unSelectAll();
					sceneShapes[toSelect].select();

				}
			}
			break;
	}
}

void addObject(int newType, int newSize){
	//Loop through object array and look for an empty slot
	for (int i = 0; i < numberOfShapes; i++){
		if (!activeShapes[i]){
			sceneShapes[i].set(0.0, 0.0 , 0.0, newSize, newType);
			activeShapes[i] = true;
			break;
		}
	}
}

void removeObject(int objectNumber){
	activeShapes[objectNumber] = false;
}


/***************************************************************************************
CONTROLS
***************************************************************************************/

void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{	
		//Exit program
		case 'q':
		case 27:
			exit (0);
			break;


		//Cycle selection
		case 'z':
			cycleSelect();
			break;

		//Toggle light
		case 'p':
		case 'P':
			if (useLight) {
				glDisable(GL_LIGHTING);
				useLight = false;
			}
				
			else {
				glEnable(GL_LIGHTING);
				useLight = true;
			}
			break;

		case 'c':
		case 'C':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].increaseSize(0.1);
			}
			break;

		case 'x':
		case 'X':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].decreaseSize(0.1);
			}
			break;

		case 'w':
		case 'W':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(-0.1,0,0);
			}
			break;

		case 's':
		case 'S':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(0.1,0,0);
			}
			break;

		case 'a':
		case 'A':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(0,0,0.1);
			}
			break;

		case 'd':
		case 'D':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(0,0,-0.1);
			}
			break;

		case 'r':
		case 'R':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(0,0.1,0);
			}
			break;

		case 'f':
		case 'F':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].move(0,-0.1,0);
			}
			break;

		case 'b':
		case 'B':
			if (drawHitBoxes)
				drawHitBoxes = false;
			else
				drawHitBoxes = true;
			break;

		case 'i':
		case 'I':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(-5,0,0);
			}
			break;

		case 'k':
		case 'K':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(5,0,0);
			}
			break;

		case 'j':
		case 'J':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(0,-5,0);
			}
			break;

		case 'l':
		case 'L':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(0,5,0);
			}
			break;

		case 'u':
		case 'U':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(0,0,5);
			}
			break;

		case 'o':
		case 'O':
			for (int i = 0; i < 20; i++){
				if (activeShapes[i] && sceneShapes[i].isSelected())
					sceneShapes[i].rotate(0,0,-5);
			}
			break;



			
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


/***************************************************************************************
THE REST (idk what to call them)
***************************************************************************************/

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < 20; i++){//(sizeof(sceneShapes)/sizeof(*sceneShapes)); i++) {
		if (activeShapes[i]) {
			sceneShapes[i].draw();
			if (drawHitBoxes)
				sceneShapes[i].drawHitBox();
		}
	}

}

void drawRay(){
	glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex3f(cX, cY, cZ);
		glVertex3f(worldX + deltaX * 10, worldY + deltaY * 10, worldZ + deltaZ * 10);
	glEnd();

	glutPostRedisplay();
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
	drawRay();

	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{	
	drawHitBoxes = false;
	printf("\nWelcome to Joseph and Gabriel's Modelling Assignment!\n\nControls:\nCamera movement -> Arrow Keys\nCycle Select -> 'z'\nMove selected object -> 'wasd'\nRotate selected object -> 'ij, kl, uo'\nToggle Lights -> p\nEnlarge selected object -> c\nShrink selected object -> x\nQuit -> 'q'\n\n");
	sceneShapes[0].set(0.0, 0.0 , 0.0, 1.0, 4);
	activeShapes[0] = true;

	sceneShapes[1].set(2.0, 3.0 , 3.0, 1.0, 2);
	activeShapes[1] = true;

	sceneShapes[2].set(-2.0, 2.0 , 2.0, 1.0, 1);
	activeShapes[2] = true;

	sceneShapes[3].set(2.0, -2.0 , 2.0, 1.0, 3);
	activeShapes[3] = true;


	sceneShapes[0].select();
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(300, 100);

	glutCreateWindow("A3:Modelling   manaloja/1304227 lopezsomething/#######");	//creates the window
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//Culling
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}