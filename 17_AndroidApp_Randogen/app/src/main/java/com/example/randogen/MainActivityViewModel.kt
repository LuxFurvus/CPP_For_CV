package com.example.randogen

import androidx.lifecycle.ViewModel

data class RollUiState(
    var SumText: String = "",
    var ResultsText: String = "",
    var SuccessesText: String = "",
    var IsInitialResult: Boolean = false
)
{
    fun Clear()
    {
        SumText = ""
        ResultsText = ""
        SuccessesText = ""
        IsInitialResult = false
    }
}

class MainActivityViewModel : ViewModel()
{
    val RollState: RollUiState = RollUiState()

    fun ClearRollState()
    {
        RollState.Clear()
    }
}