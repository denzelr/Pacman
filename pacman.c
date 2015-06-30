/*
 *  Pacman
 *  Ryan Denzel
 *
 *  Pacman game
 *
 *  Key bindings:
 *  w/a/s/d    Move Pacman
 *  ESC        Exit
 */
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

int mode=0;       //  Projection mode
int move=0;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=35;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
int light=1;      //  Lighting
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
unsigned int texture[2]; // Textures
int pos[2];           // Store pacman's new position
int press = 0;            // store key pressed for pacman's movement
int p = 0;
float posx1 = -9;   // Red Ghost initial position x
float posz1 = -5;   // Red Ghost initial position z
float posx2 = -5;   // BlueGhost initial position x
float posz2 = -5;   // Blue Ghost initial positionz
float posx3 = 7;   // Pink Ghost initial position x
float posz3 = -5;   // Pink Ghost initial positionz
float posx4 = 0;   // orange Ghost initial position x
float posz4 = -3;   // orange Ghost initial positionz
int redTrack = 0;   // Tracking red ghost's position
int blueTrack = 0;  // Tracking blue ghost's position
int pinkTrack = 0;  // Tracking pink ghost's position
int orangeTrack = 0;  // Tracking orange ghost's position
float dx = .25;      // Movement speed of ghosts
int dead = 0;       // Check if pacman hit a ghost
int lives = 3;      //Pacman's lives
int done = 0;       // Game over
int score= 0;       // Score
// For collision detection of walls
int wall[13][21] ={
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0},
    {0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,1,0},
    {0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0},
    {0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,1,0},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
// For tracking which balls need to be rendered
int balls[13][21] ={
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0},
    {0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,1,0},
    {0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0},
    {0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,1,0},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
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
 *  Set projection
 */
static void Project()
{
    glPopMatrix();
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
    gluPerspective(fov,asp,dim,4*dim);
    gluLookAt(pos[0],5,15, pos[0],2,pos[1], 0,1,0);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

int checkWallArray(){
    int len = sizeof(wall);
    int i;
    
    for(i = 0; i < len; i++)
    {
        if(pos[0] == wall[i][0])
        {
            if(pos[1] == wall[i][1])
                return 1;
        }
    }
    return 0;
}


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
    //  Save transformation
    glPushMatrix();
    wall[p][0] = x;
    wall[p][1] = z;
    //  Offset
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[0]);
    //  Cube
    //  Front
    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    glEnd();
    //  Back
    glBegin(GL_QUADS);
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
    glEnd();
    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
    glEnd();
    //  Left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void Vertex(double th,double ph)
{
    double x = Sin(th)*Cos(ph);
    double y = Sin(ph);
    double z =         Cos(th)*Cos(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);

}

static void sphere(double x,double y,double z, double r, int rotate, int col)
{
    const int d=5;
    int th,ph;
    
    glBegin(GL_TRIANGLE_FAN);
    if (col == 0){
        glColor3f(1,0,0);
    }
    else if (col == 1){
        glColor3f(0,0,1);
    }
    else if (col == 2){
        glColor3f(1,0,1);
    }
    else if (col == 3){
        glColor3f(1,.5,0);
    }
    else if (col == 4){
        glColor3f(1,1,1);
    }
    else if (col == 5){
        glColor3f(0,0,0);
    }
    Vertex(0,-90);
    for (th=0;th<=360;th+=d)
    {
        Vertex(th,d-90);
    }
    glEnd();
    
    //  Latitude bands
    for (ph=d-90;ph<=90-2*d;ph+=d)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=d)
        {
            Vertex(th,ph);
            Vertex(th,ph+d);
        }
        glEnd();
    }
    
    //  North pole cap
    glBegin(GL_TRIANGLE_FAN);
    Vertex(0,90);
    for (th=0;th<=360;th+=d)
    {
        Vertex(th,90-d);
    }
    glEnd();
    glPopMatrix();

}

static void cylinder(double r)
{
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 360; ++i)
    {
        glNormal3f(cos(i),sin(i),0);
        glVertex3f(cos(i),sin(i),0);
        glVertex3f(cos(i),sin(i),3*r);
    }
    
    glEnd();
    glPopMatrix();
    
 
}

static void bell(double x, double y, double z, double r){
    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glRotated(-90,1,0,0);
    glScaled(r,r,r);

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(.55,.47,.14);
    for (int i = 0; i < 360; ++i)
    {
        glNormal3f(cos(i),sin(i),0);
        glVertex3f(3*cos(i),3*sin(i),0);
        glVertex3f(2*cos(i),2*sin(i),2*r);
        glVertex3f(1.5*cos(i),1.5*sin(i),4*r);
        glVertex3f(2*cos(i),2*sin(i),10*r);
        glVertex3f(cos(i),sin(i),12*r);
        glVertex3f(0,0,12.1*r);
        
    }
    
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(0.5, 0.35, 0.05);
    glTranslated(x-.2,y+1,z);
    glRotated(90,0,1,0);
    glScaled(r/2,r/2,r);
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 360; ++i)
    {
        glNormal3f(cos(i),sin(i),0);
        glVertex3f(cos(i),sin(i),-20*r);
        glVertex3f(cos(i),sin(i),20*r);
    }
    
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(x-1.5,y-.1,z);
    glRotated(90,1,0,0);
    glScaled(r/2,r/2,r);
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 360; ++i)
    {
        glNormal3f(cos(i),sin(i),0);
        glVertex3f(cos(i),sin(i),-20*r);
        glVertex3f(cos(i),sin(i),20*r);
    }
    
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslated(x+1.2,y-.1,z);
    glRotated(90,1,0,0);
    glScaled(r/2,r/2,r);
    
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 360; ++i)
    {
        glNormal3f(cos(i),sin(i),0);
        glVertex3f(cos(i),sin(i),-20*r);
        glVertex3f(cos(i),sin(i),20*r);
    }
    
    glEnd();
    glPopMatrix();
    

}

static void ball(double x, double y, double z, double r, int col){
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glScaled(r,r,r);
    sphere(x,y,z,r,0,col);
    glPopMatrix();

}

static void ghost(double x,double y,double z, double r, int rotate, int col)
{
    
    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    if (rotate != 0){
        glRotated(rotate,0,1,0);
    }
    glScaled(r,r,r);
   
    //Draw Body
    sphere(x,y,z,r,rotate,col);
    
    //draw body
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(90,1,0,0);
    if (rotate != 0){
        glRotated(rotate,0,1,0);
    }
    glScaled(r,r,r);
    
    cylinder(r);
    
    // Draw eyes
    // offset for right eye
    glPushMatrix();
    glPolygonOffset(0,0);
    glTranslated(x+(r/2),y,z+r);
    glScaled(r/3,r/3,r/3);
    
    // right eye
    sphere(x,y,z,r,rotate,4);
    
    // offset for left eye
    glPushMatrix();
    glTranslated(x-(r/2),y,z+r);
    glScaled(r/3,r/3,r/3);
    
    //left eye
    sphere(x,y,z,r,rotate,4);
    
}

static void pacman(double x,double y,double z, double r, int rotate, int press)
{
    const int d=5;
    int th,ph;

    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glRotated(90,1,0,0);
    glRotated(rotate+135,0,1,0);
    if (press == 1){
        glRotated(90,1,0,1);
    }
    else if (press == 2){
        glRotated(-90,1,0,1);
    }
    else if (press == 3){
        glRotated(180,0,1,0);
    }

    glScaled(r,r,r);
    glPolygonOffset(0,0);
    glColor3f(1,1,0);
    
    //Draw pacman; requires special semi sphere
    glBegin(GL_TRIANGLE_FAN);
    Vertex(0,-90);
    for (th=0;th<=360;th+=d)
    {
        Vertex(th,d-90);
    }
    glEnd();
    for (ph=d-90;ph<=90-d;ph+=d)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=rotate;th<=360;th+=d)
        {
            if (th <= 275){
                glColor3f(1,1,0);
            }
            else { glColor3f(0,0,0);}

            Vertex(th,ph);
            Vertex(th,ph+d);
        }
        glEnd();
    }
    //  North pole cap
    glBegin(GL_TRIANGLE_FAN);
    Vertex(0,90);
    for (th=0;th<=360;th+=d)
    {
        Vertex(th,90-d);
    }
    glEnd();
    glPopMatrix();
 
    
}

static void floorTiles(double x, double y, double z, double r, float col){
    glColor3f(col,1,0);
    glNormal3d(0,1,0);
    glBegin(GL_POLYGON);
    {
        for (int i = 0; i < 6; i++)
        {
            float angle = 2 * M_PI / 6 * (i + 0.5);
            float tempx = x + r * cos(angle);
            float tempz = z + r * sin(angle);
            glVertex3f(tempx, y, tempz);
        }
    }
    glEnd();
}

static void lightSource(double x,double y,double z,double r)
{
    int th,ph;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y+10,z+5);
    glScaled(r,r,r);
    //  White ball
    glColor3f(1,1,1);
    glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    //  Bands of latitude
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal - set world orientation
   //glTranslated(pos[0],0,pos[1]);
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);
    
    //  Flat or smooth shading
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);
    
    // Add light to the scene
    if (light)
    {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        lightSource(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    else
        glDisable(GL_LIGHTING);
    // Draw Bell towers
    bell(8,2,-6,.25);
    bell(-8,2,-6,.25);
    
    for (int i = -9; i <= 9; i+=2){
        for (int j = -5; j <= 5; j +=4){
            floorTiles(i,-.5,j,1.2,0);
        }
    }
    for (int i = -8; i <= 8; i+=2){
        for (int j = -3; j <= 5; j +=4){
            floorTiles(i,-.5,j,1.2,.5);
        }
    }
   
    
   // Draw Game grid
     for (int k = -4; k <= -1; k++){
        cube(k,0,-4, .5,.5,.5 , 0);
        cube(k,0,4, .5,.5,.5 , 0);
        cube(-4,0,k, .5,.5,.5 , 0);
        cube(4,0,k, .5,.5,.5 , 0);
    }
    for (int k = 1; k <= 4; k++){
        cube(k,0,-4, .5,.5,.5 , 0);
        cube(k,0,4, .5,.5,.5 , 0);
        cube(-4,0,k, .5,.5,.5 , 0);
        cube(4,0,k, .5,.5,.5 , 0);
    }
    for (int k = -10; k <= 10; k++){
        cube(k,0,-6, .5,.5,.5 , 0);
        cube(k,0,6, .5,.5,.5 , 0);
    }
    for (int k = -6; k <= 6; k++){
        cube(-10,0,k, .5,.5,.5 , 0);
        cube(10,0,k, .5,.5,.5 , 0);
    }
    for (int k = -4; k <= 4; k++){
        cube(-8,0,k, .5,.5,.5 , 0);
        cube(8,0,k, .5,.5,.5 , 0);
    }
    for (int k = -6; k <= -5; k++){
        cube(-6,0,k, .5,.5,.5 , 0);
        cube(6,0,k, .5,.5,.5 , 0);
    }
    for (int k = -3; k <= 3; k++){
        cube(-6,0,k, .5,.5,.5 , 0);
        cube(6,0,k, .5,.5,.5 , 0);
    }
    for (int k = 5; k <= 6; k++){
        cube(-6,0,k, .5,.5,.5 , 0);
        cube(6,0,k, .5,.5,.5 , 0);
    }
    for (int j = -2; j <= -1; j++){
        cube(j,0,-2, .5,.5,.5 , 0);
        cube(j,0,2, .5,.5,.5 , 0);
        cube(-2,0,j, .5,.5,.5 , 0);
        cube(2,0,j, .5,.5,.5 , 0);
    }
    for (int l = 1; l <= 2; l++){
        cube(l,0,-2, .5,.5,.5 , 0);
        cube(l,0,2, .5,.5,.5 , 0);
        cube(-2,0,l, .5,.5,.5 , 0);
        cube(2,0,l, .5,.5,.5 , 0);
    }
    for (int i = -6; i <= 6; i++){
        for (int j = -10; j <= 10; j++){
            if (balls[i+6][j+10] == 1){
                ball(j,0,i, 0.1,4);
            }
        }
    }
    
   // Draw ghosts and animate movement
    //Red Ghost Movement
    if (redTrack == 0){
        posx1 += dx;
        ghost(posx1,0,-5 , 0.4, 0, 0);
        if (posx1 >= -7){
            redTrack = 1;
        }
        glutIdleFunc(idle);
    }
    else if (redTrack == 1){
        posz1 += dx;
        ghost(-7,0,posz1 , 0.4, 0, 0);
        if (posz1 >= 5){
            redTrack = 2;
        }
        glutIdleFunc(idle);
    }
    else if (redTrack == 2){
        posx1 -= dx;
        ghost(posx1,0,5 , 0.4, 0, 0);
        if (posx1 <= -9){
            redTrack = 3;
        }
        glutIdleFunc(idle);
    }
    else if (redTrack == 3){
        posz1 -= dx;
        ghost(-9,0,posz1 , 0.4, 0, 0);
        if (posz1 <= -5){
            redTrack = 0;
        }
        glutIdleFunc(idle);
    }
   
    //Blue Ghost Movement
    if (blueTrack == 0){
        posx2 += dx;
        ghost(posx2,0,-5 , 0.4, 0, 1);
        if (posx2 >= 5){
            blueTrack = 1;
        }
        glutIdleFunc(idle);
    }
    else if (blueTrack == 1){
        posz2 += dx;
        ghost(5,0,posz2 , 0.4, 0, 1);
        if (posz2 >= 5){
            blueTrack = 2;
        }
        glutIdleFunc(idle);
    }
    else if (blueTrack == 2){
        posx2 -= dx;
        ghost(posx2,0,5 , 0.4, 0, 1);
        if (posx2 <= -5){
            blueTrack = 3;
        }
        glutIdleFunc(idle);
    }
    else if (blueTrack == 3){
        posz2 -= dx;
        ghost(-5,0,posz2 , 0.4, 0, 1);
        if (posz2 <= -5){
            blueTrack = 0;
        }
        glutIdleFunc(idle);
    }
    
    //Pink Ghost Movement
    if (pinkTrack == 0){
        posx3 += dx;
        ghost(posx3,0,-5 , 0.4, 0, 2);
        if (posx3 >= 9){
            pinkTrack = 1;
        }
        glutIdleFunc(idle);
    }
    else if (pinkTrack == 1){
        posz3 += dx;
        ghost(9,0,posz3 , 0.4, 0, 2);
        if (posz3 >= 5){
            pinkTrack = 2;
        }
        glutIdleFunc(idle);
    }
    else if (pinkTrack == 2){
        posx3 -= dx;
        ghost(posx3,0,5 , 0.4, 0, 2);
        if (posx3 <= 7){
            pinkTrack = 3;
        }
        glutIdleFunc(idle);
    }
    else if (pinkTrack == 3){
        posz3 -= dx;
        ghost(7,0,posz3 , 0.4, 0, 2);
        if (posz1 <= -5){
            pinkTrack = 0;
        }
        glutIdleFunc(idle);
    }
    
    //Orange Ghost Movement
    if (orangeTrack == 0){
        posx4 += dx;
        ghost(posx4,0,-3 , 0.4, 0, 3);
        if (posx4 >= 3){
            orangeTrack = 1;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 1){
        posz4 += dx;
        ghost(3,0,posz4 , 0.4, 0, 3);
        if (posz4 >= 0){
            orangeTrack = 2;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 2){
        posx4 += dx;
        ghost(posx4,0,0 , 0.4, 0, 3);
        if (posx4 >= 5){
            orangeTrack = 3;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 3){
        posz4 -= dx;
        ghost(5,0,posz4 , 0.4, 0, 3);
        if (posz4 <= -4){
            orangeTrack = 4;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 4){
        posx4 += dx;
        ghost(posx4,0,-4 , 0.4, 0, 3);
        if (posx4 >= 7){
            orangeTrack = 5;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 5){
        posz4 += dx;
        ghost(7,0,posz4 , 0.4, 0, 3);
        if (posz4 >= 4){
            orangeTrack = 6;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 6){
        posx4 -= dx;
        ghost(posx4,0,4 , 0.4, 0, 3);
        if (posx4 <= 5){
            orangeTrack = 7;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 7){
        posz4 += dx;
        ghost(5,0,posz4 , 0.4, 0, 3);
        if (posz4 >= 5){
            orangeTrack = 8;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 8){
        posx4 -= dx;
        ghost(posx4,0,5 , 0.4, 0, 3);
        if (posx4 <= -5){
            orangeTrack = 9;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 9){
        posz4 -= dx;
        ghost(-5,0,posz4 , 0.4, 0, 3);
        if (posz4 <= 4){
            orangeTrack = 10;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 10){
        posx4 -= dx;
        ghost(posx4,0,4 , 0.4, 0, 3);
        if (posx4 <= -7){
            orangeTrack = 11;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 11){
        posz4 -= dx;
        ghost(-7,0,posz4 , 0.4, 0, 3);
        if (posz4 <= -4){
            orangeTrack = 12;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 12){
        posx4 += dx;
        ghost(posx4,0,-4 , 0.4, 0, 3);
        if (posx4 >= -5){
            orangeTrack = 13;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 13){
        posz4 += dx;
        ghost(-5,0,posz4 , 0.4, 0, 3);
        if (posz4 >= 0){
            orangeTrack = 14;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 14){
        posx4 += dx;
        ghost(posx4,0,0 , 0.4, 0, 3);
        if (posx4 >= -3){
            orangeTrack = 15;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 15){
        posz4 -= dx;
        ghost(-3,0,posz4 , 0.4, 0, 3);
        if (posz4 <= -3){
            orangeTrack = 16;
        }
        glutIdleFunc(idle);
    }
    else if (orangeTrack == 16){
        posx4 += dx;
        ghost(posx4,0,-3 , 0.4, 0, 3);
        if (posx4 >= 0){
            orangeTrack = 0;
        }
        glutIdleFunc(idle);
    }
    
    // Win condition
    if (score == 132){
        glPopMatrix();
        glDisable(GL_LIGHTING);
        //  Display parameters
        glColor3f(1,1,1);
        glWindowPos2i(400,400);
        Print("Congratulations: You Win");
        pos[0] = 0;
        pos[1] = 0;
    }
    
   // Detect ghost collision
   if ((pos[0] == posx1 && pos[1] == posz1) ||
        (pos[0] == posx2 && pos[1] == posz2) ||
        (pos[0] == posx3 && pos[1] == posz3) ||
        (pos[0] == posx4 && pos[1] == posz4)){
        lives--;
        dead = 1;
        if (lives == 0){
            done = 1;
        }
    }
    // Draw Pacman
    if (dead == 0){
        pacman(pos[0],0,pos[1], .4,0,press);
    }
    else if (done == 1){
        glPopMatrix();
        glDisable(GL_LIGHTING);
        //  Display parameters
        glColor3f(1,1,1);
        glWindowPos2i(400,400);
        Print("Game Over");
        pos[0] = 0;
        pos[1] = 0;
    }
    else{
        pos[0] = 0;
        pos[1] = 0;
        dead = 0;
        pacman(pos[0],0,pos[1], .4,0,press);
        glutPostRedisplay();
    }

   glPopMatrix();
   glDisable(GL_LIGHTING);
   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Lives:%i Score=%i",lives,score);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}


/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
     //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   // Change pacman's postion
   else if (ch == 'w'){
       int a = pos[0];
       int b = pos[1];
       if (wall[b+5][a+10] == 1){
           pos[1]-= 1;
           press = 1;
           if(balls[b+6][a+10] == 1){
               balls[b+6][a+10] = 0;
               score++;

           }
       }
   }
   else if (ch == 's'){
       int a = pos[0];
       int b = pos[1];
       if (wall[b+7][a+10] == 1){
           pos[1]+=1;
           press = 2;
           if(balls[b+6][a+10] == 1){
               balls[b+6][a+10] = 0;
               score++;
           }
       }

   }
   else if (ch == 'a'){
       int a = pos[0];
       int b = pos[1];
       if (wall[b+6][a+9] == 1){
           pos[0]-=1;
           press = 3;
           if(balls[b+6][a+10] == 1){
               balls[b+6][a+10] = 0;
               score++;
           }
       }

   }
   else if (ch == 'd'){
       int a = pos[0];
       int b = pos[1];
       if (wall[b+6][a+11] == 1){
           pos[0]+=1;
           press = 4;
           if(balls[b+6][a+10] == 1){
               balls[b+6][a+10] = 0;
               score++;
           }
       }
   }

   //  Reproject
   Project();
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

// fatal.c from example 15
void Fatal(const char* format , ...)
{
    va_list args;
    va_start(args,format);
    vfprintf(stderr,format,args);
    va_end(args);
    exit(1);
}

//loadtexbmp.c from example 15
/*
 *  Reverse n bytes
 */
static void Reverse(void* x,const int n)
{
    int k;
    char* ch = (char*)x;
    for (k=0;k<n/2;k++)
    {
        char tmp = ch[k];
        ch[k] = ch[n-1-k];
        ch[n-1-k] = tmp;
    }
}

/*
 *  Load texture from BMP file
 */
unsigned int LoadTexBMP(const char* file)
{
    unsigned int   texture;    // Texture name
    FILE*          f;          // File pointer
    unsigned short magic;      // Image magic
    unsigned int   dx,dy,size; // Image dimensions
    unsigned short nbp,bpp;    // Planes and bits per pixel
    unsigned char* image;      // Image data
    unsigned int   k;          // Counter
    int            max;        // Maximum texture dimensions
    
    //  Open file
    f = fopen(file,"rb");
    if (!f) Fatal("Cannot open file %s\n",file);
    //  Check image magic
    if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
    if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
    //  Seek to and read header
    if (fseek(f,16,SEEK_CUR) || fread(&dx ,4,1,f)!=1 || fread(&dy ,4,1,f)!=1 ||
        fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
        Fatal("Cannot read header from %s\n",file);
    //  Reverse bytes on big endian hardware (detected by backwards magic)
    if (magic==0x424D)
    {
        Reverse(&dx,4);
        Reverse(&dy,4);
        Reverse(&nbp,2);
        Reverse(&bpp,2);
        Reverse(&k,4);
    }
    //  Check image parameters
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
    if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
    if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
    if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
    if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
    if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
    //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
    for (k=1;k<dx;k*=2);
    if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
    for (k=1;k<dy;k*=2);
    if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif
    
    //  Allocate image memory
    size = 3*dx*dy;
    image = (unsigned char*) malloc(size);
    if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
    //  Seek to and read image
    if (fseek(f,20,SEEK_CUR) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
    fclose(f);
    //  Reverse colors (BGR -> RGB)
    for (k=0;k<size;k+=3)
    {
        unsigned char temp = image[k];
        image[k]   = image[k+2];
        image[k+2] = temp;
    }
    //  Generate 2D texture
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    //  Copy image
    glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
    //  Scale linearly when image size doesn't match
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    
    //  Free image memory
    free(image);
    //  Return texture name
    return texture;
}


/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,800);
   glutCreateWindow("Pacman: Ryan Denzel");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(key);
   // Load Texture
   texture[0] = LoadTexBMP("brick.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
