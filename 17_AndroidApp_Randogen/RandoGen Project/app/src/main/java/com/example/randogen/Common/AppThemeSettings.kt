package com.example.randogen

import android.content.Context
import androidx.appcompat.app.AppCompatDelegate

object AppThemeSettings
{
    private const val PreferencesName: String = "AppThemeSettings"
    private const val DarkThemeEnabledKey: String = "DarkThemeEnabled"
    private const val DefaultDarkThemeEnabled: Boolean = true

    fun IsDarkThemeEnabled(
        ContextObj: Context): Boolean
    {
        return ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .getBoolean(
                DarkThemeEnabledKey,
                DefaultDarkThemeEnabled
            )
    }

    fun SetDarkThemeEnabled(
        ContextObj: Context,
        IsEnabled: Boolean)
    {
        ContextObj
            .getSharedPreferences(
                PreferencesName,
                Context.MODE_PRIVATE
            )
            .edit()
            .putBoolean(
                DarkThemeEnabledKey,
                IsEnabled
            )
            .apply()
    }

    fun ApplySavedTheme(
        ContextObj: Context)
    {
        ApplyTheme(
            IsDarkThemeEnabled(ContextObj)
        )
    }

    fun ApplyTheme(
        IsDarkThemeEnabled: Boolean)
    {
        val TargetMode: Int =
            if (IsDarkThemeEnabled)
            {
                AppCompatDelegate.MODE_NIGHT_YES
            }
            else
            {
                AppCompatDelegate.MODE_NIGHT_NO
            }

        if (AppCompatDelegate.getDefaultNightMode() == TargetMode)
        {
            return
        }

        AppCompatDelegate.setDefaultNightMode(TargetMode)
    }
}