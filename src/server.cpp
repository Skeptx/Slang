#include <iostream>
#include "libslang.h"
using namespace std;

void messageHandler(char *message) {
}

int main() {
        SlangLib servConnect('S', messageHandler);
		servConnect.init("acad.kutztown.edu", 46257);
        return 0;
}
