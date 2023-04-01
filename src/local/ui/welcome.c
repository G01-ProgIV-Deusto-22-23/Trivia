#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
[[noreturn]]
#else
__attribute__ ((noreturn))
#endif
void welcome (void) {
    exit (0);
}