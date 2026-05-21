package com.example.randogen

import androidx.annotation.Keep

@Keep
class DndRollResults(
    private val ResultText: String,
    private val RollSum: Int)
{
    fun GetResultText(): String
    {
        return ResultText
    }

    fun GetRollSum(): Int
    {
        return RollSum
    }
}