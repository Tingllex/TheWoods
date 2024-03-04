// -----------------------------------------------
// Przykladowy szablon budowy klasy CCamera oraz
// pochodnych do obslugi kamery
// UWAGA: nalezy uzupelnic m.in. TO_DO
// -----------------------------------------------

#ifndef __CAMERA_H
#define __CAMERA_H

#define PI	3.1415926535

// -----------------------------------------------
class CCamera
{

public:

	// Macierze rzutowania i widoku
	glm::mat4 matProj;
	glm::mat4 matView;

	// Skladowe kontrolujace matView
	glm::vec3 Position;        // polozenie kamery
	glm::vec3 Angles;          // pitch, yaw, roll
	glm::vec3 Up;              // domyslnie wektor (0,1,0)
	glm::vec3 Direction;       // wektor kierunku obliczany z Angles

	// Skladowe kontrolujace matProj
	float Width, Height;       // proporcje bryly obcinania
	float NearPlane, FarPlane; // plaszczyzny tnace
	float Fov;	               // kat widzenia kamery

    float DistanceBehind = -9.0f; // Jak daleko za postaci
    float DistanceAbove = 4.0f;   // Jak wysoko nad postaci
    glm::vec3 PlayerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	// ---------------------------------------------
	// Domyslny konstruktor i domyslne ustawienia
	// ktore mozna swobodnie zmienic
	CCamera()
	{

		// Domyslne wartosci skladowych
		Up = glm::vec3(0.0f, 1.0f, 0.0f);
		Position = glm::vec3(0.0f, 6.0f, 15.f);
		Angles = glm::vec3(0.0f, 0.0f, 0.0f);
		Fov = 60.0f;

		NearPlane = 0.1f;
		FarPlane = 150.0f;

		// Wywolanie metody aktualizujacej
		// m.in. Direction i matView
		this->Update(PlayerPosition);
	}

	// ---------------------------------------------
	// Wirtualna metoda aktualizujaca dane kamery
	// przydatna w klasach pochodnych
    virtual void Update(glm::vec3 playerPosition)
    {
        float cameraX = playerPosition.x + DistanceBehind * sin(Angles.y);
        float cameraZ = playerPosition.z - DistanceBehind * cos(Angles.y);
        float cameraY = playerPosition.y + DistanceAbove;

        Position = glm::vec3(cameraX, cameraY, cameraZ);

        Direction = glm::normalize(playerPosition - Position);

//        matView = glm::rotate( matView, CameraRotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
//        matView = glm::rotate( matView, CameraRotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        matView = glm::lookAt(Position, playerPosition, Up);
    }

//    virtual void Update(glm::vec3 playerPosition)
//	{
//		// wektor Direction
//		Direction.x = cos(Angles.y) * cos(Angles.x);
//		Direction.y = sin(Angles.x);
//		Direction.z = sin(Angles.y) * cos(Angles.x);
//		// macierz widoku
//		matView = glm::lookAt(Position, Position+Direction, Up);
//
//	}

	// ---------------------------------------------
	// Metoda aktualizujaca macierz projection
	// wywolywana np. w Reshape()
	void UpdatePerspective(int width, int height)
	{
		Width = width;
		Height = height;
		matProj = glm::perspectiveFov(glm::radians(Fov), Width, Height, NearPlane, FarPlane);
	}

	// ---------------------------------------------
	// Metoda aktualizujaca macierz projection
	// wywolywana np. w Reshape()
	void UpdateOrtho(float width, float height)
	{
		// TO_DO:
		// matProj = glm::ortho(...);
		matProj = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, NearPlane, FarPlane);
	}


	// ---------------------------------------------
	// przesylanie obu macierzy do programu pod
	// wskazane nazwy zmiennych uniform
	void SendPV(GLuint program, const char *proj = "matProj", const char *view = "matView")
	{
		glUniformMatrix4fv( glGetUniformLocation( program, proj ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program, view ), 1, GL_FALSE, glm::value_ptr(matView) );
	}

	// ---------------------------------------------
	// przesylanie iloczynu macierzy matProj*matView
	// do programu pod wskazana nazwe
	void SendPV(GLuint program, const char *projview)
	{
		glm::mat4 matProjView = matProj * matView;
		glUniformMatrix4fv( glGetUniformLocation( program, projview ), 1, GL_FALSE, glm::value_ptr(matProjView) );
	}

	// ---------------------------------------------
	// zmiana kata widzenia kamery (np. przy zoomowaniu)
	void AddFov(GLfloat _fov)
	{
		this->Fov += _fov;
		this->UpdatePerspective(this->Width, this->Height);
	}

};

// ----------------------------------------------------------
class CFreeCamera : public CCamera
{

public:


	void Init(glm::vec3 playerPosition)
	{
		Position = glm::vec3(DistanceBehind, DistanceAbove, 0.0f);
        Angles = glm::vec3(0.0f, 1.5f, 0.0f);
        Up = glm::vec3(0.0f, 1.0f, 0.0f);
        CCamera::Update(playerPosition);
        PlayerPosition = playerPosition;
	}

	// --------------------------------------------------
	// Zmiana polozenia korzystajaca z wektora Direction
	void Move(float val, glm::vec3 playerPosition)
	{
		Position += Direction*val;

		// Aktualizacja danych kamery
		CCamera::Update(playerPosition);
		PlayerPosition = playerPosition;
	}

	// --------------------------------------------------
	// Zmiana orientacji (wektora Direction) ktory
	// wyliczany jest z katow w Angles
	void Rotate(float pitch, float yaw)
	{
	    //dziala obracanie kamery na mouse
//		Angles.x = (pitch + 1.570796f)/2 * 10;
//		Angles.y = (yaw + 1.570796f)/2 * 10;
		//dziala obracanie kamery na A/D
		Angles.x += pitch;
		Angles.y += yaw;

		// Zawijanie
		if (Angles.x > PI/2.0)  Angles.x =  PI/2.0;
		if (Angles.x < -PI/2.0) Angles.x = -PI/2.0;
		if (Angles.y > 2.0*PI)  Angles.y -= 2.0*PI;
		if (Angles.y < -2.0*PI) Angles.y += 2.0*PI;

		// Aktualizacja danych kamery
		CCamera::Update(PlayerPosition);
	}

};

#endif // __CAMERA_H
