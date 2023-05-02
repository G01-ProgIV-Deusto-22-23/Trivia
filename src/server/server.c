#include "port.h"

static _Atomic server_status_t *SERVER_STATUS = NULL;

#ifdef _WIN32
static HANDLE TERM_THREAD = NULL;
#endif

static
#ifdef _WIN32
    HANDLE
#else
    pthread_t
#endif
        UNMAP_THREAD;

static
#ifdef _WIN32
    HANDLE
#else
    int
#endif
        SERVER_PROC = 0;

static void *unmap_status (
#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
    void [[unused]] * unused
#else
    void __attribute__ ((unused)) * unused
#endif
) {
    if (!SERVER_STATUS)
        return NULL;

#ifdef _WIN32

#else
    waitpid (SERVER_PROC, NULL, WNOHANG);

    if (munmap (SERVER_STATUS, sizeof (size_t)) == -1)
        warning ("could not unmap the shared memory segment.");
#endif

    return SERVER_STATUS = NULL;
}

server_status_t get_server_status (void) {
    return SERVER_STATUS ? atomic_load (SERVER_STATUS) : server_off;
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void
    #ifdef _WIN32
    impl_term_handler
    #else
    term_handler
    #endif
    (
    #ifdef _WIN32
        void [[unused]] * unused
    #else
        const int [[unused]] signum
    #endif
    ) {
#else
__attribute__ ((__noreturn__)) static void
    #ifdef _WIN32
    impl_term_handler
    #else
    term_handler
    #endif
    (
    #ifdef _WIN32
        void __attribute__ ((unused)) * unused
    #else
        const int __attribute__ ((unused)) signum
    #endif
    ) {
#endif
    stop_server ();
    for (; atomic_load (SERVER_STATUS) != server_off;)
        ;

    exit (0);
}

#ifdef _WIN32
    #if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (int [[unused]] signum) {
    #else
static void term_handler (const int __attribute__ ((unused)) signum) {
    #endif
    ResumeThread (impl_term_handler);
    Sleep (INFINITE);
}
#endif

void start_server (void) {
    if (atomic_load (SERVER_STATUS) == server_on)
        return;

    if (SERVER_STATUS)
        unmap_status (NULL);

#ifdef _WIN32
#else
    if ((SERVER_STATUS = mmap (NULL, sizeof (_Atomic), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) ==
        MAP_FAILED)
        error ("could not map the shared memory segment.");

    if ((SERVER_PROC = vfork ()) == -1)
        error ("could not start the server.");

    if (SERVER_PROC) {
        if (pthread_create (UNMAP_THREAD, NULL, unmap_status, NULL))
            warning ("could not create the status unmap thread.");
    }
#endif

#ifdef _WIN32
    if (!TERM_THREAD &&
        !(TERM_THREAD = CreateThread (
              &(SECURITY_ATTRIBUTES
              ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
              0, NULL, term_handler, CREATE_SUSPENDED, NULL
          )))
        error ("could not create the server termination handling thread.", impl_term_handler);

    signal (SIGTERM, term_handler);
    signal (SIGABRT, term_handler);
#else
    sigaction (SIGTERM, &(struct sigaction) { .sa_handler = term_handler }, NULL);
    sigaction (SIGABRT, &(struct sigaction) { .sa_handler = term_handler }, NULL);
#endif

    if (atomic_load (SERVER_STATUS) != server_restarting)
        atomic_store (SERVER_STATUS, server_starting);

    return atomic_store (SERVER_STATUS, server_error);

    atomic_store (SERVER_STATUS, server_on);
}

void stop_server (void) {
#ifdef _WIN32
    static DWORD   ExitCode = 0;
    static WINBOOL ec       = 0;
#endif

    if (atomic_load (SERVER_STATUS) == server_off)
        return;

    if (!end_games ())
        warning ("could not end all games successfully");

    if (
#ifdef _WIN32
        ({
            if (!(ec = GetExitCodeProcess (SERVER_PROC, (LPDWORD) &ExitCode)))
                ExitCode = 0;
            !(TerminateProcess (SERVER_PROC, ExitCode) && ec);
        })
#else
        kill (SERVER_PROC, SIGTERM) && (errno != ESRCH)
#endif
    )
        warning ("could not kill the server.");

    if (atomic_load (SERVER_STATUS) != server_restarting)
        atomic_store (SERVER_STATUS, server_off);
}

void restart_server (void) {
    atomic_store (SERVER_STATUS, server_restarting);

    stop_server ();
    start_server ();
}