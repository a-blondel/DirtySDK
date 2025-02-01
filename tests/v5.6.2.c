#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../5.6.2/commudp.c"
#include "../5.6.2/dirtylib.c"

void test_CommUDPSetConnID(void) {
    CommUDPRef ref;
    memset(&ref, 0, sizeof(CommUDPRef));
    
    const char *testStr = "192.168.1.90:3659:3659#$c0a8015a$c0a8015a-$c0a8015a$c0a8015a";
    _CommUDPSetConnID(&ref, testStr);

    printf("v5.6.2 - ConnID = 0x%08X\n", ref.connident);
    
    assert(ref.connident == 0xC6627546);
}

void test_NetHash(void) {
    uint32_t hash = NetHash("$c0a8015a$c0a8015a-$c0a8015a$c0a8015a");
    assert(hash == 0xC6627546);
}

int main(void) {
    printf("Running tests...\n");
    
    test_CommUDPSetConnID();
    test_NetHash();
    
    printf("All tests passed!\n");
    return 0;
}