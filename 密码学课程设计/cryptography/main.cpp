#include <iostream>
using namespace std;
const unsigned short sBox4[16] = {0xe, 0x4, 0xd, 0x1, 0x2, 0xf, 0xb, 0x8, 0x3, 0xa, 0x6, 0xc, 0x5, 0x9, 0x0, 0x7};
unsigned short sBox16[65536], spBox[65536];
const unsigned short position[17] = {0x0,
                        0x8000, 0x4000, 0x2000, 0x1000,
                         0x0800, 0x0400, 0x0200, 0x0100,
                         0x0080, 0x0040, 0x0020, 0x0010,
                         0x0008, 0x0004, 0x0002, 0x0001};
const unsigned short pBox[17] = {0x0,
                     0x8000, 0x0800, 0x0080, 0x0008,
                     0x4000, 0x0400, 0x0040, 0x0004,
                     0x2000, 0x0200, 0x0020, 0x0002,
                     0x1000, 0x0100, 0x0010, 0x0001};
int main() {
    for (int i = 0; i < 65536; ++i) {
        sBox16[i] =
                (sBox4[i >> 12] << 12) | (sBox4[(i >> 8) & 0xf] << 8) | (sBox4[(i >> 4) & 0xf] << 4) | sBox4[i & 0xf];
        spBox[i] = 0;
        for (int j = 1; j <= 16; ++j) {
            if (sBox16[i] & position[j]) spBox[i] |= pBox[j];
        }
    }
    for (int i = 0; i < 65536; ++i) {
        cout<<spBox[i]<<",";
        if(i % 10 == 0){
            cout<<endl;
        }
    }
    return 0;
}
