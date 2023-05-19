#include "questionhandler.h"

typeof (QuestionHandler::buf)  QuestionHandler::buf  = {};
typeof (QuestionHandler::init) QuestionHandler::init = false;

QuestionHandler::QuestionHandler (size_t rounds, size_t roundtime) {
    ct_error (
        !(std::is_same<std::underlying_type<game_status_t>::type, uint8_t>::value),
        "the underlying type of " stringify (game_status_t) " must be " stringify (uint8_t) "."
    );
    ct_error (MAX_ROUNDS >= UINT8_MAX, "the maximum number of rounds cannot be bigger than " stringify (UINT8_MAX) ".");

    if (rounds == 0)
        rounds = MAX_ROUNDS;

    if (rounds > MAX_ROUNDS)
        error ("the number of rounds cannot be larger than " stringify (MAX_ROUNDS) ".");

    if (roundtime > INT_MAX)
        warning ("rounds can last a maximum of " stringify (INT_MAX) " seconds.");

    this->roundtime = (int) min ((size_t) INT_MAX, roundtime);

    if (!QuestionHandler::init) {
        // parse

        QuestionHandler::init = true;
    }

    std::random_device                                       dev;
    std::mt19937                                             rng (dev ());
    std::uniform_int_distribution<std::mt19937::result_type> dist (0, MAX_QUESTIONS - 1);
    std::vector<size_t>                                      picks = std::vector<size_t> (rounds);

    for (size_t i = 0, r; i < rounds; i++) {
        for (; (picks.size () ? true : ((r = dist (rng)), false)) &&
               std::find (std::begin (picks), std::end (picks), (r = dist (rng))) == std::end (picks);)
            ;

        picks.push_back (r);
        insert_linkedlist (this->queue, QuestionHandler::buf + r);
    }
}

QuestionHandler::~QuestionHandler () {
    destroy_linkedlist (this->queue);
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

int QuestionHandler::getRoundTime (void) {
    return this->roundtime;
}

game_status_t QuestionHandler::next (void) {
    question_t *q;
    if (!(q = (question_t *) pop_linkedlist (this->queue, 0)))
        return game_finished;

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
    display_menu (m, "");

    size_t ret = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

    delete_menu (m);

    if (ret == (size_t) -1)
        return game_afk;

    return (q->ans + ret)->correct ? game_ok : game_fail;
}

std::tuple<std::vector<game_status_t>, uint8_t, uint8_t> QuestionHandler::game (void) {
    std::vector<game_status_t> res = std::vector<game_status_t> (MAX_ROUNDS);

    for (game_status_t s;;) {
        if ((s = this->next ()) == game_finished)
            break;

        res.push_back (s);

        if (std::count (std::begin (res), std::end (res), game_afk) >= 3)
            return std::make_tuple (std::vector<game_status_t> (), UINT8_C (0), UINT8_C (0));
    }

    res.shrink_to_fit ();

    return std::make_tuple (res, std::count (std::begin (res), std::end (res), game_ok), res.size ());
}