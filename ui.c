
/**
 * This function will create the login/register form
 * @param ctx : window context will be used by nuklear to create input and buttons
 * @param title : title fonts (Robots fontsize: 18)
 * @param body : body or normal text fonts (Robots fontsize: 14)
 */
void createForm(struct nk_context ctx, struct nk_font title, struct nk_font body) {

    //frame creation
    static char username[256];
    static int usernameLength;
    static char password[256];
    static int passwordLength;

    if (SHOW_LOGIN_POPUP){
        set_style(&ctx, THEME_RED);
        if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "LOGIN", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                           nk_rect(150, 275, 200, 200))){
            nk_layout_row_static(&ctx, 30, 200, 1);
            nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
            nk_layout_row_static(&ctx, 30, 200, 1);
            if (nk_button_label(&ctx, "Login")) {
                SHOW_LOGIN_POPUP=0;
                nk_popup_close(&ctx);
            }
            nk_popup_end(&ctx);
        }
    }
    if (SHOW_REGISTER_POPUP){
        set_style(&ctx, THEME_RED);
        if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Register", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                           nk_rect(150, 275, 200, 200))){
            nk_layout_row_static(&ctx, 30, 200, 1);
            nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
            nk_layout_row_static(&ctx, 30, 200, 1);
            if (nk_button_label(&ctx, "Close")) {
                SHOW_REGISTER_POPUP=0;
                nk_popup_close(&ctx);
            }
            nk_popup_end(&ctx);
        }
    }
    set_style(&ctx, THEME_WHITE);
    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
        nk_layout_row_dynamic(&ctx, 30, 1);
        // nk_style_set_font(&ctx, &title.handle);
        nk_label(&ctx, "Create Account", NK_TEXT_CENTERED);
        nk_layout_row_static(&ctx, 500, 500, 1);
        ctx.style.window.group_padding = nk_vec2(125,100);

        if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
            nk_layout_row_static(&ctx, 30, 250, 1);
            // nk_style_set_font(&ctx, &body.handle);

            nk_label(&ctx, "Username", NK_TEXT_ALIGN_CENTERED);
            nk_edit_string(&ctx,NK_EDIT_FIELD, username, &usernameLength, 256, nk_filter_ascii);
            nk_layout_row_static(&ctx, 30, 250, 1);

            nk_label(&ctx, "Password", NK_TEXT_ALIGN_CENTERED);
            {
                int i = 0;
                int old_len = passwordLength;
                char buffer[256];
                for (i = 0; i < passwordLength; ++i) buffer[i] = '*';
                nk_layout_row_static(&ctx, 30, 250, 1);
                nk_edit_string(&ctx, NK_EDIT_FIELD, buffer, &passwordLength, 256, nk_filter_default);
                if (old_len < passwordLength)
                    memcpy(&password[old_len], &buffer[old_len], (nk_size)(passwordLength - old_len));
            }
            nk_layout_row_static(&ctx, 30, 250, 1);

            if (nk_button_label(&ctx, "Login")) {

                if (login(username,password) != -1){
                    //success login
                    glfwSetWindowSize(window, 1200, 750);
                    glViewport(0, 0, 1000, 750);
                    LOGGED_IN = 1;

                } else {
                    SHOW_LOGIN_POPUP = 1;
                }
            }
            nk_layout_row_static(&ctx, 30, 250, 1);

            if  (nk_button_label(&ctx, "Sign Up")){
                //success register
                signup(username,password);
                SHOW_LOGIN_POPUP = 1;

            }

            nk_group_end(&ctx);
        }
        nk_group_end(&ctx);
    }

}
/***
 * This function will draw the sidebar menu
  * @param ctx : window context will be used by nuklear to create input and buttons
 * @param title : title fonts (Robots fontsize: 18)
 * @param body : body or normal text fonts (Robots fontsize: 14)
 */
void navigation(struct nk_context ctx, struct nk_font title ,struct nk_font body){
    set_style(&ctx, THEME_WHITE);
    if (nk_group_begin(&ctx, "sidebarMenu", NK_WINDOW_NO_SCROLLBAR)) { // column 1

        nk_layout_row_static(&ctx, 30, 150, 1);
        if (nk_button_label(&ctx, "Home")) {
            CURRENT_SCREEN = 0;
        }
        if (nk_button_label(&ctx, "Encrypt")) {
            CURRENT_SCREEN = 1;
        }
        if (nk_button_label(&ctx, "Decrypter")) {
            CURRENT_SCREEN = 2;
        }
        if (nk_button_label(&ctx, "Logout")) {
            glfwSetWindowSize(window, 500, 750);
            glViewport(0, 0, 500, 750);
            LOGGED_IN = 0;
        }
        nk_group_end(&ctx);
    }
}


void draw_home(struct nk_context ctx, struct nk_font title, struct nk_font body, struct file_browser *browser, struct media *media){
    static char encryptionKey[256];
    static int encryptionKeyLength;
    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
        if (SHOW_DIALOG_MESSAGE) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Message", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 200, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Upload File")) {
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./");
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile);
                    upload(encryptedFile, str_replace(browser->file, browser->directory, ""));
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                if (nk_button_label(&ctx, "Skip Upload")) {
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        if (SHOW_ENCRYPT_DECRYPT_PASS) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Encrypt", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 450, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, "Enter your Encryption Key", strlen("Enter your Encryption Key"));
                {
                    int i = 0;
                    int old_len = encryptionKeyLength;
                    char buffer[256];
                    for (i = 0; i < encryptionKeyLength; ++i) buffer[i] = '*';
                    nk_layout_row_static(&ctx, 30, 250, 1);
                    nk_edit_string(&ctx, NK_EDIT_FIELD, buffer, &encryptionKeyLength, 256, nk_filter_default);
                    if (old_len < encryptionKeyLength)
                        memcpy(&encryptionKey[old_len], &buffer[old_len], (nk_size)(encryptionKeyLength - old_len));
                }
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Encrypt file")) {
                    SHOW_ENCRYPT_DECRYPT_PASS=0;
                    char* inputFile = browser->file;
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./");
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile);
                    encrypt_file(inputFile, encryptedFile, encryptionKey);
                    SHOW_DIALOG_MESSAGE = 1;
                    POPUP_MESSAGE = "Your file has been encrypted, do you want to upload it ?";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        nk_layout_row_dynamic(&ctx, 30, 1);
        // nk_style_set_font(&ctx, &title.handle);
        nk_label(&ctx, "Home", NK_TEXT_CENTERED);
        if (SHOW_FILE_MANAGER_HOME) {
           char *d = browser->directory;
           char *begin = d + 1;
           nk_layout_row_dynamic(&ctx, 25, 6);
           while (*d++) {
                if (*d == '/') {
                    *d = '\0';
                    if (nk_button_label(&ctx, begin)) {
                        *d++ = '/'; *d = '\0';
                        file_browser_reload_directory_content(browser, browser->directory);
                        break;
                    }
                    *d = '/';
                    begin = d + 1;
                }
           }
            nk_layout_row_dynamic(&ctx, 500, 1);
            nk_group_begin(&ctx, "Content", 0);
            {
                int index = -1;
                size_t i = 0, j = 0, k = 0;
                size_t rows = 0, cols = 0;
                size_t count = browser->dir_count + browser->file_count;

                cols = 15;
                rows = count / cols;
                for (i = 0; i <= rows; i += 1) {
                    {size_t n = j + cols;
                        nk_layout_row_dynamic(&ctx, 60, (int)cols);
                        for (; j < count && j < n; ++j) {
                            /* draw one row of icons */
                            if (j < browser->dir_count) {
                                /* draw and execute directory buttons */
                                if (nk_button_image(&ctx,media->icons.directory))
                                    index = (int)j;
                            } else {
                                /* draw and execute files buttons */
                                struct nk_image *icon;
                                size_t fileIndex = ((size_t)j - browser->dir_count);
                                icon = media_icon_for_file(media,browser->files[fileIndex]);
                                if (nk_button_image(&ctx, *icon)) {
                                    strncpy(browser->file, browser->directory, MAX_PATH_LEN);
                                    n = strlen(browser->file);
                                    strncpy(browser->file + n, browser->files[fileIndex], MAX_PATH_LEN - n);
                                    SHOW_ENCRYPT_DECRYPT_PASS = 1;
                                }
                            }
                        }}
                    {size_t n = k + cols;
                        nk_layout_row_dynamic(&ctx, 20, (int)cols);
                        for (; k < count && k < n; k++) {
                            /* draw one row of labels */
                            if (k < browser->dir_count) {
                                nk_label(&ctx, browser->directories[k], NK_TEXT_CENTERED);
                            } else {
                                size_t t = k-browser->dir_count;
                                nk_label(&ctx,browser->files[t],NK_TEXT_CENTERED);
                            }
                        }}
                }

                if (index != -1) {
                    size_t n = strlen(browser->directory);
                    strncpy(browser->directory + n, browser->directories[index], MAX_PATH_LEN - n);
                    n = strlen(browser->directory);
                    if (n < MAX_PATH_LEN - 1) {
                        browser->directory[n] = '/';
                        browser->directory[n+1] = '\0';
                    }
                    file_browser_reload_directory_content(browser, browser->directory);
                }
                nk_group_end(&ctx);
            }
        }
        nk_group_end(&ctx);

    }
}
void draw_encrypt(struct nk_context ctx, struct nk_font title, struct nk_font body, struct file_browser *browser, struct media *media){
    static char encryptionKey[256];
    static int encryptionKeyLength;
    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
        if (SHOW_DIALOG_MESSAGE) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Message", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 200, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Upload File")) {
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./");
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile);
                    printf(encryptedFile);
                    upload(encryptedFile, str_replace(browser->file, browser->directory, ""));
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                if (nk_button_label(&ctx, "Skip Upload")) {
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        if (SHOW_ENCRYPT_DECRYPT_PASS) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Encrypt", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 450, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, "Enter your Encryption Key", strlen("Enter your Encryption Key"));
                {
                    int i = 0;
                    int old_len = encryptionKeyLength;
                    char buffer[256];
                    for (i = 0; i < encryptionKeyLength; ++i) buffer[i] = '*';
                    nk_layout_row_static(&ctx, 30, 250, 1);
                    nk_edit_string(&ctx, NK_EDIT_FIELD, buffer, &encryptionKeyLength, 256, nk_filter_default);
                    if (old_len < encryptionKeyLength)
                        memcpy(&encryptionKey[old_len], &buffer[old_len], (nk_size)(encryptionKeyLength - old_len));
                }
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Encrypt file")) {
                    SHOW_ENCRYPT_DECRYPT_PASS=0;
                    char* inputFile = browser->file;
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./");
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile);
                    encrypt_file(inputFile, encryptedFile, encryptionKey);
                    SHOW_DIALOG_MESSAGE = 1;
                    POPUP_MESSAGE = "Your file has been encrypted, do you want to upload it ?";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        nk_layout_row_dynamic(&ctx, 30, 1);
        // nk_style_set_font(&ctx, &title.handle);
        nk_label(&ctx, "Home", NK_TEXT_CENTERED);
        if (SHOW_FILE_MANAGER_HOME) {
            char *d = browser->directory;
            char *begin = d + 1;
            nk_layout_row_dynamic(&ctx, 25, 6);
            while (*d++) {
                if (*d == '/') {
                    *d = '\0';
                    if (nk_button_label(&ctx, begin)) {
                        *d++ = '/'; *d = '\0';
                        file_browser_reload_directory_content(browser, browser->directory);
                        break;
                    }
                    *d = '/';
                    begin = d + 1;
                }
            }
            nk_layout_row_dynamic(&ctx, 500, 1);
            nk_group_begin(&ctx, "Content", 0);
            {
                int index = -1;
                size_t i = 0, j = 0, k = 0;
                size_t rows = 0, cols = 0;
                size_t count = browser->dir_count + browser->file_count;

                cols = 15;
                rows = count / cols;
                for (i = 0; i <= rows; i += 1) {
                    {size_t n = j + cols;
                        nk_layout_row_dynamic(&ctx, 60, (int)cols);
                        for (; j < count && j < n; ++j) {
                            /* draw one row of icons */
                            if (j < browser->dir_count) {
                                /* draw and execute directory buttons */
                                if (nk_button_image(&ctx,media->icons.directory))
                                    index = (int)j;
                            } else {
                                /* draw and execute files buttons */
                                struct nk_image *icon;
                                size_t fileIndex = ((size_t)j - browser->dir_count);
                                icon = media_icon_for_file(media,browser->files[fileIndex]);
                                if (nk_button_image(&ctx, *icon)) {
                                    strncpy(browser->file, browser->directory, MAX_PATH_LEN);
                                    n = strlen(browser->file);
                                    strncpy(browser->file + n, browser->files[fileIndex], MAX_PATH_LEN - n);
                                    SHOW_ENCRYPT_DECRYPT_PASS = 1;
                                }
                            }
                        }}
                    {size_t n = k + cols;
                        nk_layout_row_dynamic(&ctx, 20, (int)cols);
                        for (; k < count && k < n; k++) {
                            /* draw one row of labels */
                            if (k < browser->dir_count) {
                                nk_label(&ctx, browser->directories[k], NK_TEXT_CENTERED);
                            } else {
                                size_t t = k-browser->dir_count;
                                nk_label(&ctx,browser->files[t],NK_TEXT_CENTERED);
                            }
                        }}
                }

                if (index != -1) {
                    size_t n = strlen(browser->directory);
                    strncpy(browser->directory + n, browser->directories[index], MAX_PATH_LEN - n);
                    n = strlen(browser->directory);
                    if (n < MAX_PATH_LEN - 1) {
                        browser->directory[n] = '/';
                        browser->directory[n+1] = '\0';
                    }
                    file_browser_reload_directory_content(browser, browser->directory);
                }
                nk_group_end(&ctx);
            }
        }
        nk_group_end(&ctx);
    }
}
void draw_decrypt(struct nk_context ctx, struct nk_font title, struct nk_font body, struct file_browser *browser, struct media *media){
    static char encryptionKey[256];
    static int encryptionKeyLength;


    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
        if (SHOW_DIALOG_MESSAGE) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Message", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 200, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Upload File")) {
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./");
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile);
                    upload(encryptedFile, str_replace(browser->file, browser->directory, ""));
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                if (nk_button_label(&ctx, "Skip Upload")) {
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        if (SHOW_ENCRYPT_DECRYPT_PASS) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Decryption", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 450, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, "Enter your Decryption Key", strlen("Enter your Decryption Key"));
                {
                    int i = 0;
                    int old_len = encryptionKeyLength;
                    char buffer[256];
                    for (i = 0; i < encryptionKeyLength; ++i) buffer[i] = '*';
                    nk_layout_row_static(&ctx, 30, 250, 1);
                    nk_edit_string(&ctx, NK_EDIT_FIELD, buffer, &encryptionKeyLength, 256, nk_filter_default);
                    if (old_len < encryptionKeyLength)
                        memcpy(&encryptionKey[old_len], &buffer[old_len], (nk_size)(encryptionKeyLength - old_len));
                }
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Decrypt file")) {
                    SHOW_ENCRYPT_DECRYPT_PASS=0;
                    downloadFile(CURRENT_FILE);
                    char* encryptedFile = malloc(1024);
                    sprintf(encryptedFile, "./storage/%s", CURRENT_FILE);
                    decrypt_file(CURRENT_FILE, encryptedFile, encryptionKey);

                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }
        nk_layout_row_dynamic(&ctx, 30, 1);
        // nk_style_set_font(&ctx, &title.handle);
        nk_label(&ctx, "Decryption", NK_TEXT_CENTERED);
        nk_layout_row_static(&ctx, 30, 150, 1);
        if (nk_button_label(&ctx, "Refresh Storage")) {
            files = list_files();
        }
        nk_layout_row_dynamic(&ctx, 500, 1);
        nk_group_begin(&ctx, "Content", 0);
        {
            if (json_is_array(files))  {
                for(int i = 0; i < json_array_size(files); i++){
                    json_t *data = json_array_get(files, i);
                    if (json_is_object(data)) {
                        struct nk_image *icon;
                        icon = &media->icons.default_file;
                        nk_layout_row_dynamic(&ctx, 60, 15);
                        if (nk_button_image(&ctx, *icon))
                        {
                            CURRENT_FILE = json_string_value(json_object_get(data, "filePath"));
                            SHOW_ENCRYPT_DECRYPT_PASS = 1;
                        }
                        nk_layout_row_dynamic(&ctx, 20, 15);
                        nk_label(&ctx,json_string_value(json_object_get(data, "filePath")),NK_TEXT_CENTERED);
                    }
                }
            }

            nk_group_end(&ctx);
        }
        nk_group_end(&ctx);
    }
}
void createMainWindow(struct nk_context ctx, struct nk_font title, struct nk_font body, struct file_browser *browser, struct media *media) {

    set_style(&ctx, THEME_RED);
    switch(CURRENT_SCREEN){
        case 0:
            draw_home(ctx, title, body, browser, media);
            break;
        case 1:
            draw_encrypt(ctx, title, body, browser, media);
            break;
        case 2:
            draw_decrypt(ctx, title, body, browser, media);
            break;
        default:
            draw_home(ctx, title, body, browser, media);
            break;
    }
}



