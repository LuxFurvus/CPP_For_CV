package com.example.randogen

import android.content.Context
import androidx.core.content.edit

object AppRollCooldownSettings
{
    private const val PreferencesName: String = "AppRollCooldownSettings"
    private const val RollCooldownMsKey: String = "RollCooldownMs"

    const val DefaultRollCooldownMs: Long = 1000L
    const val MinRollCooldownMs: Long = 0L
    const val MaxRollCooldownMs: Long = 10000L

    fun GetRollCooldownMs(
        ContextObj: Context): Long
    {
        return ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .getLong(
                RollCooldownMsKey,
                DefaultRollCooldownMs
            )
    }

    fun SetRollCooldownMs(
        ContextObj: Context,
        ValueMs: Long)
    {
        ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .edit {
                putLong(
                    RollCooldownMsKey,
                    ClampRollCooldownMs(ValueMs)
                )
            }
    }

    fun ClampRollCooldownMs(
        ValueMs: Long): Long
    {
        if (ValueMs < MinRollCooldownMs)
        {
            return MinRollCooldownMs
        }

        if (ValueMs > MaxRollCooldownMs)
        {
            return MaxRollCooldownMs
        }

        return ValueMs
    }
}