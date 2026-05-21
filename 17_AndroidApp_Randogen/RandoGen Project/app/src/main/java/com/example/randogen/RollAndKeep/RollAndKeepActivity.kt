package com.example.randogen

import android.os.Bundle
import androidx.activity.OnBackPressedCallback
import androidx.lifecycle.ViewModelProvider
import com.example.randogen.databinding.ActivityRollkeepBinding

class RollAndKeepActivity : BaseActivity()
{
    private lateinit var BindingObj: ActivityRollkeepBinding
    private lateinit var ViewModelObj: RollKeepActivityViewModel
    private lateinit var ChancePanelController: RollAndKeepChancePanelController

    private lateinit var RollNumStepper: NumberStepperController
    private lateinit var KeepNumStepper: NumberStepperController
    private lateinit var BonusNumStepper: NumberStepperController

    private lateinit var RollResultController: RollResultPanelController

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        super.onCreate(SavedInstanceState)

        ViewModelObj = ViewModelProvider(this)[RollKeepActivityViewModel::class.java]

        BindingObj = ActivityRollkeepBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

        BindingObj.headerRnK.textHeaderTitle.text =
            getString(R.string.title_roll_n_keep)

        ChancePanelController = RollAndKeepChancePanelController(
            BindingObj.includeChancesRnk.valueSuccessChanceList
        )

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

        RollResultController = RollResultPanelController(
            BindingObj.includeResultPanel.root
        )

        CreateStepperControllers()
        RestoreInputState()
        BindControls()
        RestoreResultPanelState()
        UpdateChancePanel()
        ApplyManualInputSettingForScreen()
    }

    override fun OnRollButtonClicked()
    {
        ReactOnRollButtonPressed()
    }

    override fun ApplyManualInputSettingForScreen()
    {
        if (!::RollNumStepper.isInitialized)
        {
            return
        }

        val IsEnabled: Boolean = IsManualInputEnabled()

        RollNumStepper.SetManualInputEnabled(IsEnabled)
        KeepNumStepper.SetManualInputEnabled(IsEnabled)
        BonusNumStepper.SetManualInputEnabled(IsEnabled)
    }

    private fun CreateStepperControllers()
    {
        RollNumStepper = NumberStepperController(
            StepperBinding = BindingObj.includeRollNumStepper,
            GetMinValue = { InputLimits.MinDiceNumber },
            GetMaxValue = { InputLimits.MaxDiceNumber },
            OnValueChanged = { ReactOnInputUpdated() }
        )

        KeepNumStepper = NumberStepperController(
            StepperBinding = BindingObj.includeKeepNumStepper,
            GetMinValue = { InputLimits.MinDiceNumber },
            GetMaxValue = { GetRollNumValue() },
            OnValueChanged = { ReactOnInputUpdated() }
        )

        BonusNumStepper = NumberStepperController(
            StepperBinding = BindingObj.includeBonusNumStepper,
            GetMinValue = { InputLimits.MinBonusNum },
            GetMaxValue = { InputLimits.MaxBonusNum },
            OnValueChanged = { ReactOnInputUpdated() }
        )
    }

    private fun BuildCurrentInputState(): RollKeepInputState
    {
        val ExplodeTensCheckBox = GetExplodeTensCheckBox()
        val ExplodeOnesCheckBox = GetExplodeOnesCheckBox()

        return RollKeepInputState(
            RollNum = GetRollNumValue(),
            KeepNum = GetKeepNumValue(),
            BonusNum = GetBonusNumValue(),
            IsExplodeTensChecked = ExplodeTensCheckBox.isChecked,
            IsExplodeOnesChecked = ExplodeOnesCheckBox.isChecked
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
            SetStrokeChanged()
            return
        }

        val LastRolledInputState: RollKeepInputState =
            ViewModelObj.RollState.LastRolledInputState ?: run {
                SetStrokeChanged()
                return
            }

        if (BuildCurrentInputState() == LastRolledInputState)
        {
            SetStrokeInitial()
            return
        }

        SetStrokeChanged()
    }

    private fun RestoreInputState()
    {
        val RollStateRef = ViewModelObj.RollState
        val ExplodeTensCheckBox = GetExplodeTensCheckBox()
        val ExplodeOnesCheckBox = GetExplodeOnesCheckBox()

        RollNumStepper.SetValue(RollStateRef.RollNum)
        KeepNumStepper.SetValue(RollStateRef.KeepNum)
        BonusNumStepper.SetValue(RollStateRef.BonusNum)

        ExplodeTensCheckBox.isChecked =
            RollStateRef.IsExplodeTensChecked

        ExplodeOnesCheckBox.isChecked =
            RollStateRef.IsExplodeOnesChecked

        ClampKeepNumAgainstRollNum()
    }

    private fun SaveInputState()
    {
        ClampKeepNumAgainstRollNum()

        val RollStateRef = ViewModelObj.RollState
        val ExplodeTensCheckBox = GetExplodeTensCheckBox()
        val ExplodeOnesCheckBox = GetExplodeOnesCheckBox()

        RollStateRef.RollNum = GetRollNumValue()
        RollStateRef.KeepNum = GetKeepNumValue()
        RollStateRef.BonusNum = GetBonusNumValue()

        RollStateRef.IsExplodeTensChecked =
            ExplodeTensCheckBox.isChecked

        RollStateRef.IsExplodeOnesChecked =
            ExplodeOnesCheckBox.isChecked
    }

    private fun SaveUiState()
    {
        SaveInputState()
        SaveResultPanelState()
    }

    private fun UpdateChancePanel()
    {
        val ExplodeTensCheckBox = GetExplodeTensCheckBox()
        val ExplodeOnesCheckBox = GetExplodeOnesCheckBox()

        ChancePanelController.Update(
            RollNumValue = GetRollNumValue(),
            KeepNumValue = GetKeepNumValue(),
            BonusNumValue = GetBonusNumValue(),
            ExplodeTensValue = ExplodeTensCheckBox.isChecked,
            ExplodeOnesValue = ExplodeOnesCheckBox.isChecked
        )
    }

    private fun GetExplodeTensCheckBox() =
        requireNotNull(BindingObj.checkBoxExplodeTensRnK)

    private fun GetExplodeOnesCheckBox() =
        requireNotNull(BindingObj.checkBoxExplodeOnesRnK)

    private fun BindControls()
    {
        RollNumStepper.Bind()
        KeepNumStepper.Bind()
        BonusNumStepper.Bind()

        GetExplodeTensCheckBox().setOnCheckedChangeListener { _, _ ->
            ReactOnInputUpdated()
        }

        GetExplodeOnesCheckBox().setOnCheckedChangeListener { _, _ ->
            ReactOnInputUpdated()
        }

        BindingObj.includeResultPanel.buttonCleanResults.setOnClickListener {
            ReactOnCleanResultsButtonPressed()
        }

        BindingObj.headerRnK.buttonHeaderNavigation.setOnClickListener {
            OpenModeListActivity()
        }
    }

    private fun ReactOnInputUpdated()
    {
        ClampKeepNumAgainstRollNum()
        UpdateChancePanel()
        RefreshResultStrokeState()
        SaveUiState()
    }

    private fun ReactOnRollButtonPressed()
    {
        ClampAllInputs()
        UpdateChancePanel()

        val ExplodeTensCheckBox = GetExplodeTensCheckBox()
        val ExplodeOnesCheckBox = GetExplodeOnesCheckBox()

        val Result: RollResults? = RollAndKeepNativeBridge.GetRollAndKeepResultsJNI(
            GetRollNumValue(),
            GetKeepNumValue(),
            GetBonusNumValue(),
            ExplodeTensCheckBox.isChecked,
            ExplodeOnesCheckBox.isChecked,
            RollResults::class.java
        )

        if (Result == null)
        {
            return
        }

        ViewModelObj.RollState.LastRolledInputState = BuildCurrentInputState()

        ShowResult(Result)
        SetStrokeInitial()
        SaveUiState()
    }

    private fun ReactOnCleanResultsButtonPressed()
    {
        ClearResultPanel()
        ViewModelObj.ClearRollState()
        SetStrokeChanged()
        SaveInputState()
    }

    private fun ClampAllInputs()
    {
        RollNumStepper.Clamp()
        KeepNumStepper.Clamp()
        BonusNumStepper.Clamp()

        ClampKeepNumAgainstRollNum()
    }

    private fun ClampKeepNumAgainstRollNum()
    {
        KeepNumStepper.Clamp()
    }

    private fun GetRollNumValue(): Int
    {
        return RollNumStepper.GetValue()
    }

    private fun GetKeepNumValue(): Int
    {
        return KeepNumStepper.GetValue()
    }

    private fun GetBonusNumValue(): Int
    {
        return BonusNumStepper.GetValue()
    }

    private fun ShowResult(
        Result: RollResults)
    {
        val SumText: String = Result.GetRollSum().toString()

        val ResultsText: String = DiceUiHelper.GetRollSequenceText(
            Result.GetRollSequence()
        )

        RollResultController.ShowResult(
            ResultSumText = SumText,
            ResultsDataText = ResultsText
        )
    }

    private fun ClearResultPanel()
    {
        RollResultController.Clear()
    }

    private fun SaveResultPanelState()
    {
        val ResultState: RollResultPanelState =
            RollResultController.ReadState()

        ViewModelObj.RollState.SumText =
            ResultState.ResultSumText

        ViewModelObj.RollState.ResultsText =
            ResultState.ResultsDataText

        ViewModelObj.RollState.IsInitialResult =
            ResultState.IsInitialRollResult
    }

    private fun RestoreResultPanelState()
    {
        RollResultController.RestoreState(
            RollResultPanelState(
                ResultSumText = ViewModelObj.RollState.SumText,
                ResultsDataText = ViewModelObj.RollState.ResultsText,
                IsInitialRollResult = ViewModelObj.RollState.IsInitialResult
            )
        )

        RefreshResultStrokeState()
    }

    private fun SetStrokeInitial()
    {
        RollResultController.SetStrokeInitial()
    }

    private fun SetStrokeChanged()
    {
        RollResultController.SetStrokeChanged()
    }

}
