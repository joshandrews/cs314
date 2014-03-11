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

#define GLFW_INCLUDE_GLU

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

#include <iostream>

#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

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
Mesh *g_axis; // NOTE: only a single axis
float gem_radius;
glm::vec3 gem_position;


// the display loop, where all of the code that actually
// changes what you see goes
void display()
{
    /* limit framerate to 60 fps */
    double curr = 0;
    if ((curr = glfwGetTime()) < 0.016666667) // curr < ~ 1/60
        return;

    // start counting over
    glfwSetTime(0.0);

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
    gem_radius   = max(0.0f, gem_radius + c_state.gemRadius * STEP_PER_SECOND);

    // update gem mesh internal modelview transform matrix
    g_gem->m_MV = glm::translate(g_gem->m_MV, c_state.gemMove * STEP_PER_SECOND);

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

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // You need to update the light's
    // position with the gem position
    // and color
    //
    // BEWARE: vectors return a COPY of
    // elements, not the element you want to change
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // Update lights with gem's position and color:

    // Use the appropriate shader based on the mode
    if (c_state.mode == MODE_PHONG) 
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        w_state->useProgram(2);
        w_state->lights.push_back(LightInfo());
        LightInfo Light0 = w_state->lights[0];
        Light0.Position = w_state->modelview*glm::vec4(gem_position,1);
        Light0.La = glm::vec3(0.6);
        Light0.Ld = glm::vec3(1.0, 1.0, 1.0);
        w_state->lights[0] = Light0;
        glUniform1f(glGetUniformLocation(w_state->getCurrentProgram(), "current_mode"), 1.0);
    }
    else if (c_state.mode == MODE_MULTI)
    {
        w_state->useProgram(3);
        w_state->lights.push_back(LightInfo());
        LightInfo Light0 = w_state->lights[0];
        Light0.Position = w_state->modelview*glm::vec4(gem_position,1);
        Light0.La = glm::vec3(0.3);
        Light0.Ld = glm::vec3(0.0, 1.0, 0.0);
        w_state->lights[0] = Light0;

        w_state->lights.push_back(LightInfo());
        LightInfo Light1 = w_state->lights[1];
        Light1.Position = w_state->modelview*glm::vec4(1.0, 1.0, -1.0, 1);
        Light1.La = glm::vec3(0.3);
        Light1.Ld = glm::vec3(1.0, 0.0, 0.0);
        w_state->lights[1] = Light1;

        w_state->lights.push_back(LightInfo());
        LightInfo Light2 = w_state->lights[2];
        Light2.Position = w_state->modelview*glm::vec4(-1.0, 1.0, -1.0, 1);
        Light2.La = glm::vec3(0.3);
        Light2.Ld = glm::vec3(0.0, 0.0, 1.0);
        w_state->lights[2] = Light2;

        glUniform1f(glGetUniformLocation(w_state->getCurrentProgram(), "current_mode"), 2.0);
    }
    else if (c_state.mode == MODE_TOON)
    {
        w_state->useProgram(2);   
        w_state->lights.push_back(LightInfo());
        LightInfo Light0 = w_state->lights[0];
        Light0.Position = w_state->modelview*glm::vec4(gem_position,1);
        Light0.La = glm::vec3(0.6);
        Light0.Ld = glm::vec3(1.0, 1.0, 1.0);
        w_state->lights[0] = Light0;
        glUniform1f(glGetUniformLocation(w_state->getCurrentProgram(), "current_mode"), 3.0); 
    }
    else // Creative part, c_state.mode == MODE_PHOG
    {
        w_state->useProgram(4);
        w_state->lights.push_back(LightInfo());
        LightInfo Light0 = w_state->lights[0];
        Light0.Position = w_state->modelview*glm::vec4(gem_position,1);
        Light0.La = glm::vec3(0.6);
        Light0.Ld = glm::vec3(1.0, 1.0, 1.0);
        w_state->lights[0] = Light0;
        glUniform1f(glGetUniformLocation(w_state->getCurrentProgram(), "current_mode"), 4.0);
        glClearColor(0.5f, 0.5f, 0.5f, 1);
    }
    
    // load values into shader
    //glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));
    //glUniform1f(glGetUniformLocation(w_state->getCurrentProgram(), "radius"), gem_radius);

    w_state->loadTransforms();
    w_state->loadMaterials();
    w_state->loadLights();
    g_mesh->drawMesh();
    glfwSwapBuffers(c_state.window);
    glfwPollEvents();
}

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

#ifdef __APPLE__
     // UNCOMMENT IF USING APPLE
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	 glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
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
#ifdef __APPLE__
    // UNCOMMENT IF USING APPLE
     glewExperimental = GL_TRUE;
#endif
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
    
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // Create Your Shaders:
    // (1) Phone and Point Light Shader
    //
    // (2) Multiple Lights Shader
    //
    // (3) Toon Shader
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    buildShader(GL_VERTEX_SHADER, "phong.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "phong.fs.glsl", shaders[1]);

    shaderProgram[2] = buildProgram(2, shaders);

    buildShader(GL_VERTEX_SHADER, "multlights.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "multlights.fs.glsl", shaders[1]);

    shaderProgram[3] = buildProgram(2, shaders);

    buildShader(GL_VERTEX_SHADER, "fog.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "fog.fs.glsl", shaders[1]);

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
    
    g_mesh = loadMeshFromFile(*r_state[0], "Mesh/armadillo.obj");
    g_gem  = createGem(*r_state[0], 0.1);
    g_axis = createAxis(*r_state[1], 1);

    gem_radius = 0.3;

    /*********************************************
     * LOAD LIGHTS
     *********************************************/
    
    // Light is the default colour that uses the gem position.
    // We want to always have it as the last element in the lights vector

    // NOTE: there is already one default light in the world state
    // std::vector at w_state.lights

    // NOTE2: The way this code works, you MUST make
    // the lights in your shaders be named "Light#"
    // where # is a number in [0,N)
    //
    // THEY CANNOT BE IN AN ARRAY
    // opengl does not properly update uniform variables
    // of struct arrays
    
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // Create and populate two lights
    // here for Part 2
    // NOTE: keep track of the order you have your lights
    // so that you can properly reference them in
    // your shaders
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


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
    delete g_axis;

    glfwTerminate();

    exit(EXIT_SUCCESS);
}