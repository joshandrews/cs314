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

#include "ControlState.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

ControlState c_state = ControlState();

ControlState::~ControlState()
{}

int ControlState::init(WorldState &w)
{
    this->w = &w;

    width  = 640;
    height = 480;
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    if (!glutCreateWindow("Window"))
    {
        fputs("failed to initialize window", stderr);
        return 1; // error
    }

    // bind all callbacks
    glutKeyboardFunc(key_callback);
    glutKeyboardUpFunc(keyUp_callback);
    glutSpecialFunc(skey_callback);
    glutSpecialUpFunc(skeyUp_callback);
    glutReshapeFunc(reshape_callback);
    glutMotionFunc(mousePos_callback);
    glutMouseFunc(mouseBtn_callback);
    glutIdleFunc(idle_callback);

    return 0;
}

int ControlState::deltaArrLR()
{
    return arrR - arrL;
}

int ControlState::deltaArrUD()
{
    return arrD - arrU;
}

void ControlState::updateView(float dTheta, float dPhi, float dDepth)
{
    viewTheta = fmod(viewTheta + 360 + float(dTheta), 360);
    viewPhi   = std::min(90.0f, std::max(-90.0f, viewPhi + dPhi));
    viewDepth += dDepth;
    if (!dDepth)
    { // should not ever be zero due to some of the glm functions used
        dDepth = 0.00000000001;
    }
}

void ControlState::resetTimer()
{
    ellapsed_time = 0.0;
    time = clock();
}

double ControlState::queryTimer()
{
    ellapsed_time += clock() - time;
    time = clock();
    return ellapsed_time;
}

void printHelp()
{
    printf("==== Help ====\n"
           "___Control___\n"
           "a - move in -x direction\n"
           "d - move in +x direction\n"
           "w - move in +z direction\n"
           "s - move in -z direction\n"
           "x - move in -y direction\n"
           "c - move in +y direction\n\n"
           "___Command___\n"
           "q, esc - exit\n"
           "h - help (you have already figured this out)\n\n"
           "___View___\n"
           "left click and drag - adjusts view\n"
           "scroll wheel        - zoom\n");
}

/*****************************************************************************
 * Passive Callback functions
 *****************************************************************************/
// callback when window is resized
void reshape_callback(int w, int h)
{
    c_state.height = h;
    c_state.width  = w;

    glViewport( 0, 0, (GLint)w, (GLint)h );
}

void idle_callback()
{
    glutPostRedisplay();
}

/*****************************************************************************
 * Active Callback functions
 *****************************************************************************/
// callback when a key is pressed
void skey_callback(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        c_state.arrL = 1;
        break;
    case GLUT_KEY_RIGHT:
        c_state.arrR = 1;
        break;
    case GLUT_KEY_UP:
        c_state.arrU = 1;
        break;
    case GLUT_KEY_DOWN:
        c_state.arrD = 1;
        break;
    }
}

void key_callback(unsigned char key, int x, int y)
{
    /* key code goes here */

    switch(key)
    {
    case 27: //escape
    case 'q':
        exit(0);
        break;
    case 'h':
        printHelp();
        break;
    case 'w':
        c_state.gemMove[2] = 1;
        break;
    case 'a':
        c_state.gemMove[0] = -1;
        break;
    case 's':
        c_state.gemMove[2] = -1;
        break;
    case 'd':
        c_state.gemMove[0] = 1;
        break;
    case 'x':
        c_state.gemMove[1] = -1;
        break;
    case 'c':
        c_state.gemMove[1] = 1;
        break;
    case 'r':
        c_state.gemMove[1] = 1;
        break;
    case '=':
        c_state.gemRadius = 1;
        break;
    case '-':
        c_state.gemRadius = -1;
        break;
    case 'b':
    	c_state.armBlow = 1;
    	break;
    case 'n':
    	c_state.armBlow = -1;
    	break;
    }
}

void skeyUp_callback(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        c_state.arrL = 0;
        break;
    case GLUT_KEY_RIGHT:
        c_state.arrR = 0;
        break;
    case GLUT_KEY_UP:
        c_state.arrU = 0;
        break;
    case GLUT_KEY_DOWN:
        c_state.arrD = 0;
        break;
    }
}

void keyUp_callback(unsigned char key, int x, int y)
{
    /* key code goes here */

    switch(key)
    {
    case 27: //escape
    case 'q':
        exit(0);
        break;
    case 'w':
        c_state.gemMove[2] = 0;
        break;
    case 'a':
        c_state.gemMove[0] = 0;
        break;
    case 's':
        c_state.gemMove[2] = 0;
        break;
    case 'd':
        c_state.gemMove[0] = 0;
        break;
    case 'x':
        c_state.gemMove[1] = 0;
        break;
    case 'c':
        c_state.gemMove[1] = 0;
        break;
    case 'r':
        c_state.gemMove[1] = 0;
        break;
    case '=':
        c_state.gemRadius = 0;
        break;
    case '-':
        c_state.gemRadius = 0;
        break;
    case 'b':
    	c_state.armBlow = 0;
    	break;
    case 'n':
    	c_state.armBlow = 0;
    	break;
    }
}

// callback when a mouse button is pressed
void mouseBtn_callback(int button, int state, int x, int y)
{
    /* TODO: any controls relative to pressing the mouse buttons goes here */
    c_state.mouseX = x;
    c_state.mouseY = c_state.height - y;

    if (button == GLUT_LEFT_BUTTON)
        c_state.mouseBtnL = (state == GLUT_DOWN) ? 1 : 0;
    else if (button == GLUT_RIGHT_BUTTON)
        c_state.mouseBtnR = (state == GLUT_DOWN) ? 1 : 0;
    else if (button == GLUT_MIDDLE_BUTTON)
        c_state.mouseBtnR = (state == GLUT_DOWN) ? 1 : 0;
    else if (button == 3) // scroll down
        c_state.mouseScroll = 1.0/20.0f;
    else if (button == 4) // scroll up
        c_state.mouseScroll = -1.0/20.0f;
}

// callback when the mouse is moved. This will be called
// ALOT keep it as light as possible!!
void mousePos_callback(int x, int y)
{
    // screen Y coords are inverted.
    y = c_state.height - y;

    // currently used to update camera angles if mouse pressed
    if (c_state.mouseBtnL)
    {
        // Calculate change from last known mouse positon.
        int dx = x - c_state.mouseX;
        int dy = y - c_state.mouseY;
    
        // Update viewing angles.
        c_state.viewTheta = int(c_state.viewTheta + 360 + float(dx) / 2) % 360;
        c_state.viewPhi   = std::min(90.0f, std::max(-90.0f, c_state.viewPhi - dy));
    }

    c_state.mouseX = x;
    c_state.mouseY = y;
}