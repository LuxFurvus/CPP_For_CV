
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
    friend class SQLite_StatementCreationKit_T16_CopyConstructorPreservesFields_Test;
    friend class SQLite_StatementCreationKit_T17_MoveConstructorTransfersOwnership_Test;
    friend class SQLite_StatementCreationKit_T18_CopyAssignmentPreservesFields_Test;
    friend class SQLite_StatementCreationKit_T19_MoveAssignmentTransfersOwnership_Test;
    friend class SQLite_StatementCreationKit_T20_UseAfterMoveBehavesCorrectly_Test;
    friend class SQLite_StatementCreationKit_T21_ExpiredSentinelDoesNotRegister_Test;
    friend class SQLite_StatementCreationKit_T22_NullDbHandleFails_Test;
    friend class SQLite_StatementSentinel_T24_MovingSentinelContainerSafe_Test;
    friend class SQLite_StatementCreationKit_T25_DefaultConstructedKitIsSafe_Test;
    friend class SQLite_StatementCreationKit_T25_ExpiredWeakSentinelSkipsRegistration_Test;

    friend class SQLite_DbConnection_T15_StatementKitFields_Test;

