package com.example.randogen.WoD

import android.widget.CheckBox
import com.example.randogen.InputLimits
import com.example.randogen.NumberStepperController
import com.example.randogen.databinding.NumberStepperBinding

class WodInputPanelController(
    private val DiceNumberStepperBinding: NumberStepperBinding,
    private val DifficultyStepperBinding: NumberStepperBinding,
    private val CancelCheckbox: CheckBox,
    private val TenRerollCheckbox: CheckBox
)
{
    private var OnInputChangedCallback: (() -> Unit)? = null

    private val DiceNumberStepperController = NumberStepperController(
        StepperBinding = DiceNumberStepperBinding,
        GetMinValue = { InputLimits.MinDiceNumber },
        GetMaxValue = { InputLimits.MaxDiceNumber },
        OnValueChanged = { OnInputChangedCallback?.invoke() }
    )

    private val DifficultyStepperController = NumberStepperController(
        StepperBinding = DifficultyStepperBinding,
        GetMinValue = { InputLimits.MinDifficulty },
        GetMaxValue = { InputLimits.MaxDifficulty },
        OnValueChanged = { OnInputChangedCallback?.invoke() }
    )

    fun SetManualInputEnabled(
        IsEnabled: Boolean)
    {
        DiceNumberStepperController.SetManualInputEnabled(IsEnabled)
        DifficultyStepperController.SetManualInputEnabled(IsEnabled)
    }

    fun Bind(
        OnInputChanged: () -> Unit)
    {
        OnInputChangedCallback = OnInputChanged

        DiceNumberStepperController.Bind()
        DifficultyStepperController.Bind()

        CancelCheckbox.setOnCheckedChangeListener { _, _ ->
            OnInputChanged()
        }

        TenRerollCheckbox.setOnCheckedChangeListener { _, _ ->
            OnInputChanged()
        }
    }

    fun ClampAll()
    {
        DiceNumberStepperController.Clamp()
        DifficultyStepperController.Clamp()
    }

    fun GetDiceNumberValue(): Int
    {
        return DiceNumberStepperController.GetValue()
    }

    fun GetDifficultyValue(): Int
    {
        return DifficultyStepperController.GetValue()
    }

    fun IsCancelEnabled(): Boolean
    {
        return CancelCheckbox.isChecked
    }

    fun IsTenRerollEnabled(): Boolean
    {
        return TenRerollCheckbox.isChecked
    }

    fun RestoreState(
        DiceNumberValue: Int,
        DifficultyValue: Int,
        WithCancelValue: Boolean,
        WithTenReroll: Boolean)
    {
        DiceNumberStepperController.SetValue(DiceNumberValue)
        DifficultyStepperController.SetValue(DifficultyValue)

        CancelCheckbox.isChecked = WithCancelValue
        TenRerollCheckbox.isChecked = WithTenReroll

        DifficultyStepperController.Clamp()
    }
}