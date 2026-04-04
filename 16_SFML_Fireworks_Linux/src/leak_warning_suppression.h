
#ifdef __cplusplus
extern "C"
#endif
const char* __asan_default_options() {
    // Note: detect_leaks=1 is default
    // A suppressions file 'lsan_suppressions.txt' must still be provided via LSAN_OPTIONS
    return "detect_leaks=1";
}