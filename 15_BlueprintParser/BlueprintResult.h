
auto MyBpExampleFunc(int InPin1, int InPin2)
{
    const auto FromInt1 = UKismetMathLibrary::Conv_IntToBool(InPin2);

    const auto AddResult1 = UKismetMathLibrary::Add_DoubleDouble(InPin1, 5);

    const auto AddResult2 = UKismetMathLibrary::Add_DoubleDouble(AddResult1, 5.5);
    const auto AddResult3 = UKismetMathLibrary::Add_DoubleDouble(AddResult2, 7.7);

    if (FromInt1)
    {
        UBPLib::LaunchProcess("lol", "");

        if (FromInt1)
        {
            return MakeTuple(AddResult3, 3.3);
        }
        else
        {
            const auto Array0 = UTraceUtilLibrary::GetAllChannels();
        }
    }
    else
    {
        TArray<int> Array1 {InPin1};
        for (int ArrayIndex = 0; ArrayIndex < Array1.Num(); ArrayIndex++)
        {
            const auto ArrayElement = Array1[ArrayIndex];

            const auto Result4 = UKismetMathLibrary::Conv_IntToString(ArrayElement);

            UBPLib::LaunchProcess(Result4, Result4);
        }

        return MakeTuple(AddResult3, 2.2);
    }
}

