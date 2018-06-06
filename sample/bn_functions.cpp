/*
 * JS bindings for hat shuffle
 */
#include <iostream>
#include <cybozu/random_generator.hpp>
#include <string>
#include <sstream>
#include <mcl/bn256.hpp>
#include <cybozu/option.hpp>
#include <emscripten/bind.h>

static cybozu::RandomGenerator rg;

// const std::string skSuf = ".sk.txt";
// const std::string pkSuf = ".pk.txt";

using namespace emscripten;
using namespace mcl::bn256;

template<typename T>
void copyToVector(const val &typedArray, std::vector<T> &vec)
{
  unsigned int length = typedArray["length"].as<unsigned int>();
  val memory = val::module_property("buffer");

  vec.reserve(length);

  val memoryView = typedArray["constructor"].new_(memory, reinterpret_cast<uintptr_t>(vec.data()), length);

  memoryView.call<void>("set", typedArray);
}

std::string keygen()
{
	const char *aa = "15267802884793550383558706039165621050290089775961208824303765753922461897946";
	const char *ab = "9034493566019742339402378670461897774509967669562610788113215988055021632533";
	const char *ba = "644888581738283025171396578091639672120333224302184904896215738366765861164";
	const char *bb = "20532875081203448695448744255224543661959516361327385779878476709582931298750";

	initPairing(mcl::bn::CurveSNARK1);
	G2 Q(Fp2(aa, ab), Fp2(ba, bb));

    // secret key
	Fr s;
	s.setRand(rg);

    // public key
    G2 PK;
    G2::mul(PK, Q, s);

    std::stringstream ss;
    ss << s << "," << PK;
    std::string keys = ss.str();

    return keys;
}

std::string vote(std::string pk, std::string vote)
{
    initPairing(mcl::bn::CurveSNARK1);

    const char *aa = "15267802884793550383558706039165621050290089775961208824303765753922461897946";
    const char *ab = "9034493566019742339402378670461897774509967669562610788113215988055021632533";
    const char *ba = "644888581738283025171396578091639672120333224302184904896215738366765861164";
    const char *bb = "20532875081203448695448744255224543661959516361327385779878476709582931298750";

    initPairing(mcl::bn::CurveSNARK1);
    G2 Q(Fp2(aa, ab), Fp2(ba, bb));

    Fr randomizer;
    randomizer.setRand(rg);

    // vote
    Fr v;
    std::stringstream ssv;
    ssv.str (vote);
    ssv >> v;
    // std::cout << v << std::endl;

    G2 PK;
    std::stringstream sspk;
    sspk.str (pk);
    sspk >> PK;

    G2 C1;
    G2::mul(C1,Q,randomizer);
    G2 C2A;
    G2::mul(C2A,Q,v);
    G2 C2B;
    G2::mul(C2B,PK,randomizer);
    G2 C2;
    G2::add(C2,C2A,C2B);

    // affine coordinates
    C1.setIoMode(10);
    C2.setIoMode(10);

    std::stringstream ssout;
    ssout << C1 << "," << C2;
    std::string ciphertexts = ssout.str();

    return ciphertexts;
}

std::map<std::string, int> create_table()
{
    initPairing(mcl::bn::CurveSNARK1);

    const char *aa = "15267802884793550383558706039165621050290089775961208824303765753922461897946";
    const char *ab = "9034493566019742339402378670461897774509967669562610788113215988055021632533";
    const char *ba = "644888581738283025171396578091639672120333224302184904896215738366765861164";
    const char *bb = "20532875081203448695448744255224543661959516361327385779878476709582931298750";

    initPairing(mcl::bn::CurveSNARK1);
    G2 Q(Fp2(aa, ab), Fp2(ba, bb));

    std::map<std::string, int> table;
    for(int i = 1; i < 4; i++) {
        G2 elem;
        G2::mul(elem,Q,i);
        elem.setIoMode(10);
        std::stringstream ss;
        ss << elem;
        std::string s = ss.str();
        std::string key = ss.str();
        key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
        table[key] = i;
    }
    return table;
}

void decrypt(val typedArray)
{
    std::vector<std::string> ciphers;
    copyToVector(typedArray, ciphers);
    for (auto cipher : ciphers) {
        std::cout << cipher << std::endl;
    }
}

emscripten::val STLvector_to_array()
{
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(2);
    val heapU8 = val::module_property("HEAPU8");
    val asmJSHeapBuffer = heapU8["buffer"];

    return val::global("Int32Array").new_(asmJSHeapBuffer, reinterpret_cast<uint32_t>(vec.data()), vec.size());
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("keygen", &keygen);
    function("vote", &vote);
    function("decrypt", &decrypt);
    function("STLvector_to_array", &STLvector_to_array);
}
