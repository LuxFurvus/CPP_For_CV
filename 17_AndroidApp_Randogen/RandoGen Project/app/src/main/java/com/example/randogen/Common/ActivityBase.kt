package com.example.randogen

import android.content.Intent
import android.content.pm.ActivityInfo
import android.content.res.Configuration
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.graphics.Insets
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.google.android.material.button.MaterialButton

abstract class BaseActivity : AppCompatActivity()
{
    protected var RollButtonControllerObj: RollButtonController? = null

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        AppThemeSettings.ApplySavedTheme(this)

        super.onCreate(SavedInstanceState)

        enableEdgeToEdge()
    }

    override fun onContentChanged()
    {
        super.onContentChanged()

        ApplySafeAreaInsetsIfPresent()
        BindOrientationButtonIfPresent()
        BindSettingsButtonIfPresent()
        BindRollButtonIfPresent()
    }

    override fun onResume()
    {
        super.onResume()

        ApplyManualInputSettingForScreen()
    }

    protected open fun ApplyManualInputSettingForScreen()
    {
    }

    protected open fun OnRollButtonClicked()
    {
    }

    protected fun IsManualInputEnabled(): Boolean
    {
        return AppManualInputSettings.IsManualInputEnabled(this)
    }

    protected fun SetRollButtonText(
        NewText: String)
    {
        RollButtonControllerObj?.SetText(NewText)
    }

    private fun ApplySafeAreaInsetsIfPresent()
    {
        val SafeAreaView: View =
            findViewById<View?>(R.id.safeAreaRoot)
                ?: return

        val InitialLayoutParams: ViewGroup.MarginLayoutParams =
            SafeAreaView.layoutParams as? ViewGroup.MarginLayoutParams
                ?: return

        val InitialLeftMargin: Int = InitialLayoutParams.leftMargin
        val InitialTopMargin: Int = InitialLayoutParams.topMargin
        val InitialRightMargin: Int = InitialLayoutParams.rightMargin
        val InitialBottomMargin: Int = InitialLayoutParams.bottomMargin

        ViewCompat.setOnApplyWindowInsetsListener(SafeAreaView) { CurrentView, WindowInsetsObj ->
            val SystemBars: Insets =
                WindowInsetsObj.getInsets(WindowInsetsCompat.Type.systemBars())

            val CurrentLayoutParams: ViewGroup.MarginLayoutParams =
                CurrentView.layoutParams as ViewGroup.MarginLayoutParams

            CurrentLayoutParams.setMargins(
                InitialLeftMargin + SystemBars.left,
                InitialTopMargin + SystemBars.top,
                InitialRightMargin + SystemBars.right,
                InitialBottomMargin + SystemBars.bottom
            )

            CurrentView.layoutParams = CurrentLayoutParams

            WindowInsetsObj
        }

        ViewCompat.requestApplyInsets(SafeAreaView)
    }

    private fun BindRollButtonIfPresent()
    {
        val RollButtonView: MaterialButton? =
            findViewById(R.id.buttonRoll)

        if (RollButtonView == null)
        {
            RollButtonControllerObj = null
            return
        }

        RollButtonControllerObj =
            RollButtonController(RollButtonView)

        RollButtonControllerObj?.SetOnClickListener {
            OnRollButtonClicked()
        }
    }

    private fun BindOrientationButtonIfPresent()
    {
        val OrientationButton: View? =
            findViewById(R.id.buttonHeaderOrientation)

        OrientationButton?.setOnClickListener {
            ToggleOrientation()
        }
    }

    private fun BindSettingsButtonIfPresent()
    {
        val SettingsButton: View? =
            findViewById(R.id.buttonHeaderSettings)

        SettingsButton?.setOnClickListener {
            if (this is ActivitySetting)
            {
                finish()
                return@setOnClickListener
            }

            OpenSettingActivity()
        }
    }

    protected fun OpenModeListActivity()
    {
        val IntentObj = Intent(
            this,
            ActivityMainMenu::class.java
        )

        startActivity(IntentObj)
    }

    protected fun OpenSettingActivity()
    {
        val IntentObj = Intent(
            this,
            ActivitySetting::class.java
        )

        startActivity(IntentObj)
    }

    protected fun ToggleOrientation()
    {
        val IsLandscapeNow: Boolean =
            resources.configuration.orientation ==
                    Configuration.ORIENTATION_LANDSCAPE

        requestedOrientation =
            if (IsLandscapeNow)
            {
                ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            }
            else
            {
                ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
            }
    }
}