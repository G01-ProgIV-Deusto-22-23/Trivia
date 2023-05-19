#ifndef TRIVIA_QUESTIONHANDLER_H
#define TRIVIA_QUESTIONHANDLER_H

typedef enum __attribute__ ((packed)) { game_finished, game_afk, game_fail, game_ok } game_status_t;

class QuestionHandler {
    private:
        static question_t buf [MAX_QUESTIONS];
        static bool       init;
        linkedlist_t      queue;
        int               roundtime;

    public:
        QuestionHandler (size_t = MAX_ROUNDS, size_t = DEFAULT_ROUND_TIME);

        ~QuestionHandler ();

        std::vector<question_t>                                  getQuestions (void);
        int                                                      getRoundTime (void);
        game_status_t                                            next (void);
        std::tuple<std::vector<game_status_t>, uint8_t, uint8_t> game (void);
};

#endif