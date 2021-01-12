#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>


#include <GL/glew.h>
#include<jansson.h>
#include<ulfius.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define STB_IMAGE_IMPLEMENTATION
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

#include "nuklear.h"
#include "stb_image.h"
#include "nuklear_glfw_gl3.h"
#include "style.c"
#include "device.h"
#include "global.h"
#include "Encryption.c"
#include "functions.c"
#include "FileBrowser.c"
#include "ui.c"
int SHOW_DIALOG_MESSAGE = 0;
int SHOW_FILE_MANAGER_HOME = 1;
int SHOW_LOGIN_POPUP = 0;
int SHOW_REGISTER_POPUP = 0;
int LOGGED_IN = 0;
int CURRENT_SCREEN = 0;
int SHOW_ENCRYPT_DECRYPT_PASS = 0;
char* POPUP_MESSAGE;
char* CURRENT_USER;
char* CURRENT_FILE;
GLFWwindow *window;
struct nk_glfw glfw = {0};
struct file_browser browser;
struct media media;
json_t* files = NULL;

static struct nk_image
icon_load(const char *filename)
{
    int x,y,n;
    GLuint tex;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    if (!data) {
        printf("[SDL]: failed to load image: %s", filename);
        exit(1);
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return nk_image_id((int)tex);
}



static void error_callback(int e, const char* m){
    printf("Error %s \n", m);
}

int main() {
    POPUP_MESSAGE = malloc(1);


    struct device device;
    struct nk_context *ctx, *ctx2;
    struct nk_colorf bg;
    struct nk_font_atlas *atlas;
    struct nk_font *font_14;
    struct nk_font *font_18;
    struct nk_font_config cfg = nk_font_config(0);



    const void *image; int w, h;

    glfwSetErrorCallback(error_callback);

    if(!glfwInit())  {
        printf( "Error initiating GLFW3");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    window = glfwCreateWindow(500, 750, "Login", NULL, NULL);
    if(!window){
        printf("Error creating window");
        exit(-2);
    }
    glfwMakeContextCurrent(window);

    glViewport(0, 0, 500, 1000);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    ctx2 = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    cfg.oversample_h = 3; cfg.oversample_v = 2;
    nk_glfw3_font_stash_begin(&glfw, &atlas);


    font_14 = nk_font_atlas_add_from_file(atlas, "./media/Roboto-Regular.ttf", 14.0f, 0);
    font_18 = nk_font_atlas_add_from_file( atlas, "./media/Roboto-Bold.ttf", 24.0f, 0);

    nk_glfw3_font_stash_end(&glfw);
    nk_style_set_font(ctx, &font_14->handle);

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    glEnable(GL_TEXTURE_2D);
    media.icons.home = icon_load("./media/icon/home.png");
    media.icons.directory = icon_load("./media/icon/directory.png");
    media.icons.computer = icon_load("./media/icon/computer.png");
    media.icons.desktop = icon_load("./media/icon/desktop.png");
    media.icons.default_file = icon_load("./media/icon/default.png");
    media.icons.text_file = icon_load("./media/icon/text.png");
    media.icons.music_file = icon_load("./media/icon/music.png");
    media.icons.font_file =  icon_load("./media/icon/font.png");
    media.icons.img_file = icon_load("./media/icon/img.png");
    media.icons.movie_file = icon_load("./media/icon/movie.png");
    media_init(&media);
    file_browser_init(&browser, &media);


    while(!glfwWindowShouldClose(window)){

        //createImageHandler(window);
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);
        if (LOGGED_IN){
            if (nk_begin(ctx, "HOME", nk_rect(0,0,1200, 1000), NK_WINDOW_NO_SCROLLBAR)) {
                ctx->style.window.spacing = nk_vec2(0,0);
                ctx->style.window.padding = nk_vec2(0,0);
                ctx->style.window.group_padding = nk_vec2(0,0);

                nk_layout_row_begin(ctx, NK_STATIC, 1000, 2);
                nk_layout_row_push(ctx, 150);
                navigation(*ctx, *font_18, *font_14);
                nk_layout_row_push(ctx, 1050);
                createMainWindow(*ctx2, *font_18, *font_14, &browser, &media);

                nk_end(ctx);

            }

        }else {
            if (nk_begin(ctx, "LOGIN", nk_rect(0,0,500, 750), NK_WINDOW_NO_SCROLLBAR)) {
                ctx->style.window.spacing = nk_vec2(0,0);
                ctx->style.window.padding = nk_vec2(0,0);
                ctx->style.window.group_padding = nk_vec2(0,0);

                nk_layout_row_dynamic(ctx, 750, 1);
                createForm(*ctx, *font_18, *font_14);


                nk_end(ctx);
            }
        }

        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
    }

    //
    nk_glfw3_shutdown(&glfw);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
