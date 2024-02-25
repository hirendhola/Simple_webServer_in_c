#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

const char *html_content = "<html><body><h1>Hello, this is printed on the browser window!</h1></body></html>";

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failure!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }
    server_address.sin_port = htons(3000);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("SERVER IS LISTENING ON 127.0.0.1/3000...\n");

    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addrlen);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Connection accepted from %s:%d\n", client_ip, ntohs(client_address.sin_port));

        const char *http_response = "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/html\r\n"
                                    "\r\n";
        write(client_socket, http_response, strlen(http_response));
        write(client_socket, html_content, strlen(html_content));

        close(client_socket);
    }

    close(server_socket);

    return 0;
}
