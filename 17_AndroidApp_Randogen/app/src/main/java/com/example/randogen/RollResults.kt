
package com.example.randogen

class RollResults(
    private val RollSum: Int,
    private val SuccessNum: Int,
    private val RollSequence: IntArray)
{
    fun GetRollSum(): Int {
        return RollSum
    }

    fun GetSuccessNum(): Int {
        return SuccessNum
    }

    fun GetRollSequence(): IntArray {
        return RollSequence
    }
}