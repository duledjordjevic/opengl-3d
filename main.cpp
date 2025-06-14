#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "geometry.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
unsigned createTexture(const char* filePath, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT, GLint minFilter = GL_NEAREST, GLint magFilter = GL_NEAREST);

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Library was not loaded! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1200;
    unsigned int wHeight = 1200;
    const char wTitle[] = "Garden of Flowers";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Window was not created! :(\n";
        glfwTerminate();
        return 2;
    }
    
    glfwMakeContextCurrent(window);

    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW could not be loaded! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ VARIABLES AND BUFFERS +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    unsigned int stride = (3 + 4) * sizeof(float); 

    generateVertices( flowerScale, flowerSize, flowerHeight, baseY, xOffsets, zOffsets);
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);  // Can be  GL_STATIC_DRAW, but because we will change the vertices often, we use GL_DYNAMIC_DRAW

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Texture 

    float nameVertices[] = {
        1.0f, 0.9f,  1.0f, 0.0f,  
        0.1f,  0.9f,  0.0f, 0.0f,  
        1.0f, 1.0f,  1.0f, 1.0f,  
        0.1f,  1.0f,  0.0f, 1.0f 
    };

    unsigned int strideTexture = (2 + 2) * sizeof(float); 

    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nameVertices), nameVertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideTexture, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned nameTexture = createTexture("res/name.png");

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++  UNIFORMS  +++++++++++++++++++++++++++++++++++++++++++++++++

    glm::mat4 model = glm::mat4(1.0f); //Transformation matrix - mat4(1.0f) - generate unit matrix
    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    
    glm::mat4 view; //View matrix (camera matrix)
    view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // lookAt(Where the camera is, what the camera is looking at, unit vector of the positive Y axis of the world - this rotates the camera)
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    
    
    glm::mat4 projectionP = glm::perspective(glm::radians(90.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f); // Perspective projection matrix (FOV, Aspect Ratio, near plane, far plane)
    glm::mat4 projectionO = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f); // Orthographic projection matrix (left, right, bottom, top, near plane, far plane)
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");

    // Texture uniforms
    glUseProgram(textureShader);
    glUniform1i(glGetUniformLocation(textureShader, "uTexture"), 0);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++
    glUseProgram(unifiedShader); // Sending default values to the uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // (Matrix address, number of matrices we're sending, whether to transpose, pointer to matrices)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionO));
    glBindVertexArray(VAO);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK); // Choose which face to cull (only works after enabling face culling)
    
    float lastScale = flowerScale;
    
    glEnable(GL_DEPTH_TEST);

    glm::mat4 currentProjection = projectionP;

    bool keyPressed[3] = { false, false, false }; 

    double lastFrameTime = 0.0;
    const double targetFrameRate = 1.0 / 60.0;
    double currentTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        if (currentTime - lastFrameTime >= targetFrameRate) {
            lastFrameTime = currentTime;

            for (int i = 0; i < 3; ++i) {
                int key = GLFW_KEY_3 - i;
                if (glfwGetKey(window, key) == GLFW_PRESS) {
                    if (!keyPressed[i]) {
                        showRow[i] = !showRow[i]; // toggle visibility
                        keyPressed[i] = true;
    
                        // Regenerate flower vertices
                        generateVertices( flowerScale, flowerSize, flowerHeight, baseY, xOffsets, zOffsets);
                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                    }
                } else {
                    keyPressed[i] = false; // Reset key press state
                }
            }
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
    
            // Depth testing
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            {
                glEnable(GL_DEPTH_TEST); // Enable Z-buffer testing
            }
            if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            {
                glDisable(GL_DEPTH_TEST); // Disable Z-buffer testing
            }
    
            // Face culling (we previously set which face to cull using glCullFace)
            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            {
                glEnable(GL_CULL_FACE); // Enable face culling
            }
            if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            {
                glDisable(GL_CULL_FACE); // Disable face culling
            }
    
            // Switching projections
            if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) currentProjection = projectionP;
            if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) currentProjection = projectionO;
    
            // Transforming triangles
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                model = glm::rotate(model, glm::radians(-0.5f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation (Transformation matrix, rotation angle in radians, axis of rotation)
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                model = glm::rotate(model, glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }
    
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                flowerScale += 0.01f;
                if (flowerScale > 1.0f) flowerScale = 1.0f;
            }
    
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                flowerScale -= 0.01f;
                if (flowerScale < 0.1f) flowerScale = 0.1f;
            }
    
            // If scale changed, regenerate vertices and update VBO
            if (flowerScale != lastScale) {
                generateVertices( flowerScale, flowerSize, flowerHeight, baseY, xOffsets, zOffsets);
    
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
    
                lastScale = flowerScale;
            }
    
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Refresh both the color buffer and the Z-buffer
    
            glUseProgram(unifiedShader);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(currentProjection));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 7);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glUseProgram(textureShader);
            glBindVertexArray(VAO2);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, nameTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ CLEANUP +++++++++++++++++++++++++++++++++++++++++++++++++

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);

    glfwTerminate();
    return 0;

}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Successfully read file from path \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Error reading file from path \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" shader has an error! Error: \n");
        printf("%s", infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Shader Program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
unsigned createTexture(const char* filePath, GLint wrapS, GLint wrapT,GLint minFilter, GLint magFilter) {

    unsigned texture = loadImageToTexture(filePath);
    if (texture == 0) {
        std::cout << "Error on texture load: " << filePath << std::endl;
        return 0; // Return 0 if texture loading failed
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0); 

    return texture;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        // Images are usually loaded upside down by default, so they must be flipped to be upright
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Checks the color format of the loaded image
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // Free the memory allocated by stbi_load since it's no longer needed
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Texture not loaded! Texture path: " << filePath << std::endl;
        std::cout << "STB Error: " << stbi_failure_reason() << "\n";
        stbi_image_free(ImageData);
        return 0;
    }
}

