/*****************************************************************************
 * Michael Usner
 * CS433 - Asignment 3
 * November 6, 2003
 *
 * cube.cpp
 *****************************************************************************/
#include "cube.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <fstream>
#include <math.h>
#include "GL/gl.h"
#include <stdio.h>
#include "cubeModel.h"

// the size of a single cube
#define CUBESIZE 1.0

// the size of the face decals
#define FACESIZE 0.83f

int DEG=10;       // degrees to rotate the face of the cube

#define DEG2RAD 0.017453292519943295769236907684886
// make decals a just a bit higher than the face to avoid tearing
#define FACEZ    1.04f  

#define RED     0
#define GREEN   1
#define BLUE    2
#define ORANGE  3
#define WHITE   4
#define YELLOW  5

GLfloat color[6][4];        // the face colors
GLfloat cubeColor[4];
int   selFaceNum = 0;
float selAnim = 0.0;        // the animation control var for selected faces
float rotAnin = 0.0;        // animation rotational var

bool cubeRotating = false;  // the cube is rotating at the moment..
int  rotCount = 0;          // stops the rotation when we hit 90
int  rotAngle=0;

// set the values of two matrices equal (used for assignments)
void set(float m1[16], float m2[16])
{
    for (int i=0; i<16; i++)
        m1[i] = m2[i];
}

// 4x4 matrix multiply
void matmat(GLfloat ma[16], GLfloat mb[16], GLfloat maxb[16])
{
    maxb[0] = ma[0]*mb[0] + ma[4]*mb[1] + ma[8]*mb[2] + ma[12]*mb[3];
    maxb[1] = ma[1]*mb[0] + ma[5]*mb[1] + ma[9]*mb[2] + ma[13]*mb[3];
    maxb[2] = ma[2]*mb[0] + ma[6]*mb[1] + ma[10]*mb[2] + ma[14]*mb[3];
    maxb[3] = ma[3]*mb[0] + ma[7]*mb[1] + ma[11]*mb[2] + ma[15]*mb[3];
  
    maxb[4] = ma[0]*mb[4] + ma[4]*mb[5] + ma[8]*mb[6] + ma[12]*mb[7];
    maxb[5] = ma[1]*mb[4] + ma[5]*mb[5] + ma[9]*mb[6] + ma[13]*mb[7];
    maxb[6] = ma[2]*mb[4] + ma[6]*mb[5] + ma[10]*mb[6] + ma[14]*mb[7];
    maxb[7] = ma[3]*mb[4] + ma[7]*mb[5] + ma[11]*mb[6] + ma[15]*mb[7];
  
    maxb[8] = ma[0]*mb[8] + ma[4]*mb[9] + ma[8]*mb[10] + ma[12]*mb[11];
    maxb[9] = ma[1]*mb[8] + ma[5]*mb[9] + ma[9]*mb[10] + ma[13]*mb[11];
    maxb[10] = ma[2]*mb[8] + ma[6]*mb[9] + ma[10]*mb[10] + ma[14]*mb[11];
    maxb[11] = ma[3]*mb[8] + ma[7]*mb[9] + ma[11]*mb[10] + ma[15]*mb[11];
  
    maxb[12] = ma[0]*mb[12] + ma[4]*mb[13] + ma[8]*mb[14] + ma[12]*mb[15];
    maxb[13] = ma[1]*mb[12] + ma[5]*mb[13] + ma[9]*mb[14] + ma[13]*mb[15];
    maxb[14] = ma[2]*mb[12] + ma[6]*mb[13] + ma[10]*mb[14] + ma[14]*mb[15];
    maxb[15] = ma[3]*mb[12] + ma[7]*mb[13] + ma[11]*mb[14] + ma[15]*mb[15];
}

// matrix/vector multiply
void matvec(GLfloat ma[16], GLfloat v[4], GLfloat mxv[4])
{
    mxv[0] = ma[0]*v[0] + ma[4]*v[1] + ma[8]*v[2] + ma[12]*v[3];
    mxv[1] = ma[1]*v[0] + ma[5]*v[1] + ma[9]*v[2] + ma[13]*v[3];
    mxv[2] = ma[2]*v[0] + ma[6]*v[1] + ma[10]*v[2] + ma[14]*v[3];
    mxv[3] = ma[3]*v[0] + ma[7]*v[1] + ma[11]*v[2] + ma[15]*v[3];
}

// create a 4x4 identity matrix
void eye(float mat[16]) {
    mat[0]=mat[5]=mat[10]=mat[15]=1.0;
    mat[1]=mat[2]=mat[3]=mat[4]=mat[6]=mat[7]=mat[8]=mat[9]=mat[11]=mat[12]=mat[13]=mat[14]=0.0;
};

// calculate the normals for a single cube
void CubeModel::calcNormals()
{
    float ux,uy,uz,vx,vy,vz,nx,ny,nz,length;
    int count=0;
    for (int i=0; i<VERTICES; i+=9)
    {
        ux = fac[i+3] - fac[i+0];
        uy = fac[i+4] - fac[i+1];
        uz = fac[i+5] - fac[i+2];
        vx = fac[i+6] - fac[i+0];
        vy = fac[i+7] - fac[i+1];
        vz = fac[i+8] - fac[i+2];

        nx=(uy*vz) - (uz*vy);
        ny=(uz*vx) - (ux*vz);
        nz=(ux*vy) - (uy*vx);
        length = float(sqrt(pow(nx,2)+pow(ny,2)+pow(nz,2)));
        if (length!=0.0)
        {
            norm[count]=nx/length;
            norm[count+1]=ny/length;
            norm[count+2]=nz/length;
            count+=3;
        }
    }
}
 
// cube model constructor: load the model and calc normals
CubeModel::CubeModel() 
{
    // load the model
//    ifstream in("cube.raw");
//    if (in.fail()) {
//        cerr << "Failed to open cube.raw!\n";
//        exit(1);
//    }

//    int count=0;
    // read the vertices
//    for (int i=0; i<VERTICES; i++)
 //   {
//        in >> fac[count++];
//    }
    calcNormals();

    cubeColor[0] = 0.4f;
    cubeColor[1] = 0.4f;
    cubeColor[2] = 0.4f;
    cubeColor[3] = 1.0f;
}

// Rubik's cube constructor.  Initialize the cubes
RubiksCube::RubiksCube()
{
    int id=100;         // the cube's ID  

    // initialize the face colors         // Legend:
    // red                                // color[0] = red
    color[0][0] = 1.0f;                   // color[1] = green
    color[0][1] = 0.0f;                   // color[2] = blue
    color[0][2] = 0.0f;
    // green                              // color[3] = orange
    color[1][0] = 0.0f;                   // color[4] = white
    color[1][1] = 1.0f;                   // color[5] = yellow
    color[1][2] = 0.0f;

    // blue
    color[2][0] = 0.0f;
    color[2][1] = 0.0f;
    color[2][2] = 1.0f;
    
    // orange
    color[3][0] = 1.0f;
    color[3][1] = 0.5f;
    color[3][2] = 0.0f;
    
    // white
    color[4][0] = 1.0f;
    color[4][1] = 1.0f;
    color[4][2] = 1.0f;

    // yellow
    color[5][0] = 1.0f;
    color[5][1] = 1.0f;
    color[5][2] = 0.0f;
        
    // set the alpha values
    color[0][3] = color[1][3] = color[2][3] = color[3][3] = color[4][3] = color[5][3] = 1.0;

    // initialize the cubes
    for (int z=0; z<3; z++)
    {
        for (int y=0; y<3; y++)
        {
            for (int x=0; x<3; x++)
            {
                cubes[x][y][z].id = 0;
                
                // set the center cube's ID's
                if (x==1 && y==1 && z==2)
                    cubes[x][y][z].id = 1;
                if (x==1 && y==1 && z==0)
                    cubes[x][y][z].id = 2;
                if (y==1 && z==1 && x==0)
                    cubes[x][y][z].id = 3;
                if (y==1 && z==1 && x==2)
                    cubes[x][y][z].id = 4;
                if (x==1 && z==1 && y==2)
                    cubes[x][y][z].id = 5;
                if (x==1 && z==1 && y==0)
                    cubes[x][y][z].id = 6;

                // start the cubes with the id matrix
                eye((float *)&cubes[x][y][z].mat);
                
                // set up the translation matrix
                cubes[x][y][z].mat[3][0]=x-1.0f;
                cubes[x][y][z].mat[3][1]=y-1.0f;
                cubes[x][y][z].mat[3][2]=z-1.0f;

                // scale the cubes
                cubes[x][y][z].mat[0][0]=0.492f;
                cubes[x][y][z].mat[1][1]=0.492f;
                cubes[x][y][z].mat[2][2]=0.492f;

                // set the face colors to NULL
                cubes[x][y][z].f1col = cubeColor;
                cubes[x][y][z].f2col = cubeColor;
                cubes[x][y][z].f3col = cubeColor;
                cubes[x][y][z].f4col = cubeColor;
                cubes[x][y][z].f5col = cubeColor;
                cubes[x][y][z].f6col = cubeColor;

                // set the valid face colors
                if (y==0) // bottom faces
                {
                    cubes[x][y][z].f6col = color[BLUE];
                    cubes[x][y][z].f[6] = true;
                }
                if (y==2)   // top faces
                {
                    cubes[x][y][z].f5col = color[RED];
                    cubes[x][y][z].f[5] = true;
                }
                if (x==0)   // left faces
                {
                    cubes[x][y][z].f3col = color[ORANGE];
                    cubes[x][y][z].f[3] = true;
                }
                if (x==2)   // right faces
                {
                    cubes[x][y][z].f4col = color[GREEN];
                    cubes[x][y][z].f[4] = true;
                }
                if (z==0)   // back faces
                {
                    cubes[x][y][z].f2col = color[YELLOW];
                    cubes[x][y][z].f[2] = true;
                }
                if (z==2)   // front faces
                {
                    cubes[x][y][z].f1col = color[WHITE];
                    cubes[x][y][z].f[1] = true;
                }
            }
        }
    }
}


// draw the cube model
void CubeModel::draw(Cube *cube)
{
    GLfloat selCol;

    glPushMatrix();

    // if the cube is currently rotating, do the transformations
    if (cubeRotating)
    {
        glRotatef(cube->xrot, 1.0, 0.0, 0.0);
        glRotatef(cube->yrot, 0.0, 1.0, 0.0);
        glRotatef(cube->zrot, 0.0, 0.0, 1.0);
    }

    // multiply the current transformation matrix
    glMultMatrixf((float *)cube->mat);

    // if the cube is selected, pulse the color
    if (cube->selected) 
    {
        selCol = float(sin(selAnim));
        glColor4f(selCol, selCol, selCol, 1.0);    // draw selected
    }
    else    // otherwise, just draw it.
        glColor4fv(cubeColor);
    
    // draw the cube
    glBegin(GL_TRIANGLES);
    for (int i=0, n=0; i<VERTICES; i+=9, n+=3)
    {
        // draw the cube
        glNormal3fv(&norm[n]);
        glVertex3fv(&fac[i]);
        glVertex3fv(&fac[i+3]);
        glVertex3fv(&fac[i+6]);
    }
    glEnd();

    // turn off lighting so the faces look bright
    glDisable(GL_LIGHTING);
    
    // draw the colored faces of the cube
    // draw bottom faces
    glPushMatrix();
    glColor4fv(cube->f6col);   
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();

    // draw front faces
    glPushMatrix();
    glColor4fv(cube->f1col);  
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();

    // draw top faces
    glPushMatrix();
    glColor4fv(cube->f5col);  
    glRotatef(-180.0, 1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();

    // draw back faces
    glPushMatrix();
    glColor4fv(cube->f2col);  
    glRotatef(-270.0, 1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();

    // draw left faces
    glPushMatrix();
    glColor4fv(cube->f3col);  
    glRotatef(-90.0, 0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();

    // draw right faces
    glPushMatrix();
    glColor4fv(cube->f4col);  
    glRotatef(90.0, 0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex3f(-FACESIZE, -FACEZ, FACESIZE);
        glVertex3f(FACESIZE,  -FACEZ, FACESIZE);
        glVertex3f(FACESIZE, -FACEZ,-FACESIZE);
        glVertex3f(-FACESIZE,-FACEZ,-FACESIZE);
    glEnd();
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}


// draw the Rubik's Cube
void RubiksCube::draw()
{
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            for (int k=0; k<3; k++)
            {
                glPushMatrix();
             
                // draw the cube model
                glLoadName(cubes[i][j][k].id);
                cm.draw(&cubes[i][j][k]);

                glPopMatrix();
            }
        }
    }

}

// set a face as 'selected'
void RubiksCube::setSelected(int sel)
{
    selFaceNum = sel;
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            for (int k=0; k<3; k++)
            {
                if (sel==1 && cubes[i][j][k].f[1]==true || sel==2 && cubes[i][j][k].f[2]==true ||
                    sel==3 && cubes[i][j][k].f[3]==true || sel==4 && cubes[i][j][k].f[4]==true ||
                    sel==5 && cubes[i][j][k].f[5]==true || sel==6 && cubes[i][j][k].f[6]==true)
                    cubes[i][j][k].selected = true;
                else
                    cubes[i][j][k].selected = false;
            }
        }
    }
}

// clear all selected status
void RubiksCube::clearSelected()
{
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            for (int k=0; k<3; k++)
                cubes[i][j][k].selected=false;
}

// clear the temporary rotational values used for animating
void RubiksCube::clearRotVals()
{
    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                cubes[i][j][k].xrot = 0.0;
                cubes[i][j][k].yrot = 0.0;
                cubes[i][j][k].zrot = 0.0;
            }
        }
    }
}


// rotate on the X axis
void RubiksCube::rotateX(int deg)
{
// create the rotational matrix and apply it to
// the selected objects
float rz[16] = { 1, 0, 0, 0, 
                 0, float(cos(deg*DEG2RAD)), -float(sin(deg*DEG2RAD)), 0, 
                 0, float(sin(deg*DEG2RAD)), float(cos(deg*DEG2RAD)), 0,
                 0, 0, 0, 1 };
    float res[16];

    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                if (cubes[i][j][k].selected)
                {
                    matmat( rz, (float *)cubes[i][j][k].mat, res );
                    set( (float *)cubes[i][j][k].mat, res);
                }
            }
        }
    }
}

// Rotate on the Y axis
void RubiksCube::rotateY(int deg)
{
// create the rotational matrix and apply it to
// the selected objects
float rz[16] = { float(cos(deg*DEG2RAD)), 0, float(sin(deg*DEG2RAD)), 0, 
                 0, 1, 0, 0, 
                 -float(sin(deg*DEG2RAD)), 0, float(cos(deg*DEG2RAD)), 0,
                 0, 0, 0, 1 };
    float res[16];

    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                if (cubes[i][j][k].selected)
                {
                    matmat( rz, (float *)cubes[i][j][k].mat, res );
                    set( (float *)cubes[i][j][k].mat, res);
                }
            }
        }
    }
}

// rotate on the Z axis 
void RubiksCube::rotateZ(int deg)
{
// create the rotational matrix and apply it to
// the selected objects
float rz[16] = { float(cos(deg*DEG2RAD)), -float(sin(deg*DEG2RAD)), 0, 0, 
                 float(sin(deg*DEG2RAD)), float(cos(deg*DEG2RAD)), 0, 0, 
                 0, 0, 1, 0,
                 0, 0, 0, 1 };
float res[16];

    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                if (cubes[i][j][k].selected)
                {
                    matmat( rz, (float *)cubes[i][j][k].mat, res );
                    set( (float *)cubes[i][j][k].mat, res);
                }
            }
        }
    }
}


// rotate a face of cubes
void RubiksCube::rotate(int CW)
{
    int deg=-10;
    if (!CW) deg = 10;

    if (CW>1) return;   // avoid invalid directions

    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                // if the cubes are selected, rotate them
                if (cubes[i][j][k].selected)
                {
                    // check the face so we can rotate in the right direction
                    if (selFaceNum==1)
                        cubes[i][j][k].zrot += deg;
                    else if (selFaceNum==2)
                        cubes[i][j][k].zrot -= deg;
                    else if (selFaceNum==3)
                        cubes[i][j][k].xrot -= deg;
                    else if (selFaceNum==4)
                        cubes[i][j][k].xrot += deg;
                    else if (selFaceNum==5)
                     cubes[i][j][k].yrot += deg;
                    else if (selFaceNum==6)
                        cubes[i][j][k].yrot -= deg;

                    // don't go past 90 degrees
                    if (cubes[i][j][k].xrot<=-88 || cubes[i][j][k].xrot>=88 ||
                        cubes[i][j][k].yrot<=-88 || cubes[i][j][k].yrot>=88 ||
                        cubes[i][j][k].zrot<=-88 || cubes[i][j][k].zrot>=88)
                    {
                        if (CW) rotAngle = -90;
                        else rotAngle = 90;

                        // multiply the cube's matrix by the new rotation
                        if (selFaceNum==1)
                            rotateZ(-rotAngle);
                        else if (selFaceNum==2)
                            rotateZ(rotAngle);
                        else if (selFaceNum==3)
                            rotateX(rotAngle);
                        else if (selFaceNum==4)
                            rotateX(-rotAngle);
                        else if (selFaceNum==5)
                            rotateY(-rotAngle);
                        else if (selFaceNum==6)
                            rotateY(rotAngle);

                        // assin the faces
                        if (CW) assignFacesCW();
                        else assignFacesCCW();

                        cubeRotating = false;
                        clearRotVals(); // clear the animation values
                        return;
                    }
                    else
                        cubeRotating = true;
                }
            }
        }
    }
}

// assign all faces that have been rotated clockwise to their new
// faces
void RubiksCube::assignFacesCW()
{
    Cube *c;
    bool temp[7];
    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                c = &cubes[i][j][k];    // get a pointer to the cube

                //set the face assignment array to false
                temp[1]=temp[2]=temp[3]=temp[4]=temp[5]=temp[6]=false;

                if (c->selected)
                {
                    if (selFaceNum==1)
                    {
                        // face 1 always true
                        temp[1]=true;
                        if (c->f[3]) temp[5]=true;
                        if (c->f[5]) temp[4]=true;
                        if (c->f[4]) temp[6]=true;
                        if (c->f[6]) temp[3]=true;
                    }
                    else if (selFaceNum==2)
                    {
                        // face 2 always true
                        temp[2]=true;
                        if (c->f[4]) temp[5]=true;
                        if (c->f[5]) temp[3]=true;
                        if (c->f[3]) temp[6]=true;
                        if (c->f[6]) temp[4]=true;
                    }
                    else if (selFaceNum==3)
                    {
                        // face 3 always true
                        temp[3]=true;
                        if (c->f[1]) temp[6]=true;
                        if (c->f[6]) temp[2]=true;
                        if (c->f[2]) temp[5]=true;
                        if (c->f[5]) temp[1]=true;
                    }
                    else if (selFaceNum==4)
                    {
                        // face 4 always true
                        temp[4]=true;
                        if (c->f[1]) temp[5]=true;
                        if (c->f[5]) temp[2]=true;
                        if (c->f[2]) temp[6]=true;
                        if (c->f[6]) temp[1]=true;
                    }
                    else if (selFaceNum==5)
                    {
                        // face 5 always true
                        temp[5]=true;
                        if (c->f[1]) temp[3]=true;
                        if (c->f[3]) temp[2]=true;
                        if (c->f[2]) temp[4]=true;
                        if (c->f[4]) temp[1]=true;
                    }
                    else if (selFaceNum==6)
                    {
                        // face 6 always true
                        temp[6]=true;
                        if (c->f[1]) temp[4]=true;
                        if (c->f[4]) temp[2]=true;
                        if (c->f[2]) temp[3]=true;
                        if (c->f[3]) temp[1]=true;
                    }

                    // copy the new face assignment
                    c->f[1]=temp[1];
                    c->f[2]=temp[2];
                    c->f[3]=temp[3];
                    c->f[4]=temp[4];
                    c->f[5]=temp[5];
                    c->f[6]=temp[6];
                }
            }
        }
    }
}

// assign all faces that have been rotated counter clockwise to their new
// faces
void RubiksCube::assignFacesCCW()
{
        Cube *c;
    bool temp[7];
    for (int k=0; k<3; k++)
    {
        for (int j=0; j<3; j++)
        {
            for (int i=0; i<3; i++)
            {
                c = &cubes[i][j][k];    // get a pointer to the cube

                //set the face assignment array to false
                temp[1]=temp[2]=temp[3]=temp[4]=temp[5]=temp[6]=false;

                if (c->selected)
                {
                    if (selFaceNum==1)
                    {
                        // face 1 always true
                        temp[1]=true;
                        if (c->f[3]) temp[6]=true;
                        if (c->f[5]) temp[3]=true;
                        if (c->f[4]) temp[5]=true;
                        if (c->f[6]) temp[4]=true;
                    }
                    else if (selFaceNum==2)
                    {
                        // face 2 always true
                        temp[2]=true;
                        if (c->f[4]) temp[6]=true;
                        if (c->f[5]) temp[4]=true;
                        if (c->f[3]) temp[5]=true;
                        if (c->f[6]) temp[3]=true;
                    }
                    else if (selFaceNum==3)
                    {
                        // face 3 always true
                        temp[3]=true;
                        if (c->f[1]) temp[5]=true;
                        if (c->f[6]) temp[1]=true;
                        if (c->f[2]) temp[6]=true;
                        if (c->f[5]) temp[2]=true;
                    }
                    else if (selFaceNum==4)
                    {
                        // face 4 always true
                        temp[4]=true;
                        if (c->f[1]) temp[6]=true;
                        if (c->f[5]) temp[1]=true;
                        if (c->f[2]) temp[5]=true;
                        if (c->f[6]) temp[2]=true;
                    }
                    else if (selFaceNum==5)
                    {
                        // face 5 always true
                        temp[5]=true;
                        if (c->f[1]) temp[4]=true;
                        if (c->f[3]) temp[1]=true;
                        if (c->f[2]) temp[3]=true;
                        if (c->f[4]) temp[2]=true;
                    }
                    else if (selFaceNum==6)
                    {
                        // face 6 always true
                        temp[6]=true;
                        if (c->f[1]) temp[3]=true;
                        if (c->f[4]) temp[1]=true;
                        if (c->f[2]) temp[4]=true;
                        if (c->f[3]) temp[2]=true;
                    }

                    // copy the new face assignment
                    c->f[1]=temp[1];
                    c->f[2]=temp[2];
                    c->f[3]=temp[3];
                    c->f[4]=temp[4];
                    c->f[5]=temp[5];
                    c->f[6]=temp[6];
                }
            }
        }
    }
}
