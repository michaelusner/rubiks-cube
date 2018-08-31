/*****************************************************************************
 * Michael Usner
 * CS433 - Asignment 3
 * November 6, 2003
 *
 * cube.h
 *****************************************************************************/
#ifndef CUBE_H
#define CUBE_H
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#define FACETS 493
#define VERTICES FACETS*9

// the cube class
class Cube {
public:
    Cube() {selected=false; xrot=yrot=zrot=0;f1=f2=f3=f4=f5=f6=false;
        f[0]=f[1]=f[2]=f[3]=f[4]=f[5]=f[6]=false;};
    bool selected;
    GLfloat pos[3];
    GLfloat xrot, yrot, zrot;
    int id;
    
    float mat[4][4];    // the transformation matrix for the cube

    bool f1, f2, f3, f4, f5, f6;
    bool f[7];
    GLfloat *f1col;
    GLfloat *f2col;
    GLfloat *f3col;
    GLfloat *f4col;
    GLfloat *f5col;
    GLfloat *f6col;

};

// the cube model class
class CubeModel {
public:
    CubeModel();
    void calcNormals();
    //float fac[FACETS*9];        // 9 vertices/facet
    float norm[FACETS*3];       // 1 vertex/normal

    void draw(Cube *cube);
};

// the rubik's cube class
class RubiksCube {
public:
    RubiksCube();
    
    void rotate(int dir);

    void setSelected(int id);
    void clearSelected();

    void initFaces();
    void moveSelFaces(bool dir);
    void setSelFace(Cube *[3][3]);
    void clearRotVals();
    void rotateX(int deg);
    void rotateY(int deg);
    void rotateZ(int deg);
    void assignFacesCW();
    void assignFacesCCW();
    CubeModel cm;
    Cube cubes[3][3][3];
    void draw();
};

#endif

