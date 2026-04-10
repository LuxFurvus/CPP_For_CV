package com.example.randogen

import android.view.View
import android.widget.CheckBox
import androidx.core.widget.doAfterTextChanged
import com.google.android.material.textfield.TextInputEditText

class DiceInputPanelController(
    private val PanelRoot: View)
{
    private var IsProgrammaticTextUpdate: Boolean = false

    fun Bind(
        OnInputChanged: () -> Unit)
    {
        GetDiceNumberInput().doAfterTextChanged {
            if (!IsProgrammaticTextUpdate)
            {
                return@doAfterTextChanged
            }
            OnInputChanged()
        }

        GetSideNumberInput().doAfterTextChanged {
            if (!IsProgrammaticTextUpdate)
            {
                return@doAfterTextChanged
            }
            OnInputChanged()
        }

        GetDifficultyInput().doAfterTextChanged {
            if (!IsProgrammaticTextUpdate)
            {
                return@doAfterTextChanged
            }
            OnInputChanged()
        }

        GetCancelCheckbox().setOnCheckedChangeListener { _, _ ->
            OnInputChanged()
        }

        GetDiceNumberInput().setOnFocusChangeListener { _, HasFocus ->
            if (HasFocus)
            {
                return@setOnFocusChangeListener
            }

            ClampDiceNumber()
            OnInputChanged()
        }

        GetSideNumberInput().setOnFocusChangeListener { _, HasFocus ->
            if (HasFocus)
            {
                return@setOnFocusChangeListener
            }

            ClampSideNumber()
            ClampDifficulty()
            OnInputChanged()
        }

        GetDifficultyInput().setOnFocusChangeListener { _, HasFocus ->
            if (HasFocus)
            {
                return@setOnFocusChangeListener
            }

            ClampDifficulty()
            OnInputChanged()
        }

        GetUpDiceButton().setOnClickListener {
            StepDiceNumber(+1)
            OnInputChanged()
        }

        GetDownDiceButton().setOnClickListener {
            StepDiceNumber(-1)
            OnInputChanged()
        }

        GetUpSideButton().setOnClickListener {
            StepSideNumber(+1)
            OnInputChanged()
        }

        GetDownSideButton().setOnClickListener {
            StepSideNumber(-1)
            OnInputChanged()
        }

        GetUpDifficultyButton().setOnClickListener {
            StepDifficulty(+1)
            OnInputChanged()
        }

        GetDownDifficultyButton().setOnClickListener {
            StepDifficulty(-1)
            OnInputChanged()
        }
    }

    fun ClampAll()
    {
        ClampDiceNumber()
        ClampSideNumber()
        ClampDifficulty()
    }

    fun GetDiceNumberValue(): Int
    {
        return DiceUiHelper.GetClampedValue(
            GetDiceNumberInput(),
            InputLimits.MinDiceNumber,
            InputLimits.MaxDiceNumber
        )
    }

    fun GetSideNumberValue(): Int
    {
        return DiceUiHelper.GetClampedValue(
            GetSideNumberInput(),
            InputLimits.MinSideNumber,
            InputLimits.MaxSideNumber)
    }

    fun GetDifficultyValue(): Int
    {
        return DiceUiHelper.GetClampedValue(
            GetDifficultyInput(),
            InputLimits.MinDifficulty,
            GetSideNumberValue())
    }

    fun IsCancelEnabled(): Boolean
    {
        return GetCancelCheckbox().isChecked
    }

    private fun StepDiceNumber(
        DeltaValue: Int)
    {
        val NewDiceNumber: Int = DiceUiHelper.ClampValue(
            GetDiceNumberValue() + DeltaValue,
            InputLimits.MinDiceNumber,
            InputLimits.MaxDiceNumber
        )

        SetInputValue(
            GetDiceNumberInput(),
            NewDiceNumber
        )
    }

    private fun StepSideNumber(
        DeltaValue: Int)
    {
        val NewSideNumber: Int = DiceUiHelper.ClampValue(
            GetSideNumberValue() + DeltaValue,
            InputLimits.MinSideNumber,
            InputLimits.MaxSideNumber
        )

        SetInputValue(
            GetSideNumberInput(),
            NewSideNumber
        )

        ClampDifficulty()
    }

    private fun StepDifficulty(
        DeltaValue: Int)
    {
        val NewDifficulty: Int = DiceUiHelper.ClampValue(
            GetDifficultyValue() + DeltaValue,
            InputLimits.MinDifficulty,
            GetSideNumberValue()
        )

        SetInputValue(
            GetDifficultyInput(),
            NewDifficulty
        )
    }

    private fun ClampDiceNumber() {
        SetInputValue(
            GetDiceNumberInput(),
            GetDiceNumberValue()
        )
    }

    private fun ClampSideNumber() {
        SetInputValue(
            GetSideNumberInput(),
            GetSideNumberValue()
        )
    }

    private fun ClampDifficulty() {
        SetInputValue(
            GetDifficultyInput(),
            GetDifficultyValue()
        )
    }

    private fun SetInputValue(
        InputField: TextInputEditText,
        NewValue: Int
    ) {
        DiceUiHelper.SetEditTextValueIfNeeded(
            InputField,
            NewValue
        ) { NewState ->
            IsProgrammaticTextUpdate = NewState
        }
    }

    private fun GetDiceNumberInput(): TextInputEditText {
        return PanelRoot.findViewById(R.id.inputDiceNumber)
    }

    private fun GetSideNumberInput(): TextInputEditText {
        return PanelRoot.findViewById(R.id.inputSideNumber)
    }

    private fun GetDifficultyInput(): TextInputEditText {
        return PanelRoot.findViewById(R.id.inputDifficulty)
    }

    private fun GetCancelCheckbox(): CheckBox {
        return PanelRoot.findViewById(R.id.checkOneCancelsSuccess)
    }

    private fun GetUpDiceButton(): View {
        return PanelRoot.findViewById(R.id.buttonUpDiceNumber)
    }

    private fun GetDownDiceButton(): View {
        return PanelRoot.findViewById(R.id.buttonDownDiceNumber)
    }

    private fun GetUpSideButton(): View {
        return PanelRoot.findViewById(R.id.buttonUpSideNumber)
    }

    private fun GetDownSideButton(): View {
        return PanelRoot.findViewById(R.id.buttonDownSideNumber)
    }

    private fun GetUpDifficultyButton(): View {
        return PanelRoot.findViewById(R.id.buttonUpDifficulty)
    }

    private fun GetDownDifficultyButton(): View {
        return PanelRoot.findViewById(R.id.buttonDownDifficulty)
    }
}