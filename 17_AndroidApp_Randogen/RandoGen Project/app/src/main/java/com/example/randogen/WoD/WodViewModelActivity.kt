package com.example.randogen

import androidx.lifecycle.ViewModel

data class WodRollInputState(
    val DiceNumber: Int = 4,
    val Difficulty: Int = 6,
    val WithCancel: Boolean = true,
    val WithTenReroll: Boolean = false
)

data class RollUiState(
    var DiceNumber: Int = 4,
    var Difficulty: Int = 6,
    var WithCancel: Boolean = true,
    var WithTenReroll: Boolean = false,
    var ResultsText: String = "",
    var SuccessesText: String = "",
    var IsInitialResult: Boolean = false,
    var LastRolledInputState: WodRollInputState? = null
)
{
    fun ClearRollResult()
    {
        ResultsText = ""
        SuccessesText = ""
        IsInitialResult = false
        LastRolledInputState = null
    }
}

class MainActivityViewModel : ViewModel()
{
    val RollState: RollUiState = RollUiState()

    fun ClearRollState()
    {
        RollState.ClearRollResult()
    }
}