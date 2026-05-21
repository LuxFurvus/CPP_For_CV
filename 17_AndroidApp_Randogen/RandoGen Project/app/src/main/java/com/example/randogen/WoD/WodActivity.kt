
package com.example.randogen

import android.os.Bundle
import androidx.activity.OnBackPressedCallback
import androidx.lifecycle.ViewModelProvider
import com.example.randogen.WoD.WodInputPanelController
import com.example.randogen.WoD.WodNativeBridge
import com.example.randogen.databinding.ActivityWodBinding
import com.example.randogen.databinding.NumberStepperBinding
import com.example.randogen.databinding.ViewActivityHeaderBinding
import com.google.android.material.checkbox.MaterialCheckBox

class WodActivity : BaseActivity()
{
    private lateinit var BindingObj: ActivityWodBinding
    private lateinit var ViewModelObj: MainActivityViewModel

    private lateinit var InputController: WodInputPanelController
    private lateinit var SuccessChanceController: WodChancePanelController
    private lateinit var RollResultController: RollResultPanelController

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        super.onCreate(SavedInstanceState)

        ViewModelObj = ViewModelProvider(this)[MainActivityViewModel::class.java]

        BindingObj = ActivityWodBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

        val HeaderBinding: ViewActivityHeaderBinding =
            requireNotNull(BindingObj.headerWod)

        val DiceStepperBinding: NumberStepperBinding =
            requireNotNull(BindingObj.includeDiceNumStepperWod)

        val DifficultyStepperBinding: NumberStepperBinding =
            requireNotNull(BindingObj.includeDifficultyStepperWod)

        val CancelCheckbox: MaterialCheckBox =
            requireNotNull(BindingObj.checkOneCancelsSuccess)

        val TenRerollCheckbox: MaterialCheckBox =
            requireNotNull(BindingObj.checkExplodeTensWod)

        DiceStepperBinding.inputNumber.isSaveEnabled = false
        DifficultyStepperBinding.inputNumber.isSaveEnabled = false
        CancelCheckbox.isSaveEnabled = false
        TenRerollCheckbox.isSaveEnabled = false

        InputController = WodInputPanelController(
            DiceNumberStepperBinding = DiceStepperBinding,
            DifficultyStepperBinding = DifficultyStepperBinding,
            CancelCheckbox = CancelCheckbox,
            TenRerollCheckbox = TenRerollCheckbox
        )

        SuccessChanceController = WodChancePanelController(
            BindingObj.includeChancesWod.valueSuccessChanceList
        )

        RollResultController = RollResultPanelController(
            BindingObj.resultPanelWod.root
        )

        RestoreInputState()

        InputController.Bind {
            ReactOnInputUpdated()
        }

        HeaderBinding.buttonHeaderNavigation.setOnClickListener {
            OpenModeListActivity()
        }

        onBackPressedDispatcher.addCallback(
            this,
            object : OnBackPressedCallback(true)
            {
                override fun handleOnBackPressed()
                {
                    OpenModeListActivity()
                }
            }
        )

        HeaderBinding.textHeaderTitle.text =
            getString(R.string.title_world_of_darkness)

        BindingObj.resultPanelWod.buttonCleanResults.setOnClickListener {
            ReactOnCleanResultsButtonPressed()
        }

        RestoreResultPanelState()
        UpdateSuccessChancePanel()
        ApplyManualInputSettingForScreen()
    }

    override fun OnRollButtonClicked()
    {
        ReactOnRollButtonPressed()
    }

    override fun ApplyManualInputSettingForScreen()
    {
        if (!::InputController.isInitialized)
        {
            return
        }

        InputController.SetManualInputEnabled(
            IsManualInputEnabled()
        )
    }

    private fun BuildCurrentInputState(): WodRollInputState
    {
        return WodRollInputState(
            DiceNumber = InputController.GetDiceNumberValue(),
            Difficulty = InputController.GetDifficultyValue(),
            WithCancel = InputController.IsCancelEnabled(),
            WithTenReroll = InputController.IsTenRerollEnabled()
        )
    }

    private fun HasVisibleRollResult(): Boolean
    {
        return RollResultController.HasVisibleResult()
    }

    private fun RefreshResultStrokeState()
    {
        if (!HasVisibleRollResult())
        {
            RollResultController.SetStrokeChanged()
            return
        }

        val LastRolledInputState: WodRollInputState =
            ViewModelObj.RollState.LastRolledInputState ?: run {
                RollResultController.SetStrokeChanged()
                return
            }

        if (BuildCurrentInputState() == LastRolledInputState)
        {
            RollResultController.SetStrokeInitial()
            return
        }

        RollResultController.SetStrokeChanged()
    }

    private fun RestoreInputState()
    {
        val RollStateRef = ViewModelObj.RollState

        InputController.RestoreState(
            DiceNumberValue = RollStateRef.DiceNumber,
            DifficultyValue = RollStateRef.Difficulty,
            WithCancelValue = RollStateRef.WithCancel,
            WithTenReroll = RollStateRef.WithTenReroll
        )
    }

    private fun SaveInputState()
    {
        val RollStateRef = ViewModelObj.RollState

        RollStateRef.DiceNumber = InputController.GetDiceNumberValue()
        RollStateRef.Difficulty = InputController.GetDifficultyValue()
        RollStateRef.WithCancel = InputController.IsCancelEnabled()
        RollStateRef.WithTenReroll = InputController.IsTenRerollEnabled()
    }

    private fun SaveUiState()
    {
        SaveInputState()
        SaveResultPanelState()
    }

    private fun UpdateSuccessChancePanel()
    {
        SuccessChanceController.Update(
            DiceNumberValue = InputController.GetDiceNumberValue(),
            DifficultyValue = InputController.GetDifficultyValue(),
            WithCancelValue = InputController.IsCancelEnabled(),
            WithTenReroll = InputController.IsTenRerollEnabled()
        )
    }

    private fun ReactOnInputUpdated()
    {
        UpdateSuccessChancePanel()
        RefreshResultStrokeState()
        SaveUiState()
    }

    private fun ReactOnRollButtonPressed()
    {
        InputController.ClampAll()
        UpdateSuccessChancePanel()

        val DiceNumberValue: Int = InputController.GetDiceNumberValue()
        val DifficultyValue: Int = InputController.GetDifficultyValue()
        val WithCancelValue: Boolean = InputController.IsCancelEnabled()
        val WithTenReroll: Boolean = InputController.IsTenRerollEnabled()

        val Result: RollResults? = WodNativeBridge.GetRollResultsJNI(
            DiceNumberValue,
            DifficultyValue,
            WithCancelValue,
            WithTenReroll,
            RollResults::class.java
        )

        if (Result == null)
        {
            return
        }

        ViewModelObj.RollState.LastRolledInputState = BuildCurrentInputState()

        val ResultsText: String = DiceUiHelper.GetRollSequenceText(
            Result.GetRollSequence()
        )

        val SuccessesText: String = DiceUiHelper.GetSuccessNumText(
            Result.GetSuccessNum(),
            WithCancelValue
        )

        RollResultController.ShowResult(
            ResultSumText = SuccessesText,
            ResultsDataText = ResultsText
        )

        RollResultController.SetStrokeInitial()
        SaveUiState()
    }

    private fun ReactOnCleanResultsButtonPressed()
    {
        RollResultController.Clear()
        ViewModelObj.ClearRollState()
        SaveInputState()
    }

    private fun SaveResultPanelState()
    {
        val ResultState: RollResultPanelState =
            RollResultController.ReadState()

        val RollStateRef = ViewModelObj.RollState

        RollStateRef.ResultsText = ResultState.ResultsDataText
        RollStateRef.SuccessesText = ResultState.ResultSumText
        RollStateRef.IsInitialResult = ResultState.IsInitialRollResult
    }

    private fun RestoreResultPanelState()
    {
        RollResultController.RestoreState(
            RollResultPanelState(
                ResultSumText = ViewModelObj.RollState.SuccessesText,
                ResultsDataText = ViewModelObj.RollState.ResultsText,
                IsInitialRollResult = ViewModelObj.RollState.IsInitialResult
            )
        )

        RefreshResultStrokeState()
    }

}
