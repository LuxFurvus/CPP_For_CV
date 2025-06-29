
// SQLite_DbConnection
    friend class SQLite_DbConnection_T12_CorruptGetShouldCrash_Test;

// SQLite_Statement
    friend class SQLite_Statement_T01_UnusedTailNullOrEmpty_NoWarning_Test;
    friend class SQLite_Statement_T02_UnusedTailPresent_ShouldWarn_Test;
    friend class SQLite_StatementSentinel_T14_FinalizeFromSentinelIdempotent_Test;

// SQLite_StatementCreationKit
    friend class SQLite_StatementSentinel_T01_RegisterAddsPointer_Test;
    friend class SQLite_StatementSentinel_T02_UnregisterRemovesPointer_Test;
    friend class SQLite_StatementSentinel_T03_DoubleRegisterHasSingleEntry_Test;
    friend class SQLite_StatementSentinel_T04_DoubleUnregisterSafe_Test;
    friend class SQLite_StatementSentinel_T05_InvalidateAllClearsSet_Test;
    friend class SQLite_StatementSentinel_T06_RegisterAfterInvalidateWorks_Test;
    friend class SQLite_StatementSentinel_T07_RegisterAndDestroyUnregistersAutomatically_Test;
    friend class SQLite_StatementSentinel_T08_DestructorCallsInvalidateAll_Test;
    friend class SQLite_StatementSentinel_T09_ThreadSafetyParallelRegisterUnregister_Test;
    friend class SQLite_StatementSentinel_T13_NullptrSentinelSafe_Test;
    friend class SQLite_StatementSentinel_T15_ThreadSafetyConcurrentAccess_Test;
    
    friend class SQLite_Statement_T01_MoveConstructorFromValid_Test;
    friend class SQLite_Statement_T02_MoveConstructorFromFinalized_Test;
    friend class SQLite_Statement_T03_MoveAssignmentToDefaultInitialized_Test;
    friend class SQLite_Statement_T04_MoveAssignmentToPreinitialized_Test;
    friend class SQLite_Statement_T05_MoveAssignmentToSelf_Test;
    friend class SQLite_Statement_T06_ChainedMoveAssignments_Test;
    friend class SQLite_Statement_T07_DestroyMovedFromObject_Test;
    friend class SQLite_Statement_T08_MoveWithAndWithoutSentinel_Test;
    friend class SQLite_Statement_T09_RepeatedMoveAssignments_Test;
    friend class SQLite_Statement_T10_MoveFromFinalizedStatement_Test;

// SQLite_ConnectionPool
    friend class SQLite_ConnectionPool_T03_ErasesSingleTargetEntry_Test;
    friend class SQLite_ConnectionPool_T04_ClearsAllConnections_Test;

