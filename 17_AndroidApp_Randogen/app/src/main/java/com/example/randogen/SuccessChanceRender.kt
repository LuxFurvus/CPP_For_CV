package com.example.randogen

import android.text.method.ScrollingMovementMethod

object SuccessChanceRender
{
    private const val VisibleRowCount = 5

    private fun ApplyFixedViewportHeight(
        SuccessListView: android.widget.TextView)
    {
        SuccessListView.post {
            val FixedHeightPx =
                SuccessListView.lineHeight * VisibleRowCount +
                    SuccessListView.compoundPaddingTop +
                    SuccessListView.compoundPaddingBottom

            if (SuccessListView.height == FixedHeightPx)
            {
                return@post
            }

            SuccessListView.setHeight(FixedHeightPx)
            SuccessListView.requestLayout()
        }
    }

    fun UpdateSuccessChancePanel(
        SuccessChanceTextView: android.widget.TextView,
        SuccessChanceText: String)
    {
        SuccessChanceTextView.movementMethod = ScrollingMovementMethod()
        SuccessChanceTextView.text = SuccessChanceText

        ApplyFixedViewportHeight(SuccessChanceTextView)
    }
}