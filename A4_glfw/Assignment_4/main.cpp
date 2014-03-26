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
#include "TextureUtils.h"
#include "TextureTypes.h"

WorldState *w_state;
RenderState *r_state[2];
// NOTE: defined in ControlState.h 
// ControlState c_state;

Mesh *g_mesh;
Mesh *cube_mesh;
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
    w_state->view = glm::lookAt(
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

    w_state->view = glm::rotate(w_state->view, c_state.viewPhi, glm::vec3(1, 0, 0));
    w_state->view = glm::rotate(w_state->view, c_state.viewTheta, glm::vec3(0, 1, 0));

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
    if (c_state.mode == MODE_TEXMAP)
    {  
        w_state->useProgram(2);
    }
    else if (c_state.mode == MODE_ENVMAP)
    {
        /*@@@@@@@@@@@@@@@@@@@@@@@@@@@
          @ for environment mapping  
          @@@@@@@@@@@@@@@@@@@@@@@@@@@*/
        w_state->useProgram(3);
    }
    else // projection
    {
        /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
          @ for projection mapping
          @
          @ note: for this part you will need to construct
          @ and pass in a new MVP for the projection.
          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
        w_state->useProgram(4);

        //glm::mat4 TS = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
        glm::mat4 TS = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 P = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f);
        //glm::mat4 P = glm::mat4(1.0, 0.0, 0.0, 0.0,
        //                        0.0, 1.0, 0.0, 0.0,
        //                        0.0, 0.0, 0.0, -1.0,
        //                        0.0, 0.0, 0.0, 0.0);
        glm::mat4 V = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f), //eye
            glm::vec3(0.0f, 0.0f, 0.0f), //center
            glm::vec3(0.0f, 1.0f, 0.0f) //up
            );
        glUniformMatrix4fv(glGetUniformLocation(w_state->getCurrentProgram(), "uPV"), 1, false, glm::value_ptr(TS* P * V));
    }


    // load values into shader
    glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "gem_pos"), 1, glm::value_ptr(gem_position));
    glUniform3fv(glGetUniformLocation(w_state->getCurrentProgram(), "cameraPosition"), 1, glm::value_ptr(glm::vec3(glm::inverse(w_state->view)[3])));

    w_state->loadTransforms();
    w_state->loadMaterials();
    w_state->loadLights();
    w_state->loadTextures();
    g_mesh->drawMesh();

    w_state->useProgram(5);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(23.0f, 23.0f, 23.0f));
    glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(glm::inverse(w_state->view)[3]));
    w_state->loadObjectTransforms(tr*scale);
    w_state->loadMaterials();
    w_state->loadLights();
    cube_mesh->drawMesh();


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
    GLuint shaderProgram[6] = {0};
    GLuint shaders[2] = {0};

    buildShader(GL_VERTEX_SHADER, "axes.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "default.fs.glsl", shaders[1]);

    // create axis shader program
    shaderProgram[0] = buildProgram(2, shaders);

    // create gem shader
    buildShader(GL_VERTEX_SHADER, "gem.vs.glsl", shaders[0]);
    shaderProgram[1] = buildProgram(2, shaders);

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // Create shaders for:
    // (1) texture map
    // (2) cubmap
    // (3) linear projection onto the model
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    // create texture shader
    buildShader(GL_VERTEX_SHADER, "texture.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "texture.fs.glsl", shaders[1]);
    shaderProgram[2] = buildProgram(2, shaders);

    // Create cube map shader
    buildShader(GL_VERTEX_SHADER, "cubemap.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "cubemap.fs.glsl", shaders[1]);
    shaderProgram[3] = buildProgram(2, shaders);

    // Create projection shader
    buildShader(GL_VERTEX_SHADER, "projection.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "projection.fs.glsl", shaders[1]);
    shaderProgram[4] = buildProgram(2, shaders);

    // Create projection shader
    buildShader(GL_VERTEX_SHADER, "cube.vs.glsl", shaders[0]);
    buildShader(GL_FRAGMENT_SHADER, "cube.fs.glsl", shaders[1]);
    shaderProgram[5] = buildProgram(2, shaders);

    // bind shader program
    w_state->setProgram(0, shaderProgram[0]);
    w_state->setProgram(1, shaderProgram[1]);
    w_state->setProgram(2, shaderProgram[2]);
    w_state->setProgram(3, shaderProgram[3]);
    w_state->setProgram(4, shaderProgram[4]);
    w_state->setProgram(5, shaderProgram[5]);
    w_state->useProgram(0);

    // setup the transform matrices and uniform variables
    w_state->loadTransforms();
    w_state->loadLights();
    w_state->loadMaterials();

    /*********************************************
     * LOAD MESH
     *********************************************/
    
    g_mesh = loadMeshFromFile(*r_state[0], "Mesh/arma_spec_tex_low.obj");
    cube_mesh = loadMeshFromFile(*r_state[1], "Mesh/cube.obj");
    g_gem  = createGem(*r_state[0], 0.1);
    g_axis = createAxis(*r_state[1], 1);

    gem_radius = 0.3;

    /*********************************************
     * LOAD TEXTURES
     *********************************************/
    
    TGAFILE image[8];
    Texture2D tex0;
    TextureCubeMap tex1;
    Texture2D tex2;

    /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
     @ Load Textures. You will need textures for
     @ (1) One 2D texture for texture mapping
     @ (2) One cubmap (need to load six images)
     @ (3) One 2D texture for projection mapping
     @
     @ There are new functions to load in textures.
     @ To do this you will be required to:
     @ (1) load tga
     @ (2) create a "texture_info" object
     @ (3) call load function(s) on above textures
     @ (4) push into the arrays in world_state
     @
     @ NOTE: Naming of the texture samplers within the shaders
     @ must be done similar to the lights in assignment 3
     @
     @ i.e. uniform samplerCube TexSampler1;
     @
     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    
    // --------------------- FOR SIMPLE TEXTURE MAPPING --------------------- 

    LoadTGAFile("CMaps/wall_512_1_05.tga", &image[0]);
    texture_info te = texture_info(image[0].imageWidth, image[0].imageHeight);
    tex0.loadTexture(GL_TEXTURE0, te, image[0].imageData);

    w_state->textures.push_back(tex0);

    // --------------------- FOR CUBE MAPPING --------------------- 
     TextureCubeMap texCube;
    LoadTGAFile("CMaps/Classroom/posx.tga", &image[1]);
    texture_info te0 = texture_info(image[1].imageWidth, image[1].imageHeight);
    texCube.loadTexture0(GL_TEXTURE1, te0, image[1].imageData);

    LoadTGAFile("CMaps/Classroom/negx.tga",&image[2]);
    texture_info te1 = texture_info(image[2].imageWidth, image[2].imageHeight);
    texCube.loadTexture1(GL_TEXTURE1, te1, image[2].imageData);

    LoadTGAFile("CMaps/Classroom/negy.tga",&image[3]);
    texture_info te2 = texture_info(image[3].imageWidth, image[3].imageHeight);
    texCube.loadTexture2(GL_TEXTURE1, te2, image[3].imageData);

    LoadTGAFile("CMaps/Classroom/posy.tga",&image[4]);
    texture_info te3 = texture_info(image[4].imageWidth, image[4].imageHeight);
    texCube.loadTexture3(GL_TEXTURE1, te3, image[4].imageData);

    LoadTGAFile("CMaps/Classroom/posz.tga",&image[5]);
    texture_info te4 = texture_info(image[5].imageWidth, image[5].imageHeight);
    texCube.loadTexture4(GL_TEXTURE1, te4, image[5].imageData);

    LoadTGAFile("CMaps/Classroom/negz.tga",&image[6]);
    texture_info te5 = texture_info(image[6].imageWidth, image[6].imageHeight);
    texCube.loadTexture5(GL_TEXTURE1, te5, image[6].imageData);

    /*
    TextureCubeMap texCube;
    LoadTGAFile("CMaps/EDC/posx.tga", &image[1]);
    texture_info te0 = texture_info(image[1].imageWidth, image[1].imageHeight);
    texCube.loadTexture0(GL_TEXTURE1, te0, image[1].imageData);

    LoadTGAFile("CMaps/EDC/negx.tga",&image[2]);
    texture_info te1 = texture_info(image[2].imageWidth, image[2].imageHeight);
    texCube.loadTexture1(GL_TEXTURE1, te1, image[2].imageData);

    LoadTGAFile("CMaps/EDC/negy.tga",&image[3]);
    texture_info te2 = texture_info(image[3].imageWidth, image[3].imageHeight);
    texCube.loadTexture2(GL_TEXTURE1, te2, image[3].imageData);

    LoadTGAFile("CMaps/EDC/posy.tga",&image[4]);
    texture_info te3 = texture_info(image[4].imageWidth, image[4].imageHeight);
    texCube.loadTexture3(GL_TEXTURE1, te3, image[4].imageData);

    LoadTGAFile("CMaps/EDC/posz.tga",&image[5]);
    texture_info te4 = texture_info(image[5].imageWidth, image[5].imageHeight);
    texCube.loadTexture4(GL_TEXTURE1, te4, image[5].imageData);

    LoadTGAFile("CMaps/EDC/negz.tga",&image[6]);
    texture_info te5 = texture_info(image[6].imageWidth, image[6].imageHeight);
    texCube.loadTexture5(GL_TEXTURE1, te5, image[6].imageData);
    */
    /*
    TextureCubeMap texCube;
    LoadTGAFile("CMaps/church/posx.tga", &image[1]);
    texture_info te0 = texture_info(image[1].imageWidth, image[1].imageHeight);
    texCube.loadTexture0(GL_TEXTURE1, te0, image[1].imageData);

    LoadTGAFile("CMaps/church/negx.tga",&image[2]);
    texture_info te1 = texture_info(image[2].imageWidth, image[2].imageHeight);
    texCube.loadTexture1(GL_TEXTURE1, te1, image[2].imageData);

    LoadTGAFile("CMaps/church/negy.tga",&image[3]);
    texture_info te2 = texture_info(image[3].imageWidth, image[3].imageHeight);
    texCube.loadTexture2(GL_TEXTURE1, te2, image[3].imageData);

    LoadTGAFile("CMaps/church/posy.tga",&image[4]);
    texture_info te3 = texture_info(image[4].imageWidth, image[4].imageHeight);
    texCube.loadTexture3(GL_TEXTURE1, te3, image[4].imageData);

    LoadTGAFile("CMaps/church/posz.tga",&image[5]);
    texture_info te4 = texture_info(image[5].imageWidth, image[5].imageHeight);
    texCube.loadTexture4(GL_TEXTURE1, te4, image[5].imageData);

    LoadTGAFile("CMaps/church/negz.tga",&image[6]);
    texture_info te5 = texture_info(image[6].imageWidth, image[6].imageHeight);
    texCube.loadTexture5(GL_TEXTURE1, te5, image[6].imageData);
    */
    w_state->textures.push_back(texCube);

    // --------------------- For Projection Mapping --------------------- 

    Texture2D texProj;

    //LoadTGAFile("CMaps/dog.tga", &image[7]);
    //LoadTGAFile("CMaps/UBClogo1.tga", &image[7]);
    LoadTGAFile("CMaps/Leopard_fur.tga", &image[7]);
    //LoadTGAFile("CMaps/repeat_Bg.tga", &image[7]);
    texture_info te6 = texture_info(image[7].imageWidth, image[7].imageHeight);
    texProj.loadTexture(GL_TEXTURE2, te6, image[7].imageData);
    
    w_state->textures.push_back(texProj);


    // the load TGA function allocates memory that needs to be freed
    for (int i = 0; i < 8; i++) {
        delete [] image[i].imageData;
    }

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
    delete cube_mesh;
    delete g_gem;
    delete g_axis;

    glfwTerminate();

    exit(EXIT_SUCCESS);
}