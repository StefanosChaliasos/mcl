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

static cybozu::RandomGenerator rg;

// const std::string skSuf = ".sk.txt";
// const std::string pkSuf = ".pk.txt";

using namespace mcl::bn256;
using ElGamal_pair  = std::pair<G2, G2>;

std::vector<ElGamal_pair> deserialize_ciphertexts(std::vector<std::string> input)
{
    initPairing(mcl::bn::CurveSNARK1);
    std::vector<ElGamal_pair> ciphertexts;
    std::string template_g2 = "1 ";
    for(std::vector<std::string>::size_type i = 0; i != input.size(); i++) {
        std::string g2_c1 = "1 ";
        g2_c1 += input[i++];
        g2_c1 += " ";
        g2_c1 += input[i++];
        g2_c1 += " ";
        g2_c1 += input[i++];
        g2_c1 += " ";
        g2_c1 += input[i++];
        G2 c1;
        std::stringstream ssc1;
        ssc1.str (g2_c1);
        ssc1 >> c1;
        std::string g2_c2 = "1 ";
        g2_c2 += input[i++];
        g2_c2 += " ";
        g2_c2 += input[i++];
        g2_c2 += " ";
        g2_c2 += input[i++];
        g2_c2 += " ";
        g2_c2 += input[i];
        G2 c2;
        std::stringstream ssc2;
        ssc1.str (g2_c2);
        ssc1 >> c2;
        std::pair<G2, G2> c;
        c.first = c1;
        c.second = c2;
        ciphertexts.push_back(c);
    }
    return ciphertexts;
}

std::string keygen()
{
	initPairing(mcl::bn::CurveSNARK1);

	Fr s;
	s.setRand(rg);

    std::vector<std::string> string_ciphers;
    string_ciphers.push_back("14006839380044816494752215648803438187789324902305999407198630431916680922480");
    string_ciphers.push_back("18626310814369136914390349091923768460663454891890747399248176763961605862496");
    string_ciphers.push_back("20814897115674900664519012280858601557378403283685059155086607978151058852980");
    string_ciphers.push_back("12585219013454325208343046053274251180252815050732497121877121584534639676721");
    string_ciphers.push_back("17129136963355587524447714683585387393808623151535792235785655565852622757327");
    string_ciphers.push_back("4469626722779605055591026194224351093197067264193981275725454749187671973893");
    string_ciphers.push_back("3847313257757381980446158249216758334153128791795342027422287999526762206894");
    string_ciphers.push_back("19742408450608840817852620754699843989908994552839004506830299035760422111288");

    std::vector<ElGamal_pair> ciphertexts =
                                   deserialize_ciphertexts(string_ciphers);


    std::cout << "ok" << std::endl;
    std::cout << ciphertexts[0].first << std::endl;

    return "2131321321312,21312321321";
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

    std::stringstream ssv;
    ssv.str (vote);
    ssv >> v;

    // std::cout << v << std::endl;
    // v.setRand(rg);

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

    C1.setIoMode(10);
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
}

int main()
    try
{
    std::string s = keygen();
    std::size_t pos = s.find(",");
    std::string sk = s.substr(0, pos);
    std::string pk = s.substr(pos+1);
    // std::cout << sk << std::endl;
    // std::cout << pk << std::endl;
    // std::string cipher = vote(pk, "1");
    // std::cout << cipher << std::endl;
    // std::cout << pk << std::endl;
    return 1;
} catch (std::exception& e) {
    printf("ERR %s\n", e.what());
    return 1;
}

