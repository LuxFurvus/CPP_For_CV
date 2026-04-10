package com.example.randogen

import android.view.View
import android.widget.TextView
import android.graphics.drawable.GradientDrawable
import androidx.core.content.ContextCompat


class RollResultPanelController(
    private val PanelRoot: View)
{

    private var IsInitialRollResult: Boolean = false

    fun ShowResult(
        Result: RollResults,
        WithCancelValue: Boolean)
    {
        val SumText: String = Result.GetRollSum().toString()
        val ResultsText: String = DiceUiHelper.GetGetRollSequenceText(
            Result.GetRollSequence()
        )
        val SuccessesText: String = DiceUiHelper.GetSuccessNumText(
            Result.GetSuccessNum(),
            WithCancelValue
        )

        SetTexts(
            SumText = SumText,
            ResultsText = ResultsText,
            SuccessesText = SuccessesText
        )
    }

    fun RestoreState(
        SumText: String,
        ResultsText: String,
        SuccessesText: String,
        inIsInitialRoll: Boolean)
    {
        SetTexts(
            SumText = SumText,
            ResultsText = ResultsText,
            SuccessesText = SuccessesText
        )
        if (inIsInitialRoll)
        {
            SetStrokeInitial()
        }
        else
        {
            SetStrokeChanged()
        }
    }

    fun Clear()
    {
        SetTexts(
            SumText = "",
            ResultsText = "",
            SuccessesText = ""
        )
    }

    fun ReadState(): Triple<String, String, String>
    {
        return Triple(
            GetSumTextView().text.toString(),
            GetResultsTextView().text.toString(),
            GetSuccessesTextView().text.toString()
        )
    }

    fun SetStrokeInitial()
    {
        SetCardStrokeColor(R.color.purple_500, 9)
        IsInitialRollResult = true
    }

    fun SetStrokeChanged()
    {
        SetCardStrokeColor(R.color.stroke_color, 3)
        IsInitialRollResult = false
    }

    fun GetIsInitialRollResult(): Boolean
    {
        return IsInitialRollResult
    }

    private fun SetCardStrokeColor(
        inStrokeColor: Int,
        inStrokeWidth: Int)
    {
        val ResultCardView: View
            = PanelRoot.findViewById(R.id.resultCard)

        val CardDrawable: GradientDrawable =
            (ResultCardView.background.mutate() as GradientDrawable)

        val StrokeColor: Int
            = ContextCompat.getColor(
                PanelRoot.context, inStrokeColor)

        CardDrawable.setStroke(
            inStrokeWidth, StrokeColor)
    }

    private fun SetTexts(
        SumText: String,
        ResultsText: String,
        SuccessesText: String)
    {
        GetSumTextView().text = SumText
        GetResultsTextView().text = ResultsText
        GetSuccessesTextView().text = SuccessesText
    }

    private fun GetSumTextView(): TextView {
        return PanelRoot.findViewById(R.id.valueSum)
    }

    private fun GetResultsTextView(): TextView {
        return PanelRoot.findViewById(R.id.valueResults)
    }

    private fun GetSuccessesTextView(): TextView {
        return PanelRoot.findViewById(R.id.valueSuccesses)
    }
}