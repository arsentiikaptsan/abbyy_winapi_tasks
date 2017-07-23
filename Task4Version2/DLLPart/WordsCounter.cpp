#include "WordsCounter.h"

int __cdecl CountWords(const wchar_t* text) 
{
    int spaces_num = 0;
    int i = 0;
    while( iswspace(text[i]) ) {
        i++;
    }

    bool isLastCharSpace = false;
    for( ;; i++ ) {
        if( text[i] == 0 ) {
            if( isLastCharSpace ) {
                return spaces_num;
            }
            return spaces_num + 1;
        }
        if( iswspace(text[i]) ) {
            if( isLastCharSpace ) {
                continue;
            }
            spaces_num++;
            isLastCharSpace = true;
        } else {
            isLastCharSpace = false;
        }
    }
    return -1;
}