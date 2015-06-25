/*
 *  Pacman
 *  Ryan Denzel
 *
 *  Add lighting and textures to pacman scene
 *
 *  Key bindings:
 *  a          Toggle axes
 *  arrows     Change view angle
 *  </>        Move Light manually
 *  n          Toggle light movement
 *  l          Toggle Light
 *  q/w  Zoom in and out
 *  0          Reset view angle
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

int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int move=0;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=45;         //  Elevation of view angle
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
float pos[2];           // Store pacman's new position
float past[2];
int press = 0;            // store key pressed for pacman's movement
int mouthOpen = 0;
float wall[50][2];  // For collision detection of walls
int p = 0;

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
  // if (mode)
    gluPerspective(fov,asp,dim,4*dim);
    gluLookAt(pos[0],5,15, pos[0],0,pos[1], 0,1,0);
   //  Orthogonal projection
   //else
     // glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
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

    
   // Draw Game grid
   // Commented out for more convenient viewing
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
  
    
   // Draw ghost
    ghost(-3,0,-3 , 0.4, 0, 0);
    ghost(3,0,3 , 0.4, 0, 1);
    ghost(-3,0,3 , 0.4, 0, 2);
    ghost(3,0,-3 , 0.4, 0, 3);
    
   // Draw Pacman
    pacman(pos[0],0,pos[1], .4,0,press);
    

   glPopMatrix();
   glDisable(GL_LIGHTING);
   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Pacman=%f,%f ",th,ph,pos[0],pos[1]);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    zh = fmod(90*t,360.0);
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
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
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   // Change pacman's postion
   else if (ch == 'w'){
//       int check = checkWallArray();
//       if (check == 1){
           pos[1]-= .5;
           press = 1;
//       }
   }
   else if (ch == 's'){
//       int check = checkWallArray();
//       if (check == 1){
           pos[1]+=.5;
           press = 2;
//       }
   }
   else if (ch == 'a'){
//       int check = checkWallArray();
//       if (check == 1){
           pos[0]-=.5;
           press = 3;
//       }
   }
   else if (ch == 'd'){
//       int check = checkWallArray();
//       if (check == 1){
           pos[0]+=.5;
           press = 4;
//       }
   }

   else if (ch == 'l' || ch == 'L')
       light = 1-light;
   else if (ch == 'n' || ch == 'N')
       move = 1-move;
    //  Move light
   else if (ch == '<')
       zh += 1;
   else if (ch == '>')
       zh -= 1;

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
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   // Load Texture
   texture[0] = LoadTexBMP("brick.bmp");
   //texture[1] = LoadTexBMP("pacman.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
