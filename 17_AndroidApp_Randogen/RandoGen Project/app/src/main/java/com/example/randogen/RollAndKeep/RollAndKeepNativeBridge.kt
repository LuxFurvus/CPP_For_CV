package com.example.randogen

object RollAndKeepNativeBridge
{
    init
    {
        System.loadLibrary("randogen")
    }

    external fun GetRollAndKeepResultsJNI(
        InRollNum: Int,
        InKeepNum: Int,
        InBonus: Int,
        InExplodeTens: Boolean,
        InExplodeOnes: Boolean,
        ResultClass: Class<RollResults>
    ): RollResults?

    external fun StartRollAndKeepChancesAsyncJNI(
        InRequestId: Long,
        InRollNum: Int,
        InKeepNum: Int,
        InBonus: Int,
        InExplodeTens: Boolean,
        InExplodeOnes: Boolean,
        InCallback: RollAndKeepChanceCallback
    )
}