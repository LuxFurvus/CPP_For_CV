package com.example.randogen

object DndNativeBridge
{
    init
    {
        System.loadLibrary("randogen")
    }

    external fun GetDndRollResultsJNI(
        InDiceCounts: IntArray,
        InSideNumbers: IntArray,
        InRollModifier: Int,
        ResultClass: Class<DndRollResults>
    ): DndRollResults?

    external fun GetDndMinAvgMaxValuesStringJNI(
        InDiceCounts: IntArray,
        InSideNumbers: IntArray,
        InRollModifier: Int,
        ResultClass: Class<DndRollResults>
    ): String?

    external fun GetDndRollButtonTextJNI(
        InDiceCounts: IntArray,
        InSideNumbers: IntArray,
        InRollModifier: Int
    ): String?

}