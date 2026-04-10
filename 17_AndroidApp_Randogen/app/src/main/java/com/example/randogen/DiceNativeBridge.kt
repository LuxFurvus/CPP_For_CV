package com.example.randogen

object DiceNativeBridge
{
    init
    {
        System.loadLibrary("randogen")
    }

    external fun GetRollResultsJNI(
        DiceCount: Int,
        SideNumber: Int,
        DifficultyValue: Int,
        WithCancel: Boolean,
        ResultClass: Class<RollResults>
    ): RollResults?

    external fun GetSuccessChancesJNI(
        DiceCount: Int,
        SideNumber: Int,
        DifficultyValue: Int,
        WithCancel: Boolean
    ): String
}