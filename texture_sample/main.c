#include <SDL.h>
#include <GL/gl.h>
SDL_Surface* surface;

#define VERTEX_IDX 0
#define COORD_IDX 1
#define WIDTH 512
#define HEIGHT 512

typedef struct AppData {
    SDL_Window* window;
    SDL_GLContext gl;

    GLuint program;
    GLuint textureID;
} AppData;

GLfloat matrix[4][4] = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
};

GLfloat vVertices[12] = {
	-1.0, -1.0,
	 1.0, -1.0,
	 1.0,  1.0,
	-1.0,  1.0
};

GLfloat vCoord[8] = {
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 0.0f, 0.0f,
	 1.0f, 0.0f
	};

static const unsigned int cube_elements[] = {
     0,1,2,
     0,2,3
};

const GLchar vShaderStr[] =
	"attribute vec4 vVertices;\n"
	"uniform mat4 modelMatrix;\n"
	"attribute vec2 vCoord;\n"
	"varying vec2 Coord;\n"
	"void main()\n"
	"{\n"
	"	 gl_Position = modelMatrix * vVertices;\n"
	"	 Coord = vCoord;\n"
	"}\n";

const GLchar fShaderStr[] =
	"precision mediump float;\n"
	"uniform sampler2D s_texture;\n"
	"varying vec2 Coord;\n"
	"void main()\n"
	"{\n"
	"  vec4 tex = texture2D(s_texture, Coord);\n"
	"  gl_FragColor = vec4(tex.b, tex.g, tex.r, 0);\n;"
	"}\n";

GLuint LoadShader(const char *shaderSrc, GLenum type)
{
    GLint compiled;
    GLuint shader = glCreateShader(type);

    if (shader == 0)
        return 0;
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = (char*)(malloc(sizeof(char) * infoLen));
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;

}

GLuint compile_shaders(void)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;


    vertex_shader = LoadShader(vShaderStr, GL_VERTEX_SHADER);
    fragment_shader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);

    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glUseProgram(program);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}


void GLInit(AppData* ad)
{
    ad->program = compile_shaders();

    glBindAttribLocation(ad->program, VERTEX_IDX, "vVertices");
    glBindAttribLocation(ad->program, COORD_IDX, "vCoord");
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(VERTEX_IDX, 2, GL_FLOAT, GL_FALSE, 0, vVertices);
    glVertexAttribPointer(COORD_IDX, 2, GL_FLOAT, GL_FALSE, 0, vCoord);
    glEnableVertexAttribArray(VERTEX_IDX);
    glEnableVertexAttribArray(COORD_IDX);

    glGenTextures(1, &(ad->textureID));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ad->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

    GLuint uniformLocation = glGetUniformLocation(ad->program, "modelMatrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (GLfloat *)matrix);

    unsigned int ibo;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);


}

void drawGL(AppData* ad)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WIDTH, HEIGHT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glFinish();
}


int initGL(AppData* ad)
{
    ad->gl = SDL_GL_CreateContext(ad->window);
    if (ad->gl == NULL) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[SDL] GL context creation failed!");
        SDL_Quit();
        return (-1);
    }

    SDL_GL_MakeCurrent(ad->window, ad->gl);
    return (0);
}

int
main(int argc, char **argv)
{
    AppData ad = {0,};
    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
       SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[SDL] Initialize Failed");
       return -1;
    }
	ad.window = SDL_CreateWindow(NULL, 2000, 0, WIDTH, HEIGHT,	SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (ad.window == 0) {
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[SDL] Failed to create Window");
			SDL_Quit();
			return -1;
	}
    surface = SDL_LoadBMP("./cat512.bmp");
	initGL(&ad);

    GLInit(&ad);
    SDL_bool isQuit = SDL_FALSE;
    while(!isQuit) {
        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            switch(event.type)
            {
            case SDL_QUIT:
            isQuit = SDL_TRUE;
            break;
            }
        }
        drawGL(&ad);
        SDL_GL_SwapWindow(ad.window);
    }
    SDL_Quit();
    return 0;
}
