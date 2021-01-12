#ifndef INTERFACE_CRYPTOS_CLOUD_GLOBAL_H
#define INTERFACE_CRYPTOS_CLOUD_GLOBAL_H
#include <GLFW/glfw3.h>

extern int SHOW_LOGIN_POPUP;
extern int SHOW_REGISTER_POPUP;
extern int LOGGED_IN;
extern int CURRENT_SCREEN;
extern char* POPUP_MESSAGE;
extern int SHOW_FILE_MANAGER_HOME;
extern int SHOW_ENCRYPT_DECRYPT_PASS;
extern int SHOW_DIALOG_MESSAGE;
extern char* CURRENT_USER;
extern char* CURRENT_FILE;
extern json_t* files;
extern GLFWwindow *window;
extern struct nk_glfw glfw;
extern struct file_browser browser;
extern struct media media;
#endif //INTERFACE_CRYPTOS_CLOUD_GLOBAL_H
