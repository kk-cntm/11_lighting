#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

int width = 800;
int height = 600;

float last_xpos = (float)width / 2.0f;
float last_ypos = (float)height / 2.0f;

float fov = 45.0f;

float last_frame = 0.0f;
float delta_time = 0.0f;

Camera camera({
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    -90.0f,
    0.0f,
    delta_time
});

bool right_mouse_btn_pressed = false;

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

void framebuffer_size_callback(GLFWwindow* window, int new_width, int new_height) {
    width = new_width;
    height = new_height;
    glViewport(0, 0, width, height);
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!right_mouse_btn_pressed) {
        last_xpos = xpos;
        last_ypos = ypos;
        
        return;
    }
        
    float x_offset = last_xpos - xpos;
    float y_offset = ypos - last_ypos;
    
    last_xpos = xpos;
    last_ypos = ypos;
    
    const float sensetive = 0.1f * fov / 45.0f;
    
    x_offset *= sensetive;
    y_offset *= sensetive;
    
    camera.addYaw(x_offset);
    camera.addPitch(y_offset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        right_mouse_btn_pressed = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        right_mouse_btn_pressed = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= yoffset;
    
    if (fov < 1.0f) fov = 1.0f;
    else if (fov > 45.0f) fov = 45.0f;
}

void process_camera_move(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveForward();
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveBackward();
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.moveLeft();
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveRight();
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
    
    process_camera_move(window);
}

class Cube {
    unsigned int VBO, EBO, VAO, texture1ID, texture2ID;
    Texture texture1, texture2;
  
public:
    Cube(float* vertices, int size):
        texture1("./assets/container.jpg", Texture::FileType::jpg),
        texture2("./assets/awesomeface.png", Texture::FileType::png) {
        
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    void draw() {
        texture1.bind(GL_TEXTURE0);
        texture2.bind(GL_TEXTURE1);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    
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
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,   0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f,  -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f,  -3.5f),
        glm::vec3(-1.7f,  3.0f,  -7.5f),
        glm::vec3( 1.3f, -2.0f,  -2.5f),
        glm::vec3( 1.5f,  2.0f,  -2.5f),
        glm::vec3( 1.5f,  0.2f,  -1.5f),
        glm::vec3(-1.3f,  1.0f,  -1.5f)
    };
    
    Cube cube(vertices, sizeof(vertices));
    
    Shader program("./shaders/vertexShader.glsl", "./shaders/fragmentShader.glsl");
    
    while (!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        
        delta_time = time - last_frame;
        last_frame = time;
        
        camera.setDeltaTime(delta_time);
        
        process_input(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.view();
        
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
        
        program.use();
        
        program.setValue("mixValue", mix_value.get());
        program.setValue("view", view);
        program.setValue("projection", projection);
        program.setValue("tex1", 0);
        program.setValue("tex2", 1);
        
        for (int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            
            if (i % 3 == 0) {
                model = glm::rotate(model, time + glm::radians(i * 20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            
            program.setValue("model", model);
            
            cube.draw();
        }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    return 0;
}
