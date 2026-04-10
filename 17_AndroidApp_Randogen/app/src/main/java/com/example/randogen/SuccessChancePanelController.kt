package com.example.randogen

import android.widget.TextView

class SuccessChancePanelController(
    private val SuccessChanceTextView: TextView
) {
    fun Update(
        DiceNumberValue: Int,
        SideNumberValue: Int,
        DifficultyValue: Int,
        WithCancelValue: Boolean
    ) {
        val SuccessChanceText: String = DiceNativeBridge.GetSuccessChancesJNI(
            DiceNumberValue,
            SideNumberValue,
            DifficultyValue,
            WithCancelValue
        )

        SuccessChanceRender.UpdateSuccessChancePanel(
            SuccessChanceTextView = SuccessChanceTextView,
            SuccessChanceText = SuccessChanceText
        )
    }
}