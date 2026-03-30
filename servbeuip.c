#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9998
#define MAX_COUPLES 255
#define LOCALHOST "127.0.0.1"
#define LBUF 512

struct client_info {
    struct in_addr ip;
    char pseudo[LBUF];
};

struct client_info table[MAX_COUPLES];
int nb_couples = 0;

// Ajoute un utilisateur sans doublon
void ajouter_client(struct in_addr ip, const char* pseudo) {
    for (int i = 0; i < nb_couples; i++) {
        if (table[i].ip.s_addr == ip.s_addr) {
            strncpy(table[i].pseudo, pseudo, LBUF - 1);
            return; 
        }
    }
    if (nb_couples < MAX_COUPLES) {
        table[nb_couples].ip = ip;
        strncpy(table[nb_couples].pseudo, pseudo, LBUF - 1);
        table[nb_couples++].pseudo[LBUF - 1] = '\0';
#ifdef TRACE
        printf("[TRACE] Nouvel utilisateur : %s (%s)\n", pseudo, inet_ntoa(ip));
#endif
    }
}

// Initialisation du socket avec option broadcast
int init_server_socket() {
    int sid, opt = 1;
    if ((sid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) return -1;
    if (setsockopt(sid, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) return -1;
    
    struct sockaddr_in srv;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sid, (struct sockaddr *)&srv, sizeof(srv)) < 0) return -1;
    return sid;
}

// Envoi du broadcast d'identification (boucle 1-254)
void send_id_broadcast(int sid, const char* pseudo) {
    struct sockaddr_in bcast;
    char msg[LBUF];
    bcast.sin_family = AF_INET;
    bcast.sin_port = htons(PORT);
    snprintf(msg, LBUF, "1BEUIP%s", pseudo);
    
    for (int i = 1; i <= 254; i++) {
        char ip[16];
        snprintf(ip, sizeof(ip), "192.168.88.%d", i);
        bcast.sin_addr.s_addr = inet_addr(ip);
        sendto(sid, msg, strlen(msg), 0, (struct sockaddr *)&bcast, sizeof(bcast));
    }
}

// Gestion des messages privés (Code 4 -> 9)
void handle_private_msg(int sid, char* payload) {
    char *target = payload;
    char *text = payload + strlen(target) + 1;
    for (int i = 0; i < nb_couples; i++) {
        if (strcmp(table[i].pseudo, target) == 0) {
            char out[LBUF];
            int len = snprintf(out, LBUF, "9BEUIP%s", text);
            struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr = table[i].ip };
            sendto(sid, out, len, 0, (struct sockaddr *)&addr, sizeof(addr));
            return;
        }
    }
    printf("Erreur : %s introuvable.\n", target);
}

// Dispatcher selon le code du protocole
void dispatch_code(int sid, char code, char* payload, struct sockaddr_in cli, const char* mon_pseudo) {
    if (code == '1') {
        ajouter_client(cli.sin_addr, payload);
        char ar[LBUF];
        snprintf(ar, LBUF, "2BEUIP%s", mon_pseudo);
        sendto(sid, ar, strlen(ar), 0, (struct sockaddr *)&cli, sizeof(cli));
    } else if (code == '3') {
        for (int i = 0; i < nb_couples; i++) printf("- %s (%s)\n", table[i].pseudo, inet_ntoa(table[i].ip));
    } else if (code == '4') {
        handle_private_msg(sid, payload);
    } else if (code == '9') {
        printf("\n[Message] : %s\n", payload);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    int sid = init_server_socket();
    if (sid < 0) return 2;
    
    send_id_broadcast(sid, argv[1]);
    
    struct sockaddr_in cli;
    socklen_t ls = sizeof(cli);
    char buf[LBUF + 1];
    
    while (1) {
        int n = recvfrom(sid, buf, LBUF, 0, (struct sockaddr *)&cli, &ls);
        if (n < 6 || strncmp(buf + 1, "BEUIP", 5) != 0) continue;
        
        // Sécurité : codes 3, 4, 5 locaux uniquement 
        if ((buf[0] == '3' || buf[0] == '4' || buf[0] == '5') && cli.sin_addr.s_addr != inet_addr(LOCALHOST)) continue;
        
        buf[n] = '\0';
        dispatch_code(sid, buf[0], buf + 6, cli, argv[1]);
    }
    return 0;
}