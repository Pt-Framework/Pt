#include "ui_locl.h"

static int read_string(UI *ui, UI_STRING *uis);
static int write_string(UI *ui, UI_STRING *uis);

static int open_console(UI *ui);
static int close_console(UI *ui);

static UI_METHOD ui_openssl =
{
    "OpenSSL dummy user interface",
    open_console,
    write_string,
    NULL,
    read_string,
    close_console,
    NULL
};

UI_METHOD *UI_OpenSSL(void)
{ return &ui_openssl; }

static int write_string(UI *ui, UI_STRING *uis)
{ return 1; }

static int read_string(UI *ui, UI_STRING *uis)
{
    UI_set_result(ui, uis, "");
    return 1;
}

static int open_console(UI *ui)
{ return 1; }

static int close_console(UI *ui)
{ return 1; }
