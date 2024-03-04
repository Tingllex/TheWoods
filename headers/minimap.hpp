#ifndef MINIMAP_HPP
#define MINIMAP_HPP
// Identyfikatory buforow
GLuint idFrameBuffer;   // FBO
GLuint idDepthBuffer;	// Bufor na skladowa glebokosci
GLuint idTextureBuffer; // Tekstura na skladowa koloru

int Buffer_Width = 600;
int Buffer_Height = 600;

// Kwadrat wspolrzedne
GLfloat vertices_pos[] =
{
    -1.0f + 1.0f, -1.0f - 3.0f + 1.0f, 0.0f,
     2.0f + 1.0f, -1.0f - 3.0f + 1.0f, 0.0f,
     2.0f + 1.0f,  2.0f - 3.0f + 1.0f, 0.0f,
     2.0f + 1.0f,  2.0f - 3.0f + 1.0f, 0.0f,
    -1.0f + 1.0f,  2.0f - 3.0f + 1.0f, 0.0f,
    -1.0f + 1.0f, -1.0f - 3.0f + 1.0f, 0.0f,
};


// Kwadrat uv
GLfloat vertices_tex[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
};
GLuint vBuffer_pos;
GLuint vBuffer_uv;
GLuint vBuffer_normal;
GLuint vArray;

void CreateScreen()
{
    glGenVertexArrays( 1, &vArray );
    glBindVertexArray( vArray );

    glGenBuffers( 1, &vBuffer_pos );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    glGenBuffers( 1, &vBuffer_uv );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );

}

void InitMiniMap(){
    glGenFramebuffers(1, &idFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);

    // 2. Stworzenie obiektu tekstury na skladowa koloru
    glGenTextures(1, &idTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, idTextureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Buffer_Width, Buffer_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3. Polaczenie tekstury ze skladowa koloru FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        idTextureBuffer, 0);

    // 4. Stworzenie obiektu render buffer dla skladowej glebokosci
    glGenRenderbuffers(1, &idDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Buffer_Width, Buffer_Height);

    // 5. Polaczenie bufora glebokosci z aktualnym obiektem FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idDepthBuffer);

    // 6. Sprawdzenie czy pomyslnie zostal utworzony obiekt bufora ramki
    //    a nastepnie powrot do domyslnego bufora ramki

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: Framebuffer is not complete!\n");
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DrawMapTexture() {
    glBindVertexArray(vArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, idTextureBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif // MINIMAP_HPP
