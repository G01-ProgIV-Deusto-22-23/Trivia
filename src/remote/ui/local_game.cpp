void local_game (void) {
    static size_t rounds     = 0;
    static size_t round_time = 0;

    static const field_attr_t fields [] = { int_field (0, MAX_ROUNDS), int_field (0, UINT32_MAX) };
    static char               time_title [] =
        "Tiempo de ronda (en segundos, 0 para tiempo ilimitado, " stringify (UINT32_MAX) " máximo)";
    static const char *const titles [] = { "Rondas (0 para valor por defecto, " stringify (MAX_ROUNDS) " máximo)",
                                           time_title };
    memmove (
        time_title + sizeof ("Tiempo de ronda (en segundos, 0 para tiempo ilimitado, ") - 1,
        time_title + sizeof ("Tiempo de ronda (en segundos, 0 para tiempo ilimitado, "),
        sizeof (stringify (UINT32_MAX) " máximo)") - 1
    );
    *((char *) memmove (
          time_title + sizeof ("Tiempo de ronda (en segundos, 0 para tiempo ilimitado, " stringify (UINT32_MAX)) - 4,
          time_title + sizeof ("Tiempo de ronda (en segundos, 0 para tiempo ilimitado, " stringify (UINT32_MAX)) - 2,
          sizeof (" máximo)")
      ) +
      sizeof (" máximo)")) = '\0';

    static size_t f;

settings:
    f = form (0, 0, 0, 0, fields, titles, "Empezar una partida (los campos vacíos usarán los valores por defecto)");

    rounds     = std::stoul (**get_form_data (f) ? *get_form_data (f) : "0");
    round_time = std::stoul (**(get_form_data (f) + 1) ? *(get_form_data (f) + 1) : "0");

    delete_form (f);

    static QuestionHandler qh (rounds, round_time);

    static char rounds_str [sizeof (stringify (MAX_ROUNDS)) + sizeof (" rondas") - 1];
    static char round_time_str [sizeof (stringify (UINT32_MAX)) + sizeof (" segundos por ronda") - 1];
    sprintf (rounds_str, "%" PRISZ " rondas", qh.getRemainingRounds ());
    sprintf (
        rounds_str, qh.getRoundTime () ? ("%" PRIu32 " segundos por ronda") : "Tiempo de ronda ilimitado",
        qh.getRoundTime ()
    );

    static const char *const opts [][2] = { { " ", rounds_str }, { " ", round_time_str }, { " ", " " },
                                            { " ", "Sí" },       { " ", "No" },           { " ", "Cambiar ajustes" } };
    static size_t            m;
    static size_t            r;

menu:
    m = choicemenu (0, 0, 0, 0, opts, "¿Empezar partida?");
    r = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

    if (!get_menu_ret (m))
        error ("could not get the user's response.");

    if (!delete_menu (m))
        warning ("could not delete the menu properly.");

    if (r == (size_t) -1 || r == 3)
        return;

    if (r <= 2)
        goto menu;

    if (r == 5)
        goto settings;

    qh.game ();
    qh.sendResults ();
}