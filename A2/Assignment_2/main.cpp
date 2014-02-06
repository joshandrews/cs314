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

#ifdef _WIN32
#  include "GL/glew.h"
#  include "GLFW/glfw3.h"
# elif __APPLE__
#  include <GL/glew.h>
#  include <GLFW/glfw3.h>
#else
#  include <GL/glew.h>
#  include <GLFW/glfw3.h>
#endif

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderUtils.h"
#include "ControlState.h"
#include "WorldState.h"
#include "RenderState.h"
#include "Mesh.h"
#include "MeshUtils.h"

WorldState *w_state;
RenderState *r_state[2];
// NOTE: defined in ControlState.h 
// ControlState c_state;

Mesh *g_mesh;
Mesh *g_gem;
Mesh *g_eye;
Mesh *g_axis; // NOTE: only a single axis
glm::vec3 gem_position;
glm::vec3 Leye_pos;
glm::vec3 Reye_pos;
float head_rot_X;
// the neck frame
glm::mat4 N = glm::mat4( 1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, 
                         0.0, 1.0, -0.3, 1);

// the rotation of the head with repect to the neck
glm::mat4 H;

// the display loop, where all of the code that actually
// changes what you see goes
void display()
{
    /* limit framerate to 60 fps */
    double curr = 0;
    if ((curr = c_state.queryTimer()) < 0.016666667) // curr < ~ 1/60
        return;

    // start counting over
    c_state.resetTimer();

    // Clear the buffer we will draw into.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup camera projection.
    w_state->projection = glm::perspective(50.0f, c_state.aspectRatio(), 0.1f, 40.0f);

    // Setup camera position/orientation.
    w_state->modelview = glm::lookAt(
        c_state.viewDepth * glm::vec3(0.0f, 2.5f, 10.0f), // eye
                            glm::vec3(0.0f, 0.0f,  0.0f), // centre
                            glm::vec3(0.0f, 1.0f,  0.0f)  // up
    );

    /* this would let you rotate the view with arrow keys
     *  c_state.updateView(
     *      c_state.deltaArrLR() * DEGREES_PER_SECOND * curr,
     *      0,
     *      c_state.deltaArrUD() * DEPTH_PER_SECOND * curr - c_state.mouseScroll + 0.00001 );
     */
    c_state.updateView(0, 0, -c_state.mouseScroll );
    c_state.mouseScroll = 0;

    w_state->modelview = glm::rotate(w_state->modelview, c_state.viewPhi, glm::vec3(1, 0, 0));
    w_state->modelview = glm::rotate(w_state->modelview, c_state.viewTheta, glm::vec3(0, 1, 0));

    // update global gem position and radius
    gem_position += c_state.gemMove * STEP_PER_SECOND;

    // update gem mesh internal modelview transform matrix
    g_gem->m_MV = glm::translate(g_gem->m_MV, c_state.gemMove * STEP_PER_SECOND);

    Leye_pos = glm::vec3(-0.26454, 0.810, 0.00855);
    Reye_pos = glm::vec3(0.04736, 0.810, 0.05046);

	head_rot_X = min( max( double(head_rot_X - c_state.deltaArrUD() * RADIANS_PER_SECOND), -0.7), 0.7 );

    /***********************************
     * XYZ Axis Code
     ***********************************/
    w_state->useProgram(0);
    w_state->loadLights();

    //Draw X axis in red
    w_state->loadColorMaterial(glm::vec4(1, 0, 0, 1));
    w_state->loadObjectTransforms(glm::rotate(glm::mat4(),-90.0f, glm::vec3(0, 0, 1)));
    g_axis->drawMesh();

    //Draw Y axis in green
    w_state->loadColorMaterial(glm::vec4(0, 1, 0, 1));
    w_state->loadTransforms();
    g_axis->drawMesh();

    //Draw Z axis in blue
    w_state->loadColorMaterial(glm::vec4(0, 0, 1, 1));
    w_state->loadObjectTransforms(glm::rotate(glm::mat4(),90.0f, glm::vec3(1, 0, 0)));
    g_axis->drawMesh();

    /***********************************
     * Gem Code
     ***********************************/
    w_state->useProgram(1);
    glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));

    /* to help with knowing where the gem is in 3-space, it's color
     * changes according to which quadrant it is in
     * (+, +, +) = white,    (+, +, -) = yellow
     * (+, -, +) = magenta,  (+, -, -) = red
     * (-, +, +) = cyan,     (-, +, -) = green
     * (-, -, +) = blue,     (-, -, -) = dark grey
     */
    w_state->loadColorMaterial(glm::vec4(identifyQuadrant(g_gem->m_MV), 1));
    w_state->loadLights();
    w_state->loadTransforms();
    g_gem->drawMesh();

    /***********************************
     * Mesh Code
     ***********************************/
    w_state->useProgram(2);

    // load values into shader
    if (c_state.mode != MODE_HEAD_DEFORM)
        head_rot_X = 0;

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // YOUR CODE HERE:
    // calculate rotations into H (using head_rot_X)
    // load H and N into shader
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));

    w_state->loadTransforms();
    w_state->loadMaterials();
    w_state->loadLights();
    g_mesh->drawMesh();

    /***********************************
     * Eye Code
     ***********************************/
    if (c_state.mode == MODE_EYES || c_state.mode == MODE_LASERS)
    {
        w_state->useProgram(3);

        // Scaling matrix
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
        
        // Rotation matrix calculation - similar to lookAt from lecture
        glm::vec3 l_y = glm::normalize(gem_position - Leye_pos);
        glm::vec3 l_x = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f), l_y));
        glm::vec3 l_z = glm::normalize(glm::cross(l_y, l_x));
        glm::mat4 l_rotMatrix = glm::mat4(glm::vec4(l_x,0.0f),glm::vec4(l_y,0.0f),glm::vec4(l_z,0.0f),glm::vec4(0.0f,0.0f,0.0f,1.0f));
        

        // Translation Matrix
        glm::mat4 l_transMatrix = glm::translate(glm::mat4(1.0f), Leye_pos);
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        // YOUR CODE HERE
        // calculate rotation of eyes (hint: Rodrigues' formula)
	    // find the rotation of the left eye and load into the shader
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        // load values into shader

        glm::mat4 l_transRotScale = l_transMatrix*l_rotMatrix*scalingMatrix;
        glUniformMatrix4fv(glGetUniformLocation(w_state->getCurrentProgram(), "transRotScale"), 1, false, glm::value_ptr(l_transRotScale));
        glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));
        glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "eye_pos"), 1, glm::value_ptr(Leye_pos));

        w_state->loadTransforms();
        w_state->loadMaterials();
        w_state->loadLights();
        g_eye->drawMesh();

        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        // YOUR CODE HERE
        // calculate rotation of eyes (hint: Rodrigues' formula)
	    // find the rotation of the right eye and load into the shader
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        
        // Rotation matrix calculation - similar to lookAt from lecture
        glm::vec3 r_y = glm::normalize(gem_position - Reye_pos);
        glm::vec3 r_x = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f), r_y));
        glm::vec3 r_z = glm::normalize(glm::cross(r_y, r_x));
        glm::mat4 r_rotMatrix = glm::mat4(glm::vec4(r_x,0.0f),glm::vec4(r_y,0.0f),glm::vec4(r_z,0.0f),glm::vec4(0.0f,0.0f,0.0f,1.0f));
        

        // Translation Matrix
        glm::mat4 r_transMatrix = glm::translate(glm::mat4(1.0f), Reye_pos);
        
        glm::mat4 r_transRotScale = r_transMatrix*r_rotMatrix*scalingMatrix;
        
        glUniformMatrix4fv(glGetUniformLocation(w_state->getCurrentProgram(), "transRotScale"), 1, false, glm::value_ptr(r_transRotScale));
        glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "eye_pos"), 1, glm::value_ptr(Reye_pos));
       
        w_state->loadTransforms();
        g_eye->drawMesh();

        if (c_state.mode == MODE_LASERS)
        {
            // lasers
            w_state->useProgram(4);
            glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));
            glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "eye_pos"), 1, glm::value_ptr(Leye_pos));
	        
            // For Left Eye!
            float l_distance = glm::distance(gem_position, Leye_pos);

            glm::mat4 ll_scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, fabs(l_distance), 0.25f));
            
            glm::mat4 ll_transRotScale = l_transMatrix*l_rotMatrix*ll_scalingMatrix;
            
            //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            // YOUR CODE HERE-ISH:
            // apply the same rotations to both lasers
            //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

            glUniformMatrix4fv(glGetUniformLocation(w_state->getCurrentProgram(), "transRotScale"), 1, false, glm::value_ptr(ll_transRotScale));

            w_state->loadTransforms();
            w_state->loadLights();
            w_state->loadColorMaterial(glm::vec4(1, 0, 1, 1));
            g_axis->drawMesh();

            // For Right Eye!
            float r_distance = glm::distance(gem_position, Reye_pos);
            glm::mat4 lr_scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, fabs(r_distance), 0.25f));
            
            glm::mat4 lr_transRotScale = r_transMatrix*r_rotMatrix*lr_scalingMatrix;

            glUniformMatrix4fv(glGetUniformLocation(w_state->getCurrentProgram(), "transRotScale"), 1, false, glm::value_ptr(lr_transRotScale));
            glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "eye_pos"), 1, glm::value_ptr(Reye_pos));
	        w_state->loadColorMaterial(glm::vec4(1, 0, 1, 1));
            g_axis->drawMesh();
        }
    }

    glfwSwapBuffers(c_state.window);
    glfwPollEvents();}

// setup
int main(int argc, char *argv[])
{
    GLenum err = 0;
    /*********************************************
     * GLFW SETUP
     *********************************************/
    err = glfwInit();
    if (!err)
    {
        fputs("Failed to load the GLFW library", stderr);
        exit(EXIT_FAILURE);
    }
    
    // UNCOMMENT IF USING APPLE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /*********************************************
     * STATE SETUP (initialize gl context)
     *********************************************/
    // must be setup before glew so that a valid openGL
    // context exists (created with the window)

    w_state = new WorldState();
    c_state.init(*w_state);

    /*********************************************
     * GLEW SETUP
     *********************************************/
    // UNCOMMENT IF USING APPLE
    glewExperimental = GL_TRUE;
    err = glewInit();
    if (err != GLEW_OK)
    {
        fputs("Failed to initialize the GLEW library", stderr);
        exit(EXIT_FAILURE);
    }

    /*********************************************
     * STATE SETUP (construct render states)
     *********************************************/
    // must be setup after glew so that GL array
    // objects exist

    r_state[0] = new RenderState(3);
    r_state[1] = new RenderState(3);

    /*********************************************
     * SHADER SETUP
     *********************************************/
    // read default shaders from file
    GLuint shaderProgram[5] = {0};
    GLuint shaders[2] = {0};

    buildShader(GL_VERTEX_SHADER, "axes.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "default.fs.glsl", shaders[1]);

    // create axis shader program
    shaderProgram[0] = buildProgram(2, shaders);

    // create gem shader
    buildShader(GL_VERTEX_SHADER, "gem.vs.glsl", shaders[0]);
    shaderProgram[1] = buildProgram(2, shaders);

    // create armadillo shader
    buildShader(GL_VERTEX_SHADER, "armadillo.vs.glsl", shaders[0]);
    shaderProgram[2] = buildProgram(2, shaders);

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // YOUR CODE HERE:
    // Create some shaders
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    buildShader(GL_VERTEX_SHADER, "eyes.vs.glsl", shaders[0]);
    shaderProgram[3] = buildProgram(2, shaders);

    buildShader(GL_VERTEX_SHADER, "lasers.vs.glsl", shaders[0]);
    shaderProgram[4] = buildProgram(2, shaders);

    // bind shader program
    w_state->setProgram(0, shaderProgram[0]);
    w_state->setProgram(1, shaderProgram[1]);
    w_state->setProgram(2, shaderProgram[2]);
    w_state->setProgram(3, shaderProgram[3]);
    w_state->setProgram(4, shaderProgram[4]);
    w_state->useProgram(0);

    // setup the transform matrices and uniform variables
    w_state->loadTransforms();
    w_state->loadLights();
    w_state->loadMaterials();

    /*********************************************
     * LOAD MESH
     *********************************************/
    
    g_mesh = loadMeshFromFile(*r_state[0], "Mesh/arma.obj");
    g_eye  = loadMeshFromFile(*r_state[0], "Mesh/sauron.obj");
    g_gem  = createGem(*r_state[0], 0.1);
    g_axis = createAxis(*r_state[1], 1);

    /*********************************************
     * SET GL STATE
     *********************************************/ 
    glEnable(GL_DEPTH_TEST);

    /*********************************************
     * RENDER LOOP
     *********************************************/
    printHelp();
    glfwSetTime(0.0);
    while (!glfwWindowShouldClose(c_state.window))
        display();

    /*********************************************
     * CLEAN UP
     *********************************************/
    delete g_mesh;
    delete g_gem;
    delete g_eye;
    delete g_axis;

    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}