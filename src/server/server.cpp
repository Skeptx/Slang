#include <iostream>
#include "../library/library.h"
using namespace std;

int main() {
        SlangLib servConnect('S');
		servConnect.init("acad.kutztown.edu", 46257);
        return 0;
}
