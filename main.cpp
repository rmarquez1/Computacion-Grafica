#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <string.h>
#include <sstream>

using namespace std;

#define CANT_ENEMIGOS		30
#define CANT_DEFENSA		25
#define INICIO_ENE_X		-1.01
#define INICIO_ENE_Y        0.8  
#define GRID_SCALE_SHORT    0.025f
#define GRID_SCALE_LONG     0.1f
#define GRIDX               100.0f
#define GRIDY               100.0f
 

// Nave Fantasma
#define FANTASMAX -1.4f
#define FANTASMAY 0.9f
#define YARECTANGLE 0.05
#define XARECTANGLE 0.05
int fantasmaIni = 0;
#define VELOCIDADFANTASMA 50
#define APARECERFANTASMA 12000
#define DESPLAZAFANTASMA 0.01


// Variable que lleva la puntuacion acumulada
int score = 0;

// Variable que lleva la velocidad del juego
int velocidad = 500;

// Variable que determina si el jugador esta vivo
bool jugadorVivo = true;

// Variable que determina si la nave esta viva o no
bool naveViva;

// Estructura para el enemigo
struct enemigo 
{
	float posX;
	float posY;
	int   shots;
};

struct jugador{
	float posX;
	float posY;
	bool vivo;
};


// Estructura para la posicion de las balas
struct bala
{
	float posX;
	float posY;
	bool existe;
	bool disparo;
};

// Estructura para la defensa
struct defensa{
	float posX;
	float posY;
};


// Estructura para la nave fantasma que aparece cada cierto tiempo
struct fantasma{
	float posX;
	float posY;
	bool existe;
};

int BLUE_AXIS   = 20;
int RED_AXIS    = -70;
int GREEN_AXIS  = -110;
 
float POSX      = 0.8;
float POSY      = -0.8;
int   BIG_ANG   = 0;
int   LIT_ANG   = 0;

float movEneX = 0.0f;
float movEneY = 0.0f;
float traslacionNave = 0.0f;
float disparo = 0.0f;
float posicionBala = -0.6f;
float xBala = 0.0f;


// Posiciones de la defensa
float defensaX = 0.0 - 1.5f;
float defensaY = 0.0 - 0.5f;


// Mitad del bloque del enemigo
float rectX = 0.12f;
float rectY = 0.035f;


/*
	x1y1  x4y4
	x2y2  x3y3
*/


// Arreglo para llevar la posicion de todos los enemigos
enemigo VectEne[CANT_ENEMIGOS];


// Vector para llevar las balas.
vector<bala> VectBala;


// Arreglo para las balas enemigas
bala VectBalaEnemy[CANT_ENEMIGOS];


// Vector para las defensas
vector<defensa> VectDefensa;


// Nave Fantasma
fantasma nFantasma = {FANTASMAX, FANTASMAY, false};


//Booleano para llevar la direccion de los enemigos
bool MOV_DER = true;


float maxEneX = -5.0f;


//Funcion para inicializar la posicion de los enemigos
void initEnemigos()
{
	float separacionx = 0.4;
	float separaciony = 0.15;
	float iniciox = INICIO_ENE_X;
	float inicioy = INICIO_ENE_Y;
	enemigo e;
	for(int i = 1; i <= CANT_ENEMIGOS; i++)
	{
		e.posX = iniciox;
		e.posY = inicioy;
		if ((i == 2) || (i == 4) || (i == 12) || (i == 16) || (i == 20) || (i == 29)) e.shots = 2;
		else e.shots = 1;
		VectEne[i-1] = e;

		// Agregamos la bala al enemigo correspondiente
		bala b = {e.posX,e.posY,true,false};
		VectBalaEnemy[i-1] = b;

		//Si dibuje una linea completa entonces tengo que reiniciar la posicion en x y bajar la posicion en y
		if((i % 6) == 0)
		{
			int div = (int) i / 2;
			if(div % 2 == 0) iniciox = INICIO_ENE_X;
			else             iniciox = INICIO_ENE_X + 0.1;

			inicioy -= separaciony;
		} else
			iniciox += separacionx;
	}
}


// Funcion para inicializar la posicion de las defensas
void initDefensas() {

	// Dibujar Defensa
	for (int i=0; i<CANT_DEFENSA; ++i) {

		defensa d = {defensaX, defensaY};
		VectDefensa.push_back(d);

		//Primera fila
		if(i<12){
			defensaX += 0.2;
			if(i == 3 || i == 7 ){
				defensaX+= 0.3;
			}
		}
	
		//Segunda fila	
		if(i == 11){
				
			defensaX = 0.1f - 1.5f;
			defensaY = 0.07f - 0.5f;
		}

		if(i > 11 && i < 17){
			     
			defensaX +=0.4;
			if(i == 13 || i == 15 ){
				defensaX+= 0.3;
			}
			
		}

		//Tercera fila	
		if(i == 18){
			defensaX = 0.20f - 1.5f;
			defensaY = 0.12f - 0.5f;
		}

		if(i > 18 && i < 25){    
			defensaX +=0.2;
			if(i == 20 || i == 22 ){
				defensaX+= 0.7;
			}
		}

	}


}


// Configuracion de la ventana
void changeViewport(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    float aspectratio = (float) w / (float) h;
 
    if (w <= h) {
        glOrtho(-1.0f,1.0f,-1.0/aspectratio,1.0/aspectratio,1.0f,10.0f);
    } else {
        glOrtho(-1.0*aspectratio,1.0*aspectratio,-1.0f,1.0f,1.0f,10.0f);
    }
 
    glMatrixMode(GL_MODELVIEW);
}
 

void drawEqAnglePolygon(float x, float y, float radio, float segmentos, float grados, float inicio){

	glBegin(GL_POLYGON);
		float x2,y2,i;
		for (i=0.0f; i<=segmentos; i++) {
			float t = (2.0f*grados/360.0f) * 3.1415926f * i/segmentos;
			float t2 = (2.0f*inicio/360.0f) * 3.1415926f;
			x2 = radio * cos(t+t2); //Ecuaciones param�tricas
			y2 = radio * sin(t+t2);
			glVertex2f(x+x2,y+y2);
		}
	glEnd();

}
 

// Funcion que dibuja un rectangulo
 void drawRectangle(){
	glBegin(GL_POLYGON);
		glVertex2f(-0.1f, 0.1f);
		glVertex2f(-0.1f, -0.1f);
		glVertex2f(0.1f, -0.1f);
		glVertex2f(0.1f, 0.1f);
	glEnd();
}


// Funcion que dibuja una defensa
void drawDefensa(float x, float y){
	glBegin(GL_POLYGON);
		glVertex2f( x, y);
		glVertex2f( x, 0.07f + y);
		glVertex2f(0.11f + x, 0.07f + y );
		glVertex2f(0.11f + x,  y );
	glEnd();
}


// Funcion que dibuja un enemigo
void drawEnemy(float posX, float posY, int typeEnemy){
	glPushMatrix();
		glTranslatef(posX, posY, 0.0f);
		if (typeEnemy == 1) glColor3f(0.0, 0.8, 0.0);
		else glColor3f(0.309804, 0.309804, 0.184314);
		glBegin(GL_POLYGON);
			glVertex2f(-0.12f, 0.035f);
			glVertex2f(-0.12f, -0.035f);
			glVertex2f(0.12f, -0.035f);
			glVertex2f(0.12f, 0.035f);
		glEnd();
	glPopMatrix();
}



// Funcion que dibuja un circulo
void drawCircle(float radio, float segmentos, float grados, float inicio){
	glBegin(GL_POLYGON);
		float x,y,i;
		for (i=0.0f; i<=segmentos; i++) {
			float t = (2.0f*grados/360.0f) * 3.1415926f * i/segmentos;
			float t2 = (2.0f*inicio/360.0f) * 3.1415926f;
			x = radio * cos(t+t2);
			y = radio * sin(t+t2);
			glVertex2f(x,y);
		}

	glEnd();
}


// Funcion que dibuja la nave fantasma que aparece cada cierto tiempo n dado
void drawFantasma() {
	
	glColor3f( 0.0,1.0, 1.0);
	drawEqAnglePolygon(nFantasma.posX+XARECTANGLE-0.05,nFantasma.posY+YARECTANGLE, 0.05, 60, 180,0 );

	glColor3f( 0.329412,0.329412, 0.329412); 
	glBegin(GL_POLYGON);
		glVertex3f(nFantasma.posX - XARECTANGLE, nFantasma.posY + YARECTANGLE, 0.0f );		
		glVertex3f(nFantasma.posX + XARECTANGLE, nFantasma.posY + YARECTANGLE, 0.0f );		
		glVertex3f(nFantasma.posX + XARECTANGLE, nFantasma.posY - YARECTANGLE, 0.0f );		
		glVertex3f(nFantasma.posX - XARECTANGLE, nFantasma.posY - YARECTANGLE, 0.0f );		
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(nFantasma.posX - XARECTANGLE,nFantasma.posY + YARECTANGLE, 0.0f );				
		glVertex3f(nFantasma.posX - XARECTANGLE, nFantasma.posY - YARECTANGLE, 0.0f );
		glVertex3f(nFantasma.posX - XARECTANGLE - 0.1, nFantasma.posY - YARECTANGLE, 0.0f );	
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(nFantasma.posX + XARECTANGLE,nFantasma.posY + YARECTANGLE, 0.0f );				
		glVertex3f(nFantasma.posX + XARECTANGLE, nFantasma.posY - YARECTANGLE, 0.0f );
		glVertex3f(nFantasma.posX + XARECTANGLE + 0.1, nFantasma.posY - YARECTANGLE, 0.0f );	
	glEnd();
}



// MEtodo para dibujar el grid
void drawGrid(){
 
    // GRID
 
    GLfloat xLength, yLength, xPoint, yPoint, scale;
    int i,j;
 
        // SHORT GRID (0.025)
 
        glColor3f( 0.1f, 0.1f, 0.1f ); 
        scale = GRID_SCALE_LONG/GRID_SCALE_SHORT;
        glBegin( GL_LINES );
            yLength = GRID_SCALE_SHORT * GRIDY * scale;
            for(j = -GRIDX * scale; j <= GRIDX * scale; j++ )
            {
                xPoint = GRID_SCALE_SHORT * j;
                glVertex3f( xPoint, -yLength, 0.0f );
                glVertex3f( xPoint, yLength,  0.0f );
            }
            xLength = GRID_SCALE_SHORT * GRIDX * scale;
            for(i = -GRIDY * scale; i <= GRIDY * scale; i++ )
            {
                yPoint = GRID_SCALE_SHORT * i;
                glVertex3f( -xLength, yPoint, 0.0f );
                glVertex3f(  xLength, yPoint, 0.0f );
            }
        glEnd();
 
        // LONG GRID (0.1)
         
        glColor3f( 0.2f, 0.2f, 0.2f );
        glBegin( GL_LINES );
            yLength = GRID_SCALE_LONG * GRIDY;
            for(j = -GRIDX; j <= GRIDX; j++ )
            {
                xPoint = GRID_SCALE_LONG * j;
                glVertex3f( xPoint, -yLength, 0.0f );
                glVertex3f( xPoint, yLength,  0.0f );
            }
            xLength = GRID_SCALE_LONG * GRIDX ;
            for(i = -GRIDY ; i <= GRIDY ; i++ )
            {
                yPoint = GRID_SCALE_LONG * i;
                glVertex3f( -xLength, yPoint, 0.0f );
                glVertex3f(  xLength, yPoint, 0.0f );
            }
        glEnd();
 
        // AXIS
        glColor3f( 1.0f, 1.0f, 1.0f );
        glBegin( GL_LINES );
            glVertex3f( 0.0f, -yLength, 0.0f );
            glVertex3f( 0.0f, yLength, 0.0f );
 
            glVertex3f( -xLength, 0.0f, 0.0f );
            glVertex3f( xLength, 0.0f, 0.0f );
        glEnd();
}


// Funcion para el manejo del teclado
void keyPressed(unsigned char key, int x, int y){

	switch(key) {
		case ' ': 
			bala b;
			b.posX = traslacionNave;
			b.posY = -0.6f;
			b.existe = true;
			b.disparo = false;
			VectBala.push_back(b);
			break;
	}
	glutPostRedisplay();
}


// Funcion que dado un enemigo y una bala, verifica que este ultimo choque con el enemigo
bool verificarPunto(enemigo e, bala b){
	if (((e.shots > 0) && b.posX >= (e.posX - rectX) && b.posY <= (e.posY + rectY) &&
		b.posX <= (e.posX + rectX) && b.posY >= (e.posY - rectY))){
		return true;
	}
	return false;
}


// Funcion que dado una defensa y una bala, verifica que este ultimo choque con la defensa
bool verificarDefensa(defensa d, float posX, float posY){
	if (( posX >= (d.posX) && posY >= (d.posY) &&
		posX <= (d.posX + 0.11f) && posY <= (d.posY + 0.08f ))){
		return true;
	}
	return false;
}

// Funcion que dado una bala, verifica que esta colisiona con la nave fantasma
bool verificarPuntoFantasma(bala b){
	if (((nFantasma.existe) && b.posX >= (nFantasma.posX - rectX) && 
	b.posY <= (nFantasma.posY + rectY) && b.posX <= (nFantasma.posX + rectX) && 
	b.posY >= (nFantasma.posY - rectY))){
		return true;
	}
	return false;
}

bool verificarJugador(float posX, float posY) {
	if (posX >= (traslacionNave - 0.05f) && posY >= (-0.8f - 0.035f) &&
		posX <= (traslacionNave + 0.05f) && posY <= (-0.8f + 0.035f )) {
	
		for (int i = 0; i< CANT_ENEMIGOS ; i++){
			VectEne[i].shots = 0; 
	    }
		jugadorVivo = false;
		return true;
	}
	return false;
}


// Funcion que dado una bala verifica si esta choca con alguna defensa
bool iterateDefensas(bala b){

	for( vector<defensa>::iterator d = VectDefensa.begin(); d!=VectDefensa.end();){
		if (verificarDefensa(*d, b.posX, b.posY)){
			d = VectDefensa.erase(d);
			score -= 30;
			return true;
		} else {
			d++;
		}
	}

	return false;

}


// Funcion que dado una bala verifica si esta choca con algun enemigo
bool iterateEnemies(bala b){

	for (int i = 0; i< CANT_ENEMIGOS ; i++){
		bool res = verificarPunto(VectEne[i], b);
		if (res == true) {
			VectEne[i].shots -= 1; 
			score += 100;
			return true;
		}
	}

	return false;

}


// Funcion que se encarga de hacer disparar a algun enemigo aleatoriamente
void disparaEnemy(int x){
	int enemy= rand() % CANT_ENEMIGOS;

	if (VectBalaEnemy[enemy].disparo == false){
		VectBalaEnemy[enemy].disparo = true;
		VectBalaEnemy[enemy].existe = true;
	}

	glutPostRedisplay();
	glutTimerFunc(500,disparaEnemy,6);


}


// Funcion que desplaza las balas enemigas
void desplazamientoBalaEnemy(int x) {
	
	for (int i=0; i<CANT_ENEMIGOS; i++) {

		if (VectBalaEnemy[i].existe) {
			VectBalaEnemy[i].posY -= 0.02;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(45,desplazamientoBalaEnemy,2);
}

void killDefensa(int x){
	
	// Muerte porque bloques enemigos chocan contra las defensas o el jugador
	for(int i=0; i<CANT_ENEMIGOS; i++){
		if (VectEne[i].shots>0){
			for( vector<defensa>::iterator d = VectDefensa.begin(); d!=VectDefensa.end();){
				if (verificarDefensa(*d,VectEne[i].posX,VectEne[i].posY)){
 					d = VectDefensa.erase(d);
					VectEne[i].shots = 0;
					break;
				} else {
					d++;
 				}	
			}
		}

		if (VectEne[i].shots>0){
			if (verificarJugador(VectEne[i].posX, VectEne[i].posY) || VectEne[i].posY < -0.8f + 0.035f) {
				jugadorVivo = false;
				VectEne[i].shots = 0;
 			}
 		}
		

	}

	glutPostRedisplay();
	glutTimerFunc(45,killDefensa,4);
}


// Funcion que hace aparecer la nave fantasma en tiempo dado n
void apareceFantasma(int x){

	if (!nFantasma.existe)	nFantasma.existe = true;
		
	glutPostRedisplay();
	glutTimerFunc(APARECERFANTASMA,apareceFantasma,6);
}


//Movimiento de la nave alien
void mueveFantasma(int x) {

		if (nFantasma.existe){
			nFantasma.posX += DESPLAZAFANTASMA;
			if (nFantasma.posX >1.6){
				nFantasma.existe =false;
				nFantasma.posX = FANTASMAX;
			}
		}

	glutPostRedisplay();
	glutTimerFunc(VELOCIDADFANTASMA,mueveFantasma,8);
}


// Fantasma muere por una bala de la nave
bool muereFantasma(bala b){
	if (nFantasma.existe){
		if (verificarPuntoFantasma(b)){
			nFantasma.existe = false;
			nFantasma.posX = FANTASMAX;
			score += 300;
			return true;
		}
	}
	return false;
}


// Funcion que determina como perder el juego
void gameOver(int x){

}


// Funcion que indica si se gana el juego o no
bool Winner(){
	// Si todos los enemigos estan muertos se gana el juego
	for (int i = 0; i < CANT_ENEMIGOS; i++){
		if (VectEne[i].shots == 0) continue;
		else return false;
		return true;
	}
}


void doSomething(int x) {

	for( vector<bala>::iterator i = VectBala.begin(); i!=VectBala.end();) {
		if ((*i).posY > 1.0) {
			i = VectBala.erase(i);
		} else {
			(*i).posY += 0.1f;
			if (iterateEnemies(*i) || iterateDefensas(*i) || muereFantasma(*i)) {
				i = VectBala.erase(i);
			} else {
				i++;
			}
		}

	}

	//Este caso se usa cuando dibujamos el grupo de enemigo bajo un piso y queremos que comience a moverse en direccion contraria
	if(movEneX == 0.0f){
		if(MOV_DER) movEneX = 0.1f;
		else		movEneX = -0.1f;
		movEneY = 0.0f;
	}else
	{
		if(MOV_DER)
		{
			if(maxEneX > 1.4f) 
			{
				MOV_DER = false;			
				movEneY = -0.05f;
				movEneX = 0.0f;
				if (velocidad >= 120) velocidad -= 70;
			} else
			{
				movEneX = 0.1f;
				movEneY = 0.0f;
			}
		} else
		{
			if(maxEneX < 0.5f)
			{
				MOV_DER = true;			   
				movEneY = -0.05f;
				movEneX = 0.0f;
				if (velocidad >= 170) velocidad -= 70;
			} else
			{
				movEneX = -0.1f;
				movEneY = 0.0f;
			}
		}
	}

	maxEneX = 0.0f;
	for(int i = 0; i < CANT_ENEMIGOS; i++) 
	{ 
		//Si el enemigo no ha sido golpeado por ninguna bala
		if(VectEne[i].shots>0)
		{
			//Calculamos la nueva posicion del enemigo
			float posActX = VectEne[i].posX + movEneX;

			//Si la posicion del enemigo actual es mayor a la maxima posicion guardada, entonces guardamos el maximo
			if(posActX > maxEneX) maxEneX = posActX;
			VectEne[i].posX = posActX;
			VectEne[i].posY += movEneY;

			// Posicionamos las balas de cada enemigo
			if (VectBalaEnemy[i].disparo == false){
				VectBalaEnemy[i].posX = VectEne[i].posX;
				VectBalaEnemy[i].posY = VectEne[i].posY;
			}
			
		}
	}

	// Muerte por bala del enemigo
	for( int i = 0; i < CANT_ENEMIGOS; i++) {
		if (VectBalaEnemy[i].disparo && VectBalaEnemy[i].existe){
			if (iterateDefensas(VectBalaEnemy[i]) || 
				verificarJugador(VectBalaEnemy[i].posX, VectBalaEnemy[i].posY)){
				VectBalaEnemy[i].existe = false;
				VectBalaEnemy[i].disparo = false;
			}
		}

	}

	glutPostRedisplay();
	glutTimerFunc(velocidad,doSomething,1);
}


// Funcion del manejo del teclado. Botones de izquiere y derecha
void SpecialInput(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:
			traslacionNave -= 0.05;
		break;
		case GLUT_KEY_RIGHT:
			traslacionNave += 0.05;
		break;
	}
	glutPostRedisplay();
}


// Funcion que dibuja la nave del jugador
void drawNave() {

	glColor3f(0.0,1.0,0.5);
	//Cuerpo de la nave
	glBegin(GL_POLYGON);
		glVertex2f(-0.05f, 0.0f);
		glVertex2f(-0.035f, -0.025f);
		glVertex2f(-0.025f, -0.025f);
		glVertex2f(-0.010f, -0.035f);
		glVertex2f(0.010f, -0.035f);
		glVertex2f(0.025f, -0.025f);
		glVertex2f(0.035f, -0.025f);
		glVertex2f(0.05f, 0.0f);
		glVertex2f(0.035f, 0.025f);
		glVertex2f(0.025f, 0.025f);
		glVertex2f(0.010f, 0.035f);
		glVertex2f(-0.010f, 0.035f);
		glVertex2f(-0.025f, 0.025f);
		glVertex2f(-0.035f, 0.025f);
	glEnd();
	//Tapita circular
	glColor3f(0.0f,1.0f,1.0f);
	glPushMatrix();
		glTranslatef(0.0f,0.035f,0.0f);
		drawCircle(0.005f,10.0f,180.0f,0.0f);
	glPopMatrix();
	//Patica izquierda
	glPushMatrix();
		glTranslatef(-0.01f,-0.035f,0.0f);
		glColor3f(0.184314,0.309804, 0.184314);
		glBegin(GL_TRIANGLES);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(-0.005f, -0.010f);
			glVertex2f(0.005f, 0.0f);
		glEnd();
		glColor3f(0.0f,1.0f,1.0f);
		glPushMatrix();
			glTranslatef(-0.007f,-0.011f,0.0f);
			drawCircle(0.004f,10.0f,360.0f,0.0f);
		glPopMatrix();
	glPopMatrix();
	//Patica derecha
	glPushMatrix();
		glTranslatef(0.0f,-0.035f,0.0f);
		glColor3f(0.184314,0.309804, 0.184314);
		glBegin(GL_TRIANGLES);
			glVertex2f(0.005f, 0.0f);
			glVertex2f(0.015f, -0.010f);
			glVertex2f(0.01f, 0.0f);
		glEnd();
		glColor3f(0.0f,1.0f,1.0f);
		glPushMatrix();
			glTranslatef(0.016f,-0.011f,0.0f);
			drawCircle(0.004f,10.0f,360.0f,0.0f);
		glPopMatrix();
	glPopMatrix();
}


// Funcion para escribir la puntuacion en pantalla
void printScore(string text, float x, float y) {
    glColor3f(0.0f,1.0f,0.0f);
    glRasterPos2f(x,y);
    for(int i = 0; i < text.length(); i++){ 
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text.data()[i]);
    }
}


void render(){
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
		glLoadIdentity();
		gluLookAt(  0.0f, 0.0f, 5.0f,   // Eye position
					0.0f, 0.0f, 0.0f,   // Look at position
					0.0f, 1.0f, 0.0f);  // Vector up direction
 
		drawGrid();
	
		if ((!Winner()) && jugadorVivo){
			//Nave del jugador
			glPushMatrix();
		
				glTranslatef(traslacionNave,-0.8f,0.0f);
				//Cuerpo
				glPushMatrix();
					glScalef(3.0f,3.0f,1.0f);
					drawNave();
				glPopMatrix();

			glPopMatrix();

			//Dibujo cada bala.
			for( vector<bala>::iterator i = VectBala.begin(); i!=VectBala.end(); i++) {
				glPushMatrix();
					glColor3f(1.0f,1.0f,1.0f);
					glTranslatef((*i).posX,(*i).posY,0.0f);
					drawCircle(0.02f,5.0f,360.0f,0.0f);
					glColor3f(1.0f,0.35f,0.75f);
				glPopMatrix();

			}

			// Dibujo de las balas de cada enemigo
			for (int i = 0; i < CANT_ENEMIGOS; i++){
				if ((VectEne[i].shots>0) && (VectBalaEnemy[i].disparo)){
					glPushMatrix();
						glColor3f(1.0f,1.0f,0.0f);
						glTranslatef(VectBalaEnemy[i].posX,VectBalaEnemy[i].posY,0.0f);
						drawCircle(0.02f,5.0f,360.0f,0.0f);
						glColor3f(1.0f,0.35f,0.75f);
					glPopMatrix();
				}
		

			}

			//Dibujo las defensas
			for( vector<defensa>::iterator i = VectDefensa.begin(); i!=VectDefensa.end(); i++) {
				glColor3f(0.658824, 0.658824, 0.658824);
				drawDefensa((*i).posX, (*i).posY);
				glColor3f(1.0f,0.35f,0.75f);

			}
	
			//Dibujamos los enemigos
			for(int i = 0; i < CANT_ENEMIGOS; i++) 
			{ 
				//Si el enemigo no ha muerto
				if(VectEne[i].shots>0)
				{
					float posActX = VectEne[i].posX;

					//Dibujamos el enemigo en la nueva posicion
					if (VectEne[i].shots==1){
						drawEnemy(posActX, VectEne[i].posY, 1);
					}else drawEnemy(posActX, VectEne[i].posY,2);

				}
			}

			// Nave Alien
			if (nFantasma.existe)	drawFantasma();
		}

		// Puntuacion
		printScore("Score: ", -1.5, -0.9);
		std::string s = std::to_string(score);
		printScore(s, -1.17, -0.9);

		// Mensaje al ganar el juego
		if (Winner() && jugadorVivo) printScore("�YOU WIN!", -0.15, 0);
		if (!jugadorVivo) printScore("�YOU LOSE!", -0.15, 0);

	glPopMatrix();
    glutSwapBuffers();
}
 
int main (int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800,500);
    glutCreateWindow("Test Opengl");
    glutReshapeFunc(changeViewport);
	initEnemigos(); 
	initDefensas();
	glutDisplayFunc(render);
    glutSpecialFunc(SpecialInput);
		glutTimerFunc(5000,killDefensa,4);
		glutTimerFunc(45,desplazamientoBalaEnemy,2);
		glutTimerFunc(100,doSomething,1);
		glutTimerFunc(1000,disparaEnemy,10);
		glutTimerFunc(APARECERFANTASMA,apareceFantasma,6);
		glutTimerFunc(VELOCIDADFANTASMA,mueveFantasma,8);
		
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW error");
        return 1;
    }
	glutKeyboardFunc(keyPressed);
    glutMainLoop();
    return 0;
}