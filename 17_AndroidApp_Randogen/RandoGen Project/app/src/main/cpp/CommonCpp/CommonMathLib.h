
struct CommonMathLib
{
    static long double BinomialCoefficient(
        int N, int K)
    {
        if (K < 0 || K > N)
        {
            return 0.0L;
        }

        if (K == 0 || K == N)
        {
            return 1.0L;
        }

        if (K > N - K)
        {
            K = N - K;
        }

        long double Result = 1.0L;

        for (int I = 1; I <= K; ++I)
        {
            Result *= static_cast<long double>(N - K + I);
            Result /= static_cast<long double>(I);
        }

        return Result;
    }
};
