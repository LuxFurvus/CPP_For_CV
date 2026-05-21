package com.example.randogen

import android.text.method.KeyListener
import androidx.core.widget.doAfterTextChanged
import com.example.randogen.databinding.NumberStepperBinding

class NumberStepperController(
    private val StepperBinding: NumberStepperBinding,
    private val GetMinValue: () -> Int,
    private val GetMaxValue: () -> Int,
    private val OnValueChanged: () -> Unit,
    private val OnValueCommitted: () -> Unit = {}
)
{
    private val OriginalKeyListener: KeyListener? =
        StepperBinding.inputNumber.keyListener

    private var IsProgrammaticTextUpdate: Boolean = false
    private var IsBound: Boolean = false
    private var IsStepperEnabled: Boolean = true
    private var IsManualInputEnabled: Boolean = true

    fun Bind()
    {
        if (IsBound)
        {
            return
        }

        IsBound = true

        StepperBinding.inputNumber.doAfterTextChanged {
            if (IsProgrammaticTextUpdate)
            {
                return@doAfterTextChanged
            }

            OnValueChanged()
        }

        StepperBinding.inputNumber.setOnFocusChangeListener { _, HasFocus ->
            if (HasFocus)
            {
                return@setOnFocusChangeListener
            }

            Clamp()
            OnValueCommitted()
            OnValueChanged()
        }

        StepperBinding.buttonUpNumber.setOnClickListener {
            Step(+1)
            OnValueCommitted()
            OnValueChanged()
        }

        StepperBinding.buttonDownNumber.setOnClickListener {
            Step(-1)
            OnValueCommitted()
            OnValueChanged()
        }

        ApplyInteractionState()
    }

    fun GetValue(): Int
    {
        return DiceUiHelper.GetClampedValue(
            StepperBinding.inputNumber,
            GetMinValue(),
            GetEffectiveMaxValue()
        )
    }

    fun SetValue(
        NewValue: Int)
    {
        val ClampedValue: Int = DiceUiHelper.ClampValue(
            NewValue,
            GetMinValue(),
            GetEffectiveMaxValue()
        )

        DiceUiHelper.SetEditTextValueIfNeeded(
            StepperBinding.inputNumber,
            ClampedValue
        ) { NewState ->
            IsProgrammaticTextUpdate = NewState
        }
    }

    fun Clamp()
    {
        SetValue(GetValue())
    }

    fun Step(
        DeltaValue: Int)
    {
        val NewValue: Int = DiceUiHelper.ClampValue(
            GetValue() + DeltaValue,
            GetMinValue(),
            GetEffectiveMaxValue()
        )

        SetValue(NewValue)
    }

    fun SetEnabled(
        IsEnabled: Boolean)
    {
        IsStepperEnabled = IsEnabled

        StepperBinding.root.alpha =
            if (IsEnabled)
            {
                1.0f
            }
            else
            {
                0.35f
            }

        ApplyInteractionState()
    }

    fun SetManualInputEnabled(
        IsEnabled: Boolean)
    {
        IsManualInputEnabled = IsEnabled

        ApplyInteractionState()
    }

    private fun ApplyInteractionState()
    {
        StepperBinding.buttonUpNumber.isEnabled = IsStepperEnabled
        StepperBinding.buttonDownNumber.isEnabled = IsStepperEnabled
        StepperBinding.inputNumber.isEnabled = IsStepperEnabled

        val CanEditManually: Boolean =
            IsStepperEnabled && IsManualInputEnabled

        if (CanEditManually)
        {
            StepperBinding.inputNumber.keyListener = OriginalKeyListener
            StepperBinding.inputNumber.isFocusable = true
            StepperBinding.inputNumber.isFocusableInTouchMode = true
            StepperBinding.inputNumber.isCursorVisible = true
            StepperBinding.inputNumber.isLongClickable = true
            StepperBinding.inputNumber.showSoftInputOnFocus = true
            return
        }

        StepperBinding.inputNumber.clearFocus()
        StepperBinding.inputNumber.keyListener = null
        StepperBinding.inputNumber.isFocusable = false
        StepperBinding.inputNumber.isFocusableInTouchMode = false
        StepperBinding.inputNumber.isCursorVisible = false
        StepperBinding.inputNumber.isLongClickable = false
        StepperBinding.inputNumber.showSoftInputOnFocus = false
    }

    private fun GetEffectiveMaxValue(): Int
    {
        val MinValue: Int = GetMinValue()
        val MaxValue: Int = GetMaxValue()

        if (MaxValue < MinValue)
        {
            return MinValue
        }

        return MaxValue
    }
}