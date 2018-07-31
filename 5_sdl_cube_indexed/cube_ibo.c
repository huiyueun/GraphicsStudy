#include <SDL.h>
#include <GL/gl.h>

static const float cube_colors[] = {
    /* front surface is blue */
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    /* left surface is green */ 
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    /* top surface is red */
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    /* right surface is yellow */
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    /* back surface is cyan */
    0.0, 1.0, 1.0,
    0.0, 1.0, 1.0,
    0.0, 1.0, 1.0,
    0.0, 1.0, 1.0,
    /* bottom surface is magenta */
    1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 1.0
};
 
static const float cube_vertices[] = {
    /* front surface is blue */
    -0.5,  0.5, 0.5,
    -0.5, -0.5, 0.5,
     0.5, -0.5, 0.5,
     0.5,  0.5, 0.5,
    /* left surface is green */ 
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
    /* top surface is red */
    -0.5, 0.5, 0.5,
     0.5, 0.5, 0.5,
     0.5, 0.5, -0.5,
    -0.5, 0.5, -0.5,
    /* right surface is yellow */
    0.5,  0.5, -0.5,
    0.5,  0.5,  0.5,
    0.5, -0.5,  0.5,
    0.5, -0.5, -0.5,
    /* back surface is cyan */
    -0.5,  0.5, -0.5,
     0.5,  0.5, -0.5,
     0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    /* bottom surface is magenta */
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5, -0.5, -0.5,
};

static const unsigned int cube_elements[] = {
    3,1,2,
    3,0,1,
    4,6,7,
    4,5,6,
    8,9,10,
    8,10,11,
    12,13,14,
    12,14,15,
    16,17,19,
    17,18,19,
    20,23,21,
    21,23,22
};

/* Vertex Shader Source */
static const char vertex_shader[] =
    "attribute vec4 vPosition;\n"
    "attribute vec3 inColor;\n"
    "uniform mat4 mvpMatrix;"
    "varying vec3 outColor;\n"
    "void main()\n"
    "{\n"
    "   outColor = inColor;\n"
    "   gl_Position = mvpMatrix * vPosition;\n"
    "}\n";

/* Fragment Shader Source */
static const char fragment_shader[] =
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "varying vec3 outColor;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor = vec4 ( outColor, 1.0 );\n"
    "}\n";

typedef struct {
    float x, y;
} FloatPoint;

typedef struct appdata {
    SDL_bool game_exit;
    SDL_Window* window;
    SDL_GLContext gl;
    SDL_DisplayMode mode;

    int window_rotation;

    SDL_bool mouse_down;
    
    float model[16];
    float view[16];
    float mvp[16];

    FloatPoint anglePoint;
    FloatPoint curPoint;
    FloatPoint prevPoint;

    unsigned int program;

    unsigned int vbo;
    unsigned int ibo;
    unsigned int colorBuffer;
} appdata_s;

int initGL(appdata_s* ad)
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

void generateAndBindBuffer(appdata_s* ad)
{
    glGenBuffers(1, &ad->colorBuffer);
    glGenBuffers(1, &ad->vbo);
    glGenBuffers(1, &ad->ibo);

    //vertex position buffer
    glBindBuffer(GL_ARRAY_BUFFER, (ad->vbo));
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    //vertex color buffer
    glBindBuffer(GL_ARRAY_BUFFER, (ad->colorBuffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(1);

    //Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (ad->ibo));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
}

void init_matrix(float matrix[16])
{
    matrix[0] = 1.0f;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    matrix[4] = 0.0f;
    matrix[5] = 1.0f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

void init_shaders(appdata_s* ad)
{
    const char *p = vertex_shader;
    unsigned int vtx_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vtx_shader, 1, &p, NULL);
    glCompileShader(vtx_shader);

    p = fragment_shader;
    unsigned int fgmt_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fgmt_shader, 1, &p, NULL);
    glCompileShader(fgmt_shader);

    ad->program = glCreateProgram();
    glAttachShader(ad->program, vtx_shader);
    glAttachShader(ad->program, fgmt_shader);
    glBindAttribLocation(ad->program, 0, "vPosition");
    glBindAttribLocation(ad->program, 1, "inColor");

    glLinkProgram(ad->program);
    glUseProgram(ad->program);
}

void multiply_matrix(float matrix[16], const float matrix0[16], const float matrix1[16])
{
    int i;
    int row;
    int column;
    float temp[16];

    for (column = 0; column < 4; column++) {
        for (row = 0; row < 4; row++) {
            temp[column * 4 + row] = 0.0f;
            for (i = 0; i < 4; i++)
                temp[column * 4 + row] += matrix0[i * 4 + row] * matrix1[column * 4 + i];
        }
    }

    for (i = 0; i < 16; i++)
        matrix[i] = temp[i];
}

void translate_xyz(float* result, const float translatex, const float translatey, const float translatez)
{
    result[12] += result[0] * translatex + result[4] * translatey + result[8] * translatez;
    result[13] += result[1] * translatex + result[5] * translatey + result[9] * translatez;
    result[14] += result[2] * translatex + result[6] * translatey + result[10] * translatez;
    result[15] += result[3] * translatex + result[7] * translatey + result[11] * translatez;
}

void rotate_xyz(float matrix[16], const float anglex, const float angley, const float anglez)
{
    const float pi = 3.141592f;
    float temp[16];
    float rz = 2.0f * pi * anglez / 360.0f;
    float rx = 2.0f * pi * anglex / 360.0f;
    float ry = 2.0f * pi * angley / 360.0f;

    float sy = SDL_sinf(ry);
    float cy = SDL_cosf(ry);
    float sx = SDL_sinf(rx);
    float cx = SDL_cosf(rx);
    float sz = SDL_sinf(rz);
    float cz = SDL_cosf(rz);
    init_matrix(temp);

    temp[0] = cy * cz - sx * sy * sz;
    temp[1] = cz * sx * sy + cy * sz;
    temp[2] = -cx * sy;

    temp[4] = -cx * sz;
    temp[5] = cx * cz;
    temp[6] = sx;

    temp[8] = cz * sy + cy * sx * sz;
    temp[9] = -cy * cz * sx + sy * sz;
    temp[10] = cx * cy;

    multiply_matrix(matrix, matrix, temp);
}

int view_set_ortho(float result[16], const float left, const float right,
                        const float bottom, const float top, const float near, const float far)
{
    if ((right - left) == 0.0f || (top - bottom) == 0.0f || (far - near) == 0.0f)
        return 0;

    result[0] = 2.0f / (right - left);
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 0.0f;
    result[4] = 0.0f;
    result[5] = 2.0f / (top - bottom);
    result[6] = 0.0f;
    result[7] = 0.0f;
    result[8] = 0.0f;
    result[9] = 0.0f;
    result[10] = -2.0f / (far - near);
    result[11] = 0.0f;
    result[12] = -(right + left) / (right - left);
    result[13] = -(top + bottom) / (top - bottom);
    result[14] = -(far + near) / (far - near);
    result[15] = 1.0f;

    return 1;
}

void drawScene(appdata_s* ad)
{
    int static rot = 0.0f;
    rot+=5.0f;
    if(rot>=360)
        rot = 0.0f;
    int w, h;
    SDL_GetWindowSize(ad->window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    init_matrix(ad->model);
    rotate_xyz(ad->model, rot, ad->anglePoint.y, ad->window_rotation);

    multiply_matrix(ad->mvp, ad->view, ad->model);
    glUseProgram(ad->program);

    glUniformMatrix4fv(glGetUniformLocation(ad->program, "mvpMatrix"), 1, GL_FALSE, ad->mvp);

    glDrawElements(GL_TRIANGLES, (sizeof(cube_elements)/sizeof(unsigned int)), GL_UNSIGNED_INT, 0);
    SDL_GL_SwapWindow(ad->window);
}

int main()
{
    float width = 480;
    float height = 800;

    appdata_s ad = {0, };

    ad.anglePoint.x = 45.f;
    ad.anglePoint.y = 45.f;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[SDL] Initialize Failed");
        return -1;
    }

    ad.window = SDL_CreateWindow(NULL, 2000, 0, width, height,  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (ad.window == 0) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[SDL] Failed to create Window");
        SDL_Quit();
        return -1;
    }

    initGL(&ad);
    init_shaders(&ad);
    init_matrix(ad.view);
    generateAndBindBuffer(&ad);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    float aspect = (width > height ? (float)width/height : (float)height/width);
    if (width > height)
        view_set_ortho(ad.view, -1.0*aspect, 1.0*aspect, -1.0, 1.0, -1.0, 100.0);
    else
        view_set_ortho(ad.view, -1.0, 1.0, -1.0*aspect, 1.0*aspect, -1.0, 100.0);

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
        drawScene(&ad);
        SDL_GL_SwapWindow(ad.window);
    }

    SDL_Quit();
    return 0;
}

