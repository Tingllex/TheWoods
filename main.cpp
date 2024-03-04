#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <time.h>

#include "headers/utilities.hpp"
#include "headers/obj_loader.hpp"

#include "headers/CProgram.h"
#include "headers/CMesh.h"
#include "headers/CLight.h"
#include "headers/CMaterial.h"
#include "headers/CSkyBox.hpp"
#include "headers/CMultipleMesh.h"
#include "headers/minimap.hpp"
#include "headers/postprocessing.hpp"
#include "headers/shadow_dir.hpp"
#include "headers/CTexture.h"
#include "headers/collider.hpp"

#include "ground.hpp"
CGround myGround;

#include "player.hpp"
CPlayer myPlayer;

#include "headers/camera.hpp"
CFreeCamera myCamera;

#include "text-ft.hpp"
#include <chrono>

std::chrono::steady_clock::time_point lastPressTimeB;
std::chrono::steady_clock::time_point lastPressTimeN;
std::chrono::steady_clock::time_point gameStartTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point currentTime;
std::chrono::steady_clock::time_point lastClockSpawnTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point stunnedStartTime;
//std::chrono::steady_clock::time_point miniMapEnabledTime;
std::chrono::steady_clock::time_point stunEndTime;
std::chrono::steady_clock::time_point lastUpdateTime = std::chrono::steady_clock::now();

float speedMultiplier = 1.0f;

enum Objects {
    TREE,
	LEGO,
    OBJ_BALL1,
    OBJ_BALL2,
    OBJ_BALL3,
    OBJ_TERRAIN,
    OBJ_KOLIBER,
    SCREEN,
    FENCE,
    CLOUDS,
    GIFT,
    CLOCK,
    GHOST,
    NUM_OBJECTS
};

enum Lights {
    LIGHT_LIGHT1,
    LIGHT_LIGHT2,
    LIGHT_LIGHT3
};

// Macierze transformacji i rzutowania
glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matModel = glm::mat4(1.0);

GLuint idTextureArray[NUM_OBJECTS];
// Wektory na dane z pliku OBJ
std::vector<glm::vec3> OBJ_vertices[NUM_OBJECTS ];
std::vector<glm::vec2> OBJ_uvs[NUM_OBJECTS ];
std::vector<glm::vec3> OBJ_normals[NUM_OBJECTS ];

GLuint idTexture;
GLuint idVAO[NUM_OBJECTS];
GLuint vInstances;

float Time = 0.0;
int lightingModel;
int SkyBoxModel = 1;
using glm::vec3;
GLuint idVBO_coord, idVBO_uv, idVBO_normal;

CSkyBox mySkyBox;
// Zmienne do kontroli obrotu obiektu
GLfloat angleY;
GLfloat angleX;

clock_t last_T = 0;
float rotation = 0.0f;
float position = 0.0f;
float moveSpeed = 0.0f;
float zoom = 1.0f;

bool isMiniMapEnabled = false;
bool isMirrorEffectEnabled = false;
bool isTransparencyEffectEnabled = false;
bool isEffect1Enabled = false;
bool isEffect2Enabled = false;
bool KeyboardTable[128] = { false };
glm::mat4x4 matModels[NUM_KOLIBER_INSTANCES];

float treeX, treeY, treeZ;
CProgram mainProgram;
CProgram koliberProgram;
CProgram miniMapProgram;
CProgram postProcessingProgram;

//CMesh object[static_cast<int>(NUM_OBJECTS)];
CSceneObject object[static_cast<int>(NUM_OBJECTS)];
CSceneObject testBall;

const int TREE_COUNT = 50;
std::array<glm::vec3, TREE_COUNT> treePositions;
CSceneObject treeColliders[static_cast<int>(TREE_COUNT)];

const int GIFT_COUNT = 200;
std::array<glm::vec3, GIFT_COUNT> giftPositions;
CSceneObject giftColliders[static_cast<int>(GIFT_COUNT)];

const int CLOCK_COUNT = 10;
std::array<glm::vec3, GIFT_COUNT> clockPositions;
CSceneObject clockColliders[static_cast<int>(CLOCK_COUNT)];

const int GHOST_COUNT = 5;
std::array<glm::vec3, GHOST_COUNT> ghostPositions;
CSceneObject ghostColliders[static_cast<int>(GHOST_COUNT)];

char MoveSpeedTxt[255];
char SpeedTxt[255];

int playerScore = 0;
std::vector<bool> giftCollected(GIFT_COUNT, false);
std::vector<bool> clockCollected(CLOCK_COUNT, false);
std::vector<bool> ghostCollected(GHOST_COUNT, false);

int activeGifts = 1;
int collectedGifts = 0;
int collectedAllGifts = 0;

int activeClocks = 1;
int collectedClocks = 0;
int collectedAllClocks = 0;

int activeGhosts = 5;
int collectedGhosts = 0;
int collectedAllGhosts = 0;

bool isPlayerStunned = false;

float extraTime = 0.0f;

void CreateInstanced() {
    srand(time(NULL));
    for (int i=0; i<NUM_KOLIBER_INSTANCES; i++)
    {
        float x = -10.0 + (rand()%200)/10.0;
        float y = -10.0 + (rand()%200)/10.0;
        float z = -10.0 + (rand()%200)/10.0;
        float r = (rand()%(360))/100.0;

        matModels[i] = glm::translate(glm::mat4(1.0), glm::vec3(x,y,z));
        matModels[i] = glm::rotate(matModels[i], r, glm::vec3(0.0, 1.0, 0.0));
    }
}

void SendMaterialParameters(MaterialParam material) {
    GLint idProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);
    //wysylanie
    glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, glm::value_ptr(material.Ambient));
    glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, glm::value_ptr(material.Diffuse));
    glUniform3fv(glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, glm::value_ptr(material.Specular));
    glUniform1f(glGetUniformLocation(idProgram, "myMaterial.Shininess"), material.Shininess);
}

void SkyBoxReflection(int option) {
    switch (option) {
        case 0:
            isMirrorEffectEnabled = !isMirrorEffectEnabled;
            break;
        case 1:
            isTransparencyEffectEnabled = !isTransparencyEffectEnabled;
            break;
    }
    printf("opcja lustrzana: %d\nopcja przezroczysta: %d\n", isMirrorEffectEnabled, isTransparencyEffectEnabled);
    glutPostRedisplay();
}

void ToggleMiniMap(int option) {
    switch (option) {
        case 0:
            isMiniMapEnabled = true;
            break;
        case 1:
            isMiniMapEnabled = false;
            break;
    }
    glutPostRedisplay();
}

void UpdateSpeedMultiplier() {
    auto now = std::chrono::steady_clock::now();

    if (KeyboardTable['b'] && KeyboardTable['n'])
    {
        speedMultiplier = 1.0f;
        return;
    }
    if (KeyboardTable['b']) {
        lastPressTimeB = now;
    }
    if (KeyboardTable['n']) {
        lastPressTimeN = now;
    }

    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - std::min(lastPressTimeB, lastPressTimeN)).count();

    if (timeDiff < 40) {
        speedMultiplier = 3.0f;
    } else if (timeDiff < 100) {
        speedMultiplier = 2.5f;
    } else if (timeDiff < 250) {
        speedMultiplier = 2.0f;
    } else if (timeDiff < 500) {
        speedMultiplier = 1.5f;
    } else if (timeDiff < 1000) {
        speedMultiplier = 1.2f;
    } else {
        speedMultiplier = 1.0f;
    }
}

void UpdateMovement() {
    UpdateSpeedMultiplier();

    float baseMoveSpeed = 0.1f;
    float giftSpeedBonus = collectedAllGifts * 0.05f;
    float moveSpeed = baseMoveSpeed * (1.0f + giftSpeedBonus) * speedMultiplier;

    sprintf(MoveSpeedTxt, "Movement speed: %.2f", moveSpeed * 10);
    RenderText(MoveSpeedTxt, 25, 40, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));

    sprintf(SpeedTxt, "Speed Multiplier: %.2f", speedMultiplier);
    RenderText(SpeedTxt, 25, 80, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));

    float rotateSpeed = 0.05;

    if (!isPlayerStunned) {
        if (KeyboardTable['w']) {
            myPlayer.Move(moveSpeed, giftColliders, treeColliders, clockColliders, ghostColliders);
            myCamera.Move(moveSpeed, myPlayer.Position);
        }
        if (KeyboardTable['s']) {
            myPlayer.Move(-moveSpeed / 2, giftColliders, treeColliders, clockColliders, ghostColliders);
            myCamera.Move(-moveSpeed / 2, myPlayer.Position);
        }
    }
//    if (KeyboardTable['r']) {
//        myPlayer.Jump();
//    }
    if (KeyboardTable['a']) {
        myPlayer.Rotate(rotateSpeed);
        myCamera.Rotate(0, -rotateSpeed);
    }
    if (KeyboardTable['d']) {
        myPlayer.Rotate(-rotateSpeed);
        myCamera.Rotate(0, rotateSpeed);
    }
}


void ChangeFilterEffect(int option) {
    switch (option) {
    case 0:
        isEffect1Enabled = true;
        break;
    case 1:
        isEffect1Enabled = false;
        break;
    case 2:
        isEffect2Enabled = true;
        break;
    case 3:
        isEffect2Enabled = false;
        break;
    }
    glutPostRedisplay();
}

void SkyBoxModelMenu(int option)
{
    SkyBoxModel = option;
    mySkyBox.Initialize(SkyBoxModel);
    printf("SkyBoxModel = %d\n", SkyBoxModel);
    glutPostRedisplay();
}

void LightingModelMenu(int value)
{
    lightingModel = value;
    switch (lightingModel) {
        case 0:
            printf("punktowe- Phonga %d\n", lightingModel);
            break;
        case 1:
            printf("punktowe- Blinn-Phong %d\n", lightingModel);
            break;
        case 2:
            printf("Brak cieni %d\n", lightingModel);
            break;
        case 3:
            printf("Brak oswietlenia %d\n", lightingModel);
            break;
        case 4:
            printf("kierunkowe- Phonga %d\n", lightingModel);
            break;
        case 5:
            printf("kierunkowe- Blinn-Phong %d\n", lightingModel);
            break;
        case 6:
            printf("animacja kierunkowego %d\n", lightingModel);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void AnimationKoliber() {
    for (int i=0; i < NUM_KOLIBER_INSTANCES; i++)
    {
        matModels[i] = glm::translate(matModels[i], glm::vec3(Time, 0.0, 0.0));
    }
    glBindBuffer(GL_ARRAY_BUFFER, object[OBJ_KOLIBER].GetvInstances());
    glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_KOLIBER_INSTANCES * sizeof(glm::mat4), &matModels[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TimeAnimation(int frame) {
    Time += 0.01;
	glutPostRedisplay();
	glutTimerFunc(1000/60, TimeAnimation, 0);
}


void InitializeTreePositions() {
    srand(time(NULL) + 1);

    for (int i = 0; i < TREE_COUNT; ++i) {
        bool positionOK;
        float x, z, y;
        do {
            positionOK = true;
            x = static_cast<float>(rand() % 91) - 45.0f;
            z = static_cast<float>(rand() % 91) - 45.0f;
            y = myGround.getAltitute(glm::vec2(x, z));

            glm::vec3 newPos = glm::vec3(x, y, z);

            for (int j = 0; j < activeGifts && j < GIFT_COUNT; ++j) {
                if (glm::distance(newPos, giftPositions[j]) < 1.0f) {
                    positionOK = false;
                    break;
                }
            }
        } while (!positionOK);

        treePositions[i] = glm::vec3(x, y, z);
    }
}


void GenerateTrees() {
    for (int i = 0; i < TREE_COUNT; ++i) {
        float angle = std::sin(Time) * 5.0f;

        glm::mat4 model = glm::translate(glm::mat4(1.0), treePositions[i]);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

        object[TREE].SetMatModel(model);
        treeColliders[i].SetPosition(treePositions[i]);
        treeColliders[i].Collider = new CSphereCollider(&treeColliders[i].Position, 0.4f);
        object[TREE].Draw();
    }
}

void InitializeGiftPositions() {
    srand(time(NULL));

    for (int i = 0; i < activeGifts && i < GIFT_COUNT; ++i) {
        float x = static_cast<float>(rand() % 91) - 45.0f;
        float z = static_cast<float>(rand() % 91) - 45.0f;
        float y = myGround.getAltitute(glm::vec2(x, z)) + 1.0f;

        giftPositions[i] = glm::vec3(x, y, z);
        giftCollected[i] = false;

        giftColliders[i].Collider = new CSphereCollider(&giftPositions[i], 0.6f);
    }
}

void CheckGiftCollisions() {
    glm::vec3 playerPosition = myPlayer.Position;
    float playerColliderRadius = 1.0f;

    bool allCollected = true;

    for (int i = 0; i < activeGifts; ++i) {
        if (!giftCollected[i]) {
            float distance = glm::distance(playerPosition, giftPositions[i]);

            if (distance < (playerColliderRadius + 0.4f)) {
                giftCollected[i] = true;
                playerScore += 1;
                delete giftColliders[i].Collider;
                giftColliders[i].Collider = nullptr;
                collectedGifts += 1;
                collectedAllGifts += 1;
            } else {
                allCollected = false;
            }
        }
    }

    if (allCollected && collectedGifts > 0) {
        activeGifts = std::min(activeGifts * 2, GIFT_COUNT);
        collectedGifts = 0;
        InitializeGiftPositions();
    }
}


void GenerateGifts() {
    CheckGiftCollisions();

    for (int i = 0; i < activeGifts; ++i) {
        if (!giftCollected[i]) {
            float upDown = std::sin(Time*2) * 0.2f + 0.2f;
            glm::mat4 model = glm::translate(glm::mat4(1.0), giftPositions[i] + glm::vec3(0.0f, upDown, 0.0f));
            model = glm::rotate(model, Time, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
            object[GIFT].SetMatModel(model);
            giftColliders[i].SetPosition(giftPositions[i]);
            giftColliders[i].Collider = new CSphereCollider(&giftColliders[i].Position, 0.6f);
            object[GIFT].Draw();
        }
    }
}

void InitializeClockPositions() {
    srand(time(NULL) + 2);

    for (int i = 0; i < activeClocks && i < CLOCK_COUNT; ++i) {
        float x = static_cast<float>(rand() % 91) - 45.0f;
        float z = static_cast<float>(rand() % 91) - 45.0f;
        float y = myGround.getAltitute(glm::vec2(x, z)) + 1.0f;

        clockPositions[i] = glm::vec3(x, y, z);
        clockCollected[i] = false;

        clockColliders[i].Collider = new CSphereCollider(&clockPositions[i], 0.6f);
    }
}

void CheckClockCollisions() {
    glm::vec3 playerPosition = myPlayer.Position;
    float playerColliderRadius = 1.0f;
    currentTime = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastClockSpawnTime).count() >= 10) {
        for (int i = 0; i < activeClocks; ++i) {
            if (!clockCollected[i]) {
                delete clockColliders[i].Collider;
                clockColliders[i].Collider = nullptr;
                clockCollected[i] = true;
            }
        }
        InitializeClockPositions();
        lastClockSpawnTime = currentTime;
    } else {
        for (int i = 0; i < activeClocks; ++i) {
            if (!clockCollected[i]) {
                float distance = glm::distance(playerPosition, clockPositions[i]);
                if (distance < (playerColliderRadius + 0.6f)) {
                    clockCollected[i] = true;
                    extraTime += 5.0f;
                    delete clockColliders[i].Collider;
                    clockColliders[i].Collider = nullptr;
                    collectedClocks += 1;
                    collectedAllClocks += 1;
                }
            }
        }
    }
}

void GenerateClocks() {
    CheckClockCollisions();

    for (int i = 0; i < activeClocks; ++i) {
        if (!clockCollected[i]) {
            float upDown = std::sin(Time*2) * 0.2f + 0.2f;
            glm::mat4 model = glm::translate(glm::mat4(1.0), clockPositions[i] + glm::vec3(0.0f, upDown, 0.0f));
            model = glm::rotate(model, Time, glm::vec3(0.0f, -1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
            object[CLOCK].SetMatModel(model);
            clockColliders[i].SetPosition(clockPositions[i]);
            clockColliders[i].Collider = new CSphereCollider(&clockColliders[i].Position, 0.6f);
            object[CLOCK].Draw();
        }
    }
}


void InitializeGhostPositions() {
    srand(time(NULL) + 3);

    for (int i = 0; i < activeGhosts && i < GHOST_COUNT; ++i) {
        float x = static_cast<float>(rand() % 91) - 45.0f;
        float z = static_cast<float>(rand() % 91) - 45.0f;
        float y = myGround.getAltitute(glm::vec2(x, z)) + 1.0f;

        ghostPositions[i] = glm::vec3(x, y, z);
        ghostCollected[i] = false;

        ghostColliders[i].Collider = new CSphereCollider(&ghostPositions[i], 0.6f);
    }
}

void CheckGhostCollisions() {
    glm::vec3 playerPosition = myPlayer.Position;
    float playerColliderRadius = 1.0f;

    for (int i = 0; i < GHOST_COUNT; ++i) {
        if (!ghostCollected[i]) {
            float distance = glm::distance(playerPosition, ghostPositions[i]);
            if (distance < (playerColliderRadius + 0.4f)) {
                isPlayerStunned = true;
                extraTime -= 1;
                stunnedStartTime = std::chrono::steady_clock::now();
                delete ghostColliders[i].Collider;
                ghostColliders[i].Collider = nullptr;
                ghostCollected[i] = true;
            }
        }
    }
}

void UpdatePlayerState() {
    if (isPlayerStunned) {
        auto now = std::chrono::steady_clock::now();
        auto stunnedDuration = std::chrono::duration_cast<std::chrono::seconds>(now - stunnedStartTime).count();
        if (stunnedDuration >= 3) {
            isPlayerStunned = false;
        }
    }
}

void GenerateGhosts() {
    CheckGhostCollisions();
    UpdatePlayerState();

    for (int i = 0; i < GHOST_COUNT; ++i) {
        if (ghostColliders[i].Collider != nullptr) {
            float movement = std::sin(Time * 4) * 2.0f;
            glm::vec3 position = ghostPositions[i] + glm::vec3(0.0f, 0.0f, movement);

            float direction = std::cos(Time * 4);
            float angle = direction >= 0 ? 0.0f : M_PI;

            glm::mat4 model = glm::translate(glm::mat4(1.0), position);
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

            object[GHOST].SetMatModel(model);
            ghostColliders[i].SetPosition(position);
            object[GHOST].Draw();
        }
    }
}

void UpdateExtraTime() {
    auto now = std::chrono::steady_clock::now();
    if (isMiniMapEnabled && std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime).count() >= 1) {
        extraTime -= 1;
        lastUpdateTime = now; // Zaktualizuj czas ostatniej aktualizacji
    }
}

void RenderCenteredText(std::string text, float x, float y, float scale, glm::vec3 color) {
    float textWidth = 0.0f;
    for (char c : text) {
        Character ch = text_Characters[c];
        textWidth += (ch.Advance >> 6) * scale;
    }
    x -= textWidth / 2;
    RenderText(text, x, y, scale, color);
}

void RenderMyText() {
    RenderText("ESC - Exit", 800, 100, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
    RenderText("AWSD - Movement", 800, 80, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
    RenderText("M - Minimap", 800, 60, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
    RenderText("B, N - SPEED BOOST", 800, 40, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));

    char scoreText[50];
    sprintf(scoreText, "Wynik: %d", playerScore);
    RenderText(scoreText, 25, 950, 0.5f, glm::vec3(1.0, 0.0, 0.0));

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    currentTime = std::chrono::steady_clock::now();
    float timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - gameStartTime).count();
    float countdown = 60 - timeElapsed + extraTime;
    if (timeElapsed <= 3) {
        float scale = 2.0f;
        glm::vec3 color = glm::vec3(0.0, 0.8, 0.8);

        std::string text = "Zbieraj prezenty";
        RenderCenteredText(text, screenWidth / 2, screenHeight / 2, scale, color);
    }
    if (countdown > 0 && isPlayerStunned)
    {
        std::string stunText = "Masz stuna!";
        RenderCenteredText(stunText, screenWidth / 2, screenHeight / 2, 2.0f, glm::vec3(1.0, 0.0, 0.0));
    }

    char timeText[50];
    if (countdown > 0) {
        sprintf(timeText, "Czas: %.0f", countdown);
    } else {
        sprintf(timeText, "Czas: 0");
    }
    RenderText(timeText, 800, 950, 0.5f, glm::vec3(1.0, 0.0, 0.0));

    if (countdown <= 0) {
        std::string endText = "Koniec czasu!";
        RenderCenteredText(endText, screenWidth / 2, screenHeight / 2, 2.0f, glm::vec3(1.0, 0.0, 0.0));
        isPlayerStunned = true;
    }
}


void RenderScene_to_ShadowMap()
{
    // 1. Ustawienie viewportu i framebuffera
    glViewport (0 , 0, DepthMap_Width , DepthMap_Height );
    glBindFramebuffer ( GL_FRAMEBUFFER , DepthMap_idFrameBuffer );
    glClear ( GL_DEPTH_BUFFER_BIT );

    // 2. Aktywacja programu
    glUseProgram ( DepthMap_idProgram );

    // 3. Wysylanie macierzy proj i view swiatla
    glUniformMatrix4fv( glGetUniformLocation( DepthMap_idProgram, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
    glUniformMatrix4fv( glGetUniformLocation( DepthMap_idProgram, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );

//    matView = UpdateViewMatrix();
//	vec3 cameraPos = ExtractCameraPos(matView);

    // 4. Rendering obiektow ktore maja sie znalezc w mapie cieni
//	object[OBJ_BALL1].SetMatModel(glm::rotate(glm::mat4(1.0), Time, glm::vec3(0.0, 1.0, 0.0)) *
//                                              glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0)));
//    object[OBJ_BALL1].Draw();
//
//    object[OBJ_BALL2].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(2.0, 2.0, Time)));
//    object[OBJ_BALL2].Draw();
//
//    object[OBJ_BALL3].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(Time, 1.0, 2.0)));
//    object[OBJ_BALL3].Draw();

    object[OBJ_TERRAIN].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
    object[OBJ_TERRAIN].Draw();

    object[FENCE].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
    object[FENCE].Draw();

    GenerateTrees();
    GenerateGifts();
    GenerateClocks();

    myPlayer.Draw();

//    object[OBJ_KOLIBER].SetMatModel(glm::mat4(1.0));
//    LoadAndApplyTexture("objects/koliber.jpg", koliberProgram.GetId(), OBJ_KOLIBER);
//    object[OBJ_KOLIBER].DrawInstances();

    glUseProgram (0);
}

void RenderScene() {
    // Obliczanie macierzy widoku (pozycja kamery)
//	matView = UpdateViewMatrix();
    matView = myCamera.matView;

	vec3 cameraPos = ExtractCameraPos(matView);

    mainProgram.UseProgram();
    mainProgram.SetUniform("lightingModel", lightingModel);
    mainProgram.SetUniform("uIsMirrorEffect", isMirrorEffectEnabled);
    mainProgram.SetUniform("uIsTransparencyEffect", isTransparencyEffectEnabled);
    mainProgram.SetLightParameters(myLight[LIGHT_LIGHT1]);
    mainProgram.SetMatrices(matProj, matView);
    mainProgram.SetUniform("cameraPos", cameraPos);
    mainProgram.SetTime("Time", Time);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, DepthMap_idTexture);
    glUniform1i(glGetUniformLocation(mainProgram.GetId(), "tex_shadowMap"), 2);

//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL1]);
//    object[OBJ_BALL1].SetMatModel(
//                                  glm::rotate(glm::mat4(1.0), Time, glm::vec3(0.0, 1.0, 0.0)) *
//                                              glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0)));
//    object[OBJ_BALL1].Draw();
//
//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL2]);
//    object[OBJ_BALL2].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(2.0, 2.0, Time)));
//    object[OBJ_BALL2].Draw();
//
//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL3]);
//    object[OBJ_BALL3].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(Time, 1.0, 2.0)));
//    object[OBJ_BALL3].Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[OBJ_TERRAIN]);
    object[OBJ_TERRAIN].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
    object[OBJ_TERRAIN].Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[FENCE]);
    object[FENCE].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
    object[FENCE].Draw();

//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[GHOST]);
//    object[GHOST].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
//    object[GHOST].Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[CLOUDS]);

    float startPositionX = -100.0f;
    float endPositionX = 200.0f;
    float range = endPositionX - startPositionX;

    float currentPositionX = startPositionX + fmod(Time * 5, range);
    glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(currentPositionX, 0.0, 0.0));

    object[CLOUDS].SetMatModel(model);
    object[CLOUDS].Draw();

//    SendMaterialParameters(myMaterial[OBJ_BALL1]);
//    testBall.SetMatModel(glm::translate(glm::mat4(1.0), testBall.Position));
//    testBall.Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[TREE]);
    GenerateTrees();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[GIFT]);
    GenerateGifts();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[CLOCK]);
    GenerateClocks();

    CheckGhostCollisions();
    UpdatePlayerState();

    myPlayer.Draw();
    myCamera.SendPV(mainProgram.GetId());


//    koliberProgram.UseProgram();
//    koliberProgram.SetUniform("lightingModel", lightingModel);
//    koliberProgram.SetUniform("uIsMirrorEffect", isMirrorEffectEnabled);
//    koliberProgram.SetUniform("uIsTransparencyEffect", isTransparencyEffectEnabled);
//    koliberProgram.SetLightParameters(myLight[LIGHT_LIGHT1]);
//    koliberProgram.SetMatrices(matProj, matView);
//    koliberProgram.SetUniform("cameraPos", cameraPos);
//    koliberProgram.SetTime("Time", Time);
//
//    SendLightParameters(koliberProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_KOLIBER]);
//
//    object[OBJ_KOLIBER].SetMatModel(glm::mat4(1.0));
//    AnimationKoliber();
//    object[OBJ_KOLIBER].DrawInstances();

    glUseProgram( 0 );


    if (isMiniMapEnabled && !isEffect1Enabled) {
        glm::mat4 matProjMap, matViewMap, matModelMap;
        matProjMap = glm::perspective(glm::radians(50.0f), (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 2.0f, 100.0f);
        matViewMap = glm::lookAt(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        matModelMap = glm::mat4(1.0f);

        miniMapProgram.UseProgram();
        miniMapProgram.SetMatrices(matProjMap, matViewMap, matModelMap);
        DrawMapTexture();

        glUseProgram( 0 );
    }

    if (isEffect1Enabled || isEffect2Enabled) {
        glm::mat4 matProjMap, matViewMap, matModelMap;
        matProjMap = glm::perspective(glm::radians(50.0f), (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 2.0f, 100.0f);
        matViewMap = glm::lookAt(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        matModelMap = glm::mat4(1.0f);

        postProcessingProgram.UseProgram();
        postProcessingProgram.SetMatrices(matProjMap, matViewMap, matModelMap);
        postProcessingProgram.SetUniform("effect1", isEffect1Enabled);
        postProcessingProgram.SetUniform("effect2", isEffect2Enabled);
        DrawFilterTexture();

        glUseProgram( 0 );
    }

    GLuint skyboxTextureId = mySkyBox.GetTextureId();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
    glUniform1i(glGetUniformLocation(mainProgram.GetId(), "tex_skybox"), 1);

    mySkyBox.Render(matProj, matView);

    UpdateExtraTime();

    RenderMyText();
}
// ---------------------------------------

void RenderMiniMap() {
//    matView = myCamera.matView;
//
//	vec3 cameraPos = ExtractCameraPos(matView);
	if (isMiniMapEnabled) {
        matView = myCamera.matView;
        glm::vec3 cameraPos = ExtractCameraPos(matView);
        glm::vec3 miniMapCameraPos = cameraPos + glm::vec3(0.0f, 10.0f, 0.0f);
        matView = glm::lookAt(miniMapCameraPos, myPlayer.Position, glm::vec3(0.0f, 1.0f, 0.0f));
    }

	else {
        matView = myCamera.matView;
	}

	vec3 cameraPos = ExtractCameraPos(matView);

    mainProgram.UseProgram();
    mainProgram.SetUniform("lightingModel", lightingModel);
    mainProgram.SetUniform("uIsMirrorEffect", isMirrorEffectEnabled);
    mainProgram.SetUniform("uIsTransparencyEffect", isTransparencyEffectEnabled);
    mainProgram.SetLightParameters(myLight[LIGHT_LIGHT1]);
    mainProgram.SetMatrices(matProj, matView);
    mainProgram.SetUniform("cameraPos", cameraPos);
    mainProgram.SetTime("Time", Time);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, DepthMap_idTexture);
    glUniform1i(glGetUniformLocation(mainProgram.GetId(), "tex_shadowMap"), 2);

//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL1]);
//    object[OBJ_BALL1].SetMatModel(
//                                  glm::rotate(glm::mat4(1.0), Time, glm::vec3(0.0, 1.0, 0.0)) *
//                                              glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0)));
//    object[OBJ_BALL1].Draw();
//
//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL2]);
//    object[OBJ_BALL2].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(2.0, 2.0, Time)));
//
//    object[OBJ_BALL2].Draw();
//
//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_BALL3]);
//    object[OBJ_BALL3].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(Time, 1.0, 2.0)));
//    object[OBJ_BALL3].Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[OBJ_TERRAIN]);
    object[OBJ_TERRAIN].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0)));
    object[OBJ_TERRAIN].Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[FENCE]);
    object[FENCE].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
    object[FENCE].Draw();

//    SendLightParameters(mainProgram.GetId());
//    SendMaterialParameters(myMaterial[GHOST]);
//    object[GHOST].SetMatModel(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0)));
//    object[GHOST].Draw();

//    SendMaterialParameters(myMaterial[OBJ_BALL1]);
//    testBall.SetMatModel(glm::translate(glm::mat4(1.0), testBall.Position));
//    testBall.Draw();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[TREE]);
    GenerateTrees();

    SendLightParameters(mainProgram.GetId());
    SendMaterialParameters(myMaterial[GHOST]);
    GenerateGhosts();

    myPlayer.Draw();
    myCamera.SendPV(mainProgram.GetId());

//    koliberProgram.UseProgram();
//    koliberProgram.SetUniform("lightingModel", lightingModel);
//    koliberProgram.SetUniform("uIsMirrorEffect", isMirrorEffectEnabled);
//    koliberProgram.SetUniform("uIsTransparencyEffect", isTransparencyEffectEnabled);
//    koliberProgram.SetLightParameters(myLight[LIGHT_LIGHT1]);
//    koliberProgram.SetMatrices(matProj, matView);
//    koliberProgram.SetUniform("cameraPos", cameraPos);
//    koliberProgram.SetTime("Time", Time);
//
//    SendLightParameters(koliberProgram.GetId());
//    SendMaterialParameters(myMaterial[OBJ_KOLIBER]);
//    object[OBJ_KOLIBER].SetMatModel(glm::mat4(1.0));
//    AnimationKoliber();
//    object[OBJ_KOLIBER].DrawInstances();

    glUseProgram( 0 );
    mySkyBox.Render(matProj, matView);
}

void DisplayScene()
{
    if (isMiniMapEnabled && (!isEffect1Enabled && !isEffect2Enabled)) {
        glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);
        glViewport(0, 0, Buffer_Width, Buffer_Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderMiniMap();
    }
    if (isEffect1Enabled || isEffect2Enabled) {
        glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer_PP);
        glViewport(0, 0, Buffer_Width, Buffer_Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderMiniMap();
    }
    RenderScene_to_ShadowMap();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    RenderScene();
    UpdateMovement();
	glutSwapBuffers();
}


void Initialize()
{
    CreateInstanced();

    glEnable(GL_DEPTH_TEST);
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CameraTranslate_x = -1.0;
	CameraTranslate_y = -1.0;
	CameraTranslate_z = -5.0;

    stbi_set_flip_vertically_on_load(true);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    lightingModel = 1;

    mainProgram.CreateProgram();
    mainProgram.LoadShaders("vertex.glsl", "fragment.glsl");

//    object[OBJ_BALL1].CreateFromOBJ(mainProgram.GetId(),"objects/sphere.obj", "objects/ball1.jpg");
//    object[OBJ_BALL2].CreateFromOBJ(mainProgram.GetId(),"objects/sphere.obj", "objects/ball2.jpg");
//    object[OBJ_BALL3].CreateFromOBJ(mainProgram.GetId(),"objects/sphere.obj", "objects/ball3.jpg");
    object[OBJ_TERRAIN].CreateFromOBJ(mainProgram.GetId(),"objects/terrain.obj", "objects/terrain2.jpg");
    object[TREE].CreateFromOBJ(mainProgram.GetId(),"objects/tree.obj", "objects/grass.jpg");
    object[FENCE].CreateFromOBJ(mainProgram.GetId(),"objects/fence.obj", "objects/fence.jpg");
    object[CLOUDS].CreateFromOBJ(mainProgram.GetId(),"objects/clouds.obj", "objects/clouds.jpg");
    object[GIFT].CreateFromOBJ(mainProgram.GetId(),"objects/giftBox.obj", "objects/ball1.jpg");
    object[CLOCK].CreateFromOBJ(mainProgram.GetId(),"objects/hourglass.obj", "objects/hourglass.jpg");
    object[GHOST].CreateFromOBJ(mainProgram.GetId(),"objects/ghost.obj", "objects/clouds.jpg");
//    koliberProgram.CreateProgram();
//    koliberProgram.LoadShaders("vertex-koliber.glsl", "fragment-koliber.glsl");
//    object[OBJ_KOLIBER].CreateFromOBJMultiple(matModels, koliberProgram.GetId(), "objects/koliber.obj", "objects/koliber.jpg");

//    testBall.SetPosition(glm::vec3(3.0, 1.0, 1.0));
//    testBall.CreateFromOBJ(mainProgram.GetId(),"objects/sphere.obj", "objects/ball1.jpg");
//    testBall.Collider = new CSphereCollider(&testBall.Position, 0.3f);

    miniMapProgram.CreateProgram();
    miniMapProgram.LoadShaders("vertex-miniMap.glsl", "fragment-miniMap.glsl");
    CreateScreen();
    InitMiniMap();

    postProcessingProgram.CreateProgram();
    postProcessingProgram.LoadShaders("vertex-pp.glsl", "fragment-pp.glsl");
    CreateFilter();
    InitPostProcessing();

    myGround.Init(object[OBJ_TERRAIN].GetVertices());

    myPlayer.Init(&myGround);
    myPlayer.SetPosition(glm::vec3(-1.0, 0.0, 0.0));
    myPlayer.Collider = new CSphereCollider(&myPlayer.Position, 0.7f);

    myCamera.Init(myPlayer.Position);

    InitializeGiftPositions();
    InitializeTreePositions();
    InitializeClockPositions();
    InitializeGhostPositions();

    mySkyBox.Initialize(SkyBoxModel);
    ShadowMapDir_Init();

    InitText("arial.ttf", 36);
}




// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

//    myCamera.UpdatePerspective(width, height);

	// Obliczanie macierzy rzutowania perspektywicznego
	matProj = glm::perspective(glm::radians(90.0f / zoom), width/(float)height, 0.1f, 200.0f );
}


// ---------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    if (key < 128) {
        KeyboardTable[key] = true;
    }

    if (key == 27) {
        exit(0);
    }

    if (key == 109) {
        isMiniMapEnabled = !isMiniMapEnabled;
    }

    glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y) {
    if (key < 128) {
        KeyboardTable[key] = false;
    }
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 1000, 1000 );
	glutCreateWindow( "Projekt koncowy GRA" );

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	//glutMouseFunc( MouseButton );
	//glutMotionFunc( MouseMotion );
	//glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );
    glutKeyboardUpFunc( KeyboardUp );
	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}
    int pointLightSubMenu = glutCreateMenu(LightingModelMenu);
    glutAddMenuEntry("Phong Shading", 0);
    glutAddMenuEntry("Blinn-Phong Shading", 1);

    int directionalLightSubMenu = glutCreateMenu(LightingModelMenu);
    glutAddMenuEntry("Phong Shading", 4);
    glutAddMenuEntry("Blinn-Phong Shading", 5);
    glutAddMenuEntry("Animation", 6);

    int skyBoxSubMenu = glutCreateMenu(SkyBoxModelMenu);
    glutAddMenuEntry("SkyBox 1", 0);
    glutAddMenuEntry("SkyBox 2", 1);

    int skyBoxReflection = glutCreateMenu(SkyBoxReflection);
    glutAddMenuEntry("Toggle Mirror Effect", 0);
    glutAddMenuEntry("Toggle Transparency Effect", 1);

    int toggleMiniMap = glutCreateMenu(ToggleMiniMap);
    glutAddMenuEntry("Turn on minimap", 0);
    glutAddMenuEntry("Turn off minimap", 1);

    int toggleEffects = glutCreateMenu(ChangeFilterEffect);
    glutAddMenuEntry("Turn on effect1", 0);
    glutAddMenuEntry("Turn off effect1", 1);
    glutAddMenuEntry("Turn on effect2" ,2);
    glutAddMenuEntry("Turn off effect2", 3);

    glutCreateMenu(LightingModelMenu);
    glutAddSubMenu("Point Light", pointLightSubMenu);
    glutAddSubMenu("Directional Light", directionalLightSubMenu);
    glutAddMenuEntry("No Shadows", 2);
    glutAddMenuEntry("No Lighting", 3);
    glutAddSubMenu("SkyBox Model", skyBoxSubMenu);
    glutAddSubMenu("SkyBox Effects", skyBoxReflection);
    glutAddSubMenu("Minimap", toggleMiniMap);
    glutAddSubMenu("Filters", toggleEffects);

    //glutAttachMenu(GLUT_RIGHT_BUTTON);

	Initialize();
    glutTimerFunc(1000/60, TimeAnimation, 0);
	glutMainLoop();

	object[OBJ_BALL1].Delete();
	object[OBJ_BALL2].Delete();
	object[OBJ_BALL3].Delete();
	object[OBJ_TERRAIN].Delete();
    mainProgram.Delete();
    koliberProgram.Delete();
    miniMapProgram.Delete();

	return 0;
}
