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
/***
 * this function is used to create a nuklear image handler
 * @param filename
 * @return
 */
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


/**
 * Error handler (used to write error messages to the console)
 * @param e
 * @param m
 */
static void error_callback(int e, const char* m){
    printf("Error %s \n", m);
}

int main() {
    POPUP_MESSAGE = malloc(1);


    struct device device; //device structure will be used by nuklear to handle the fonts and images
    struct nk_context *ctx, *ctx2; //nuklear frame context
    struct nk_colorf bg; //background color
    struct nk_font_atlas *atlas; //atlas font baker (this will be used by nuklear in order to prepare text fonts)
    /**
     * Declare two fonts handler font_14 and font_18 which will hold both fonts with sizes 14px and 18px
     */
    struct nk_font *font_14;
    struct nk_font *font_18;
    struct nk_font_config cfg = nk_font_config(0); //create  nuklear font config



    const void *image; int w, h;

    glfwSetErrorCallback(error_callback); // set the default GLFW3 callback error to error_callback method defined previously
    /**
     * Try to initiate GLFW3 in case of failure exit the program
     */
    if(!glfwInit())  {
        printf( "Error initiating GLFW3");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //set MAjor ctx GLFW version to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //set the Minor version to 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    window = glfwCreateWindow(500, 750, "Login", NULL, NULL); //create GLFW3 window 500x750 and title login exit in case of failure
    if(!window){
        printf("Error creating window");
        exit(-2);
    }
    glfwMakeContextCurrent(window); //make the window as the current glfw3 context

    glViewport(0, 0, 500, 1000); //set the OpenGL view port (it's used on 3D to specify the camera position of the X,Y axis and it's width and height
    glewExperimental = 1;
    /**
     * Try to inititate GLEW which helps to query and load opengl extensions such as GLFW3
     */
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);//inititater the nuklear GLFW3 context
    ctx2 = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);//inititater the nuklear GLFW3 context for second frame context
    cfg.oversample_h = 3; cfg.oversample_v = 2;
    nk_glfw3_font_stash_begin(&glfw, &atlas); //start preparing fonts

    //use atlas to load both fonts from the files roboto located in the media folder
    font_14 = nk_font_atlas_add_from_file(atlas, "./media/Roboto-Regular.ttf", 14.0f, 0);
    font_18 = nk_font_atlas_add_from_file( atlas, "./media/Roboto-Bold.ttf", 24.0f, 0);

    nk_glfw3_font_stash_end(&glfw); //end the fonts preparation
    nk_style_set_font(ctx, &font_14->handle); //set the default fonts to fonts_14

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f; //specify the default background RGBA colors

    glEnable(GL_TEXTURE_2D); //set open GL to 2D world
    /**
     * load file icons list
     */
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
    file_browser_init(&browser, &media); //initiate the file browser provided by nuklear

    /**
     * Keep lopping (rendering the window frames whenever it's not time to exit)
     */
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents(); //wait for events trigger
        nk_glfw3_new_frame(&glfw); //create new frame
        /**
         * check if the user flag LOGGED_IN  is active show the home page
         */
        if (LOGGED_IN){
            //create a non scrollable frame that cover 1200x1000
            if (nk_begin(ctx, "HOME", nk_rect(0,0,1200, 1000), NK_WINDOW_NO_SCROLLBAR)) {
                ctx->style.window.spacing = nk_vec2(0,0); //set spacing padding and group padding to 0
                ctx->style.window.padding = nk_vec2(0,0);
                ctx->style.window.group_padding = nk_vec2(0,0);
                //create a group of frames composed of two child each of them has 1000px height and dynamic width
                nk_layout_row_begin(ctx, NK_STATIC, 1000, 2);
                nk_layout_row_push(ctx, 150); //add a first child with 150 in width
                navigation(*ctx, *font_18, *font_14); //call the function that render the menu (side bar)
                nk_layout_row_push(ctx, 1050); //create the second child with 1050px in width in order to cover the rest of the screen
                createMainWindow(*ctx2, *font_18, *font_14, &browser, &media); //call the function that render the main window (encryption /decryption screen)

                nk_end(ctx);//end the frame context

            }

        }else {
            //if the user is not logged in create a frame (login form) with 500x750 dimensions
            if (nk_begin(ctx, "LOGIN", nk_rect(0,0,500, 750), NK_WINDOW_NO_SCROLLBAR)) {
                ctx->style.window.spacing = nk_vec2(0,0);
                ctx->style.window.padding = nk_vec2(0,0);
                ctx->style.window.group_padding = nk_vec2(0,0);

                nk_layout_row_dynamic(ctx, 750, 1);//create one column group
                createForm(*ctx, *font_18, *font_14); //call the createForm function to render the login/register frame


                nk_end(ctx);
            }
        }

        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER); //render the created frame
        glfwSwapBuffers(window); //refresh the window
    }

    //shutdown GLFW3 proicess
    nk_glfw3_shutdown(&glfw);
    glfwDestroyWindow(window); //destory the window
    glfwTerminate();//terminate the application
    return 0;
}
