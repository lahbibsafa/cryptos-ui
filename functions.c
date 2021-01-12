
json_t* response_to_json(struct _u_response *response) {
    char response_body[response->binary_body_length + 1];
    o_strncpy(response_body, response->binary_body, response->binary_body_length);
    response_body[response->binary_body_length] = '\0';
    json_t *body;
    json_error_t errorJSon;
    printf("%s", response_body);
    body = json_loads(response_body, 0, &errorJSon);
    return body;
}
int login(char* username, char* password){
    struct _u_map headers, url_params, post_params, req_headers;

    u_map_init(&req_headers);
    u_map_put(&req_headers, "Content-Type", MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
    struct _u_request req;
    ulfius_init_request (&req);
    req.http_verb=o_strdup("POST");
    req.http_url=o_strdup("http://localhost:8080/api/login");
    u_map_init(&post_params);
    u_map_put(&post_params, "username", username);
    u_map_put(&post_params, "password", password);
    u_map_copy_into(req.map_header, &req_headers);

    u_map_copy_into(req.map_post_body, &post_params);
    req.timeout = 20;

    //send request
    struct _u_response response;
    ulfius_init_response(&response);
    int res = ulfius_send_http_request(&req, &response);
    if (res == U_OK) {
        // check server response
        json_t* body = response_to_json(&response);
        int success = json_integer_value(json_object_get(body, "success"));
        if (success == -1){
            POPUP_MESSAGE = json_string_value(json_object_get(body, "message"));
        }
        ulfius_clean_response(&response);
        CURRENT_USER = username;
        return success;
    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
        return -1;
    }
}

void signup(char* username , char* password){
    struct _u_map headers, url_params, post_params, req_headers;

    u_map_init(&req_headers);
    u_map_put(&req_headers, "Content-Type", MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
    struct _u_request req;
    ulfius_init_request (&req);
    req.http_verb=o_strdup("POST");
    req.http_url=o_strdup("http://localhost:8080/api/register");
    u_map_init(&post_params);
    u_map_put(&post_params, "username", username);
    u_map_put(&post_params, "password", password);

    u_map_copy_into(req.map_header, &req_headers);

    u_map_copy_into(req.map_post_body, &post_params);
    req.timeout = 20;

    //send request
    struct _u_response response;
    ulfius_init_response(&response);
    int res = ulfius_send_http_request(&req, &response);
    if (res == U_OK) {
        // check server response
        json_t* body = response_to_json(&response);
        int success = json_integer_value(json_object_get(body, "success"));
        POPUP_MESSAGE = json_string_value(json_object_get(body, "message"));

        ulfius_clean_response(&response);
    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
    }
}

void upload(char* localFile, char* fileName) {
    struct _u_map headers, url_params, post_params, req_headers;
    unsigned char buffer[4096 * 1024];

    u_map_init(&req_headers);
    //u_map_put(&req_headers, "Content-Type", MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
    u_map_put(&req_headers, "file_name", fileName);
    u_map_put(&req_headers, "username", CURRENT_USER);
    struct _u_request req;
    ulfius_init_request (&req);
    req.http_verb=o_strdup("POST");
    req.http_url=o_strdup("http://localhost:8080/api/upload");


    FILE *ptr;
    ptr = fopen(localFile,"rb");
    fread(buffer,sizeof(unsigned char), 4096 * 1024,ptr);
    fclose(ptr);
    /*char *body = (char*)malloc(4096 * 1024 * 2);
    int boundry = (int)time(NULL);
    sprintf(body, "---------------------------%d\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: application/octet-stream\r\n\r\n%s\r\n---------------------------%d-\r\n", boundry, fileName, fileName, buffer, boundry);
    r*/
    req.binary_body = buffer;
    req.binary_body_length = strlen(buffer);
    req.timeout = 20;
    u_map_copy_into(req.map_header, &req_headers);

    //send request
    struct _u_response response;
    ulfius_init_response(&response);
    int res = ulfius_send_http_request(&req, &response);
    if (res == U_OK) {
        char response_body[response.binary_body_length + 1];
        o_strncpy(response_body, response.binary_body, response.binary_body_length);
        response_body[response.binary_body_length] = '\0';
        printf("%s", response_body);
        ulfius_clean_response(&response);
    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
    }
}


json_t* list_files(){
        struct _u_map headers, url_params, post_params, req_headers;

        u_map_init(&req_headers);
        u_map_put(&req_headers, "Content-Type", MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
        u_map_put(&req_headers, "username", CURRENT_USER);

        struct _u_request req;
        ulfius_init_request (&req);
        req.http_verb=o_strdup("GET");
        req.http_url=o_strdup("http://localhost:8080/api/files");

        u_map_copy_into(req.map_header, &req_headers);

        req.timeout = 20;

        //send request
        struct _u_response response;
        ulfius_init_response(&response);
        int res = ulfius_send_http_request(&req, &response);
        if (res == U_OK) {
            // check server response
            json_t *body = response_to_json(&response);
            ulfius_clean_response(&response);
            return body;
        } else {
            printf("Error in http request: %d\n", res);
            ulfius_clean_response(&response);
        }
}


void downloadFile(char* fileName){
    struct _u_map headers, url_params, post_params, req_headers;

    u_map_init(&req_headers);
    u_map_put(&req_headers, "username", CURRENT_USER);
    u_map_put(&req_headers, "file", fileName);

    struct _u_request req;
    ulfius_init_request (&req);
    req.http_verb=o_strdup("GET");
    req.http_url=o_strdup("http://localhost:8080/api/file");

    u_map_copy_into(req.map_header, &req_headers);

    req.timeout = 20;

    //send request
    struct _u_response response;
    ulfius_init_response(&response);
    int res = ulfius_send_http_request(&req, &response);
    if (res == U_OK) {
        char* file_name = malloc(1024);
        sprintf(file_name, "./%s", fileName);
        FILE *f;
        f=fopen(file_name,"wb");
        fwrite(response_body, response.binary_body_length + 1, 1, f);
        fclose(f);

    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
    }
}