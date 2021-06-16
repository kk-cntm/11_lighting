#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"

class MixValue {
    static constexpr float max = 1.0f;
    static constexpr float min = 0.1f;
    
    float m_value;
    
public:
    MixValue(float value): m_value(value) {}
    
    float get() { return m_value; }
    void inc() {
        if (m_value + 0.1f < max) m_value += 0.1f;
    }
    void dec() {
        if (m_value - 0.1f > min) m_value -= 0.1f;
    }
};

MixValue mix_value(0.2f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void handle_key(GLFWwindow* window, int key, void (*callback)()) {
    static double last_call_time = glfwGetTime();
    
    double time = glfwGetTime();
    double diff = time - last_call_time;
    
    if (glfwGetKey(window, key) == GLFW_PRESS && diff > 0.2) {
        callback();
        last_call_time = time;
    }
}

void handle_arrow_left() {
    mix_value.dec();
}

void handle_arrow_right() {
    mix_value.inc();
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    handle_key(window, GLFW_KEY_LEFT, handle_arrow_left);
    handle_key(window, GLFW_KEY_RIGHT, handle_arrow_right);
}

class TriangleData {
public:
    static constexpr int length = 32;
private:
    unsigned int VBO, EBO, VAO, texture1ID, texture2ID;
    Texture texture1, texture2;
  
public:
    TriangleData(float vertices[length]):
        texture1("./assets/container.jpg", Texture::FileType::jpg),
        texture2("./assets/awesomeface.png", Texture::FileType::png) {
        unsigned int elementsIndicies[] = {
            0, 1, 3,
            1, 2, 3
        };
        
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), elementsIndicies, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    
    void draw() {
        texture1.bind(GL_TEXTURE0);
        texture2.bind(GL_TEXTURE1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
};

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))) {
        std::cout << "Failed to initilize glad" << std::endl;
        glfwTerminate();
        return -2;
    }
    
    glViewport(0, 0, 800, 600);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    float vertices[] = {
        // position         // color          // texture
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    
    TriangleData triangle(vertices);
    
    Shader program("./shaders/vertexShader.glsl", "./shaders/fragmentShader.glsl");
    
    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        
        program.use();
        
        program.setValue("mixValue", mix_value.get());
        program.setValue("model", model);
        program.setValue("view", view);
        program.setValue("projection", projection);
        program.setValue("tex1", 0);
        program.setValue("tex2", 1);
        
        triangle.draw();
        
        float time = glfwGetTime();
        float scaleValue = sin(time) / 2.0f + 0.5f;
        
        glm::mat4 transform2 = glm::mat4(1.0f);
        transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
        transform2 = glm::scale(transform2, glm::vec3(scaleValue, scaleValue, 0.0f));
        
        program.setValue("transform", transform2);
        
        triangle.draw();
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    return 0;
}
