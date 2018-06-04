/*
	tripartie Diffie-Hellman
*/
#include <iostream>
// #include <fstream>
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

std::string keygen()
{
	const char *aa = "15267802884793550383558706039165621050290089775961208824303765753922461897946";
	const char *ab = "9034493566019742339402378670461897774509967669562610788113215988055021632533";
	const char *ba = "644888581738283025171396578091639672120333224302184904896215738366765861164";
	const char *bb = "20532875081203448695448744255224543661959516361327385779878476709582931298750";

	initPairing(mcl::bn::CurveSNARK1);
	G2 Q(Fp2(aa, ab), Fp2(ba, bb));

	Fr s;
	s.setRand(rg);

    G2 PK;
    G2::mul(PK, Q, s);
    std::cout << PK << std::endl;

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
    // A random vote
    Fr v;
    v.setRand(rg);
    std::cout << v << std::endl;

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

    // C1.setIoMode(10);
    // C2.setIoMode(10);

    std::stringstream ssout;
    ssout << C1 << "," << C2;
    std::string ciphertexts = ssout.str();

    return ciphertexts;
}

void load(std::string pk)
{
    initPairing(mcl::bn::CurveSNARK1);
    G2 Q;
    std::stringstream ss;
    ss.str (pk);
    ss >> Q;
    // Q.setIoMode(10);
    std::cout << Q << std::endl;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("keygen", &keygen);
    function("load", &load);
    function("vote", &vote);
}

// void load(G2& Q, const std::string& fileName)
// {
	// std::ifstream ifs(fileName.c_str(), std::ios::binary);
	// ifs >> Q;
// }

// void vote(const std::string& pkFile, const std::string& file)
// {
	// initPairing(mcl::bn::CurveSNARK1);

	// const char *aa = "15267802884793550383558706039165621050290089775961208824303765753922461897946";
	// const char *ab = "9034493566019742339402378670461897774509967669562610788113215988055021632533";
	// const char *ba = "644888581738283025171396578091639672120333224302184904896215738366765861164";
	// const char *bb = "20532875081203448695448744255224543661959516361327385779878476709582931298750";

	// initPairing(mcl::bn::CurveSNARK1);
	// G2 Q(Fp2(aa, ab), Fp2(ba, bb));

	// Fr randomizer;
	// randomizer.setRand(rg);
    // // A random vote
	// Fr v;
	// v.setRand(rg);
    // std::cout << v << std::endl;

	// G2 PK;
	// load(PK, pkFile);
    // // std::cout << PK << std::endl;

    // G2 C1;
    // G2::mul(C1,Q,randomizer);
    // G2 C2A;
    // G2::mul(C2A,Q,v);
    // G2 C2B;
    // G2::mul(C2B,PK,randomizer);
    // G2 C2;
    // G2::add(C2,C2A,C2B);

    // C1.setIoMode(10);
    // C2.setIoMode(10);

	// {
        // std::string name = file;
        // std::ofstream ofs(name.c_str(), std::ios::binary);
        // ofs << C1 << std::endl;
        // ofs << C2 << std::endl;
	// }
// }

// int main()
	// try
// {
    // keygen();
    // return 1;
// } catch (std::exception& e) {
	// printf("ERR %s\n", e.what());
	// return 1;
// }

