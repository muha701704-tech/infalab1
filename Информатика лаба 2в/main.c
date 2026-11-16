#include <stdio.h>

struct Command {
    int code;
    int operand;
};

int main(void)
{
    int cpu_state[8] = {0};
    int memory[256] = {0};
    struct Command command;
    FILE *file;
    int count = 0;
    int old_pc;
    int error;
    int needs_operand;
    int errors = 0;

    file = fopen("program.txt", "r");
    if (file == NULL) {
        printf("Не удалось открыть program.txt\n");
        return 1;
    }

    while (count < 256 && fscanf(file, "%d", &memory[count]) == 1) {
        count++;
    }
    fclose(file);

    if (count < 7) {
        printf("В файле недостаточно чисел.\n");
        return 1;
    }
    cpu_state[0] = 6;
    cpu_state[3] = memory[0];

    while (cpu_state[4] == 0) {
        old_pc = cpu_state[0];
        error = 0;
        command.code = -1;
        command.operand = -1;

        if (old_pc < 0 || old_pc >= count) {
            error = 1;
        } else {
            command.code = memory[old_pc];
            cpu_state[2] = command.code;

            if (command.code < 0 || command.code > 5) {
                error = 2;
            }

            needs_operand = command.code == 1 || command.code == 2 ||
                            command.code == 4 || command.code == 5;

            if (needs_operand && error == 0) {
                if (old_pc + 1 >= count) {
                    error = 1;
                } else {
                    command.operand = memory[old_pc + 1];
                    if (command.operand < 0 || command.operand >= 256) {
                        error = 1;
                    }
                }
            }
        }

        if (error != 0) {
            cpu_state[5] = error;
            errors++;
            printf("pc=%d: ОШИБКА %d (команда %d, operand=%d)\n",
                   old_pc, error, command.code, command.operand);

            if (errors > 1 || cpu_state[3] < 0 || cpu_state[3] >= count) {
                cpu_state[4] = 1;
            } else {
                cpu_state[0] = cpu_state[3];
                printf("Прерывание: переход на адрес %d\n", cpu_state[0]);
            }
            continue;
        }

        if (command.code == 0) {
            cpu_state[4] = 1;
        }
        if (command.code == 1) {
            cpu_state[1] = memory[command.operand];
            cpu_state[0] += 2;
        }
        if (command.code == 2) {
            cpu_state[1] += memory[command.operand];
            cpu_state[0] += 2;
        }
        if (command.code == 3) {
            printf("Вывод аккумулятора: %d\n", cpu_state[1]);
            cpu_state[0]++;
        }
        if (command.code == 4) {
            cpu_state[0] = command.operand;
        }
        if (command.code == 5) {
            if (cpu_state[1] == 0) {
                cpu_state[0] = command.operand;
            } else {
                cpu_state[0] += 2;
            }
        }

        if (needs_operand) {
            printf("pc=%d: команда %d, operand=%d, acc=%d\n",
                   old_pc, command.code, command.operand, cpu_state[1]);
        } else {
            printf("pc=%d: команда %d, operand=нет, acc=%d\n",
                   old_pc, command.code, cpu_state[1]);
        }
    }

    if (cpu_state[5] == 0) {
        printf("Программа завершена.\n");
    } else {
        printf("Программа завершена с ошибкой.\n");
    }

    return 0;
}
