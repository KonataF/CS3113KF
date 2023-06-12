/**
* Author: [Your name here]
* Assignment: Simple 2D Scene
* Date due: 2023-06-11, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"


#define LOG(argument) std::cout << argument << '\n'

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_second = 45.0f;

const int TRIANGLE_RED = 1.0;
const int TRIANGLE_BLUE = 0.4;
const int TRIANGLE_GREEN = 0.4;
const int TRIANGLE_OPACITY = 1.0;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;


const char PLAYER_SPRITE_FILEPATH[] = "death.png";
const char PLAYER_SPRITE_FILEPATH2[] = "metallica.png";

ShaderProgram program;
ShaderProgram program2;
glm::mat4 model_matrix, proj_matrix, tran_matrix, view_matrix, model_matrix2;

float previous_ticks = 0.0f;

const float GROWTH_FACTOR = 1.01f;
const float SHRINK_FACTOR = 0.99f;
const int MAX_FRAME = 40;


const float ROT_ANGLE = glm::radians(1.5f);
const float TRAN_VALUE = 0.025f;


SDL_Window* display_window;
bool game_is_running = true;
bool is_growing = true;
int frame_counter = 0;

GLuint player_texture_id;
GLuint player_texture_id2;

glm::vec3 player_position = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 player_movement = glm::vec3(0.0f, 0.0f, 0.0f);



GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);


    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }


    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;

}


void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Drawing 2D Textures",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    program.Load(V_SHADER_PATH, F_SHADER_PATH);
    program2.Load(V_SHADER_PATH, F_SHADER_PATH);

    view_matrix = glm::mat4(1.0f);
    model_matrix = glm::mat4(1.0f);
    model_matrix2 = glm::mat4(1.0f);
    proj_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    tran_matrix = model_matrix;


    program.SetProjectionMatrix(proj_matrix);
    program.SetViewMatrix(view_matrix);

    program2.SetProjectionMatrix(proj_matrix);
    program2.SetViewMatrix(view_matrix);

    program.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);
    program2.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    glUseProgram(program.programID);
    glUseProgram(program2.programID);
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    player_texture_id2 = load_texture(PLAYER_SPRITE_FILEPATH2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            game_is_running = false;
        }
    }
}

void update() {
    frame_counter++;

    glm::vec3 scale_vector;

    // Step 2
    if (frame_counter >= MAX_FRAME)
    {
        is_growing = !is_growing;
        frame_counter = 0;
    }

    // Step 3
    scale_vector = glm::vec3(is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
        is_growing ? GROWTH_FACTOR : SHRINK_FACTOR,
        1.0f);

    model_matrix = glm::translate(model_matrix, glm::vec3(TRAN_VALUE, TRAN_VALUE, 0.0f));
    model_matrix = glm::scale(model_matrix, scale_vector);
    model_matrix2 = glm::rotate(model_matrix2, ROT_ANGLE, glm::vec3(0.0f, 0.0f, 1.0f));


}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id, ShaderProgram program) {
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] =
    {
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,   //object 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    float vertices2[] =
    {
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   //object 2
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };

    float texture_coordinates2[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };



    // We disable two attribute arrays now

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    glEnableVertexAttribArray(program2.positionAttribute);
    glEnableVertexAttribArray(program2.texCoordAttribute);

    draw_object(model_matrix, player_texture_id, program);
    draw_object(model_matrix2, player_texture_id2, program2);

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program2.positionAttribute);

    glDisableVertexAttribArray(program.texCoordAttribute);
    glDisableVertexAttribArray(program2.texCoordAttribute);

    SDL_GL_SwapWindow(display_window);
}

void shutdown() { SDL_Quit(); }

int main(int argc, char* argvp[]) {
    initialise();

    while (game_is_running)
    {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
