#include "gabheader.h"

Req *request(const char *dstip, const int dstport) {
Req *req = malloc(reqsize);

if (req == NULL) {  
    perror("malloc for request packet");  
    return NULL;  
}  

req->vn = 4;  
req->cd = 1;  
req->dstport = htons(dstport);  
req->dstip = inet_addr(dstip);  
strncpy((char*)req->userid, USERNAME, 8);  

return req;

}

int main(int argc, char *argv[]) {
char *host;
int port, s;
struct sockaddr_in sock;
Req *req_packet;
Res res_packet;

if (argc < 3) {  
    fprintf(stderr, "Usage: %s <host_ip> <port>\n", argv[0]);  
    return -1;  
}  

host = argv[1];  
port = atoi(argv[2]);  

s = socket(AF_INET, SOCK_STREAM, 0);  
if (s < 0) {  
    perror("socket creation");  
    return -1;  
}  

sock.sin_family = AF_INET;  
sock.sin_port = htons(PROXYPORT);  
sock.sin_addr.s_addr = inet_addr(PROXY);  

if (connect(s, (struct sockaddr *)&sock, sizeof(sock)) < 0) {  
    perror("connect to proxy");  
    close(s);  
    return -1;  
}  
printf("Connected to Tor proxy at %s:%d\n", PROXY, PROXYPORT);  

req_packet = request(host, port);  
if (req_packet == NULL) {  
    close(s);  
    return -1;  
}  

if (write(s, req_packet, reqsize) < 0) {  
    perror("write request to proxy");  
    free(req_packet);  
    close(s);  
    return -1;  
}  

if (read(s, &res_packet, ressize) < 1) {  
    perror("read response from proxy");  
    free(req_packet);  
    close(s);  
    return -1;  
}  

if (res_packet.cd == 90) {  
    printf("Successfully connected through proxy to %s:%d\n", host, port);  
} else {  
    fprintf(stderr, "Proxy reported an error: code %d\n", res_packet.cd);  
    free(req_packet);  
    close(s);  
    return -1;  
}  
  
printf("Closing connection.\n");  
free(req_packet);  
close(s);  

return 0;

}
