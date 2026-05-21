package com.example.randogen

import android.animation.Animator
import android.animation.AnimatorListenerAdapter
import android.animation.ValueAnimator
import android.content.res.ColorStateList
import android.graphics.Color
import android.view.View
import android.view.animation.LinearInterpolator
import com.google.android.material.button.MaterialButton

class RollButtonController(
    private val RollButton: MaterialButton)
{
    private var IsCooldownActive: Boolean = false
    private var IsButtonEnabledByOwner: Boolean = true

    init
    {
        ApplyTextLineBreakRules()
    }

    fun SetText(
        NewText: String)
    {
        RollButton.text = NewText
    }

    fun SetOnClickListener(
        Listener: View.OnClickListener)
    {
        RollButton.setOnClickListener { ClickedView ->
            if (IsCooldownActive)
            {
                return@setOnClickListener
            }

            if (!IsButtonEnabledByOwner)
            {
                return@setOnClickListener
            }

            StartCooldownAnimation()
            Listener.onClick(ClickedView)
        }
    }

    fun SetSingleLineEnabled(
        IsEnabled: Boolean)
    {
        RollButton.setSingleLine(IsEnabled)
    }

    private fun ApplyTextLineBreakRules()
    {
        SetSingleLineEnabled(true)
        RollButton.setHorizontallyScrolling(false)
    }

    private fun StartCooldownAnimation()
    {
        val CooldownDurationMs: Long =
            AppRollCooldownSettings.GetRollCooldownMs(RollButton.context)

        if (CooldownDurationMs <= 0L)
        {
            return
        }

        val OriginalTintList: ColorStateList? =
            RollButton.backgroundTintList

        val TargetColor: Int =
            OriginalTintList?.getColorForState(
                RollButton.drawableState,
                OriginalTintList.defaultColor
            ) ?: Color.BLACK

        IsCooldownActive = true
        ApplyInteractionState()

        RollButton.backgroundTintList =
            ColorStateList.valueOf(Color.BLACK)

        val AnimatorObj: ValueAnimator =
            ValueAnimator.ofArgb(
                Color.BLACK,
                TargetColor
            )

        AnimatorObj.duration = CooldownDurationMs
        AnimatorObj.interpolator = LinearInterpolator()

        AnimatorObj.addUpdateListener { ValueAnimatorObj ->
            val AnimatedColor: Int =
                ValueAnimatorObj.animatedValue as Int

            RollButton.backgroundTintList =
                ColorStateList.valueOf(AnimatedColor)
        }

        AnimatorObj.addListener(object : AnimatorListenerAdapter()
        {
            override fun onAnimationEnd(
                AnimationObj: Animator)
            {
                RollButton.backgroundTintList =
                    OriginalTintList ?: ColorStateList.valueOf(TargetColor)

                IsCooldownActive = false
                ApplyInteractionState()
            }
        })

        AnimatorObj.start()
    }

    private fun ApplyInteractionState()
    {
        RollButton.isEnabled = IsButtonEnabledByOwner
        RollButton.isClickable = IsButtonEnabledByOwner && !IsCooldownActive
    }
}