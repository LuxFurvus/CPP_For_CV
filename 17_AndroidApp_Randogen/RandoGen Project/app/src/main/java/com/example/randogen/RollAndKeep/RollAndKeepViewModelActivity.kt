package com.example.randogen

import androidx.lifecycle.ViewModel

data class RollKeepInputState(
    val RollNum: Int = 2,
    val KeepNum: Int = 1,
    val BonusNum: Int = 0,
    val IsExplodeTensChecked: Boolean = true,
    val IsExplodeOnesChecked: Boolean = false
)

data class RollKeepUiState(
    var RollNum: Int = 2,
    var KeepNum: Int = 1,
    var BonusNum: Int = 0,
    var IsExplodeTensChecked: Boolean = true,
    var IsExplodeOnesChecked: Boolean = false,
    var SumText: String = "",
    var ResultsText: String = "",
    var IsInitialResult: Boolean = false,
    var LastRolledInputState: RollKeepInputState? = null
)
{
    fun ClearRollResult()
    {
        SumText = ""
        ResultsText = ""
        IsInitialResult = false
        LastRolledInputState = null
    }
}

class RollKeepActivityViewModel : ViewModel()
{
    val RollState: RollKeepUiState = RollKeepUiState()

    fun ClearRollState()
    {
        RollState.ClearRollResult()
    }
}