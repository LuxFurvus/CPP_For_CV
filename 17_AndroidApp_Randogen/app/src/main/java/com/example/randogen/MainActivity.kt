@file:Suppress(
    "FunctionName",
    "ConstPropertyName",
    "PrivatePropertyName",
    "LocalVariableName",
    "PropertyName")

package com.example.randogen

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import com.example.randogen.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity()
{
    private lateinit var BindingObj: ActivityMainBinding
    private lateinit var ViewModelObj: MainActivityViewModel

    private lateinit var InputController: DiceInputPanelController
    private lateinit var SuccessChanceController: SuccessChancePanelController
    private lateinit var RollResultController: RollResultPanelController

    override fun onCreate(
        savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)

        ViewModelObj = ViewModelProvider(this)[MainActivityViewModel::class.java]

        BindingObj = ActivityMainBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

        InputController = DiceInputPanelController(
            BindingObj.inputPanel.root
        )

        SuccessChanceController = SuccessChancePanelController(
            BindingObj.successChancePanel.valueSuccessChanceList
        )

        RollResultController = RollResultPanelController(
            BindingObj.resultPanel.root
        )

        InputController.Bind {
            ReactOnInputUpdated()
        }

        BindingObj.rollButtonView.buttonRoll.setOnClickListener {
            ReactOnRollButtonPressed()
        }

        BindingObj.resultPanel.buttonCleanResults.setOnClickListener {
            ReactOnCleanResultsButtonPressed()
        }

        RestoreResultPanelState()
    }

    private fun ReactOnInputUpdated()
    {
        SuccessChanceController.Update(
            DiceNumberValue = InputController.GetDiceNumberValue(),
            SideNumberValue = InputController.GetSideNumberValue(),
            DifficultyValue = InputController.GetDifficultyValue(),
            WithCancelValue = InputController.IsCancelEnabled())

        RollResultController.SetStrokeChanged()
        SaveResultPanelState()
    }

    private fun ReactOnRollButtonPressed()
    {
        InputController.ClampAll()

        RollResultController.SetStrokeInitial()

        val DiceNumberValue: Int = InputController.GetDiceNumberValue()
        val SideNumberValue: Int = InputController.GetSideNumberValue()
        val DifficultyValue: Int = InputController.GetDifficultyValue()
        val WithCancelValue: Boolean = InputController.IsCancelEnabled()

        val Result: RollResults? = DiceNativeBridge.GetRollResultsJNI(
            DiceNumberValue,
            SideNumberValue,
            DifficultyValue,
            WithCancelValue,
            RollResults::class.java
        )

        if (Result == null)
        {
            return
        }

        RollResultController.ShowResult(
            Result = Result,
            WithCancelValue = WithCancelValue
        )

        SaveResultPanelState()
    }

    private fun ReactOnCleanResultsButtonPressed()
    {
        RollResultController.Clear()
        ViewModelObj.ClearRollState()
        RollResultController.SetStrokeChanged()
    }

    private fun SaveResultPanelState()
    {
        val ResultState: Triple<String, String, String>
            = RollResultController.ReadState()

        val IsInitialRollResult: Boolean
            = RollResultController.GetIsInitialRollResult()

        val RollStateRef = ViewModelObj.RollState

        RollStateRef.SumText = ResultState.first
        RollStateRef.ResultsText = ResultState.second
        RollStateRef.SuccessesText = ResultState.third
        RollStateRef.IsInitialResult = IsInitialRollResult
    }

    private fun RestoreResultPanelState()
    {
        RollResultController.RestoreState(
            SumText = ViewModelObj.RollState.SumText,
            ResultsText = ViewModelObj.RollState.ResultsText,
            SuccessesText = ViewModelObj.RollState.SuccessesText,
            inIsInitialRoll = ViewModelObj.RollState.IsInitialResult
        )
    }
}
