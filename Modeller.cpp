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

//Parallel array to terrain[][] that signies if the point should be used in
//the circles algorithm.
bool toCircle[300][300];

//0 = solid, 1 = wireframe, 2 = both
int drawMode = 0;

//Terrain dimensions
int terrainSizeX = 0;
int terrainSizeZ = 0;

float terrainCircleSize = 30;

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

//Circles algorithm used to make hills out of a single hightened point
void circles(int centerX, int centerZ, float disp){

	for (int i = 0; i < terrainSizeX-1; i++){
		for (int j = 0; j < terrainSizeZ-1; j++){
			//pythagorean
			float distance = sqrt(pow((i-centerX),2) + pow((j-centerZ),2));

			float pd = (distance * 2) / terrainCircleSize;

			if (fabs(pd) <= 1.0 && pd != 0){
				terrain[i][j] += (disp/2 + (cos(pd*3.14) * disp/2));
			}
		}
	}

	//For some reason the center of the circle had a different height.
	//This ensures that is is the appropriate height
	terrain[centerX][centerZ] = (terrain[centerX+1][centerZ] + terrain[centerX][centerZ+1] + terrain[centerX-1][centerZ] + terrain[centerX][centerZ-1]) / 4;
}

//Resets terrain to flat
void resetTerrain(){
		for (int i = 0; i < terrainSizeX; i++){
			for (int j = 0; j < terrainSizeZ; j++){
				toCircle[i][j] = false;
				terrain[i][j] = 0;
			}
		}
}

//Randomly generates terrain and applies circles algorithm
void generateTerrain(){	
	//Loop through terrain[][], randomly assign height values
	//Edge cases ignored to remove artifacts
	for (int i = 1; i < terrainSizeX-1; i++){
		for (int j = 1; j < terrainSizeZ-1; j++){

			if (rand() % 1000 >= 994){ //Lower second number = more hilly
				terrain[i][j] = (rand() % 100)/100;
				toCircle[i][j] = true;
			}
			else
				terrain[i][j] = 0;
		}
	}

	//Run hightened points through circles algorithm
	for (int i = 0; i < terrainSizeX; i++){
		for (int j = 0; j < terrainSizeZ; j++){
			if(toCircle[i][j])
				circles(i,j,1);
		}
	}
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

		//Reset terrain to flat
		case 'r':
		case 'R':
			resetTerrain();
			generateTerrain();
			break;

		//Toggle draw mode, solid/wire/both
		case 'e':
		case 'E':
			drawMode++;
			if (drawMode == 3)
				drawMode = 0;
			printf("Draw Mode: %i", drawMode);
			break;

		//WASD: Move light source
		case 'a':
		case 'A':
			if(lightpos[0] > -15)
				lightpos[0]-=1;
			break;

		case 'w':
		case 'W':
			if(lightpos[2] > -15)
				lightpos[2] -= 1;
			break;
		case 'd':
		case 'D':
			if(lightpos[0] < 15)
				lightpos[0]+=1;
			break;

		case 's':
		case 'S':
			if(lightpos[2] < 15)
				lightpos[2] += 1;
			//printf('hi %f\n', lightpos[1]);
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

		//IJKL move terrain selection
		case 'i':
		case 'I':
			if (selZ < terrainSizeZ-3)
				selZ++;
			break;

		case 'k':
		case 'K':
			if (selZ > 3)
				selZ--;
			break;

		case 'j':
		case 'J':
			if (selX > 3)
				selX--;
			break;

		case 'l':
		case 'L':
			if (selX < terrainSizeX-3)
				selX++;
			break;


		//Increase height of selected terrain
		case 'y':
		case 'Y':
			terrain[selX][selZ] += 0.01;
			toCircle[selX][selZ] = true;
			circles(selX, selZ, 1);
			break; 

		//Decrease height (doesn't work as the circles algorithm doesn't work completely
		//as it raises the values around the hightened point too much)
		case 'h':
		case 'H':
			terrain[selX][selZ] -= 0.1;
			toCircle[selX][selZ] = true;
			circles(selX, selZ, 1);
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
void drawTerrain(float* pos, float* rot)
{

	for (int i = 1; i < terrainSizeX-2; i++){
		for (int j = 1; j < terrainSizeZ-2; j++){
			glBegin(GL_QUAD_STRIP);

			//Determine colour
			float R = 0.0;
			float G = 0.4;
			float B = 0.0;

			R = terrain[i][j] * 0.5 - 0.3;
			B = terrain[i][j] * 1.2 - 0.8;

			//Make selected strip red
			if ((i == selX) && j == selZ)
				glColor3f(1, 0, 0);
			else
				glColor3f(R, G, B);

			//Ensure the terrain is centered, and draw it
			//Each x,z quad is 0.1x0.1 units, hence all the multiplication
			float scaledX = i*0.1;
			float scaledZ = j*0.1;
			float centerViewZ = (float)terrainSizeZ / 20;
			float centerViewX = (float)terrainSizeX / 20;

			glVertex3f(scaledX-centerViewX, terrain[i][j], scaledZ-centerViewZ);
			glVertex3f(scaledX-centerViewX + 0.1, terrain[i+1][j], scaledZ-centerViewZ);
			glVertex3f(scaledX-centerViewX, terrain[i][j+1], scaledZ + 0.1-centerViewZ);
			glVertex3f(scaledX-centerViewX + 0.1, terrain[i+1][j+1], scaledZ + 0.1-centerViewZ);
			glEnd();
		}
	}
	
	glFlush();
}

//Draw wireframe of the terrain
void drawWire(float* pos, float* rot)
{

	for (int i = 1; i < terrainSizeX-2; i++){
		for (int j = 1; j < terrainSizeZ-2; j++){
			glBegin(GL_LINES);

			glColor3f(1, 1, 1);

			//Each x,z quad is 0.1x0.1 units, hence all the multiplication
			float scaledX = i*0.1;
			float scaledZ = j*0.1;
			float centerViewZ = (float)terrainSizeZ / 20;
			float centerViewX = (float)terrainSizeX / 20;

			glVertex3f(scaledX-centerViewX, terrain[i][j], scaledZ-centerViewZ);
			glVertex3f(scaledX-centerViewX, terrain[i][j+1], scaledZ + 0.1-centerViewZ);
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(scaledX-centerViewX, terrain[i][j], scaledZ-centerViewZ);
			glVertex3f(scaledX-centerViewX + 0.1, terrain[i+1][j], scaledZ-centerViewZ);
			glEnd();
		}
	}
	
	glFlush();
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

	if(drawMode == 0)
		drawTerrain(pos, rot);
	else if(drawMode == 1)
		drawWire(pos,rot);
	else{
		drawTerrain(pos,rot);
		drawWire(pos,rot);
	}


	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{

	printf("\nWelcome to Joseph's Terrain Assignment!\n\nControls:\nArrow Keys -> Camera movement\n'e' -> Toggle wireframe modes'\n'f' -> Toggle lighting\n'i,j,k,l' -> Move terrain selection (red square)\n'y' -> Increase height of selected terrain\n'q' -> Quit\n\n");


	//Obtain surface dimensions
	printf("Please enter the X dimension of the terrain.\n");
	scanf("%i", &terrainSizeX);
	printf("Please enter the Z dimension of the terrain.\n");
	scanf("%i", &terrainSizeZ);
	printf("X: %i Z: %i\n", terrainSizeX, terrainSizeZ);

	selX = terrainSizeX-3;
	selZ = terrainSizeZ-3;

	generateTerrain();


	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("A2:Terrain   manaloja/1304227");	//creates the window
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