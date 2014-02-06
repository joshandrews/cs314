/* Copyright (c) Russell Gillette
 * December 2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * CPSC Computer Graphics 2014
 * University of British Columbia
 */

/* == ControlState.h ==
 * State and functions that operate on user input, window management,
 * or other functionality outside of the domain of OpenGL
 *
 */

#ifndef CONTROL_STATE_H
#define CONTROL_STATE_H

#include "WorldState.h"
#include <time.h>

/* due to GLFW/glut/most windowing systems being c based it is infeasible to store 
 * all the control state within our class and pass member function pointers (the c 
 * interface does not allow it) thus the callback functions must be defined external
 * to the class
 */
#define DEGREES_PER_SECOND 100
#define DEPTH_PER_SECOND 3
#define STEP_PER_SECOND 0.002f

/** 
 * This class is responsible for all the interaction outside of GL,
 * which includes all of the window manager functionality (GLUT, GLFW)
 * such as inputs, window resizing, etc, as well as all all related state.
 */
class ControlState
{
public:
    float viewTheta;
    float viewPhi;
    float viewDepth;

    // the current state of arrow keys, updated by the
    bool arrL;
    bool arrR;
    bool arrU;
    bool arrD;

    // the current mouse position, updated by the mousePose_callback function
    int mouseX;
    int mouseY;
    bool mouseBtnL;
    bool mouseBtnC;
    bool mouseBtnR;

    // unprocessed mouse scroll amount
    float mouseScroll;

    // the current dimensions of the window
    int height;
    int width;

    //frame timer
    clock_t time;
    double  ellapsed_time;

    WorldState* w;

    //program specific control scheme
    glm::vec3 gemMove;
    float gemRadius;
	float armBlow;
	
    ControlState()
        : viewTheta(0),
          viewPhi(0),
          viewDepth(1),
          arrL(0), arrR(0),
          arrU(0), arrD(0),
          mouseX(0), mouseY(0),
          mouseBtnL(0), mouseBtnC(0),
          mouseBtnR(0), w(NULL),
          ellapsed_time(0),
          time(clock())
    {}

    ~ControlState();
    int init(WorldState &w);

    float aspectRatio()
    {return (float)width/height;}

    void splitViewportLeft()
    {glViewport(0, 0, width/2, height);}
    void splitViewportRight()
    {glViewport(width/2, 0, width/2, height);}
    void splitViewportTop()
    {glViewport(0, height/2, width, height/2);}
    void splitViewportBottom()
    {glViewport(0, 0, width, height/2);}
    void viewportFull()
    {glViewport(0, 0, width, height);}

    void resetTimer();
    double queryTimer();

    int deltaArrLR();
    int deltaArrUD();
    void updateView(float dTheta, float dPhi, float dDepth);
};
extern ControlState c_state;

extern void printHelp();

/* GLUT callback funtions */
static void idle_callback();
static void reshape_callback(int w, int h);
static void key_callback(unsigned char key, int x, int y);
static void keyUp_callback(unsigned char key, int x, int y);
static void skey_callback(int key, int x, int y);
static void skeyUp_callback(int key, int x, int y);
static void mouseBtn_callback(int button, int state, int x, int y);
static void mousePos_callback(int x, int y);

#endif // CONTROL_STATE_H