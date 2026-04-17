#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>

#define STDOUT 1
#define ON 1
#define OFF 0

typedef struct {
    //b and n
    char number_non_blank;
    char number_all;
    char b_and_n;

    //v, e and t
    char display_non_printing;
    char dollar_sign_at_end;
    char tab_as_i;

    //s
    char squeeze;
} flags_state;

int main(int argc, char **argv) {

    int ch = 0;
    flags_state flags = { 0 };
    while ((ch = getopt(argc, argv, "benstv")) != -1) {
        switch(ch) {
            case 'b':
                flags.number_non_blank = ON;
                break;
            case 'e':
                flags.display_non_printing = ON;
                flags.dollar_sign_at_end = ON;
                break;
            case 'n':
                flags.number_all = ON;
                break;
            case 's':
                flags.squeeze = ON;
                break;
            case 't':
                flags.display_non_printing = ON;
                flags.tab_as_i = ON;
                break;
            case 'v':
                flags.display_non_printing = ON;
                break;
            default:
                break;
        }
    }

    if (flags.number_non_blank == ON && flags.number_all == ON) {
        flags.b_and_n = ON;
    }

    argc -= optind;
    // printf("Active flags: \n%6d\n%6d\n%6d\n%6d\n%6d\n%6d\n%6d\n", 
    //        flags.number_non_blank,
    //        flags.number_all, 
    //        flags.b_and_n,
    //        flags.display_non_printing, 
    //        flags.dollar_sign_at_end, 
    //        flags.tab_as_i, 
    //        flags.squeeze);
    //
    int fd = -1;
    off_t file_size = 0;
    int result = 0;
    fd = -1;
    for (int i = 0; i < argc; i++) {
        if ((fd = open(argv[optind + i], O_RDONLY)) == -1) {
            perror("Error opening file");
            return -1;
        }

        file_size = 0;

        if ((file_size = lseek(fd, 0, SEEK_END)) == -1 ) {
            close(fd);
            perror("Error seeking in file");
            return -1;
        }

        result = 0;
        if ((result = lseek(fd, 0, SEEK_SET)) == -1) {
            close(fd);
            perror("Error seeking in file");
            return - 1;
        }

        char* buf= malloc(sizeof(char) * file_size);

        if (buf == NULL) {
            close(fd);
            free(buf);
            return -1;
        }

        
        result = 0;
        if ((result = read(fd, buf, file_size)) == -1) {
            close(fd);
            free(buf);
            return -1;
        }


        close(fd);

        char* temp_buf = buf;

        char prev_char = 0;

        result = 0;
        int line_number = 1;
        char line_to_buf[100] = { 0 };
        if (flags.number_all == ON || (flags.number_non_blank == ON 
            && !isspace((unsigned char)temp_buf[0]))) {
            write(STDOUT, "1", 1);
            write(STDOUT, "  ", 2);
        }
        for (int i = 0; i < file_size; i++) {
            switch(*temp_buf) {
                case '\n':
                    if (flags.dollar_sign_at_end == ON) {
                        write(STDOUT, "$", 1);
                    }
                case '\t':
                    if (*temp_buf == '\t') {
                        if (flags.tab_as_i == ON) {
                            if ((result = write(STDOUT, "^I", 2)) == -1) { 
                                free(buf);
                                return -1;
                            }
                            prev_char = *temp_buf;
                            temp_buf++;
                            break;
                        }
                    }
                default:
                    if (flags.squeeze == ON) {
                        if (prev_char == '\n' && *temp_buf == '\n') {
                            prev_char = *temp_buf;
                            temp_buf++;
                            break;
                        }
                    }
                    prev_char = *temp_buf;
                    if ((result = write(STDOUT, temp_buf++, 1)) == -1) { 
                        free(buf);
                        return -1;
                    }
            }
            if (prev_char == '\n' && flags.number_all == ON && flags.b_and_n == OFF) {
                line_number++;
                int len = snprintf(line_to_buf, sizeof(buf), "%d  ", line_number);
                write(STDOUT, line_to_buf, len);
            } else if (prev_char == '\n' && *temp_buf != '\n' && flags.number_non_blank == ON) {
                line_number++;
                int len = snprintf(line_to_buf, sizeof(buf), "%d  ", line_number);
                write(STDOUT, line_to_buf, len);
            }
        }
        free(buf);
    }
}
