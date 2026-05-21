package com.example.randogen

import android.widget.TextView

class DndChancePanelController(
    private val SuccessChanceTextView: TextView)
{
    fun Update(
        DiceCounts: IntArray,
        SideNumbers: IntArray,
        BonusValue: Int)
    {
        if (DiceCounts.isEmpty() || SideNumbers.isEmpty())
        {
            SuccessChanceRender.UpdateSuccessChancePanel(
                SuccessChanceTextView = SuccessChanceTextView,
                SuccessChanceText = ""
            )
            return
        }

        if (DiceCounts.size != SideNumbers.size)
        {
            SuccessChanceRender.UpdateSuccessChancePanel(
                SuccessChanceTextView = SuccessChanceTextView,
                SuccessChanceText = ""
            )
            return
        }

        val SuccessChanceText: String =
            DndNativeBridge.GetDndMinAvgMaxValuesStringJNI(
                InDiceCounts = DiceCounts,
                InSideNumbers = SideNumbers,
                InRollModifier = BonusValue,
                ResultClass = DndRollResults::class.java
            ) ?: ""

        SuccessChanceRender.UpdateSuccessChancePanel(
            SuccessChanceTextView = SuccessChanceTextView,
            SuccessChanceText = SuccessChanceText
        )
    }
}