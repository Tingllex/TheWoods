#ifndef __CPLAYER
#define __CPLAYER
#include "headers/CMesh.h"
// ----------------------------------------------
// Klasa do reprezentacji postaci
// - obiektu, ktory porusza sie po scenie
// ----------------------------------------------
class CPlayer
{

public:

	// pozycja obiektu
	glm::vec3 Position = glm::vec3(0.0, 0.0, 0.0);

	// kat orientacji obiektu
	float Angle = 0.0;

	// wektor orientacji (UWAGA! wyliczany z Angle)
	glm::vec3 Direction = glm::vec3(1.0, 0.0, 0.0);

	// wskaznik do obiektu podloza
	CGround *myGround = NULL;

	// macierz modelu
	glm::mat4 matModel = glm::mat4(1.0);

	CMesh LEGO;


	CCollider *Collider = NULL;

	CPlayer() { }

	// Inicjalizacja obiektu
	void Init(CGround *ground)
	{
		myGround = ground;

		// Ludzik LEGO
		GLint idProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);

        LEGO.CreateFromOBJ(idProgram,"objects/LEGO.obj", "objects/lego.png");

		// Aktualizacja polozenia/macierzy itp.
		Update();

	}

	// renderowanie obiektu
	void Draw()
	{
		LEGO.SetMatModel(matModel);
        LEGO.Draw();
	}


	// Obliczenie wysokosci nad ziemia
	void Update()
	{
		matModel = glm::translate(glm::mat4(1.0), Position);
		matModel = glm::rotate(matModel, Angle, glm::vec3(0.0, 1.0, 0.0));
	}



	// ustawienie pozycji na scenie
	void SetPosition(glm::vec3 pos)
	{
		Position = pos;
		Update();
	}
    void Move(float val) {
        glm::vec3 newPosition = Position + Direction * val;

        if (newPosition.x > 35.0f) newPosition.x = 35.0f;
        if (newPosition.x < -35.0f) newPosition.x = -35.0f;
        if (newPosition.z > 35.0f) newPosition.z = 35.0f;
        if (newPosition.z < -35.0f) newPosition.z = -35.0f;

        glm::vec2 pos = glm::vec2(newPosition.x, newPosition.z);
        newPosition.y = myGround->getAltitute(pos);

        Position = newPosition;


        Update();
    }

	// zmiana pozycji na scenie
	void Move(float val, const CSceneObject &_other) {
	    glm::vec3 oldPosition = Position;
        glm::vec3 newPosition = Position + Direction * val;

        if (newPosition.x > 35.0f) newPosition.x = 35.0f;
        if (newPosition.x < -35.0f) newPosition.x = -35.0f;
        if (newPosition.z > 35.0f) newPosition.z = 35.0f;
        if (newPosition.z < -35.0f) newPosition.z = -35.0f;

        glm::vec2 pos = glm::vec2(newPosition.x, newPosition.z);
        newPosition.y = myGround->getAltitute(pos);

        Position = newPosition;

        if (this->Collider->isCollision(_other.Collider))
		{
			printf("Mamy kolizje!\n");
			Position = oldPosition;
			return;
		}

        Update();
    }

    void Move(float val, CSceneObject giftColliders[200], CSceneObject treeColliders[50], CSceneObject clockColliders[10], CSceneObject ghostColliders[5]) {
	    glm::vec3 oldPosition = Position;
        glm::vec3 newPosition = Position + Direction * val;

        if (newPosition.x > 50.0f) newPosition.x = 50.0f;
        if (newPosition.x < -50.0f) newPosition.x = -50.0f;
        if (newPosition.z > 50.0f) newPosition.z = 50.0f;
        if (newPosition.z < -50.0f) newPosition.z = -50.0f;

        glm::vec2 pos = glm::vec2(newPosition.x, newPosition.z);
        newPosition.y = myGround->getAltitute(pos);

        Position = newPosition;

//        float walkSpeed = 2.0f;
//        float walkAmplitude = 0.05f;
//        Position.y += sin(Time * walkSpeed * 10) * walkAmplitude;
//        matModel = glm::translate(glm::mat4(1.0), Position);
//
//        float scaleSpeed = 2.0f;
//        float scaleAmplitude = 0.02f;
//        float scale = 1.0f + sin(Time * scaleSpeed) * scaleAmplitude;
//        matModel = glm::scale(matModel, glm::vec3(scale, 1.0f, scale));
//
//
//        float tiltSpeed = 2.0f;
//        float tiltAmplitude = 0.1f;
//        float tilt = sin(Time * tiltSpeed) * tiltAmplitude;
//        matModel = glm::rotate(matModel, tilt, glm::vec3(1.0f, 0.0f, 0.0f));


        for (int i = 0; i < 200; ++i) {
            if (this->Collider->isCollision(giftColliders[i].Collider)) {
                //printf("Mamy kolizje z prezentem nr %d!\n", i);
                Position = oldPosition;
                return;
            }
        }
        for (int i = 0; i < 50; ++i) {
            if (this->Collider->isCollision(treeColliders[i].Collider)) {
                //printf("Mamy kolizje z drzewem nr %d!\n", i);
                Position = oldPosition;
                return;
            }
        }
        for (int i = 0; i < 10; ++i) {
            if (this->Collider->isCollision(clockColliders[i].Collider)) {
                //printf("Mamy kolizje z clock nr %d!\n", i);
                Position = oldPosition;
                return;
            }
        }

        for (int i = 0; i < 5; ++i) {
            if (this->Collider->isCollision(ghostColliders[i].Collider)) {
                //printf("Mamy kolizje z duch nr %d!\n", i);
                Position = oldPosition;
                return;
            }
        }

        Update();
    }


	// zmiana orientacji obiektu
	void Rotate(float angle)
	{
		Angle += angle;
		Direction.x = cos(Angle);
		Direction.z = -sin(Angle);

		// aktualizacja
		Update();
	}

};


#endif
