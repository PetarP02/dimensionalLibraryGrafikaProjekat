#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;

    glm::vec3 candle1Position = glm::vec3(1.5f, 10.79f, -2.5f);
    float candle1Scale = 4.0f;

    glm::vec3 globePostiotn = glm::vec3(1.0f, 10.2f, 2.3f);
    float globeScale = 4.0f;

    glm::vec3 potionPostiotn = glm::vec3(-1.0f, 11.9f, 2.0f);
    float potionScale = 3.0f;

    glm::vec3 bookStackPostiotn = glm::vec3(1.5f, 9.7f, -2.4f);
    float bookStackScale = 0.04f;

    glm::vec3 plantPosition = glm::vec3(-1.0f, 12.6f, -1.5f);
    float plantScale = 0.3f;

    glm::vec3 plant1Position = glm::vec3(-1.0f, 11.9f, -3.0f);
    float plant1Scale = 4.0f;

    glm::vec3 plant2Position = glm::vec3(-1.4f, 14.0f, -3.0f);
    float plant2Scale = 0.005f;

    glm::vec3 plant3Position = glm::vec3(-0.4f, 11.7f, 3.0f);
    float plant3Scale = 4.0f;

    glm::vec3 bookopenPosition = glm::vec3(1.5f, 9.0f, 0.0f);
    float bookopenScale = 0.5f;

    glm::vec3 tablePosition = glm::vec3(0, 5, 0);
    float tableScale = 0.05f;

    glm::vec3 bookcasePosition = glm::vec3(-3.1f, 6.5, 0);
    float bookcaseScale = 0.15f;

    PointLight pointLight;
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");

    // load models
    // -----------
    Model candle1("resources/objects/candle2obj/untitled.obj");
    candle1.SetShaderTextureNamePrefix("material.");

    Model globe("resources/objects/globeobj/untitled.obj");
    globe.SetShaderTextureNamePrefix("material.");

    Model bookstack("resources/objects/bookstackobj/untitled.obj");
    bookstack.SetShaderTextureNamePrefix("material.");

    Model potion("resources/objects/potionobj/untitled.obj");
    potion.SetShaderTextureNamePrefix("material.");

    Model plant("resources/objects/plant/plant.obj");
    plant.SetShaderTextureNamePrefix("material.");

    Model plant1("resources/objects/plant1obj/untitled.obj");
    plant1.SetShaderTextureNamePrefix("material.");

    Model plant2("resources/objects/plant2obj/untitled.obj");
    plant2.SetShaderTextureNamePrefix("material.");

    Model plant3("resources/objects/plant3obj/untitled.obj");
    plant3.SetShaderTextureNamePrefix("material.");

    Model bookopen("resources/objects/openbookobj/untitled.obj");
    bookopen.SetShaderTextureNamePrefix("material.");

    Model table("resources/objects/desk/scene.gltf");
    table.SetShaderTextureNamePrefix("material.");

    Model bookcase("resources/objects/bookcaseobj/untitled.obj");
    bookcase.SetShaderTextureNamePrefix("material.");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);


    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(8.0f, 13.0f, 1.0f);
    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 1.0f;
    pointLight.quadratic = 1.0f;

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();
        pointLight.position = glm::vec3(8.0f, 13.0f, 1.0f);
        ourShader.setVec3("pointLight.position", programState->camera.Position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight.specular", pointLight.specular);
        ourShader.setFloat("pointLight.constant", pointLight.constant);
        ourShader.setFloat("pointLight.linear", pointLight.linear);
        ourShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,programState->bookopenPosition); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->bookopenScale));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        bookopen.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->globePostiotn);
        model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->globeScale));
        ourShader.setMat4("model", model);
        globe.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->bookStackPostiotn);
        model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->bookStackScale));
        ourShader.setMat4("model", model);
        bookstack.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->potionPostiotn);
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->potionScale));
        ourShader.setMat4("model", model);
        potion.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->plantPosition);
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->plantScale));
        ourShader.setMat4("model", model);
        plant.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->plant1Position);
        model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->plant1Scale));
        ourShader.setMat4("model", model);
        plant1.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 11.9f, -3.0f));
        //model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 14.0f, -3.0f));
        model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 14.4f, -3.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 14.4f, -1.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 14.4f, 1.2f));
        model = glm::rotate(model, glm::radians(60.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.4f, 15.5f, 3.2f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(0.005f));
        ourShader.setMat4("model", model);
        plant2.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->plant3Position);
        //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));
        model = glm::scale(model, glm::vec3(programState->plant3Scale));
        ourShader.setMat4("model", model);
        plant3.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,glm::vec3(1.5f, 9.7f, 3.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        ourShader.setMat4("model", model);
        candle1.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->candle1Position);
        model = glm::scale(model, glm::vec3 (3.50f));
        ourShader.setMat4("model", model);
        candle1.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->tablePosition);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->tableScale));
        ourShader.setMat4("model", model);
        table.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->bookcasePosition);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(programState->bookcaseScale));
        ourShader.setMat4("model", model);
        bookcase.Draw(ourShader);

        if (programState->ImGuiEnabled)
            DrawImGui(programState);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);

        ImGui::DragFloat3("Candle1 position", (float*)&programState->candle1Position);
        ImGui::DragFloat("Candle1 scale", &programState->candle1Scale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Globe position", (float*)&programState->globePostiotn);
        ImGui::DragFloat("Globe scale", &programState->globeScale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Book Stack position", (float*)&programState->bookStackPostiotn);
        ImGui::DragFloat("Book Stack scale", &programState->bookStackScale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Potion position", (float*)&programState->potionPostiotn);
        ImGui::DragFloat("Potion scale", &programState->potionScale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Plant position", (float*)&programState->plantPosition);
        ImGui::DragFloat("Plant scale", &programState->plantScale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Plant1 position", (float*)&programState->plant1Position);
        ImGui::DragFloat("Plant1 scale", &programState->plant1Scale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Plant2 position", (float*)&programState->plant2Position);
        ImGui::DragFloat("Plant2 scale", &programState->plant2Scale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Plant3 position", (float*)&programState->plant3Position);
        ImGui::DragFloat("Plant3 scale", &programState->plant3Scale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Bookopen position", (float*)&programState->bookopenPosition);
        ImGui::DragFloat("Bookopen scale", &programState->bookopenScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat3("Table position", (float*)&programState->tablePosition);
        ImGui::DragFloat("Table scale", &programState->tableScale, 0.05,0.1,4.0);

        ImGui::DragFloat3("Bookcase position", (float*)&programState->bookcasePosition);
        ImGui::DragFloat("Bookcase scale", &programState->bookcaseScale, 0.05,0.1,4.0);

        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);

        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
