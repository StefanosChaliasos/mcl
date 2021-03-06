#define CYBOZU_TEST_DISABLE_AUTO_RUN
#include <cybozu/test.hpp>
#include <cybozu/benchmark.hpp>
#include <cybozu/option.hpp>
#include <cybozu/xorshift.hpp>
#include <mcl/bn384.hpp>
#include <mcl/bn.hpp>
#include <mcl/lagrange.hpp>

using namespace mcl::bn384;

mcl::fp::Mode g_mode;

#include "bench.hpp"

void testLagrange()
{
	puts("testLagrange");
	const int k = 7;
	Fr c[k], x[k], y[k];
	for (size_t i = 0; i < k; i++) {
		c[i].setByCSPRNG();
		x[i].setByCSPRNG();
	}
	for (size_t i = 0; i < k; i++) {
		mcl::evaluatePolynomial(y[i], c, k, x[i]);
	}
	Fr s;
	mcl::LagrangeInterpolation(s, x, y, k);
	CYBOZU_TEST_EQUAL(s, c[0]);
}

void testCurve(const mcl::CurveParam& cp)
{
	initPairing(cp, g_mode);
	G1 P;
	G2 Q;
	mapToG1(P, 1);
	mapToG2(Q, 1);
	GT e1, e2;
	pairing(e1, P, Q);
	cybozu::XorShift rg;
	mpz_class a, b;
	Fr r;
	r.setRand(rg); a = r.getMpz();
	r.setRand(rg); b = r.getMpz();
	G1 aP;
	G2 bQ;
	G1::mul(aP, P, a);
	G2::mul(bQ, Q, b);
	pairing(e2, aP, bQ);
	GT::pow(e1, e1, a * b);
	CYBOZU_TEST_EQUAL(e1, e2);
	testBench(P, Q);
	testLagrange();
}

CYBOZU_TEST_AUTO(pairing)
{
	puts("BN254");
	// support 256-bit pairing
	testCurve(mcl::BN254);
	puts("BN381_1");
	testCurve(mcl::BN381_1);
	puts("BN381_2");
	testCurve(mcl::BN381_2);
	puts("BLS12_381");
	testCurve(mcl::BLS12_381);
	// Q is not on EcT, but bad order
	{
		const char *s = "1 18d3d8c085a5a5e7553c3a4eb628e88b8465bf4de2612e35a0a4eb018fb0c82e9698896031e62fd7633ffd824a859474 1dc6edfcf33e29575d4791faed8e7203832217423bf7f7fbf1f6b36625b12e7132c15fbc15562ce93362a322fb83dd0d 65836963b1f7b6959030ddfa15ab38ce056097e91dedffd996c1808624fa7e2644a77be606290aa555cda8481cfb3cb 1b77b708d3d4f65aeedf54b58393463a42f0dc5856baadb5ce608036baeca398c5d9e6b169473a8838098fd72fd28b50";
		G2 Q;
		CYBOZU_TEST_EXCEPTION(Q.setStr(s, 16), std::exception);
	}
}

int main(int argc, char *argv[])
	try
{
	cybozu::Option opt;
	std::string mode;
	opt.appendOpt(&mode, "auto", "m", ": mode(gmp/gmp_mont/llvm/llvm_mont/xbyak)");
	if (!opt.parse(argc, argv)) {
		opt.usage();
		return 1;
	}
	g_mode = mcl::fp::StrToMode(mode);
	return cybozu::test::autoRun.run(argc, argv);
} catch (std::exception& e) {
	printf("ERR %s\n", e.what());
	return 1;
}
