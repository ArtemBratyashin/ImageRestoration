#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int can_be_placed_vertically(int ***chr, int m, int previous_block, int next_block) //проверка совпадения вертикальных сторон
{
    for(int i=0; i<m+1; ++i){
        if(chr[previous_block][i][m] != chr[next_block][i][0]){
            return 0;
        }
    }
    return 1;
}

int has_equal_horz_edge(int ***chr, int m, int previous_block, int next_block) //проверка совпадения горизонтальных сторон
{
    for(int i=0; i<m+1; ++i){
        if(chr[previous_block][m][i] != chr[next_block][0][i]){
            return 0;
        }
    }
    return 1;
}

//опережающее объявление
int first_horz_search();
int first_vert_search();

// восстановление оставшихся элементов следующей горизонтали
int next_horz_search(int ***chr, int *final_sequence, int *free_fragments, int k, int m, int k_side, int place_id)
{
    for(int i=0; i<k; ++i){
        if(!free_fragments[i] && can_be_placed_vertically(chr, m-1, final_sequence[place_id-1], i) && has_equal_horz_edge(chr, m-1, final_sequence[place_id-k_side], i)){
            final_sequence[place_id]=i;
            free_fragments[i]=1;
            if( place_id == k-1 ){
                return 1;
            }
            else if( place_id%k_side == k_side-1){
                if(first_vert_search(chr, final_sequence, free_fragments, k, m, k_side, place_id+1)){
                    return 1;
                }
            }
            else{
                if(next_horz_search(chr, final_sequence, free_fragments, k, m, k_side, place_id+1)){
                    return 1;
                }
            }
            free_fragments[i]=0;
        }
    }
    return 0;
}

//восстановление первого элемента следующей горизонтали
int first_vert_search(int ***chr, int *final_sequence, int *free_fragments, int k, int m, int k_side, int place_id)
{
    for(int i=0; i<k; ++i){
        if(!free_fragments[i] && has_equal_horz_edge(chr, m-1, final_sequence[place_id-k_side], i)){
            final_sequence[place_id]=i;
            free_fragments[i]=1;
            if(next_horz_search(chr, final_sequence, free_fragments, k, m, k_side, place_id+1)){
                return 1;
            }
            free_fragments[i]=0;
        }
    }
    return 0;
}

//восстановление первой горизонтали
int first_horz_search(int ***chr, int *final_sequence, int *free_fragments, int k, int m, int k_side, int place_id)
{
    for(int i=0; i<k; ++i){
        if(!free_fragments[i] && can_be_placed_vertically(chr, m-1, final_sequence[place_id-1], i)){
            final_sequence[place_id]=i;
            free_fragments[i]=1;
            if( place_id == k_side-1 ){
                if(first_vert_search(chr, final_sequence, free_fragments, k, m, k_side, place_id+1)){
                    return 1;
                }
            }
            else{
                if(first_horz_search(chr, final_sequence, free_fragments, k, m, k_side, place_id+1)){
                    return 1;
                }
            }
            free_fragments[i]=0;
        }
    }
    return 0;
}

int main()
{
    FILE *demo_file;
    demo_file=fopen("demo.txt", "r");
    if(!demo_file){ printf("Невозможно открыть файл"); exit (1);}
    
    int k, m, k_side;
    fscanf(demo_file,"%d%d", &k, &m);
    k_side=sqrt(k);
    
    //массив для записи отмеченных и неотмеченных позиций
    int *free_fragments = calloc(k, sizeof(*free_fragments));
    if(!free_fragments) {printf("Недостаточно памяти"); exit(1);}
    
    //массив для записи номеров фрагментов
    int *primary_sequence = calloc(k,sizeof(*primary_sequence));
    if(!primary_sequence) {printf("Недостаточно памяти"); exit(1);}
    
    //массив для записи финальной последовательности фрагментов
    int *final_sequence = calloc(k,sizeof(*final_sequence));
    if(!final_sequence) {printf("Недостаточно памяти"); exit(1);}
    for(int i=0; i<k; ++i){
        final_sequence[i]=-1;
    }
    
    //создается трехмерный массив, где первый элемент- фрагмент, второй- строка, третий- столбец
    int ***chr = calloc(k, sizeof(int **));
    if(!chr) {printf("Недостаточно памяти"); exit(1);}
    for(int i=0; i<k; ++i){
        chr[i] = calloc(m, sizeof(int *));
        if(!chr[i]) {printf("Недостаточно памяти"); exit(2);}
        for(int j=0; j<m; ++j){
            chr[i][j] = calloc(m, sizeof(int));
            if(!chr[i][j]) {printf("Недостаточно памяти"); exit(3);}
        }
    }
    
    //запись символов и номеров
    for(int q=0; q<k; ++q){
        fscanf(demo_file,"%d", &primary_sequence[q]);
        fgetc(demo_file);
        for(int i=0; i<m; ++i){
            for(int j=0; j<m; ++j){
                chr[q][i][j]=fgetc(demo_file);
            }
            fgetc(demo_file);
        }
    }
    
    fclose(demo_file);
    // конец обработки ввода
    
    //начало восстановления последовательности
    for(int i=0; i<k; ++i){
        final_sequence[0]=i;
        free_fragments[i]=1;
        if(first_horz_search(chr, final_sequence, free_fragments, k, m, k_side, 1)){
            printf("Последовательность восстановлена\n");
            break;
        }
        free_fragments[i]=0;
    }
    if(final_sequence[k-1]==-1){
        printf("Не удается восстановить последовательность");
        return 0;
    }
    //конец восстановления
    
    //начало вывода результата
    FILE *result_file;
    result_file=fopen("test_result.txt", "w");
    if(!result_file){ printf("Невозможно открыть файл"); exit (1);}
    
    for(int i=0; i<m; ++i){
        for(int j=0; j<m; ++j){
            fputc(chr[final_sequence[0]][i][j], result_file);
        }
        for(int q=1; q<k_side; ++q){
            for(int c=1; c<m; ++c){
                fputc(chr[final_sequence[q]][i][c], result_file);
            }
        }
        fputc('\n', result_file);
    }
    for(int p=k_side; p<k; p+=k_side){
        for(int i=1; i<m; ++i){
            for(int j=0; j<m; ++j){
                fputc(chr[final_sequence[p]][i][j], result_file);
            }
            for(int q=1; q<k_side; ++q){
                for(int c=1; c<m; ++c){
                    fputc(chr[final_sequence[p+q]][i][c], result_file);
                }
            }
            fputc('\n', result_file);
        }
    }
    for(int i=0; i<k; i+=k_side){
        fputc('\n', result_file);
        for(int j=0; j<k_side; ++j){
            fprintf(result_file, "%4d ", primary_sequence[final_sequence[i+j]]);
        }
    }
    
    fclose(result_file);
    //конец вывода результата
    
    
    free(primary_sequence);
    free(final_sequence);
    for(int i=0; i<m; ++i){
        for(int j=0; j<m; ++j){
            free(chr[i][j]);
        }
        free(chr[i]);
    }
    free(chr);
}
