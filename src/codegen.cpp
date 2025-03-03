#include <codegen.h>
#include <iomanip>

using namespace std;

inline void outhex(ofstream &file, NUM_TYPE w, NUM_TYPE val) {
    file << setw(1ll + ((w-1)/4)) << setfill('0') << hex << val;
}

/*
 * Logisim v3.0 Hex Addressed format for loading data into memory.
 */
void v3HexAddressed(const std::string &fname, const NUM_TYPE key_w, const NUM_TYPE val_w, const map<NUM_TYPE, NUM_TYPE> &out, const NUM_TYPE default_val) {
    std::ofstream file(fname, ios_base::out);
    file << "v3.0 hex words addressed";
    NUM_TYPE addr = 0;

    for (auto [key, val] : out) {
        while (addr != key) {
            if (addr % 8 == 0) {
                file << '\n';
                outhex(file, key_w, addr);
                file << ": ";
            }

            outhex(file, val_w, default_val);
            file << ' ';
            addr++;
        }

        if (addr % 8 == 0) {
            file << '\n';
            outhex(file, key_w, key);
            file << ": ";
        }

        outhex(file, val_w, val);
        file << ' ';

        addr++;
    }
}
