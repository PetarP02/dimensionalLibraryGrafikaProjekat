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

void renderQuad();

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void createFBO(unsigned int& depthMapFBO, unsigned int& depthCubemap, float SHADOW_WIDTH, float SHADOW_HEIGHT);

void renderCube();

//skybox vertices
unsigned int loadCubemap(vector<std::string>& faces);

void renderScene(Shader& ourShader, float currentFrame,
                 Model& arch, Model& portal, Model& bookopen, Model& lamp, Model& globe,
                 Model& floorglobe, Model& bookstack, Model& potion, Model& plant, Model& plant1,
                 Model& plant2, Model& plant3, Model& candle1, Model& candle2, Model& table,
                 Model& floor, Model& bookcase, Model& luster);

float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

// settings
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 750;
float exposure = 1.0f;
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

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;

    bool hdr = true;
    bool bloom = true;

    glm::vec3 prozorPosition = glm::vec3(-6.0f, 5.79f, 8.3f);
    float prozorScale = 2.35f;

    glm::vec3 portalPosition = glm::vec3(22.0f, 10.79f, -0.3f);
    float portalScale = 1.5f;

    glm::vec3 archPosition = glm::vec3(1.5f, 10.79f, -2.5f);
    float archScale = 0.04f;

    glm::vec3 candle1Position = glm::vec3(1.5f, 10.79f, -2.5f);
    float candle1Scale = 4.0f;

    glm::vec3 candle2Position = glm::vec3(-0.5f, 11.9f, 1.5f);
    float candle2Scale = 3.7f;

    glm::vec3 floorglobePosition = glm::vec3(4.0f, 4.9f, -7.5f);
    float floorglobeScale = 0.06f;

    glm::vec3 lampPosition = glm::vec3(16.0f, 5.7f, 15.0f);
    float lampScale = 0.05f;

    glm::vec3 globePostiotn = glm::vec3(1.0f, 10.2f, 2.3f);
    float globeScale = 4.0f;

    glm::vec3 potionPostiotn = glm::vec3(-1.0f, 11.9f, 2.0f);
    float potionScale = 3.0f;

    glm::vec3 bookStackPostiotn = glm::vec3(1.5f, 9.7f, -2.4f);
    float bookStackScale = 0.04f;

    glm::vec3 plantPosition = glm::vec3(9.0f, 6.6f, 9.5f);
    float plantScale = 2.3f;

    glm::vec3 plant1Position = glm::vec3(-1.0f, 11.9f, -3.0f);
    float plant1Scale = 4.0f;

    glm::vec3 plant2Position = glm::vec3(-1.4f, 14.0f, -3.0f);
    float plant2Scale = 0.005f;

    glm::vec3 plant3Position = glm::vec3(6.6f, 5.7f, 20.0f);
    float plant3Scale = 6.0f;

    glm::vec3 bookopenPosition = glm::vec3(1.5f, 9.0f, 0.0f);
    float bookopenScale = 0.5f;

    glm::vec3 tablePosition = glm::vec3(0, 5, 0);
    float tableScale = 0.05f;

    glm::vec3 floorPosition = glm::vec3(0, 5.5f, 0);
    float floorScale = 0.03f;

    glm::vec3 bookcasePosition = glm::vec3(4.9f, 5.5f, 23.0f);
    float bookcaseScale = 0.15f;

    glm::vec3 svetloPoz = glm::vec3(8.9f, 21.5f, 0.0f);

    glm::vec3 lusterPosition = glm::vec3(8.9f, 21.5f, 0.0f);
    float lusterScale = 0.2f;

    PointLight pointLight;
    SpotLight spotLight;
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Dimensional Library", NULL, NULL);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader simpleDepthShader("resources/shaders/pointShadowsDepth.vs", "resources/shaders/pointShadowsDepth.fs", "resources/shaders/pointShadowsDepth.gs"); //Shadows

    //hdr + bloom
    Shader hdrShader("resources/shaders/hdr.vs", "resources/shaders/hdr.fs");
    Shader blurShader("resources/shaders/blur.vs", "resources/shaders/blur.fs");

    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
    };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //pingpong baferi
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    //Inicijalizovanje buffera za senke T
    const unsigned int SHADOW_WIDTH = 1200, SHADOW_HEIGHT = 1200;
    unsigned int depthMapFBO;
    unsigned int depthCubemap;
    createFBO(depthMapFBO, depthCubemap, SHADOW_WIDTH, SHADOW_HEIGHT);

    // creating and loading skybox
    unsigned int cubemapTexture;

    vector<std::string> faces
        {
            "resources/textures/skybox/left.png",
            "resources/textures/skybox/right.png",
            "resources/textures/skybox/top.png",
            "resources/textures/skybox/bottom.png",
            "resources/textures/skybox/back.png",
            "resources/textures/skybox/front.png"
        };
    cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    ourShader.use();
    ourShader.setInt("material.texture_diffuse1", 0);
    ourShader.setInt("material.texture_specular1", 1);
    ourShader.setInt("depthMap", 2); //vezemo depthMap sa nasim shaderom
    blurShader.use();
    blurShader.setInt("image", 0);
    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);
    hdrShader.setInt("bloomBlur", 1);

    // load models
    // -----------
    Model prozor("resources/objects/windowobj/untitled.obj");
    prozor.SetShaderTextureNamePrefix("material.");

    Model portal("resources/objects/portalobj/untitled.obj");
    portal.SetShaderTextureNamePrefix("material.");

    Model arch("resources/objects/archobj/test.obj");
    arch.SetShaderTextureNamePrefix("material.");

    Model candle1("resources/objects/candle2obj/untitled.obj");
    candle1.SetShaderTextureNamePrefix("material.");

    Model candle2("resources/objects/candles2obj/untitled.obj");
    candle2.SetShaderTextureNamePrefix("material.");

    Model floorglobe("resources/objects/floorglobeobj/untitled.obj");
    floorglobe.SetShaderTextureNamePrefix("material.");

    Model lamp("resources/objects/lampobj/untitled.obj");
    lamp.SetShaderTextureNamePrefix("material.");

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

    Model floor("resources/objects/floorobj/untitled.obj");
    floor.SetShaderTextureNamePrefix("material.");

    Model bookcase("resources/objects/bookcaseobj/untitled.obj");
    bookcase.SetShaderTextureNamePrefix("material.");

    Model luster("resources/objects/lusterobj/untitled.obj");
    luster.SetShaderTextureNamePrefix("material.");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    PointLight& pointLight = programState->pointLight;
    pointLight.ambient = glm::vec3(0.05f);
    pointLight.diffuse = glm::vec3(80.0f, 40.0f, 15.0f);
    pointLight.specular = glm::vec3(1.0f, 0.77f, 0.35f);

    pointLight.constant = 1.0f;
    pointLight.linear = 1.0f;
    pointLight.quadratic = 5.0f;

    SpotLight& spotLight = programState->spotLight;
    spotLight.ambient = glm::vec3(0.2f);
    spotLight.diffuse = glm::vec3(15.0f, 5.0f, 0.0f);
    spotLight.specular = glm::vec3(0.7f, 0.5f, 0.5f);

    spotLight.constant = 1.0f;
    spotLight.linear = 0.05f;
    spotLight.quadratic = 0.05f;

    spotLight.cutOff = 5.0f;
    spotLight.outerCutOff = 23.0f;

    //pozicija plamena svake svece
    vector<glm::vec3> sveceSvetlo = {
            glm::vec3 (1.358f, 10.932f, 3.105f),
            glm::vec3 (1.436f, 11.774f, -2.470f),
            glm::vec3 (-0.214f, -0.908f, 0.358),
    };

    vector<glm::vec3> lampeSvetlo = {
            glm::vec3 (11.492f, 14.181f, -11.498f),
            glm::vec3 (15.989f, 14.181f, 14.922f),
            glm::vec3 (-0.488f, 14.181f, 6.494f)
    };

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

        float near_plane = 1.0f;
        float far_plane = 25.0f;

        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;

        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(programState->svetloPoz, programState->svetloPoz + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        //         1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", programState->svetloPoz);
        renderScene(simpleDepthShader, currentFrame,arch, portal, bookopen, lamp, globe,
                    floorglobe, bookstack, potion, plant, plant1,plant2, plant3, candle1,
                    candle2, table,floor, bookcase, luster);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //hdr render
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera .GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        ourShader.setVec3("luster.position", programState->svetloPoz);

        ourShader.setVec3("luster.ambient", glm::vec3(0.3f));
        ourShader.setVec3("luster.diffuse", glm::vec3(50.0f, 10.0f, 5.0f));
        ourShader.setVec3("luster.specular", glm::vec3(1.0f, 0.77f, 0.35f));

        ourShader.setFloat("luster.constant", 1.0f);
        ourShader.setFloat("luster.linear", 0.5f);
        ourShader.setFloat("luster.quadratic", 0.3f);

        for(int i = 0; i < 3; i++) {
            ourShader.setVec3("pointLights["+ to_string(i) +"].position", sveceSvetlo[i]);

            ourShader.setVec3("pointLights["+ to_string(i) +"].ambient", pointLight.ambient);
            ourShader.setVec3("pointLights["+ to_string(i) +"].diffuse", pointLight.diffuse);
            ourShader.setVec3("pointLights["+ to_string(i) +"].specular", pointLight.specular);

            ourShader.setFloat("pointLights["+ to_string(i) +"].constant", pointLight.constant);
            ourShader.setFloat("pointLights["+ to_string(i) +"].linear", pointLight.linear);
            ourShader.setFloat("pointLights["+ to_string(i) +"].quadratic", pointLight.quadratic);
        }

        for(int i = 0; i < 3; i++) {
            ourShader.setVec3("spotLights["+ to_string(i) +"].position", lampeSvetlo[i]);
            ourShader.setVec3("spotLights["+ to_string(i) +"].direction", glm::vec3 (0, -1, 0));

            ourShader.setVec3("spotLights["+ to_string(i) +"].ambient", spotLight.ambient);
            ourShader.setVec3("spotLights["+ to_string(i) +"].diffuse", spotLight.diffuse);
            ourShader.setVec3("spotLights["+ to_string(i) +"].specular", spotLight.specular);

            ourShader.setFloat("spotLights["+ to_string(i) +"].constant", spotLight.constant);
            ourShader.setFloat("spotLights["+ to_string(i) +"].linear", spotLight.linear);
            ourShader.setFloat("spotLights["+ to_string(i) +"].quadratic", spotLight.quadratic);

            ourShader.setFloat("spotLights["+ to_string(i) +"].cutOff", glm::cos(glm::radians(spotLight.cutOff)));
            ourShader.setFloat("spotLights["+ to_string(i) +"].outerCutOff", glm::cos(glm::radians(spotLight.outerCutOff)));
        }

        ourShader.setVec3("portalLight.position", glm::vec3(21.759f, 11.266f, -0.581f));
        ourShader.setVec3("portalLight.direction", glm::vec3 (-1, 0, 0));
        ourShader.setVec3("portalLight1.position", glm::vec3(18.759f, 11.266f, -0.581f));
        ourShader.setVec3("portalLight1.direction", glm::vec3 (1, 0, 0));

        ourShader.setVec3("portalLight.ambient", spotLight.ambient);
        ourShader.setVec3("portalLight.diffuse", glm::vec3(1.0f, 30.0f, 10.0f));
        ourShader.setVec3("portalLight.specular", glm::vec3(0.1f, 1.00f, 0.33f));
        ourShader.setVec3("portalLight1.ambient", spotLight.ambient);
        ourShader.setVec3("portalLight1.diffuse", glm::vec3(1.0f, 30.0f, 10.0f));
        ourShader.setVec3("portalLight1.specular", glm::vec3(0.1f, 1.00f, 0.33f));

        ourShader.setFloat("portalLight.constant", 1.0f);
        ourShader.setFloat("portalLight.linear", 0.01f);
        ourShader.setFloat("portalLight.quadratic", 0.1f);
        ourShader.setFloat("portalLight1.constant", 1.0f);
        ourShader.setFloat("portalLight1.linear", 0.01f);
        ourShader.setFloat("portalLight1.quadratic", 0.1f);

        ourShader.setFloat("portalLight.cutOff", glm::cos(glm::radians(45.0f)));
        ourShader.setFloat("portalLight.outerCutOff", glm::cos(glm::radians(60.0f)));
        ourShader.setFloat("portalLight1.cutOff", glm::cos(glm::radians(45.0f)));
        ourShader.setFloat("portalLight1.outerCutOff", glm::cos(glm::radians(60.0f)));

        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);
        // view/projection transformations
        projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setFloat("far_plane", far_plane);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap); //ako ne radi stavi ispod render modela

        // render the loaded model
        renderScene(ourShader, currentFrame,arch, portal, bookopen, lamp, globe,
                    floorglobe, bookstack, potion, plant, plant1,plant2, plant3, candle1,
                    candle2, table,floor, bookcase, luster);

        // drawing skybox
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //BLOOM
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //hdr 3.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        hdrShader.setBool("bloom", programState->bloom);
        hdrShader.setBool("hdr", programState->hdr);
        hdrShader.setFloat("exposure", exposure);
        renderQuad();

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
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
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
    //glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
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

void renderScene(Shader& ourShader, float currentFrame,
                 Model& arch, Model& portal, Model& bookopen, Model& lamp, Model& globe,
                 Model& floorglobe, Model& bookstack, Model& potion, Model& plant, Model& plant1,
                 Model& plant2, Model& plant3, Model& candle1, Model& candle2, Model& table,
                 Model& floor, Model& bookcase, Model& luster) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(19.5f, 4.7f, -0.5f)); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->archScale));    // it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    arch.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,programState->portalPosition); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians((sin(currentFrame) + 1) * 360.f), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(programState->portalScale));    // it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    portal.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,programState->lusterPosition); // translate it down so it's at the center of the scene
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(programState->lusterScale));    // it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    luster.Draw(ourShader);

//        model = glm::mat4(1.0f);
//        model = glm::translate(model,programState->prozorPosition); // translate it down so it's at the center of the scene
//        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
//        model = glm::scale(model, glm::vec3(programState->prozorScale));    // it's a bit too big for our scene, so scale it down
//        ourShader.setMat4("model", model);
//        prozor.Draw(ourShader);
//
//        model = glm::mat4(1.0f);
//        model = glm::translate(model,glm::vec3(-6.0f, 5.79f, -8.3f)); // translate it down so it's at the center of the scene
//        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
//        model = glm::scale(model, glm::vec3(programState->prozorScale));    // it's a bit too big for our scene, so scale it down
//        ourShader.setMat4("model", model);
//        prozor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,programState->bookopenPosition); // translate it down so it's at the center of the scene
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->bookopenScale));    // it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    bookopen.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, 5.7f, 6.5f));//
    //model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->lampScale));
    ourShader.setMat4("model", model);
    lamp.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(11.5f, 5.7f, -11.5f));//glm::vec3(-0.5f, 5.7f, 6.5f)
    //model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->lampScale));
    ourShader.setMat4("model", model);
    lamp.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, programState->lampPosition);//glm::vec3(-0.5f, 5.7f, 6.5f)
    //model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->lampScale));
    ourShader.setMat4("model", model);
    lamp.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, programState->globePostiotn);
    model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->globeScale));
    ourShader.setMat4("model", model);
    globe.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, programState->floorglobePosition);
    model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorglobeScale));
    ourShader.setMat4("model", model);
    floorglobe.Draw(ourShader);

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
    model = glm::translate(model, glm::vec3(-1.0f, 11.9f, -3.0f));
    model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->plant1Scale));
    ourShader.setMat4("model", model);
    plant1.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(15.0f, 5.5f, 14.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
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
    model = glm::translate(model, glm::vec3(-0.4f, 11.7f, 3.0f));
    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(programState->plant3Scale));
    ourShader.setMat4("model", model);
    plant3.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.6f, 5.5f, -10.0f));//
    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(programState->plant3Scale));
    ourShader.setMat4("model", model);
    plant3.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, programState->plant3Position);//glm::vec3(10.6f, 11.7f, -5.0f)
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
    model = glm::translate(model,programState->candle2Position);
    model = glm::rotate(model, glm::radians(110.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->candle2Scale));
    ourShader.setMat4("model", model);
    candle2.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,programState->tablePosition);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->tableScale));
    ourShader.setMat4("model", model);
    table.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(0, 5.5f, 0));
    //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorScale));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(0, 5.5f, 11.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorScale));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(0, 5.5f, -11.0f));
    //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorScale));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(11.0f, 5.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorScale));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(9.0f, 5.5f, 9.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.02f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(9.0f, 5.5f, -9.5f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.02f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(22.0f, 5.5f, 0.0f)); //18 5.5 -12
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->floorScale));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(18.0f, 5.5f, -12.0f)); //18 5.5 -12
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.02f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(7.0f, 5.5f, -21.0f)); //18 5.5 -12
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.01f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(16.0f, 5.5f, 15.0f)); //18 5.5 -12
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.01f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-5.0f, 5.5f, -23.0f)); //18 5.5 -12
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.02f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(5.0f, 5.5f, 22.0f)); //5.0f, 5.5f, 22.0f
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.02f));
    ourShader.setMat4("model", model);
    floor.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-3.1f, 6.5, 0));//-3.1f, 6.5, 0
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->bookcaseScale));
    ourShader.setMat4("model", model);
    bookcase.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-0.5f, 5.5f, -15.0f));//-3.1f, 6.5, 0
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->bookcaseScale));
    ourShader.setMat4("model", model);
    bookcase.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(-1.0f, 5.5f, 13));//-3.1f, 6.5, 0
    model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->bookcaseScale));
    ourShader.setMat4("model", model);
    bookcase.Draw(ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model,programState->bookcasePosition);//-3.1f, 6.5, 0
    model = glm::rotate(model, glm::radians(160.0f), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(programState->bookcaseScale));
    ourShader.setMat4("model", model);
    bookcase.Draw(ourShader);

    renderCube();
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Pozicioniranje i skaliranje");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);

        ImGui::Checkbox("HDR", (bool* ) &programState->hdr);
        ImGui::Checkbox("BLOOM", (bool* ) &programState->bloom);


//        ImGui::DragFloat3("Portal position", (float*)&programState->portalPosition);
//        ImGui::DragFloat("Portal scale", &programState->portalScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Arch position", (float*)&programState->archPosition);
//        ImGui::DragFloat("Arch scale", &programState->archScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Prozor position", (float*)&programState->prozorPosition);
//        ImGui::DragFloat("Prozor scale", &programState->prozorScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Lamp position", (float*)&programState->lampPosition);
//        ImGui::DragFloat("Lamp scale", &programState->lampScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Candle1 position", (float*)&programState->candle1Position);
//        ImGui::DragFloat("Candle1 scale", &programState->candle1Scale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Candle2 position", (float*)&programState->candle2Position);
//        ImGui::DragFloat("Candle2 scale", &programState->candle2Scale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("FloorGlobe position", (float*)&programState->floorglobePosition);
//        ImGui::DragFloat("FloorGlobe scale", &programState->floorglobeScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Globe position", (float*)&programState->globePostiotn);
//        ImGui::DragFloat("Globe scale", &programState->globeScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Book Stack position", (float*)&programState->bookStackPostiotn);
//        ImGui::DragFloat("Book Stack scale", &programState->bookStackScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Potion position", (float*)&programState->potionPostiotn);
//        ImGui::DragFloat("Potion scale", &programState->potionScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Plant position", (float*)&programState->plantPosition);
//        ImGui::DragFloat("Plant scale", &programState->plantScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Plant1 position", (float*)&programState->plant1Position);
//        ImGui::DragFloat("Plant1 scale", &programState->plant1Scale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Plant2 position", (float*)&programState->plant2Position);
//        ImGui::DragFloat("Plant2 scale", &programState->plant2Scale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Plant3 position", (float*)&programState->plant3Position);
//        ImGui::DragFloat("Plant3 scale", &programState->plant3Scale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Bookopen position", (float*)&programState->bookopenPosition);
//        ImGui::DragFloat("Bookopen scale", &programState->bookopenScale, 0.05, 0.1, 4.0);
//
//        ImGui::DragFloat3("Table position", (float*)&programState->tablePosition);
//        ImGui::DragFloat("Table scale", &programState->tableScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Floor position", (float*)&programState->floorPosition);
//        ImGui::DragFloat("Floor scale", &programState->floorScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Bookcase position", (float*)&programState->bookcasePosition);
//        ImGui::DragFloat("Bookcase scale", &programState->bookcaseScale, 0.05,0.1,4.0);

//        ImGui::DragFloat3("Luster position", (float*)&programState->lusterPosition);
//        ImGui::DragFloat("Luster scale", &programState->lusterScale, 0.05,0.1,4.0);
//
//        ImGui::DragFloat3("Svetlo poz", (float*)&programState->svetloPoz);

        ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);

        ImGui::DragFloat3("pointLight.ambient", (float*)&programState->pointLight.ambient);
        ImGui::DragFloat3("pointLight.diffuse", (float*)&programState->pointLight.diffuse);
        ImGui::DragFloat3("pointLight.specular", (float*)&programState->pointLight.specular);


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

unsigned int loadCubemap(vector<std::string>& faces){

    unsigned int skyboxID;
    glGenTextures(1, &skyboxID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    int width, height, nrChannels;
    unsigned char *data;
    for(unsigned int i = 0; i < faces.size(); i++) {

        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }else{
            std::cerr << "Failed to load cubemap face at path: " << faces[i] << '\n';
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return skyboxID;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void createFBO(unsigned int& depthMapFBO, unsigned int& depthCubemap, float SHADOW_WIDTH, float SHADOW_HEIGHT) {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
