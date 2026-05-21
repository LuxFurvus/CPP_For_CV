package com.example.randogen

import android.content.Context
import androidx.core.content.edit

object AppManualInputSettings
{
    private const val PreferencesName: String = "AppManualInputSettings"
    private const val ManualInputEnabledKey: String = "ManualInputEnabled"
    private const val DefaultManualInputEnabled: Boolean = false

    fun IsManualInputEnabled(
        ContextObj: Context): Boolean
    {
        return ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .getBoolean(
                ManualInputEnabledKey,
                DefaultManualInputEnabled
            )
    }

    fun SetManualInputEnabled(
        ContextObj: Context,
        IsEnabled: Boolean)
    {
        ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .edit {
                putBoolean(
                    ManualInputEnabledKey,
                    IsEnabled
                )
            }
    }
}