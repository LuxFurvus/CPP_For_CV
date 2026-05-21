package com.example.randogen

import android.graphics.drawable.GradientDrawable
import android.view.View
import android.widget.TextView
import androidx.core.content.ContextCompat

data class RollResultPanelState(
    val ResultSumText: String = "",
    val ResultsDataText: String = "",
    val IsInitialRollResult: Boolean = false
)

class RollResultPanelController(
    private val PanelRoot: View)
{
    private var IsInitialRollResult: Boolean = false

    fun ShowResult(
        ResultSumText: String,
        ResultsDataText: String)
    {
        SetTexts(
            ResultSumText = ResultSumText,
            ResultsDataText = ResultsDataText
        )
    }

    fun RestoreState(
        State: RollResultPanelState)
    {
        SetTexts(
            ResultSumText = State.ResultSumText,
            ResultsDataText = State.ResultsDataText
        )

        if (State.IsInitialRollResult)
        {
            SetStrokeInitial()
            return
        }

        SetStrokeChanged()
    }

    fun ReadState(): RollResultPanelState
    {
        return RollResultPanelState(
            ResultSumText = GetResultSumTextView().text.toString(),
            ResultsDataText = GetResultsDataTextView().text.toString(),
            IsInitialRollResult = IsInitialRollResult
        )
    }

    fun Clear()
    {
        SetTexts(
            ResultSumText = "",
            ResultsDataText = ""
        )

        SetStrokeChanged()
    }

    fun HasVisibleResult(): Boolean
    {
        return GetResultSumTextView().text?.isNotEmpty() == true ||
                GetResultsDataTextView().text?.isNotEmpty() == true
    }

    fun SetStrokeInitial()
    {
        SetCardStrokeColor(
            StrokeColorResId = R.color.purple_500,
            StrokeWidth = 9
        )

        IsInitialRollResult = true
    }

    fun SetStrokeChanged()
    {
        SetCardStrokeColor(
            StrokeColorResId = R.color.stroke_color,
            StrokeWidth = 3
        )

        IsInitialRollResult = false
    }

    private fun SetTexts(
        ResultSumText: String,
        ResultsDataText: String)
    {
        GetResultSumTextView().text = ResultSumText
        GetResultsDataTextView().text = ResultsDataText
    }

    private fun SetCardStrokeColor(
        StrokeColorResId: Int,
        StrokeWidth: Int)
    {
        val ResultCardView: View =
            PanelRoot.findViewById(R.id.resultCardGeneral)

        val CardDrawable: GradientDrawable =
            ResultCardView.background.mutate() as GradientDrawable

        val StrokeColor: Int = ContextCompat.getColor(
            PanelRoot.context,
            StrokeColorResId
        )

        CardDrawable.setStroke(
            StrokeWidth,
            StrokeColor
        )
    }

    private fun GetResultSumTextView(): TextView
    {
        return PanelRoot.findViewById(R.id.valueResultSum)
    }

    private fun GetResultsDataTextView(): TextView
    {
        return PanelRoot.findViewById(R.id.valueResultsData)
    }
}