// Get a line from UART
int getline(char* str) {
    unsigned int i=0;
    char ch = 0;
    for (;;) {
        ch = getch();
        if (ch == 0xd) {
            putch(0xd);
            putch('\n');
            break;
        }
        // Backspace
        else if ((ch == 8) || (ch == 127)) {
            if (i > 0) {
                str[i] = 0;
                i--;
                putch(8);
                putch(' ');
                putch(8);
            }
        }
        else {
            str[i] = ch;
            putch(ch);
            i++;
        }
    }
    str[i] = 0;
    return i;
}

// Put a line to the UART
void putline(char* str) {
    int i=0;
    while (str[i] != 0) {
        if (str[i] == '\n') {
            putch(0xd);
            putch('\n');
        }
        else {
            putch(str[i]);
        }
        i++;
    }
}
