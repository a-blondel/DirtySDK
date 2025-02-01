#include <stdio.h>
#include <string.h>

typedef unsigned int            uint32_t;

uint32_t ip_to_hex(const char *ip_addr) {
    unsigned int a, b, c, d;
    sscanf(ip_addr, "%u.%u.%u.%u", &a, &b, &c, &d);
    return (a << 24) | (b << 16) | (c << 8) | d;
}

int main() {
    // Test IP conversion
    const char *ip = "192.168.1.90";
    uint32_t hex_ip = ip_to_hex(ip);
    printf("IP %s = 0x%08X\n", ip, hex_ip); // 0xC0A8015A

    return 0;
}