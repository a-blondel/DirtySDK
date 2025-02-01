#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../4.7.0/commudp.c"

void test_CommUDPSetConnID(void) {
    CommUDPRef ref;
    memset(&ref, 0, sizeof(CommUDPRef));
    
    const char *testStr = "192.168.1.90:3659:3659#$c0a8015a$c0a8015a-$c0a8015a$c0a8015a";
    _CommUDPSetConnID(&ref, testStr);

    printf("v4.7.0 - ConnID = 0x%08X\n", ref.connident);
    
    assert(ref.connident == 0x08F43358);
}

int main(void) {
    printf("Running tests...\n");
    
    test_CommUDPSetConnID();
    
    printf("All tests passed!\n");
    return 0;
}