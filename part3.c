#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

const int maximum_size = 50000;
char words[5][6] = {"while", "switch", "if", "else", "for"}; // выбранные слова
int len[5] = {5, 6, 2, 4, 3}; // их длины

int main(int argc, char* argv[]) {
    int fd1[2];
    int fd2[2];
    int result, len_ans;
    char ans[maximum_size];
    size_t size;
    char str[maximum_size];

    if (pipe(fd1) < 0) {
        printf("Can\'t open pipe 1\n");
        exit(-1);
    }
    if (pipe(fd2) < 0) {
        printf("Can\'t open pipe 2\n");
        exit(-1);
    }
    result = fork();
    if (result < 0) {
        printf("Can\'t fork\n");
        exit(-1);
    } else if (result > 0) {
        size = read(fd1[0], str, maximum_size);
        if (close(fd1[0]) < 0){
            printf("parent: Can\'t close reading side of pipe\n");
            exit(-1);
        }
        if (close(fd1[1]) < 0) {
            printf("parent: Can\'t close writing side of pipe\n");
            exit(-1);
        }

        int count[5] = {0, 0, 0, 0, 0};
        for (int i = 0; i < size; ++i) { // перебираем старт
            for (int j = 0; j < 5; ++j) { // перебираем слово
                int check = 1; // чекер совпадения
                if (i + len[j] - 1 < size) {
                    for (int q = 0; q < len[j]; ++q) { // перебираем символы
                        if (words[j][q] != str[i + q]) {
                            check = 0;
                            break;
                        }
                    }
                } else {
                    check = 0;
                }
                if (check) { // проверка на совпадение
                    count[j] += 1;
                }
            }
        }

        len_ans = 0;
        for (int i = 0; i < 5; ++i) { // перебираем слово
            for (int q = 0; q < len[i]; ++q) { // записываем слово
                ans[len_ans] = words[i][q];
                len_ans += 1;
            }
            ans[len_ans] = ':';
            len_ans += 1;
            ans[len_ans] = ' ';
            len_ans += 1;
            char integer[maximum_size];
            sprintf(integer, "%d", count[i]);
            int len_int = 0;
            while (integer[len_int]) {
                ans[len_ans] = integer[len_int];
                len_int += 1;
                len_ans += 1;
            }
            ans[len_ans] = '\n';
            len_ans += 1;
        }
        ans[len_ans] = '\0';
        len_ans += 1;

        size = write(fd2[1], ans, len_ans);
        if (close(fd2[0]) < 0){
            printf("parent: Can\'t close reading side of pipe\n");
            exit(-1);
        }
        if (close(fd2[1]) < 0) {
            printf("parent: Can\'t close writing side of pipe\n");
            exit(-1);
        }
    } else {
        // считывание из файла, потом запись
        if ((fd1[0] = open(argv[1], O_RDONLY)) < 0) {
            printf("Can\'t open for reading\n");
            exit(-1);
        }
        size = read(fd1[0], str, maximum_size);
        if (size == -1) {
            printf("Can\'t read\n");
            exit(-1);
        }
        str[size] = '\0';
        size = write(fd1[1], str, size);
        if (close(fd1[1]) < 0) {
            printf("child: Can\'t close writing side of pipe\n");
            exit(-1);
        }
        if (close(fd1[0]) < 0) {
            printf("child: Can\'t close reading side of pipe\n");
            exit(-1);
        }
        if ((fd2[1] = open(argv[2], O_WRONLY | O_CREAT)) < 0) {
            printf("Can\'t open for writting\n");
            exit(-1);
        }

        len_ans = read(fd2[0], ans, maximum_size);
        size = write(fd2[1], ans, len_ans);
        if (size != len_ans) {
            printf("Can\'t write all string\n");
            exit(-1);
        }
        if (close(fd2[1]) < 0) {
            printf("child: Can\'t close writing side of pipe\n");
            exit(-1);
        }
        if (close(fd2[0]) < 0) {
            printf("child: Can\'t close reading side of pipe\n");
            exit(-1);
        }
        exit(0);
    }
    return 0;
}