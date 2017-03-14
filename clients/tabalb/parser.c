#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "linenoise.h"
#include "commands.h"

// build_completion_table();

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'd') {
        linenoiseAddCompletion(lc,"dump");
        linenoiseAddCompletion(lc,"dump stats");
        linenoiseAddCompletion(lc,"dump neighbours");
        linenoiseAddCompletion(lc,"dump routes");
        linenoiseAddCompletion(lc,"dump xroutes");
    }
    if (buf[0] == 'm') {
        linenoiseAddCompletion(lc,"monitor");
        linenoiseAddCompletion(lc,"monitor stats");
        linenoiseAddCompletion(lc,"monitor neighbours");
        linenoiseAddCompletion(lc,"monitor routes");
        linenoiseAddCompletion(lc,"monitor xroutes");
    }

    if (buf[0] == 'e') {
        linenoiseAddCompletion(lc,"exit");
    }

    if (buf[0] == 's') {
        linenoiseAddCompletion(lc,"set");
        linenoiseAddCompletion(lc,"set output");
        linenoiseAddCompletion(lc,"set output json");
        linenoiseAddCompletion(lc,"set output text");
    }
    if (buf[0] == 'l') {
        linenoiseAddCompletion(lc,"log");
        linenoiseAddCompletion(lc,"log off");
        linenoiseAddCompletion(lc,"log on");
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"dump")) {
        *color = 35;
        *bold = 0;
        return " World";
    }
    return NULL;
}

int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];

    /* Parse options, with --multiline we enable multi line editing. */
    while(argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv,"--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (!strcmp(*argv,"--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }

    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */
    while((line = linenoise("hello> ")) != NULL) {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            printf("echo: '%s'\n", line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            /* The "/historylen" command will change the history len. */
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}
