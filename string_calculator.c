/*
Задача B-6. Программа калькулятор для строк
Time limit:	14 s
Memory limit:	64 M

Разработайте программу-калькулятор, умеющую вычислять арифметические выражения над строками.
На стандартный поток ввода программы подается входное выражение, а результат вычислений программа должна вывести на стандартный поток вывода.

Строка - это заключенный в двойные кавычки произвольный набор символов.
Например,
"1", "123", "zz!@#111' ad x" - строки,
'asd', "asddas - не строки.
"" - пустая строка, допустимо.


Поддерживаемые операции: '+' - конкатенация, '*' - умножение, '()' - задание приоритета вычислений.
Конкатенировать (складывать) можно только строки! Иначе необходимо вывести "[error]".
Примеры:
"123" + "456", ответ - "123456".
"123" + a: ошибка, необходимо вывести "[error]".

Умножать можно только строку на число! Иначе необходимо вывести "[error]".
Примеры:
"123" * 1, ответ - "123".
"123" * 2, ответ - "123123".
"123" * 3 ответ - "123123123" и т.д.
"123" * a: ошибка, необходимо вывести "[error]".

Между операциями и строками может быть произвольное количество пробелов - их необходимо пропускать.
Например,
"11"     * 2 +     ( "22" + "33" ) - валидное выражение, результат тут будет "11112233".
При вычислениях должны учитываться приоритеты операций (в том числе, заданные вручную при помощи круглых скобочек).

В случае, если введенное выражение содержит ошибку (невалидное выражение), необходимо вывести в поток стандартного вывода сообщение "[error]" и завершить выполнение программы.

ВАЖНО! Программа в любом случае должна возвращать 0. Не пишите return -1, exit(1) и т.п. Даже если обнаружилась какая-то ошибка, все равно необходимо вернуть 0! (и напечатать [error] в stdout).
*/

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
    union{
        int number;
        char *string;
        char operator;
    } element;
    char type; /* тип данных: 's' - строка, 'n' - число, 'o' - оператор */
} SElement; /* элемент выходной строки */


/* чтение открывающейся скобки */
bool ReadLeftBracket() 
{   
    /* чтение ведущих пробелов */
    char space = '0';
    while( scanf( "%1[ ]", &space ) == 1 );
    
    char bracket = '0';
    if( scanf( "%1[(]", &bracket ) == 1 ){
        return true;
    } else{
        return false;
    }
}


/* чтение строки */
bool ReadString( char* *string, bool *error_state )
{
    /* чтение ведущих пробелов */
    char space = '0';
    while( scanf( "%1[ ]", &space ) == 1 );

    /* чтение " - начала строки */
    char inverted_comma = '0';
    if( scanf( "%1[\"]", &inverted_comma ) == 0 ){
        return false;
    }
    
    size_t default_length = 20; /* длина блока считываемой строки, не включая '\0' */
    size_t string_length = default_length; 
    (*string) = (char*) malloc( ( default_length + (size_t)1 ) * sizeof( char ) );
    if( *string == NULL ){
        *error_state = true;
        return false;
    }
    char *start = (*string);

    /* чтение до default_length символов либо до встречи " (конца строки) или символа перевода строки */
    while( scanf( "%20[^\"\n]", start ) == 1 ){
        if( strlen( start ) < default_length ){
            /* встретился либо конце строки ", либо символ перевода строки */
            break;
        } else{
            char *temp = realloc( *string, string_length += ( default_length + 1 ) );
            if( temp == NULL ){
                free( *string );
                *error_state = true;
                return false;
            }
            (*string) = temp;
            start = (*string) + string_length - default_length - 1;
        }
    }

    if( getchar() == '"' ){
        return true;
    }else{
        /* встретился символ перевода строки - ошибка записи строки */
        free( *string );
        *error_state = true;
        return false;
    }
}


/* чтение операторов + и * */
bool ReadOperator( char *operator )
{
    /* чтение ведущих пробелов */
    char space = '0';
    while( scanf( "%1[ ]", &space ) == 1 );

    if( scanf( "%1[+*]", operator ) == 1 ){
        return true;
    } else{
        return false;
    }
}


/* чтение числа, на которое "умножается" строка */
bool ReadNumber( int *number, bool *error_state )
{
    size_t default_length = 3; /* длина блока считываемого числа */
    size_t string_length = default_length; /* количество разрядов числа как строки, не включая '\0' */
    char *string = (char*) malloc( ( string_length + (size_t)1 ) * sizeof( char ) );
    if( string == NULL ){
        *error_state = true;
        return false;
    }
    char *start = string;

    /* чтение ведущих пробелов */
    char space = '0';
    while( scanf( "%1[ ]", &space ) == 1 );
    
    while( scanf( "%3[0-9]", start ) == 1 ){
        if( strlen( start ) < default_length ){
            *number = atoi( string );
            free( string );
            return true;
        } 
        char *temp = realloc( string, string_length += ( default_length + (size_t)1 ) );
        if( temp == NULL ){
            *error_state = true;
            break;
        }
        string = temp;
        start = string + default_length;
    }

    free( string );

    return false;
}


/* чтение закрывающейся круглой скобки */
bool ReadRightBracket()
{
    /* чтение ведущих пробелов */
    char space = '0';
    while( scanf( "%1[ ]", &space ) == 1 );
    
    char bracket = '0';
    if( scanf( "%1[)]", &bracket ) == 1 ){
        return true;
    } else{
        return false;
    }
}


/* перевыделение памяти под строку, записанную в форме обратной польской записи */
SElement* ReallocNotation( SElement* *postfix_notation, int *notation_size )
{
    const int kNotationIncrease = 2;
    SElement *copy = (SElement*) malloc( (*notation_size) * kNotationIncrease * sizeof( SElement ) );
    if( copy == NULL ){
        return NULL;
    }
    
    /* создать копию польской нотации */
    for( int i = 0; i < *notation_size; ++i ){
        copy[i].type = (*postfix_notation)[i].type;
        if( (*postfix_notation)[i].type == 's' ){
            size_t string_length = strlen( (*postfix_notation)[i].element.string );
            copy[i].element.string = (char*) malloc( ( string_length + (size_t)1 ) * sizeof( char ) );
            if( copy[i].element.string == NULL ){
                for( int j = 0; j < i; ++j ){
                    if( copy[j].type == 's' ){
                        free( copy[j].element.string );
                    }
                }
                free( copy );
                free( (*postfix_notation)[i].element.string );
                (*postfix_notation)[i].element.string = NULL;

                return NULL;
            }
            strcpy( copy[i].element.string, (*postfix_notation)[i].element.string );

            free( (*postfix_notation)[i].element.string );
            (*postfix_notation)[i].element.string = NULL;
        } else{
            if( (*postfix_notation)[i].type == 'o' ){
                copy[i].element.operator = (*postfix_notation)[i].element.operator;
            } else{
                copy[i].element.number = (*postfix_notation)[i].element.number;
            }
        }    
    }

    (*notation_size) *= kNotationIncrease;

    free( *postfix_notation );

    return copy;
}


/* построить по входной строке обратную польскую запись */
bool ReadExpression( SElement* *postfix_notation, size_t *notation_length )
{
    int notation_size = 10;
    /* выходная строка, представленная в обратной польской нотации */ 
    (*postfix_notation) = (SElement*) calloc( notation_size, sizeof( SElement ) );
    if( *postfix_notation == NULL ){
        return false;
    }
    int i_notation = -1;
    SElement *notation_copy = NULL;
    
    int stack_size = 10;
    const int kStackIncrease = 2;
    /* стек, хранящий открывающиеся круглые скобки, операторы + и * */
    char *stack = (char*) calloc( stack_size, sizeof( char ) );
    if( stack == NULL ){
        free( *postfix_notation );
        return false;
    }
    int i_stack_top = -1;

    char *string = NULL;
    int number = 0;
    char operator = '0';

    bool error_state = false;

    while( true ){
        if( ReadLeftBracket() == true ){
            if( i_stack_top == ( stack_size - (int)1 ) ){
                char *temp = (char*) realloc( stack, stack_size *= kStackIncrease );
                if( temp == NULL ){
                    error_state = true;
                    break;
                }
                stack = temp;
            }
            stack[++i_stack_top] = '(';
        } else  {
            if( ReadString( &string, &error_state ) == true ){
                if( i_notation == notation_size - 1 ){
                    notation_copy = ReallocNotation( postfix_notation, &notation_size );
                    if( notation_copy == NULL ){
                        error_state = true;
                        break;
                    }
                    (*postfix_notation) = notation_copy;
                }
                (*postfix_notation)[++i_notation].type = 's';
                (*postfix_notation)[i_notation].element.string = (char*) malloc( 
                    (strlen(string) + (size_t)1) * sizeof( char ) );
                if( (*postfix_notation)[i_notation].element.string == NULL ){
                    error_state = true;
                    break;
                }
                strcpy( (*postfix_notation)[i_notation].element.string, string );
                ++(*notation_length);
                free( string );
            }else{
                if( error_state == true ){
                    /* ошибка выделения памяти */
                    break;
                }
                if( ReadOperator( &operator ) == true ){
                     if( i_stack_top == stack_size - 1 ){
                         char *temp = (char*) realloc( stack, stack_size *= kStackIncrease );
                         if( temp == NULL ){
                             error_state = true;
                             break;
                         }
                         stack = temp;
                     }
                     if( i_notation == notation_size - 1 ){
                        notation_copy = ReallocNotation( postfix_notation, &notation_size );
                        if( notation_copy == NULL ){
                            /* ошибка выделения памяти */
                            error_state = true;
                            break;
                        }
                        (*postfix_notation) = notation_copy;
                     }
                     /* достаём из стека операторы * - операторы с большим приоритетом, чем оператор +, - 
                        и помещаем их в выходную строку */
                     while( i_stack_top >= 0 && stack[i_stack_top] == '*' ){
                        if( i_notation == notation_size - 1 ){
                            notation_copy = ReallocNotation( postfix_notation, &notation_size );
                            if( notation_copy == NULL ){
                                /* ошибка выделения памяти */
                                error_state = true;
                                break;
                            }
                            (*postfix_notation) = notation_copy;
                        }
                         (*postfix_notation)[++i_notation].type = 'o';
                         (*postfix_notation)[i_notation].element.operator = stack[i_stack_top];
                         ++(*notation_length);
                         --i_stack_top;
                     }
                     stack[++i_stack_top] = operator;
                 } else{ 
                    if( ReadNumber( &number, &error_state ) == true ){
                        if( i_notation == notation_size - 1 ){
                            notation_copy = ReallocNotation( postfix_notation, &notation_size );
                            if( notation_copy == NULL ){
                                /* ошибка выделения памяти */
                                error_state = true;
                                break;
                            }
                            (*postfix_notation) = notation_copy;
                        }
                        (*postfix_notation)[++i_notation].type = 'n';
                        (*postfix_notation)[i_notation].element.number = number;
                        ++(*notation_length);
                    } else{ 
                        if( error_state == true ){
                            break;
                        }
                        if( ReadRightBracket() == true ){
                            /* достаём из стека все элементы до встречи открывающейся круглой скобки и 
                                помещаем их в выходную строку */
                            while( i_stack_top > 0 && stack[i_stack_top] != '(' ){
                                if( i_notation == notation_size - 1 ){
                                    notation_copy = ReallocNotation( postfix_notation, &notation_size );
                                    if( notation_copy == NULL ){
                                        /* ошибка выделения памяти */
                                        error_state = true;
                                        break;
                                    }
                                    (*postfix_notation) = notation_copy;
                                }
                                (*postfix_notation)[++i_notation].type = 'o';
                                (*postfix_notation)[i_notation].element.operator = stack[i_stack_top];
                                ++(*notation_length);
                                --i_stack_top;
                            }
                            if( stack[i_stack_top] != '(' ){
                                /* непарные круглые скобки: закрывающихся больше открывающихся */
                                error_state = true;
                                break;
                            }
                            /* удаляем из стека открывающуюся круглую скобку */
                            --i_stack_top;
                        } else{ 
                            if( getchar() != '\n' ){
                                /* некорректный формат входного выражения */
                                error_state = true;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    if( !error_state ){
        /* достаём из стека оставшиеся операторы и помещаем их в выходную строку */
        while( i_stack_top >= 0 ){
            if( stack[i_stack_top] == '(' ){
                /* непарные круглые скобки: открывающихся больше закрывающихся */
                error_state = true;
                break;
            }
            if( i_notation == notation_size - 1 ){
                notation_copy = ReallocNotation( postfix_notation, &notation_size );
                if( notation_copy == NULL ){
                    /* ошибка выделения памяти */
                    error_state = true;
                    break;
                }
                (*postfix_notation) = notation_copy;
            }
            (*postfix_notation)[++i_notation].type = 'o';
            (*postfix_notation)[i_notation].element.operator = stack[i_stack_top];
            ++(*notation_length);
            --i_stack_top;
        }
    }
    
    free( stack );

    if( !error_state ){
        return true;
    } else{
        for( size_t i = 0; i < *notation_length; ++i ){
            if( (*postfix_notation)[i].type == 's' && (*postfix_notation)[i].element.string != NULL ){
                free( (*postfix_notation)[i].element.string );
            }
        }
        free( *postfix_notation );

        return false;
    }
}


/* конкатенация строк (str2 + str1): результат сохраняется в строку str1 */
bool ConcatenateStrings( char* *str1, char* *str2 )
{
    char *copy = (char*) malloc( ( strlen(*str1) + 1 ) * sizeof( char ) );
    if( copy == NULL ){
        return false;
    }

    strcpy( copy, *str1 );

    char *temp = realloc( *str1, strlen( *str2 ) + strlen( *str1 ) + (size_t)1 );
    if( temp == NULL ){
        free( copy );
        return false;
    }
    *str1 = temp;
    
    strcpy( *str1, *str2 );
    char *start = *str1 + strlen( *str2 );
    strcpy( start, copy );

    free( copy );

    return true;
}


/* "умножение" строки на число */
bool MultiplyString( char* *str, const int *kNumber )
{
    if( *kNumber != 0 ){
        char *copy = (char*) malloc( ( strlen(*str) + 1 ) * sizeof( char ) );
        if( copy == NULL ){
            return false;
        }
        strcpy( copy, *str );
        size_t string_length = strlen( copy );

        char *temp = realloc( *str, string_length * (size_t)(*kNumber) + 1 );
        if( temp == NULL ){
            free( copy );
            return false;
        }
        *str = temp;
    
        char *current = *str + string_length;
        for( int i = 1; i < *kNumber; ++i ){
            strcpy( current, copy );
            current += string_length;
        }
        free( copy );
    } else{
        /* получилась пустая строка */
        char *temp = (char*) malloc( sizeof(char) );
        if( temp == NULL ){
            return false;
        }
        free( *str );
        *str = temp;
        *str = '\0';
    }

    return true;
}


/* найти результирующую строку по входной строке в формате обратной польской записи */
char* CalcExpression( SElement* *postfix_notation, const size_t *kNotationLength )
{
    bool error_state = false;
    char *result_string = NULL;
    /* массив индексов элементов входной строки, на которые действуют операторы */
    size_t indices_of_strings[(*kNotationLength)/2 + 1];
    for( size_t i = 0, j = 0; i < *kNotationLength ; ++i ){
        if( (*postfix_notation)[i].type == 'o' ){
            if( j == 1 ){
                /* не хватает одного операнда */
                error_state = true;
                break;
            }
            if( (*postfix_notation)[i].element.operator == '+' ){
                if( (*postfix_notation)[ indices_of_strings[j-1] ].type == 'n' 
                    || (*postfix_notation)[ indices_of_strings[j-2] ].type == 'n' ){
                        /* попытка конкатенации строки и числа */
                        error_state = true;
                        break;
                }
                /* конкатенировать строки; результат записать в элемент, стоящий перед оператором во входной строке */
                if( ConcatenateStrings( &((*postfix_notation)[ indices_of_strings[j-1] ].element.string), 
                                        &((*postfix_notation)[ indices_of_strings[j-2] ].element.string) ) 
                    == false ){
                    /* ошибка выделения памяти */
                    error_state = true;
                    break;
                }
                /* "удалить" оставшийся элемент из входной строки */
                indices_of_strings[j-2] = indices_of_strings[j-1];
                --j;
            } else{
                /* определить, на каких позициях перед оператором стоят строка и число */
                size_t i_number = ( (*postfix_notation)[ indices_of_strings[j-1] ].type == 'n' ) 
                                  ? ( indices_of_strings[j-1] ) 
                                  : ( indices_of_strings[j-2] );
                if( (*postfix_notation)[i_number].type == 's' ){
                    /* попытка умножения строки на строку */
                    error_state = true;
                    break;
                }
                size_t i_string = ( i_number == ( indices_of_strings[j-1] ) )
                                  ? ( indices_of_strings[j-2] ) 
                                  : ( indices_of_strings[j-1] );
                int number = (*postfix_notation)[i_number].element.number;
                if( MultiplyString( &((*postfix_notation)[i_string].element.string), &number )
                    == false ){
                    /* ошибка выделения памяти */
                    error_state = true;
                    break;
                }
                /* "удалить" из входной строки число */
                indices_of_strings[j-2] = i_string;
                --j;
            }
        } else{
            /* запомнить элемент-неоператор */
            indices_of_strings[j++] = i;
        }
    }

    if( !error_state ){
        size_t result_length = strlen( (*postfix_notation)[ indices_of_strings[0] ].element.string );
        result_string = (char*) malloc( ( result_length + (size_t)1 ) * sizeof( char ) );
        if( result_string == NULL ){
            error_state = true;
        }else{
            strcpy( result_string, (*postfix_notation)[ indices_of_strings[0] ].element.string ); 
        }
    }
    
    for( size_t i = 0; i < *kNotationLength; ++i ){
        if( (*postfix_notation)[i].type == 's' ){
            free( (*postfix_notation)[i].element.string );
        }
    }
    free( (*postfix_notation) );

    return result_string;
}   


int main()
{
    char *result_string = NULL; /* результирующая строка */
    SElement* read_expression = NULL; /* прочитанное выражение в постфиксной форме */
    size_t expression_length = 0;

    if( ReadExpression( &read_expression, &expression_length ) == true ){
        result_string = CalcExpression( &read_expression, &expression_length ); 
        if( result_string != NULL ){
            printf( "%s\n", result_string );
            free( result_string );
        }else{
            /* ошибка выделения памяти либо некорректные операнды операторов */
            printf( "[error]\n" );
        }
    } else{
        /* ошибка выделения памяти либо некорректное входное выражение */
        printf( "[error]\n" );
    }

    return 0;
}
