#include "lab_m1/Tema3/tema3.h"

#include <iostream>
#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include "lab_m1/Tema3/transform3.h"
#include "constants.h"

using namespace std;
using namespace m1;

Tema3::Tema3() {}
Tema3::~Tema3() {}

void Tema3::Init()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "textures");

    // Helper to load meshes
    auto loadMesh = [&](const string& name, const string& path, const string& file) {
        Mesh* mesh = new Mesh(name);
        mesh->LoadMesh(path, file);
        meshes[mesh->GetMeshID()] = mesh;
        };


    // Load meshes
    loadMesh("sphere", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
    loadMesh("box", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
	loadMesh("terrain", PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane256.obj");

    for (int i = 1; i <= 5; ++i) {
        loadMesh("sphere" + to_string(i), PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        loadMesh("box" + to_string(i), PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        for (int j = 1; j <= 4; ++j) {
            loadMesh("pilon" + to_string(i) + to_string(j), PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        }
    }

    // Load shaders
    auto loadShader = [&](const string& name, const string& vertex, const string& fragment) {
        Shader* shader = new Shader(name);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", vertex), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", fragment), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
        };

    loadShader("balon", "balonVertex.glsl", "balonFragment.glsl");
    loadShader("default", "defaultVertex.glsl", "defaultFragment.glsl");
	loadShader("heightmap", "terrainVertex.glsl", "terrainFragment.glsl");

    // Load textures
    auto loadTexture = [&](const string& name, const string& file) {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, file).c_str(), GL_REPEAT);
        mapTextures[name] = texture;
        };

    loadTexture("coca_cola", "coca_cola.jpg");
    loadTexture("roca", "roca.jpeg");
    loadTexture("baloane", "baloane.jpg");
    loadTexture("culori", "culori.png");
    loadTexture("iarba", "iarba.jpg");
    loadTexture("crate", "crate.jpg");
	loadTexture("heightmap", "heightmap256.png");
    loadTexture("pamant", "pamant.jpeg");
    loadTexture("deal", "deal.jpg");

    mixTextures = false;
}

void Tema3::FrameStart()
{
    glClearColor(0.7, 0.8, 0.969, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema3::RenderBalloon(const glm::vec3& position, const string& sphereName, const string& boxName,
    const vector<string>& pilons, const string& textureName, float scale, float yTranslate)
{
    // Render sphere
    auto renderSphere = [&]() {
        glUseProgram(shaders["balon"]->program);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3::Translate(position.x, position.y, position.z);
        modelMatrix *= transform3::Translate(0, yTranslate, 0);
        modelMatrix *= transform3::Scale(scale, scale, scale);
        RenderSimpleMesh(meshes[sphereName], shaders["balon"], modelMatrix, mapTextures[textureName]);
        };

    // Render box
    auto renderBox = [&]() {
        glUseProgram(shaders["default"]->program);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3::Translate(position.x, position.y, position.z);
        modelMatrix *= transform3::Translate(0, TRANSLATE_BOX_Y, 0);
        modelMatrix *= transform3::Scale(SCALE_BOX_X, SCALE_BOX_Y, SCALE_BOX_Z);
        RenderSimpleMesh(meshes[boxName], shaders["default"], modelMatrix, mapTextures["crate"]);

        // Render pilons
        for (int i = 0; i < pilons.size(); ++i) {
            glm::mat4 pilonMatrix = glm::mat4(1);
            pilonMatrix *= transform3::Translate(position.x, position.y, position.z);

            float offsetX = (i % 2 == 0 ? 1 : -1) * TRANSLATE_PILON_X;
            float offsetZ = (i < 2 ? 1 : -1) * TRANSLATE_PILON_Z;

            pilonMatrix *= transform3::Translate(offsetX, TRANSLATE_PILON_Y, offsetZ);
            pilonMatrix *= transform3::Scale(SCALE_PILON_X, SCALE_PILON_Y, SCALE_PILON_Z);
            RenderSimpleMesh(meshes[pilons[i]], shaders["default"], pilonMatrix, mapTextures["crate"]);
        }
        };

    renderSphere();
    renderBox();
}

void Tema3::UpdateBalloonPosition(glm::vec3& position, float radius, float speed, float height, float elapsedTime)
{
    position.x = radius * cos(elapsedTime * speed) + X_DEPLACEMENT;
    position.z = radius * sin(elapsedTime * speed) + Z_DEPLACEMENT;
	// if position is for balloon 1, then we do nothing
	if (position == balloon1Position)
		return;
    position.y = 2 * cos(elapsedTime) * sin(elapsedTime * speed) + height;
}

void Tema3::RenderTerrain()
{
	glUseProgram(shaders["heightmap"]->program);
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= transform3::Scale(100, 1, 100);

	mixTextures = true;
	RenderSimpleMesh(meshes["terrain"], shaders["heightmap"], modelMatrix, mapTextures["deal"]);
	mixTextures = false;
}

void Tema3::UpdateBalloonPhysics(float deltaTime, glm::vec3& position, glm::vec3& velocity, float windForce, float windChangeRate, bool applyAcceleration)
{
    // Constante pentru gravitație și accelerație
    const float GRAVITY = -9.8f;         // Gravitația (m/s^2)
    const float FIRE_ACCELERATION = 20.0f; // Accelerația pozitivă pe OY (la apăsarea tastei)
    const float WIND_FORCE_MAX = windForce; // Forța maximă a vântului
    const float DRAG = 0.1f; // Factor de rezistență a aerului

    // Efectul gravitației
    velocity.y += GRAVITY * deltaTime;

    // Adaugă accelerația dacă tasta este apăsată
    if (applyAcceleration) {
        velocity.y += FIRE_ACCELERATION * deltaTime;
    }

    // Efectul vântului pe axa X și Z
    float elapsedTime = Engine::GetElapsedTime();
    velocity.x += WIND_FORCE_MAX * sin(elapsedTime * windChangeRate) * deltaTime;
    velocity.z += WIND_FORCE_MAX * cos(elapsedTime * windChangeRate) * deltaTime;

    // Aplicăm un factor de rezistență (drag) pentru a reduce viteza treptat
    velocity *= 1.0f - DRAG * deltaTime;

    // Actualizăm poziția balonului pe baza vitezei
    position += velocity * deltaTime;

    // Constrângeri: Menținem balonul deasupra solului
    const float MIN_HEIGHT = 12.0f;
    if (position.y < MIN_HEIGHT) {
        position.y = MIN_HEIGHT;
        velocity.y = 0.0f;
    }
}

void Tema3::Update(float deltaTimeSeconds)
{
    float elapsedTime = Engine::GetElapsedTime();

    UpdateBalloonPosition(balloon1Position, BALLOON1_RADIUS, BALLOON1_SPEED, BALLOON1_HEIGHT, elapsedTime);
    UpdateBalloonPosition(balloon2Position, BALLOON2_RADIUS, BALLOON2_SPEED, BALLOON2_HEIGHT, elapsedTime);
    UpdateBalloonPosition(balloon3Position, BALLOON3_RADIUS, BALLOON3_SPEED, BALLOON3_HEIGHT, elapsedTime);
    UpdateBalloonPosition(balloon4Position, BALLOON4_RADIUS, BALLOON4_SPEED, BALLOON4_HEIGHT, elapsedTime);
    UpdateBalloonPosition(balloon5Position, BALLOON5_RADIUS, BALLOON5_SPEED, BALLOON5_HEIGHT, elapsedTime);

    bool isKeyPressed = window->KeyHold(GLFW_KEY_SPACE);
    UpdateBalloonPhysics(deltaTimeSeconds, balloon1Position, balloonVelocity1, 5.0f, 1.0f, isKeyPressed);

    RenderBalloon(balloon1Position, "sphere1", "box1", { "pilon11", "pilon12", "pilon13", "pilon14" }, "coca_cola", SCALE_BALON, Y_TRANSLATE);
    RenderBalloon(balloon2Position, "sphere2", "box2", { "pilon21", "pilon22", "pilon23", "pilon24" }, "roca", SCALE_BALON, Y_TRANSLATE);
    RenderBalloon(balloon3Position, "sphere3", "box3", { "pilon31", "pilon32", "pilon33", "pilon34" }, "culori", SCALE_BALON, Y_TRANSLATE);
    RenderBalloon(balloon4Position, "sphere4", "box4", { "pilon41", "pilon42", "pilon43", "pilon44" }, "iarba", SCALE_BALON, Y_TRANSLATE);
    RenderBalloon(balloon5Position, "sphere5", "box5", { "pilon51", "pilon52", "pilon53", "pilon54" }, "baloane", SCALE_BALON, Y_TRANSLATE);

	RenderTerrain();
}

void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "view");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniform1i(glGetUniformLocation(shader->program, "mix_textures"), mixTextures);

    // send the heightmap texture to the shader
    Texture2D* txt2 = mapTextures["heightmap"];
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, txt2->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "heightmap"), 2);

    if (texture1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);
    }

	if (mixTextures)
	{
        Texture2D* texture2 = mapTextures["pamant"];
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture2"), 1);
	}

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::FrameEnd()
{
    DrawCoordinateSystem();
}

void Tema3::OnInputUpdate(float deltaTime, int mods) {}
void Tema3::OnKeyPress(int key, int mods) {}
void Tema3::OnKeyRelease(int key, int mods) {}
void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
void Tema3::OnWindowResize(int width, int height) {}
