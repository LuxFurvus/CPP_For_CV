package com.example.randogen

import android.widget.TextView
import com.example.randogen.WoD.WodNativeBridge

class WodChancePanelController(
    private val SuccessChanceTextView: TextView)
{
    fun Update(
        DiceNumberValue: Int,
        DifficultyValue: Int,
        WithCancelValue: Boolean,
        WithTenReroll: Boolean
    ) {
        val SuccessChanceText: String = WodNativeBridge.GetSuccessChancesJNI(
            DiceNumberValue,
            DifficultyValue,
            WithCancelValue,
            WithTenReroll
        )

        SuccessChanceRender.UpdateSuccessChancePanel(
            SuccessChanceTextView = SuccessChanceTextView,
            SuccessChanceText = SuccessChanceText
        )
    }
}