#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

// Identyfikatory buforow
GLuint idFrameBuffer_PP;   // FBO
GLuint idDepthBuffer_PP;	// Bufor na skladowa glebokosci
GLuint idTextureBuffer_PP; // Tekstura na skladowa koloru

// Kwadrat wspolrzedne
GLfloat vertices_pos_PP[] =
{
    -3.5f, -3.5f, 0.0f,
    3.5f, -3.5f, 0.0f,
    3.5f, 3.5f, 0.0f,

    3.5f, 3.5f, 0.0f,
    -3.5f, 3.5f, 0.0f,
    -3.5f, -3.5f, 0.0f,
};
// Kwadrat uv
GLfloat vertices_tex_PP[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
};

GLuint vBuffer_pos_PP;
GLuint vBuffer_uv_PP;
GLuint vBuffer_normal_PP;
GLuint vArray_PP;

void InitPostProcessing(){
    glGenFramebuffers(1, &idFrameBuffer_PP);
    glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer_PP);

    // 2. Stworzenie obiektu tekstury na skladowa koloru
    glGenTextures(1, &idTextureBuffer_PP);
    glBindTexture(GL_TEXTURE_2D, idTextureBuffer_PP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Buffer_Width, Buffer_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 3. Polaczenie tekstury ze skladowa koloru FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        idTextureBuffer_PP, 0);

    // 4. Stworzenie obiektu render buffer dla skladowej glebokosci
    glGenRenderbuffers(1, &idDepthBuffer_PP);
    glBindRenderbuffer(GL_RENDERBUFFER, idDepthBuffer_PP);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Buffer_Width, Buffer_Height);

    // 5. Polaczenie bufora glebokosci z aktualnym obiektem FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idDepthBuffer_PP);

    // 6. Sprawdzenie czy pomyslnie zostal utworzony obiekt bufora ramki
    //    a nastepnie powrot do domyslnego bufora ramki

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: Framebuffer is not complete!\n");
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CreateFilter()
{
    glGenVertexArrays( 1, &vArray_PP );
    glBindVertexArray( vArray_PP );

    glGenBuffers( 1, &vBuffer_pos_PP );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos_PP );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos_PP), vertices_pos_PP, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );

    glGenBuffers( 1, &vBuffer_uv_PP );
    glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv_PP );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex_PP), vertices_tex_PP, GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );
}

void DrawFilterTexture() {
    glBindVertexArray(vArray_PP);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, idTextureBuffer_PP);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif // POSTPROCESSING_HPP
