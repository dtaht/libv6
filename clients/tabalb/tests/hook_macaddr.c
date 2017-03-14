#include <stdio.h>
#include <printf.h>

// http://codingrelic.geekhold.com/2008/12/printf-acular.html

static int
printf_arginfo_M(const struct printf_info *info,
                 size_t      n,
                 int        *argtypes)
{
    /* "%M" always takes one argument, a pointer to uint8_t[6]. */
    if (n > 0) {
        argtypes[0] = PA_POINTER;
    }

    return 1;
} /* printf_arginfo_M */

static int
printf_output_M(FILE *stream,
                const struct printf_info *info,
                const void *const *args)
{
    const unsigned char *mac;
    int len;

    mac = *(unsigned char **)(args[0]);

    len = fprintf(stream, "%02x:%02x:%02x:%02x:%02x:%02x",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return len;
} /* printf_output_M */

static int
printf_output_M(FILE *stream,
                const struct printf_info *info,
                const void *const *args)
{
    const unsigned char *mac;
    char macstr[18]; /* 6 hex bytes, with colon seperators and trailing NUL */
    char fmtstr[32];
    int len;

    mac = *(unsigned char **)(args[0]);
    snprintf(macstr, sizeof(macstr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /* We handle some conversion specifier options as appropriate:
     * '-' and field width */
    if (info->width > 0) {
        snprintf(fmtstr, sizeof(fmtstr), "%%%s%ds",
                (info->left ? "-" : ""), info->width);
    } else {
        snprintf(fmtstr, sizeof(fmtstr), "%%s");
    }

    len = fprintf(stream, fmtstr, macstr);

    return len;
} /* printf_output_M */

/* and I really need bits too!

It went like a breeze. 0x7FFFFFFF becomes 01111111 11111111 11111111 11111111 in a matter of printf("%B\n", foo);
Thanks gain.

*/
// cisco version

    if (info->alt) {
        /* Cisco style formatting */
        snprintf(macstr, sizeof(macstr), "%02x%02x.%02x%02x.%02x%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        /* Unix/Linux/Windows/MacOS style formatting */
        snprintf(macstr, sizeof(macstr), "%02x:%02x:%02x:%02x:%02x:%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

main() {
printf("%30M\n", mac);
}

