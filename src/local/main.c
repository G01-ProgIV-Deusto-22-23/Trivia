trivia_main {
    setup_ui ();

    size_t m = create_menu (choicemenu, 0, 0, 0, 0, ((const char *[]) { "jijiji" }));
    timeout_menu (m, 10);
    display_menu (m);

    exit (0);

    welcome ();
}