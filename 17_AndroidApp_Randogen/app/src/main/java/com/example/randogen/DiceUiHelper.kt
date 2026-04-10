package com.example.randogen

import com.google.android.material.textfield.TextInputEditText

object DiceUiHelper
{

    private fun ReadIntOrDefault(
        EditTextObj: TextInputEditText,
        DefaultValue: Int): Int
    {
        return EditTextObj.text
            ?.toString()
            ?.trim()
            ?.toIntOrNull()
            ?: DefaultValue
    }

    fun ClampValue(
        Value: Int,
        MinValue: Int,
        MaxValue: Int): Int
    {
        if (Value < MinValue)
        {
            return MinValue
        }

        if (Value > MaxValue)
        {
            return MaxValue
        }

        return Value
    }

    fun GetClampedValue(
        InEditTextObj: TextInputEditText,
        InMinValue: Int,
        InMaxValue: Int): Int
    {
        val RawDiceNumberValue: Int = DiceUiHelper.ReadIntOrDefault(
            InEditTextObj,
            InMinValue
        )

        val ClampedDiceNumberValue: Int = DiceUiHelper.ClampValue(
            RawDiceNumberValue,
            InMinValue,
            InMaxValue
        )

        return ClampedDiceNumberValue
    }

    fun SetEditTextValueIfNeeded(
        EditTextObj: TextInputEditText,
        Value: Int,
        SetProgrammaticTextUpdateState: (Boolean) -> Unit)
    {
        val NewText: String = Value.toString()
        val CurrentText: String = EditTextObj.text?.toString().orEmpty()

        if (CurrentText == NewText)
        {
            return
        }

        SetProgrammaticTextUpdateState(true)
        EditTextObj.setText(NewText)
        EditTextObj.setSelection(NewText.length)
        SetProgrammaticTextUpdateState(false)
    }

    fun GetSuccessNumText(
        inSuccessNum: Int,
        inWithCancel: Boolean): String
    {
        val SuccessesText: String = if (inWithCancel && inSuccessNum < 0)
        {
            "BOTCH (${inSuccessNum})"
        }
        else
        {
            inSuccessNum.toString()
        }
        return SuccessesText
    }

    fun GetGetRollSequenceText(
        inSortedRolls: IntArray) : String
    {
        val SortedRollsText: String =
            if (inSortedRolls.isEmpty())
            {
                "--"
            }
            else
            {
                inSortedRolls.joinToString(", ")
            }
        return SortedRollsText
    }
}