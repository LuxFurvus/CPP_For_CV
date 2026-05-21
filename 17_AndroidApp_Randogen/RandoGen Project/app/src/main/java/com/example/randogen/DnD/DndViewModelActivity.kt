package com.example.randogen.DnD

import androidx.lifecycle.ViewModel

data class DndRollInputState(
    val DiceNum1: Int = 1,
    val DiceNum2: Int = 1,
    val DiceNum3: Int = 1,
    val SideNum1: Int = 4,
    val SideNum2: Int = 6,
    val SideNum3: Int = 8,
    val BonusNum: Int = 0,
    val IsDiceType1Checked: Boolean = true,
    val IsDiceType2Checked: Boolean = false,
    val IsDiceType3Checked: Boolean = false
)

data class DndUiState(
    var DiceNum1: Int = 1,
    var DiceNum2: Int = 1,
    var DiceNum3: Int = 1,
    var SideNum1: Int = 4,
    var SideNum2: Int = 6,
    var SideNum3: Int = 8,
    var BonusNum: Int = 0,
    var IsDiceType1Checked: Boolean = true,
    var IsDiceType2Checked: Boolean = false,
    var IsDiceType3Checked: Boolean = false,
    var SumText: String = "",
    var ResultsText: String = "",
    var IsInitialResult: Boolean = false,
    var LastRolledInputState: DndRollInputState? = null
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

class DndActivityViewModel : ViewModel()
{
    val RollState: DndUiState = DndUiState()

    fun ClearRollState()
    {
        RollState.ClearRollResult()
    }
}