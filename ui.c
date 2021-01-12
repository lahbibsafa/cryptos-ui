
/**
 * This function will create the login/register form
 * @param ctx : window context will be used by nuklear to create input and buttons
 * @param title : title fonts (Robots fontsize: 18)
 * @param body : body or normal text fonts (Robots fontsize: 14)
 */
void createForm(struct nk_context ctx, struct nk_font title, struct nk_font body) {
    static char username[256]; //username text
    static int usernameLength;
    static char password[256]; //password text
    static int passwordLength;

    /** fi the show login popup flag is active create our popup **/
    if (SHOW_LOGIN_POPUP){
        //set the theme to THEME_RED
        set_style(&ctx, THEME_RED);
        //create the popup with dimension 200x200 on the positition 150x275
        if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "LOGIN", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                           nk_rect(150, 275, 200, 200))){
            //create a static fragment 30 in height and 200px in width
            nk_layout_row_static(&ctx, 30, 200, 1);
            nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE)); //add the global POPUP_MESSAGE
            nk_layout_row_static(&ctx, 30, 200, 1);
            //create the login button
            if (nk_button_label(&ctx, "Login")) {
                SHOW_LOGIN_POPUP=0; //when the user click on the login desactivate the show_login_popup flag (hide the popup) and close it
                nk_popup_close(&ctx);
            }
            nk_popup_end(&ctx);
        }
    }
    /**
     * if the register popup flag is active do the same as the login popup
     */
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
    set_style(&ctx, THEME_WHITE); //change to white hteme
    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
        nk_layout_row_dynamic(&ctx, 30, 1);
        // nk_style_set_font(&ctx, &title.handle);
        nk_label(&ctx, "Create Account", NK_TEXT_CENTERED); //add a centered text as title (CREATE Account)
        nk_layout_row_static(&ctx, 500, 500, 1); //add a group of component 500x500px
        ctx.style.window.group_padding = nk_vec2(125,100);//Set the padding to 125 top and 100 left

        if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) { // column 2
            nk_layout_row_static(&ctx, 30, 250, 1);
            // nk_style_set_font(&ctx, &body.handle);

            nk_label(&ctx, "Username", NK_TEXT_ALIGN_CENTERED);
            nk_edit_string(&ctx,NK_EDIT_FIELD, username, &usernameLength, 256, nk_filter_ascii);
            nk_layout_row_static(&ctx, 30, 250, 1);

            nk_label(&ctx, "Password", NK_TEXT_ALIGN_CENTERED); //create the custom password field (show typing as *)
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
                //if the user click the login button call the login function
                if (login(username,password) != -1){
                    //success login
                    glfwSetWindowSize(window, 1200, 750); //resize the window
                    glViewport(0, 0, 1000, 750);
                    LOGGED_IN = 1; //mark the user as logged in

                } else {
                    SHOW_LOGIN_POPUP = 1; //show login popup (error)
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
    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) {
        //if the show dialog message is activev show the upload/skip popup
        if (SHOW_DIALOG_MESSAGE) {
            if (nk_popup_begin(&ctx, NK_POPUP_STATIC, "Message", NK_WINDOW_CLOSABLE|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE,
                               nk_rect(150, 275, 200, 450))){
                nk_layout_row_static(&ctx, 30, 200, 1);
                nk_text_wrap(&ctx, POPUP_MESSAGE, strlen(POPUP_MESSAGE));
                nk_layout_row_static(&ctx, 30, 200, 1);
                if (nk_button_label(&ctx, "Upload File")) {
                    //if the useer click on upload file
                    char* encryptedFile = str_replace(browser->file, browser->directory, "./"); //get the current file name
                    sprintf(encryptedFile, "%s.encrypted", encryptedFile); //add the .encrypted extension to the filename
                    upload(encryptedFile, str_replace(browser->file, browser->directory, "")); //uplaod the file to the server
                    SHOW_DIALOG_MESSAGE = 0; //close the modal
                    POPUP_MESSAGE = ""; //clear the moessage
                    nk_popup_close(&ctx);//close the popup
                }

                if (nk_button_label(&ctx, "Skip Upload")) {
                    SHOW_DIALOG_MESSAGE = 0;
                    POPUP_MESSAGE = "";
                    nk_popup_close(&ctx);
                }
                nk_popup_end(&ctx);
            }
        }

        //show the enter your password modal if the user clicked on a file
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
                    encrypt_file(inputFile, encryptedFile, encryptionKey);//encrypt the selected file when the user click on encrypt file
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
        if (SHOW_FILE_MANAGER_HOME) { //show the file management screen
            /**
             * this code was copied from the nuklear examples it render all the files and folders in the current working directory(starting by home)
             */
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
                                if (nk_button_image(&ctx, *icon)) {//if the user clicked on file
                                    strncpy(browser->file, browser->directory, MAX_PATH_LEN);
                                    n = strlen(browser->file);
                                    strncpy(browser->file + n, browser->files[fileIndex], MAX_PATH_LEN - n);
                                    SHOW_ENCRYPT_DECRYPT_PASS = 1;//show the encyrpt decrypt modal (type yopur pass for encryption)
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
//ssame as the home
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


    if (nk_group_begin(&ctx, "form_input", NK_WINDOW_NO_SCROLLBAR)) {
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
        //show enter yopur keyphrase to decrypt file

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
                    //if the user click on decrypt file
                    SHOW_ENCRYPT_DECRYPT_PASS=0; //hide the popup
                    downloadFile(CURRENT_FILE); //download the file (current file)
                    char* encryptedFile = malloc(1024); //create decrypted file path
                    sprintf(encryptedFile, "./storage/%s", CURRENT_FILE);
                    decrypt_file(CURRENT_FILE, encryptedFile, encryptionKey); //decrypt file

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
            files = list_files(); //if the user click on the refresh storage button, reload the remote files
        }
        nk_layout_row_dynamic(&ctx, 500, 1);
        nk_group_begin(&ctx, "Content", 0);
        {//render the file list
            if (json_is_array(files))  {
                for(int i = 0; i < json_array_size(files); i++){
                    json_t *data = json_array_get(files, i);
                    if (json_is_object(data)) {
                        struct nk_image *icon;
                        icon = &media->icons.default_file;
                        nk_layout_row_dynamic(&ctx, 60, 15);
                        if (nk_button_image(&ctx, *icon))
                        {//if the user select a file show the decyrpt modall and set the curretn file to the selected file
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



