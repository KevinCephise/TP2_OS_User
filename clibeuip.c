#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9998
#define LOCALHOST "127.0.0.1"
#define LBUF 512

// Prépare le message binaire pour le MP (Code 4) 
int prepare_mp(char* buf, char* pseudo, char* text) {
    buf[0] = '4';
    memcpy(buf + 1, "BEUIP", 5);
    strcpy(buf + 6, pseudo);
    int p_len = strlen(pseudo);
    buf[6 + p_len] = '\0'; // Séparateur 
    strcpy(buf + 6 + p_len + 1, text);
    return 6 + p_len + 1 + strlen(text);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    
    int sid = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in srv = { .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr.s_addr = inet_addr(LOCALHOST) };
    char buf[LBUF];
    int len = 0;

    if (strcmp(argv[1], "liste") == 0) {
        len = snprintf(buf, LBUF, "3BEUIP");
    } else if (strcmp(argv[1], "mp") == 0 && argc >= 4) {
        len = prepare_mp(buf, argv[2], argv[3]);
    } else if (strcmp(argv[1], "all") == 0 && argc >= 3) {
        len = snprintf(buf, LBUF, "5BEUIP%s", argv[2]);
    } else {
        return 3;
    }

    sendto(sid, buf, len, 0, (struct sockaddr *)&srv, sizeof(srv));
    close(sid);
    return 0;
}