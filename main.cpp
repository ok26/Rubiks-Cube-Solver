#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util/shader.h"
#include "Util/camera.h"
#include "Util/progressBar.h"

#include "Model/cube.h"
#include "Model/animation.h"
#include "Model/cubeIndexModel.h"

#include "Command/thistlewaiteSolver.h"
#include "Command/kociembaSolver.h"
#include "Command/cubeScramble.h"

#include "Database/moveStore.h"
#include "Database/symmetryIndexer.h"

#include <iostream>
#include <queue>
#include <map>
#include <thread>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void configureLight(Shader& shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int FPS = 144;

Camera camera(glm::vec3(0.0f, 7.0f, 20.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

Animation currentAnimation(false);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::queue<Move> moveQueue;
MoveStore moveStore;

std::map<int, bool> pressedKeys;
std::array<int, 13> moveKeys = { GLFW_KEY_U, GLFW_KEY_N, GLFW_KEY_R, GLFW_KEY_L, GLFW_KEY_F,
                                GLFW_KEY_B, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN, 
                                GLFW_KEY_Y, GLFW_KEY_T};

CubeScrambler cubeScrambler((unsigned int)time(nullptr));
ThistlewaiteSolver thistlewaiteSolver(false);
KociembaSolver kociembaSolver;
CubeIndexModel rubiksCubeIndexModel;

std::array<int, 6> recurrentKocimebaG1TimeBlock = { 10, 100, 1000, 5000, 10000, 30000 };

int currentTimeBlock = 1;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rubik's Cube Solver", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    
    Shader cubeShader("resources/shaders/cubeShader.vs", "resources/shaders/cubeShader.fs");

    glm::vec3 cubePos = glm::vec3(0.0f, -2.5f, -15.0f);
    Cube rubiksCube(cubePos);
    rubiksCube.LoadTextures(cubeShader);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        std::this_thread::sleep_for(std::chrono::milliseconds((int)((1.0f / FPS - deltaTime) * 1000)));

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentAnimation.running) {
            currentAnimation.updateAnimation((float)glfwGetTime());
            if (currentAnimation.finished)
                rubiksCube.FinishMove(currentAnimation);
        }
        if (!moveQueue.empty() && !currentAnimation.running) {
            Move move = moveQueue.front();
            moveQueue.pop();

            if (move.target == 6) 
                rubiksCubeIndexModel.fullRotation(move.axis, move.inverse);
            else
                rubiksCubeIndexModel.doMove(moveStore.animationMoveToIndex(move));

            currentAnimation.doAnimation(move.target, move.duration, move.axis, move.inverse, (float)glfwGetTime());
        }

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        cubeShader.use();
        configureLight(cubeShader, projection, view, model);
        rubiksCube.Draw(cubeShader, currentAnimation);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void configureLight(Shader& shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model) {

    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    for (int i = 0; i < 1; i++) {
        shader.setVec3("pointLights[" + std::to_string(i) + "].position", camera.Position);
        shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.1f, 0.1f, 0.1f);
        shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.022f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0019f);
    }

    shader.setVec3("viewPos", camera.Position);
    
    shader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("material.shininess", 64.0f);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void processMove(Move move, int key) {
    pressedKeys[key] = true;
    moveQueue.push({ move.target, move.duration, move.axis, move.inverse });
}

bool solverReady(bool sovlerTablesInitialized) {
    return !rubiksCubeIndexModel.isSolved() && !currentAnimation.running && sovlerTablesInitialized;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    bool inverseMove = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        inverseMove = true;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !pressedKeys[GLFW_KEY_F])
        processMove({ 1, 0.2f, 2, !inverseMove }, GLFW_KEY_F);
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !pressedKeys[GLFW_KEY_B])
        processMove({ 4, 0.2f, 2, inverseMove }, GLFW_KEY_B);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !pressedKeys[GLFW_KEY_U])
        processMove({ 0, 0.2f, 1, !inverseMove }, GLFW_KEY_U);
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !pressedKeys[GLFW_KEY_N])
        processMove({ 5, 0.2f, 1, inverseMove }, GLFW_KEY_N);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !pressedKeys[GLFW_KEY_R])
        processMove({ 2, 0.2f, 0, !inverseMove }, GLFW_KEY_R);
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !pressedKeys[GLFW_KEY_L])
        processMove({ 3, 0.2f, 0, inverseMove }, GLFW_KEY_L);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !pressedKeys[GLFW_KEY_LEFT])
        processMove({ 6, 0.3f, 1, true }, GLFW_KEY_LEFT);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !pressedKeys[GLFW_KEY_RIGHT])
        processMove({ 6, 0.3f, 1, false }, GLFW_KEY_RIGHT);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !pressedKeys[GLFW_KEY_UP])
        processMove({ 6, 0.3f, 0, true }, GLFW_KEY_UP);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !pressedKeys[GLFW_KEY_DOWN])
        processMove({ 6, 0.3f, 0, false }, GLFW_KEY_DOWN);
    
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !currentAnimation.running)
        cubeScrambler.scrambleCube100(&moveQueue);

    else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !currentAnimation.running)
        cubeScrambler.scrambleCubeWithRandomMoveNr(&moveQueue, 25, 35, 0.13f);

    else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && solverReady(kociembaSolver.tablesInitialized)) {
        vector<int> solution = kociembaSolver.tripleSearch(rubiksCubeIndexModel, recurrentKocimebaG1TimeBlock[currentTimeBlock]);
        vector<Move> moveSequence = moveStore.indexToAnimationMove(solution, 0.15f);
        for (Move move : moveSequence)
            moveQueue.push(move);
    }
    else if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) { // Dont hold!
        int CUBES = 1000;
        ProgressBar progressBar(70);
        int avg = 0;
        float lastProgress = 0.0f, curProgress = 0.0f;
        progressBar.updateProgressBar(0.0f);
        for (int i = 0; i < CUBES; i++) {
            CubeIndexModel cube;
            cubeScrambler.scrambleCube(cube, 1000);
            vector<int> solution = kociembaSolver.tripleSearch(cube, recurrentKocimebaG1TimeBlock[currentTimeBlock], false);
            avg += solution.size();
            curProgress = (float)((i + 1.0f) / (float)CUBES) * 100.0f;
            if ((int)(curProgress) > (int)(lastProgress)) {
                progressBar.updateProgressBar(curProgress / 100);
                lastProgress = curProgress;
            }
        }
        cout << "Solved " << CUBES << " cubes with " << recurrentKocimebaG1TimeBlock[currentTimeBlock] << "ms time block in " << (float)avg / (float)CUBES << " average moves\n";
    }
  
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !pressedKeys[GLFW_KEY_Y]) {
        pressedKeys[GLFW_KEY_Y] = true;
        rubiksCubeIndexModel.printData();
        std::cout << "\n";
    }
    
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !pressedKeys[GLFW_KEY_T]) {
        pressedKeys[GLFW_KEY_T] = true;
        currentTimeBlock = (currentTimeBlock + 1) % 6;
        int& timeBlock = recurrentKocimebaG1TimeBlock[currentTimeBlock];
        cout << "New time block: ";
        if (timeBlock > 1000)
            cout << timeBlock / 1000 << " seconds\n";
        else if (timeBlock == 1000)
            cout << "1 second\n";
        else
            cout << timeBlock << " milliseconds\n";
    }

    for (unsigned int key : moveKeys)
        if (glfwGetKey(window, key) == GLFW_RELEASE)
            pressedKeys[key] = false;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}