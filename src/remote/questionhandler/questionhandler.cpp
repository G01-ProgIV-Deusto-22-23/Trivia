#include "questionhandler.h"

typeof (QuestionHandler::buf)  QuestionHandler::buf  = {};
typeof (QuestionHandler::init) QuestionHandler::init = false;

QuestionHandler::QuestionHandler (
    const char *const restrict ip, const int port, const Usuario user, const char id [sizeof ("XXXX")]
) {
    if (QuestionHandler::init)
        error ("there can only be a single " stringify (QuestionHandler) " instance at once.");

    QuestionHandler::init = true;

    cmd_t resp =
        send_server (ip, port ? port : get_server_port (), connect_game_command (id), QuestionHandler::buf, MAX_ROUNDS);

    if (resp.cmd == cmd_error) {
        warning ("there was an error trying to fetch the questions, fetching the questions locally.");
        this->local (MAX_ROUNDS, DEFAULT_ROUND_TIME);

        return;
    }

    this->isLocal = false;

    memcpy (this->game_id, id, sizeof ("XXXX"));

    if (ip)
        *(char *) mempcpy (this->ip, ip, strlen (ip)) = '\0';
    else
        memcpy (this->ip, "127.0.0.1", sizeof ("127.0.0.1"));

    this->port = port;

    this->user = user;

    this->roundtime = resp.info.game.round_time;

    for (size_t i = 0;
         !((QuestionHandler::buf + i)->cmd == cmd_packet || (QuestionHandler::buf + i)->cmd == cmd_packet_cont);
         insert_linkedlist (this->queue, (QuestionHandler::buf + i++)->info.pack.text))
        ;

    this->results = std::make_tuple (std::vector<game_status_t> (), UINT8_C (0));
}

QuestionHandler::QuestionHandler (size_t rounds, size_t roundtime) {
    ct_error (
        !(std::is_same<std::underlying_type<game_status_t>::type, uint8_t>::value),
        "the underlying type of " stringify (game_status_t) " must be " stringify (uint8_t) "."
    );
    ct_error (MAX_ROUNDS >= UINT8_MAX, "the maximum number of rounds cannot be bigger than " stringify (UINT8_MAX) ".");

    if (QuestionHandler::init)
        error ("there can only be a single " stringify (QuestionHandler) " instance at once.");

    QuestionHandler::init = true;

    this->isLocal = true;

    this->local (rounds, roundtime);
    this->results = std::make_tuple (std::vector<game_status_t> (), UINT8_C (0));
}

QuestionHandler::~QuestionHandler () {
    memset (QuestionHandler::buf, 0, sizeof (QuestionHandler::buf));
    destroy_linkedlist (this->queue);
    QuestionHandler::init = false;
}

void QuestionHandler::local (size_t rounds, size_t roundtime) {
    if (!rounds)
        rounds = MAX_ROUNDS;

    /*     else if (rounds > MAX_ROUNDS)
            error ("the number of rounds cannot be larger than " stringify (MAX_ROUNDS) "."); */

    if (roundtime > UINT32_MAX)
        warning ("rounds can last a maximum of " stringify (UINT32_MAX) " seconds.");

    this->roundtime = (uint32_t) min ((size_t) UINT32_MAX, roundtime);

    size_t l;
    {
        linkedlist_t qs = get_questions ();
        if (!qs)
            error ("could not fetch any questions.");

        if ((l = length_linkedlist (qs)) < rounds) {
            warning ("not enough questions for the amount of rounds requested.");

            rounds = l;
        }

        size_t i = 0;
        for (question_t *q;;) {
            if (!(q = static_cast<question_t *> (pop_linkedlist (qs, 0))))
                break;

            memcpy (reinterpret_cast<question_t *> (buf) + i++, q, sizeof (question_t));
        }

        destroy_linkedlist (qs);
    }

    {
        static const char     c [] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        static uint64_t       v;
        static struct timeval tv;

        gettimeofday (&tv, NULL);
        v += ((uint64_t) tv.tv_usec << 16) ^ (uint64_t) tv.tv_usec ^
             (uint64_t)
#ifdef _WIN32
                 GetProcessId (GetCurrentProcess ())
#else
                 getpid ()
#endif
            ;

        *this->game_id       = *(c + v % (sizeof (l) - 1));
        v                    /= sizeof (l) - 1;
        *(this->game_id + 1) = *(c + v % (sizeof (l) - 1));
        v                    /= sizeof (l) - 1;
        *(this->game_id + 2) = *(c + v % (sizeof (l) - 1));
        v                    /= sizeof (l) - 1;
        *(this->game_id + 3) = *(c + v % (sizeof (l) - 1));
    }

    std::random_device                                       dev;
    std::mt19937                                             rng (dev ());
    std::uniform_int_distribution<std::mt19937::result_type> dist (0, l - 1);
    std::unordered_set<size_t>                               picks;

    for (size_t i = 0, r; i < rounds; i++) {
        for (; picks.find (r = dist (rng)) != picks.end ();)
            ;

        picks.insert (r);
        insert_linkedlist (this->queue, reinterpret_cast<question_t *> (buf) + r);
    }
}

std::vector<question_t> QuestionHandler::getQuestions (void) {
    size_t len;
    question_t **const restrict buf = (question_t **) toarray_linkedlist (this->queue, &len);
    if (!buf) {
        warning ("could not allocate space for the array of questions.");

        return std::vector<question_t> ();
    }

    std::vector<question_t> vec = std::vector<question_t> (len);
    for (size_t i = 0; i < len; i++)
        vec [i] = **(buf + i);

    return vec;
}

size_t QuestionHandler::getRemainingRounds (void) {
    return length_linkedlist (this->queue);
}

uint32_t QuestionHandler::getRoundTime (void) {
    return this->roundtime;
}

game_status_t QuestionHandler::next (void) {
    question_t *q;
    if (!(q = (question_t *) pop_linkedlist (this->queue, 0)))
        return game_finished;

    static char buf [MAX_QUESTION_TYPE_TEXT + sizeof (": ") + MAX_QUESTION_TEXT - 2];

    *(char *) memcpy (
        mempcpy (mempcpy (buf, q->type, strlen (q->type)), ": ", sizeof (": ") - 1), q->text, strlen (q->text)
    ) = '\0';

    static const char *a1 [1] = {};
    static const char *a2 [2] = {};
    static const char *a3 [3] = {};
    static const char *a4 [4] = {};

    size_t m = (size_t) -1;

    if (q->n == 1) {
        *a1 = q->ans->text;
        m   = create_menu (choicemenu, 0, 0, 0, 0, a1);
    }

    else if (q->n == 2) {
        *a2       = q->ans->text;
        *(a2 + 1) = (q->ans + 1)->text;
        create_menu (choicemenu, 0, 0, 0, 0, a2);
    }

    else if (q->n == 3) {
        *a3       = q->ans->text;
        *(a3 + 1) = (q->ans + 1)->text;
        *(a3 + 2) = (q->ans + 2)->text;
        create_menu (choicemenu, 0, 0, 0, 0, a3);
    }

    else {
        *a4       = q->ans->text;
        *(a4 + 1) = (q->ans + 1)->text;
        *(a4 + 2) = (q->ans + 2)->text;
        *(a4 + 3) = (q->ans + 3)->text;
        create_menu (choicemenu, 0, 0, 0, 0, a4);
    }

    timeout_menu (m, this->roundtime);
    display_menu (m, buf);

    size_t ret = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

    delete_menu (m);

    if (ret == (size_t) -1)
        return game_afk;

    return (q->ans + ret)->correct ? game_ok : game_fail;
}

typeof (QuestionHandler::results) QuestionHandler::game (void) {
    std::vector<game_status_t> res = std::vector<game_status_t> (MAX_ROUNDS);

    for (game_status_t s;;) {
        if ((s = this->next ()) == game_finished)
            break;

        if (s == game_exit)
            return this->results = std::make_tuple (std::vector<game_status_t> { game_exit }, UINT8_C (0));

        res.push_back (s);

        if (([res] () {
                for (size_t i = 0, c = 0; i < res.size (); i++) {
                    if (res [i] == game_afk) {
                        c++;

                        if (c == 3)
                            return true;

                        continue;
                    }

                    c = 0;
                }

                return false;
            }) ())
            return this->results = std::make_tuple (std::vector<game_status_t> (), UINT8_C (0));
    }

    res.shrink_to_fit ();

    return this->results = std::make_tuple (res, std::count (std::begin (res), std::end (res), game_ok));
}

typeof (QuestionHandler::results) QuestionHandler::getResults (void) {
    return this->results;
}

const char *QuestionHandler::resultsToStr (void) {
    memset (this->res_str, 0, sizeof (this->res_str));

    char   *pos = this->res_str;
    size_t  i   = 0;
    ssize_t j;
    if ((j = std::get<0> (this->results).size ()
                 ? std::get<0> (this->results) [0] == game_exit
                       ? sprintf (pos, "Partida abandonada.")
                       : sprintf (
                             pos, "Total: %" PRIu8 "de %" PRIu8 "\n\n", std::get<1> (this->results),
                             (uint8_t) std::get<0> (this->results).size ()
                         )
                 : sprintf (pos, "Partida terminada por inactividad.")) == -1)
        goto err;
    pos += j;

    if (std::get<0> (this->results).size () && std::get<0> (this->results) [0] != game_exit)
        for (; i < std::get<0> (this->results).size (); i++, pos += j) {
            if ((j = sprintf (
                     pos, "%" PRIu8 ". %s%s", (uint8_t) (i + 1),
                     std::get<0> (this->results) [i] == game_ok     ? "Acierto"
                     : std::get<0> (this->results) [i] == game_fail ? "Fallo"
                                                                    : "No respondido",
                     i == std::get<0> (this->results).size () - 1 ? "" : "\n"
                 )) == -1) {
            err:
                warning ("could not print the results.");

                memset (this->res_str, 0, (size_t) (ptrdiff_t) (pos - this->res_str));
                memcpy (this->res_str, "Could not print the results.", sizeof ("Could not print the results.") - 1);

                break;
            }
        }

    return const_cast<char *> (this->res_str);
}

void QuestionHandler::sendResults (void) {
    this->resultsToStr ();

    char fname [sizeof ("XXXX.results")] = "XXXX.results";
    memcpy (fname, this->game_id, sizeof ("XXXX") - 1);

    FILE *f;
    if ((f = fopen (fname, "w+"))) {
        const size_t l = strlen (this->res_str);
        if (fwrite (this->res_str, 1, l, f) < l)
            warning ("could not write the results to the file properly.");

        if (fclose (f) == EOF)
            warning ("could not close the results file properly.");
    }

    else
        warning ("could not open the file to write the results.");

    if (!this->isLocal && send_server (
                              this->ip, this->port,
                              update_user_command (
                                  (this->user.aciertosTotales += std::get<1> (this->results),
                                   this->user.fallosTotales +=
                                   (uint32_t) std::get<0> (this->results).size () - std::get<1> (this->results),
                                   this->user)
                              ),
                              NULL, 0
                          )
                                  .cmd == cmd_error)
        warning ("could not update the user's data.");

    if (f) {
        WINDOW *const restrict w = create_window (0, 0, 0, 0);
        if (!w)
            warning ("could not create the window.");

        else {
            box (w, 0, 0);
            mvwprintw (w, 5, 2, "Puedes consultar los resultados de partida desde el archivo %s.", fname);
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");
        }
    }
}