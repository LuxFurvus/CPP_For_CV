package com.example.randogen

import android.content.Intent
import android.os.Bundle
import com.example.randogen.databinding.ActivityMainMenuBinding

class ActivityMainMenu : BaseActivity()
{
    private lateinit var BindingObj: ActivityMainMenuBinding

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        super.onCreate(SavedInstanceState)

        BindingObj = ActivityMainMenuBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

        BindingObj.buttonWod.setOnClickListener {
            val IntentObj = Intent(
                this,
                WodActivity::class.java
            ).apply {
                addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
            }

            startActivity(IntentObj)
            finish()
        }

        BindingObj.buttonRnK.setOnClickListener {
            val IntentObj = Intent(
                this,
                RollAndKeepActivity::class.java
            ).apply {
                addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
            }

            startActivity(IntentObj)
            finish()
        }

        BindingObj.buttonDnd.setOnClickListener {
            val IntentObj = Intent(
                this,
                DndActivity::class.java
            ).apply {
                addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
            }

            startActivity(IntentObj)
            finish()
        }

        BindingObj.headerModeList.buttonHeaderNavigation.setOnClickListener {
            finish()
        }

        BindingObj.headerModeList.textHeaderTitle.text =
            getString(R.string.title_main_menu)
    }
}
