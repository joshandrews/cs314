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
{
    glfwDestroyWindow(window);
}

int ControlState::init(WorldState &w)
{
    this->w = &w;

    width  = 640;
    height = 480;
    
    /* As of right now we only have one window */
    window = glfwCreateWindow(width, height, "Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fputs("failed to initialize window", stderr);
        return 1; // error
    }
    glfwMakeContextCurrent(window);

    // bind all callbacks
//    glutKeyboardFunc(key_callback);
//    glutKeyboardUpFunc(keyUp_callback);
//    glutSpecialFunc(skey_callback);
//    glutSpecialUpFunc(skeyUp_callback);
//    glutReshapeFunc(reshape_callback);
//    glutMotionFunc(mousePos_callback);
//    glutMouseFunc(mouseBtn_callback);
//    glutIdleFunc(idle_callback);

    // bind all callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetCursorPosCallback(window, mousePos_callback);
    glfwSetMouseButtonCallback(window, mouseBtn_callback);
    glfwSetScrollCallback(window, mouseScroll_callback);
    
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
           "b - heat up ball\n"
           "a - move in -x direction\n"
           "d - move in +x direction\n"
           "w - move in +z direction\n"
           "s - move in -z direction\n"
           "x - move in -y direction\n"
           "c - move in +y direction\n\n"
           "___Command___\n"
           "q, esc - exit\n"
           "m - toggle mode\n"
           "h - help (you have already figured this out)\n\n"
           "___View___\n"
           "left click and drag - adjusts view\n"
           "scroll wheel        - zoom\n");
}

/*****************************************************************************
 * Passive Callback functions
 *****************************************************************************/
// error callback for GLFW
static void error_callback(int error, const char* desc)
{
    fputs(desc, stderr);
}

// callback when window is resized
void reshape_callback(GLFWwindow* window, int w, int h)
{
    c_state.height = h;
    c_state.width  = w;

    glViewport( 0, 0, (GLint)w, (GLint)h );}

/*****************************************************************************
 * Active Callback functions
 *****************************************************************************/
// callback when a key is pressed
// callback when a key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    /* key code goes here */
    
    switch(key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_H:
            printHelp();
            break;
        case GLFW_KEY_LEFT:
            c_state.arrL = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_RIGHT:
            c_state.arrR = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_UP:
            c_state.arrU = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_DOWN:
            c_state.arrD = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_W:
            c_state.gemMove[2] = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_A:
            c_state.gemMove[0] = (action == GLFW_RELEASE) ? 0 : -1;
            break;
        case GLFW_KEY_S:
            c_state.gemMove[2] = (action == GLFW_RELEASE) ? 0 : -1;
            break;
        case GLFW_KEY_D:
            c_state.gemMove[0] = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_X:
            c_state.gemMove[1] = (action == GLFW_RELEASE) ? 0 : -1;
            break;
        case GLFW_KEY_C:
            c_state.gemMove[1] = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_R:
            c_state.gemMove[1] = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_EQUAL:
            c_state.gemRadius = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_MINUS:
            c_state.gemRadius = (action == GLFW_RELEASE) ? 0 : -1;
            break;
        case GLFW_KEY_B:
            c_state.ballGlow = (action == GLFW_RELEASE) ? 0 : 1;
            break;
        case GLFW_KEY_V:
            c_state.ballGlow = (action == GLFW_RELEASE) ? 0 : -1;
            break;
        case GLFW_KEY_M:
            c_state.mode = (RENDER_MODE)((c_state.mode + 1) % MODE_MAX);
            break;
    }
}

// callback when a mouse button is pressed
static void mouseBtn_callback(GLFWwindow* win, int button, int action, int mod)
{
    /* TODO: any controls relative to pressing the mouse buttons goes here */
    
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        c_state.mouseBtnL = (action == GLFW_PRESS) ? 1 : 0;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        c_state.mouseBtnR = (action == GLFW_PRESS) ? 1 : 0;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        c_state.mouseBtnC = (action == GLFW_PRESS) ? 1 : 0;
}

// callback when the mouse is moved. This will be called
// ALOT keep it as light as possible!!
static void mousePos_callback(GLFWwindow* win, double x, double y)
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

static void mouseScroll_callback(GLFWwindow* win, double x_offset, double y_offset)
{
    c_state.mouseScroll = y_offset/20;
}