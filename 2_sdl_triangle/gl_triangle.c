#include <SDL.h>
#include <GL/gl.h>
 
GLuint compile_shaders(void)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
 
    //vertext shader source code
    static const GLchar * vertex_shader_source[] = 
    {
        "#version 430 core                           \n"
        "                                            \n"
        "void main(void)                             \n"
                "{                                           \n"
        "    const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),"
                "                             vec4(-0.25, -0.25, 0.5, 1.0)," 
                "                             vec4(0.25, 0.25, 0.5, 1.0));\n"
                "    gl_Position = vertices[gl_VertexID];    \n"
                "}                                           \n"
    };
 
    //fragment shader source code
    static const GLchar * fragment_shader_source[] = 
    {
        "#version 430 core                          \n"
                "                                           \n"
                "out vec4 color;                            \n"
                "                                           \n"
                "void main(void)                            \n"
                "{                                          \n"
                "    color = vec4(0.0, 0.0, 1.0, 1.0);      \n"
                "}                                          \n"
    };
 
    //create vertex shader and compile
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
 
    //create fragment shader and compile
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
 
    //create program->attach shader->link
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    //delete shader 
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
 
    return program;
}
 
int main()
{
    SDL_Window *win = NULL;
    SDL_GLContext context;
    GLuint rendering_program;
 
    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("window", 2000, 0, 480, 800, SDL_WINDOW_OPENGL);
    
    context = SDL_GL_CreateContext(win);
    rendering_program = compile_shaders();
 
    SDL_bool isQuit = SDL_FALSE;
 
    while(!isQuit) {
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                isQuit = SDL_TRUE;
            }
            
        }
        static const GLfloat red[] = {0.0f, 0.2f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, red);
        glUseProgram(rendering_program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(win);
    }
    
    SDL_Quit();
    return 0;
}

