package com.example.randogen

import android.os.Bundle
import android.view.inputmethod.EditorInfo
import com.example.randogen.databinding.ActivitySettingBinding

class ActivitySetting : BaseActivity()
{
    private lateinit var BindingObj: ActivitySettingBinding

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        super.onCreate(SavedInstanceState)

        BindingObj = ActivitySettingBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

        BindingObj.headerSettingList.textHeaderTitle.text =
            getString(R.string.title_settings)

        BindingObj.switchDarkTheme.isChecked =
            AppThemeSettings.IsDarkThemeEnabled(this)

        BindingObj.switchDarkTheme.setOnCheckedChangeListener { _, IsChecked ->
            AppThemeSettings.SetDarkThemeEnabled(
                this,
                IsChecked
            )

            AppThemeSettings.ApplyTheme(IsChecked)
        }

        BindingObj.switchManualInput.isChecked =
            AppManualInputSettings.IsManualInputEnabled(this)

        BindingObj.switchManualInput.setOnCheckedChangeListener { _, IsChecked ->
            AppManualInputSettings.SetManualInputEnabled(
                this,
                IsChecked
            )
        }

        SetupRollCooldownSetting()

        BindingObj.headerSettingList.buttonHeaderNavigation.setOnClickListener {
            finish()
        }
    }

    private fun SetupRollCooldownSetting()
    {
        val CurrentCooldownMs: Long =
            AppRollCooldownSettings.GetRollCooldownMs(this)

        BindingObj.inputRollCooldownMs.setText(
            CurrentCooldownMs.toString()
        )

        BindingObj.inputRollCooldownMs.setSelection(
            BindingObj.inputRollCooldownMs.text?.length ?: 0
        )

        BindingObj.inputRollCooldownMs.setOnFocusChangeListener { _, HasFocus ->
            if (HasFocus)
            {
                return@setOnFocusChangeListener
            }

            CommitRollCooldownSetting()
        }

        BindingObj.inputRollCooldownMs.setOnEditorActionListener { _, ActionId, _ ->
            if (ActionId != EditorInfo.IME_ACTION_DONE)
            {
                return@setOnEditorActionListener false
            }

            CommitRollCooldownSetting()
            BindingObj.inputRollCooldownMs.clearFocus()

            true
        }
    }

    private fun CommitRollCooldownSetting()
    {
        val RawText: String =
            BindingObj.inputRollCooldownMs.text
                ?.toString()
                ?.trim()
                .orEmpty()

        val RawValueMs: Long =
            RawText.toLongOrNull()
                ?: AppRollCooldownSettings.DefaultRollCooldownMs

        val ClampedValueMs: Long =
            AppRollCooldownSettings.ClampRollCooldownMs(RawValueMs)

        AppRollCooldownSettings.SetRollCooldownMs(
            this,
            ClampedValueMs
        )

        val ClampedText: String =
            ClampedValueMs.toString()

        if (BindingObj.inputRollCooldownMs.text?.toString() == ClampedText)
        {
            return
        }

        BindingObj.inputRollCooldownMs.setText(ClampedText)
        BindingObj.inputRollCooldownMs.setSelection(ClampedText.length)
    }
}