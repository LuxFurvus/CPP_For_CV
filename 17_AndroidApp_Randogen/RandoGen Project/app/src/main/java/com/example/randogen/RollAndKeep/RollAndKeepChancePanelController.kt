package com.example.randogen

import android.widget.TextView

import androidx.annotation.Keep

@Keep
interface RollAndKeepChanceCallback
{
    fun OnRollAndKeepChancesReady(
        RequestId: Long,
        SuccessChanceText: String)

    fun OnRollAndKeepChancesFailed(
        RequestId: Long,
        ErrorText: String)
}

class RollAndKeepChancePanelController(
    private val SuccessChanceTextView: TextView)
{
    private var LastRequestId: Long = 0L

    private val ChanceCallback = object : RollAndKeepChanceCallback
    {
        override fun OnRollAndKeepChancesReady(
            RequestId: Long,
            SuccessChanceText: String)
        {
            if (RequestId != LastRequestId)
            {
                return
            }

            SuccessChanceTextView.post {
                if (RequestId != LastRequestId)
                {
                    return@post
                }

                SuccessChanceRender.UpdateSuccessChancePanel(
                    SuccessChanceTextView = SuccessChanceTextView,
                    SuccessChanceText = SuccessChanceText
                )
            }
        }

        override fun OnRollAndKeepChancesFailed(
            RequestId: Long,
            ErrorText: String)
        {
            if (RequestId != LastRequestId)
            {
                return
            }

            SuccessChanceTextView.post {
                if (RequestId != LastRequestId)
                {
                    return@post
                }

                SuccessChanceRender.UpdateSuccessChancePanel(
                    SuccessChanceTextView = SuccessChanceTextView,
                    SuccessChanceText = ErrorText
                )
            }
        }
    }

    fun Update(
        RollNumValue: Int,
        KeepNumValue: Int,
        BonusNumValue: Int,
        ExplodeTensValue: Boolean,
        ExplodeOnesValue: Boolean)
    {
        LastRequestId += 1L

        RollAndKeepNativeBridge.StartRollAndKeepChancesAsyncJNI(
            InRequestId = LastRequestId,
            InRollNum = RollNumValue,
            InKeepNum = KeepNumValue,
            InBonus = BonusNumValue,
            InExplodeTens = ExplodeTensValue,
            InExplodeOnes = ExplodeOnesValue,
            InCallback = ChanceCallback
        )
    }
}