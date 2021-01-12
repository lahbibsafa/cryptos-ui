/**
 * this function allow the transformation of a binary response (received from the ulfius API) to a JSON object
 * @param response: ulifus response structure
 * @return json object (usually will be : {"success": 1/-1, message: "Error message"}
 */
json_t* response_to_json(struct _u_response *response) {
    char response_body[response->binary_body_length + 1]; // create a string (this will be our json string) with the size equals to the binary body response and an additional character \0
    o_strncpy(response_body, response->binary_body, response->binary_body_length); //copy the binary content into our string variable
    response_body[response->binary_body_length] = '\0'; //add the end of string character
    json_t *body;
    json_error_t errorJSon;
    body = json_loads(response_body, 0, &errorJSon); //convert the json string response into a json object, the errorJSon will contain the error message in case of failure
    return body;
}
/**
 * this function will handle the login request and return the result of authentication
 * @param username
 * @param password
 * @return 1 in success and -1 in case of failure
 */
int login(char* username, char* password){
    struct _u_map post_params, req_headers; //create two map structure, post params which represent the data to be posted to the server and the req_headers which represent the headers of the request
/** Create Request **/
    u_map_init(&req_headers); //initiate the req_headers dict
    u_map_put(&req_headers, "Content-Type", MHD_HTTP_POST_ENCODING_FORM_URLENCODED); //set the request content type to URLENCODED as we gonna use post_params
    struct _u_request req; // create our request object
    ulfius_init_request (&req); //initiate the request
    req.http_verb=o_strdup("POST"); //set the request method to POST
    req.http_url=o_strdup("http://localhost:8080/api/login"); // specify our Backend endpoint (login router)
    u_map_init(&post_params); // initiate the post parameters dict
    u_map_put(&post_params, "username", username); //append the username to the post params
    u_map_put(&post_params, "password", password); // append the password to the user params
    u_map_copy_into(req.map_header, &req_headers);  //add the headers to the request

    u_map_copy_into(req.map_post_body, &post_params); //add the post params to the request
    req.timeout = 20; //set the request timeout to 20 seconds
    /** Execute Request **/
    struct _u_response response;
    ulfius_init_response(&response); //inititate response object
    int res = ulfius_send_http_request(&req, &response); //execute the request
    if (res == U_OK) {
        // check server response is okay
        json_t* body = response_to_json(&response); //convert the response to json
        int success = json_integer_value(json_object_get(body, "success")); //get the success value
        if (success == -1){ //if the success is -1 it means there is no user registered with the provided login password
            POPUP_MESSAGE = json_string_value(json_object_get(body, "message")); //add the message to the flag POPUP_MESSAGE which will be displayed later on the MESSAGE_POPUP
        } else {
            CURRENT_USER = username; // in case of success set the CURRENT_USER flag to the username that successfully logged in
            //TODO - add the json web token
        }
        ulfius_clean_response(&response); //clear the response to free the memory space
        return success; //return the success result
    } else {
        //can't send the request
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
        return -1; //return failure status
    }
}

/**
 * this function will handle the registration request and return the result of registration
 * @param username
 * @param password
 * @return 1 in success and -1 in case of failure
 */
void signup(char* username , char* password){
    struct _u_map post_params, req_headers;

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

/**
 * This function will handle the file uploading process (assuming the localFile is the path of an already encrypted file)
 * @param localFile
 * @param fileName
 */
void upload(char* localFile, char* fileName) {
    struct _u_map  req_headers;
    unsigned char buffer[4096 * 1024]; //set the buffer size (read 4MB per chunk of file)

    u_map_init(&req_headers);//intitiate our header
    u_map_put(&req_headers, "file_name", fileName); //add the filename we will be, use to save the file in the server
    u_map_put(&req_headers, "username", CURRENT_USER); //set the header username to the current active user
    struct _u_request req;
    ulfius_init_request (&req);
    req.http_verb=o_strdup("POST");
    req.http_url=o_strdup("http://localhost:8080/api/upload");


    FILE *ptr; //create a file descriptor pointer
    ptr = fopen(localFile,"rb"); // open the file in read binary mode
    fread(buffer,sizeof(unsigned char), 4096 * 1024,ptr); //read from the file 4MB and save it to the buffer
    fclose(ptr); //close the file descriptor

    req.binary_body = buffer; //set the binary body request to the content of the file (post the content to the server)
    req.binary_body_length = strlen(buffer); //set the content length to the size of the file
    req.timeout = 20; //set the default timeout to 20 seconds
    u_map_copy_into(req.map_header, &req_headers);

    //send request
    struct _u_response response;
    ulfius_init_response(&response);
    int res = ulfius_send_http_request(&req, &response);
    if (res == U_OK) {
        ulfius_clean_response(&response);
    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
    }
}

/**
 * this function will  be used to list the encrypted files on the server that belongs to the current user
 * @return json array
 */
json_t* list_files(){
        struct _u_map req_headers;

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
            json_t *body = response_to_json(&response); //convert response to json
            ulfius_clean_response(&response); //clear response
            return body;
        } else {
            printf("Error in http request: %d\n", res);
            ulfius_clean_response(&response);
        }
}

/**
 * this function will be used to download the file we want to decrypt from the remote host
 * @param fileName
 */
void downloadFile(char* fileName){
    struct _u_map req_headers;

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
        char* file_name = malloc(1024); //create a filename variable that will contain the local file path
        sprintf(file_name, "./%s", fileName); //download the file in the current directory
        FILE *f; //Create file descriptor
        f=fopen(file_name,"wb"); //open the file in write binary mode
        fwrite(response.binary_body, response.binary_body_length, 1, f); //write the binary body response into the file
        fclose(f); //close the file descriptor
    } else {
        printf("Error in http request: %d\n", res);
        ulfius_clean_response(&response);
    }
}