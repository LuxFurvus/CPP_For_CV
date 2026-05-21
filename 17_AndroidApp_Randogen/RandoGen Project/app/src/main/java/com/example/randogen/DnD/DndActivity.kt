package com.example.randogen

import android.os.Bundle
import androidx.activity.OnBackPressedCallback
import androidx.lifecycle.ViewModelProvider
import com.example.randogen.DnD.DndActivityViewModel
import com.example.randogen.DnD.DndRollInputState
import com.example.randogen.databinding.ActivityDndBinding
import com.example.randogen.databinding.NumberStepperBinding

class DndActivity : BaseActivity()
{
    private lateinit var BindingObj: ActivityDndBinding
    private lateinit var ViewModelObj: DndActivityViewModel
    private lateinit var SuccessChanceController: DndChancePanelController

    private lateinit var DiceNum1Stepper: NumberStepperController
    private lateinit var DiceNum2Stepper: NumberStepperController
    private lateinit var DiceNum3Stepper: NumberStepperController

    private lateinit var SideNum1Stepper: NumberStepperController
    private lateinit var SideNum2Stepper: NumberStepperController
    private lateinit var SideNum3Stepper: NumberStepperController

    private lateinit var BonusNumStepper: NumberStepperController

    private lateinit var AllSteppers: List<NumberStepperController>

    private lateinit var RollResultController: RollResultPanelController

    override fun onCreate(
        SavedInstanceState: Bundle?)
    {
        super.onCreate(SavedInstanceState)

        ViewModelObj = ViewModelProvider(this)[DndActivityViewModel::class.java]

        BindingObj = ActivityDndBinding.inflate(layoutInflater)
        setContentView(BindingObj.root)

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

        SuccessChanceController = DndChancePanelController(
            BindingObj.includeChancesDnd.valueSuccessChanceList
        )

        BindingObj.headerDnd.textHeaderTitle.text =
            getString(R.string.title_dnd)

        RollResultController = RollResultPanelController(
            BindingObj.resultPanelDnd.root
        )

        RollButtonControllerObj?.SetSingleLineEnabled(false)

        CreateStepperControllers()
        RestoreInputState()
        BindControls()
        UpdateOptionalGroupsEnabledState()
        UpdateRollButtonText()
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
        if (!::AllSteppers.isInitialized)
        {
            return
        }

        val IsEnabled: Boolean = IsManualInputEnabled()

        AllSteppers.forEach { StepperController ->
            StepperController.SetManualInputEnabled(IsEnabled)
        }
    }

    private fun CreateStepperControllers()
    {
        DiceNum1Stepper = CreateDiceNumStepper(BindingObj.includeDiceNumStepper1Dnd)
        DiceNum2Stepper = CreateDiceNumStepper(BindingObj.includeDiceNumStepper2Dnd)
        DiceNum3Stepper = CreateDiceNumStepper(BindingObj.includeDiceNumStepper3Dnd)

        SideNum1Stepper = CreateSideNumStepper(BindingObj.includeSideNumStepper1Dnd)
        SideNum2Stepper = CreateSideNumStepper(BindingObj.includeSideNumStepper2Dnd)
        SideNum3Stepper = CreateSideNumStepper(BindingObj.includeSideNumStepper3Dnd)

        BonusNumStepper = NumberStepperController(
            StepperBinding = BindingObj.includeBonusNumStepperDnd,
            GetMinValue = { InputLimits.MinBonusNum },
            GetMaxValue = { InputLimits.MaxBonusNum },
            OnValueChanged = { ReactOnInputUpdated() }
        )

        AllSteppers = listOf(
            DiceNum1Stepper,
            DiceNum2Stepper,
            DiceNum3Stepper,
            SideNum1Stepper,
            SideNum2Stepper,
            SideNum3Stepper,
            BonusNumStepper
        )
    }

    private fun CreateDiceNumStepper(
        StepperBinding: NumberStepperBinding): NumberStepperController
    {
        return NumberStepperController(
            StepperBinding = StepperBinding,
            GetMinValue = { InputLimits.MinDiceNumber },
            GetMaxValue = { InputLimits.MaxDiceNumber },
            OnValueChanged = { ReactOnInputUpdated() }
        )
    }

    private fun CreateSideNumStepper(
        StepperBinding: NumberStepperBinding): NumberStepperController
    {
        return NumberStepperController(
            StepperBinding = StepperBinding,
            GetMinValue = { InputLimits.MinSideNumber },
            GetMaxValue = { InputLimits.MaxSideNumber },
            OnValueChanged = { ReactOnInputUpdated() }
        )
    }

    private fun RestoreInputState()
    {
        val RollStateRef = ViewModelObj.RollState

        DiceNum1Stepper.SetValue(RollStateRef.DiceNum1)
        DiceNum2Stepper.SetValue(RollStateRef.DiceNum2)
        DiceNum3Stepper.SetValue(RollStateRef.DiceNum3)

        SideNum1Stepper.SetValue(RollStateRef.SideNum1)
        SideNum2Stepper.SetValue(RollStateRef.SideNum2)
        SideNum3Stepper.SetValue(RollStateRef.SideNum3)

        BonusNumStepper.SetValue(RollStateRef.BonusNum)

        BindingObj.checkBoxDiceType1Dnd.isChecked =
            RollStateRef.IsDiceType1Checked

        BindingObj.checkBoxDiceType2Dnd.isChecked =
            RollStateRef.IsDiceType2Checked

        BindingObj.checkBoxDiceType3Dnd.isChecked =
            RollStateRef.IsDiceType3Checked
    }

    private fun SaveInputState()
    {
        val RollStateRef = ViewModelObj.RollState

        RollStateRef.DiceNum1 = GetDiceNum1Value()
        RollStateRef.DiceNum2 = GetDiceNum2Value()
        RollStateRef.DiceNum3 = GetDiceNum3Value()

        RollStateRef.SideNum1 = GetSideNum1Value()
        RollStateRef.SideNum2 = GetSideNum2Value()
        RollStateRef.SideNum3 = GetSideNum3Value()

        RollStateRef.BonusNum = GetBonusNumValue()

        RollStateRef.IsDiceType1Checked =
            BindingObj.checkBoxDiceType1Dnd.isChecked

        RollStateRef.IsDiceType2Checked =
            BindingObj.checkBoxDiceType2Dnd.isChecked

        RollStateRef.IsDiceType3Checked =
            BindingObj.checkBoxDiceType3Dnd.isChecked
    }

    private fun SaveUiState()
    {
        SaveInputState()
        SaveResultPanelState()
    }

    private fun BindControls()
    {
        AllSteppers.forEach { StepperController ->
            StepperController.Bind()
        }

        BindingObj.checkBoxDiceType1Dnd.setOnCheckedChangeListener { _, _ ->
            UpdateOptionalGroupsEnabledState()
            ReactOnInputUpdated()
        }

        BindingObj.checkBoxDiceType2Dnd.setOnCheckedChangeListener { _, _ ->
            UpdateOptionalGroupsEnabledState()
            ReactOnInputUpdated()
        }

        BindingObj.checkBoxDiceType3Dnd.setOnCheckedChangeListener { _, _ ->
            UpdateOptionalGroupsEnabledState()
            ReactOnInputUpdated()
        }

        BindingObj.resultPanelDnd.buttonCleanResults.setOnClickListener {
            ReactOnCleanResultsButtonPressed()
        }

        BindingObj.headerDnd.buttonHeaderNavigation.setOnClickListener {
            OpenModeListActivity()
        }
    }

    private fun UpdateSuccessChancePanel()
    {
        SuccessChanceController.Update(
            DiceCounts = BuildDiceCounts(),
            SideNumbers = BuildSideNumbers(),
            BonusValue = GetBonusNumValue()
        )
    }

    private fun ReactOnInputUpdated()
    {
        UpdateRollButtonText()
        UpdateSuccessChancePanel()
        RefreshResultStrokeState()
        SaveUiState()
    }

    private fun BuildCurrentInputState(): DndRollInputState
    {
        return DndRollInputState(
            DiceNum1 = GetDiceNum1Value(),
            DiceNum2 = GetDiceNum2Value(),
            DiceNum3 = GetDiceNum3Value(),
            SideNum1 = GetSideNum1Value(),
            SideNum2 = GetSideNum2Value(),
            SideNum3 = GetSideNum3Value(),
            BonusNum = GetBonusNumValue(),
            IsDiceType1Checked = BindingObj.checkBoxDiceType1Dnd.isChecked,
            IsDiceType2Checked = BindingObj.checkBoxDiceType2Dnd.isChecked,
            IsDiceType3Checked = BindingObj.checkBoxDiceType3Dnd.isChecked
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

        val LastRolledInputState: DndRollInputState =
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

    private fun UpdateRollButtonText()
    {
        val RollButtonText: String =
            DndNativeBridge.GetDndRollButtonTextJNI(
                BuildDiceCounts(),
                BuildSideNumbers(),
                GetBonusNumValue()
            )
                ?.takeIf { TextValue ->
                    TextValue.isNotBlank()
                }
                ?: getString(R.string.label_roll)

        SetRollButtonText(RollButtonText)
    }

    private fun ReactOnRollButtonPressed()
    {
        ClampAllInputs()
        UpdateRollButtonText()
        UpdateSuccessChancePanel()

        val DiceCounts: IntArray = BuildDiceCounts()
        val SideNumbers: IntArray = BuildSideNumbers()

        val Result: DndRollResults? = DndNativeBridge.GetDndRollResultsJNI(
            DiceCounts,
            SideNumbers,
            GetBonusNumValue(),
            DndRollResults::class.java
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

    private fun UpdateOptionalGroupsEnabledState()
    {
        DiceNum1Stepper.SetEnabled(BindingObj.checkBoxDiceType1Dnd.isChecked)
        SideNum1Stepper.SetEnabled(BindingObj.checkBoxDiceType1Dnd.isChecked)

        DiceNum2Stepper.SetEnabled(BindingObj.checkBoxDiceType2Dnd.isChecked)
        SideNum2Stepper.SetEnabled(BindingObj.checkBoxDiceType2Dnd.isChecked)

        DiceNum3Stepper.SetEnabled(BindingObj.checkBoxDiceType3Dnd.isChecked)
        SideNum3Stepper.SetEnabled(BindingObj.checkBoxDiceType3Dnd.isChecked)
    }

    private fun ClampAllInputs()
    {
        AllSteppers.forEach { StepperController ->
            StepperController.Clamp()
        }
    }

    private fun BuildDiceCounts(): IntArray
    {
        val ResultList = mutableListOf<Int>()

        if (BindingObj.checkBoxDiceType1Dnd.isChecked)
        {
            ResultList.add(GetDiceNum1Value())
        }

        if (BindingObj.checkBoxDiceType2Dnd.isChecked)
        {
            ResultList.add(GetDiceNum2Value())
        }

        if (BindingObj.checkBoxDiceType3Dnd.isChecked)
        {
            ResultList.add(GetDiceNum3Value())
        }

        return ResultList.toIntArray()
    }

    private fun BuildSideNumbers(): IntArray
    {
        val ResultList = mutableListOf<Int>()

        if (BindingObj.checkBoxDiceType1Dnd.isChecked)
        {
            ResultList.add(GetSideNum1Value())
        }

        if (BindingObj.checkBoxDiceType2Dnd.isChecked)
        {
            ResultList.add(GetSideNum2Value())
        }

        if (BindingObj.checkBoxDiceType3Dnd.isChecked)
        {
            ResultList.add(GetSideNum3Value())
        }

        return ResultList.toIntArray()
    }

    private fun GetDiceNum1Value(): Int
    {
        return DiceNum1Stepper.GetValue()
    }

    private fun GetDiceNum2Value(): Int
    {
        return DiceNum2Stepper.GetValue()
    }

    private fun GetDiceNum3Value(): Int
    {
        return DiceNum3Stepper.GetValue()
    }

    private fun GetSideNum1Value(): Int
    {
        return SideNum1Stepper.GetValue()
    }

    private fun GetSideNum2Value(): Int
    {
        return SideNum2Stepper.GetValue()
    }

    private fun GetSideNum3Value(): Int
    {
        return SideNum3Stepper.GetValue()
    }

    private fun GetBonusNumValue(): Int
    {
        return BonusNumStepper.GetValue()
    }

    private fun ShowResult(
        Result: DndRollResults)
    {
        RollResultController.ShowResult(
            ResultSumText = Result.GetRollSum().toString(),
            ResultsDataText = Result.GetResultText()
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