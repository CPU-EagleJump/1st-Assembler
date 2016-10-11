#include <cstdint>

#include <string>
#include <vector>

using namespace std;

vector<string> split_string(const string &str, const string &delims)
{
    vector<string> elems;
    string el;

    for (char c : str) {
		if (delims.find(c) != string::npos) {
            if (!el.empty()) {
                elems.push_back(el);
                el.clear();
            }
        } else {
            el += c;
        }
    }
    if (!el.empty())
        elems.push_back(el);

    return elems;
}

// string of binary digits -> 32 bit word
uint32_t bin_to_word(string bin)
{
    uint32_t word = 0;

    for (int i = 0; i < 32; i++) {
        if (bin[31 - i] == '1')
			word |= UINT32_C(1) << i;
	}

	return word;
}

// num -> string of binary digits
// len must be < 32
string num_to_bin(uint32_t num, int len)
{
	string bin;

	for (int i = len - 1; i >= 0; i--) {
		bin += (num & (UINT32_C(1) << i)) ? '1' : '0';
	}

	return bin;
}

