#ifndef TRIVIA_QUESTIONHANDLER_H
#define TRIVIA_QUESTIONHANDLER_H

typedef enum __attribute__ ((packed)) { game_finished, game_afk, game_fail, game_ok } game_status_t;

class QuestionHandler {
    private:
        static cmd_t                                    buf [MAX_ROUNDS];
        static bool                                     init;
        linkedlist_t                                    queue;
        int                                             roundtime;
        char                                            game_id [sizeof ("XXXX")];
        std::tuple<std::vector<game_status_t>, uint8_t> results;
        char                                            res_str
            [sizeof ("Total: " stringify (MAX_ROUNDS) " de " stringify (MAX_ROUNDS) "\n\n") +
             (sizeof (stringify (MAX_ROUNDS) ". No respondido\n") - 1) * MAX_ROUNDS];
        char    ip [sizeof ("255.255.255.0")];
        int     port;
        Usuario user;
        bool    isLocal;

        void local (size_t, size_t);

    public:
        QuestionHandler (const char *const restrict, const int, const Usuario, const char [sizeof ("XXXX")]);
        QuestionHandler (size_t = MAX_ROUNDS, size_t = DEFAULT_ROUND_TIME);

        ~QuestionHandler ();

        std::vector<question_t> getQuestions (void);
        int                     getRoundTime (void);
        game_status_t           next (void);
        typeof (results)        game (void);
        typeof (results)        getResults (void);
        const char             *resultsToStr (void);
        void                    sendResults (void);
};

#endif