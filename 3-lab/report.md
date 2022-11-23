# Выполнение:

---

```text
➜  3-lab git:(3-lab) ✗ sh run.sh 4
Slave#1 | stopped | exec time=0.004711238
Slave#2 | stopped | exec time=0.004785497
Slave#3 | stopped | exec time=0.00476772
Master | interrupted | counter=3001 | exec time=0.004897476
➜  3-lab git:(3-lab) ✗ sh run.sh 8
Slave#3 | stopped | exec time=0.006503283
Slave#2 | stopped | exec time=0.006018256
Slave#5 | stopped | exec time=0.007635277
Slave#1 | stopped | exec time=0.007985566
Slave#7 | stopped | exec time=0.007943439
Slave#6 | stopped | exec time=0.00836294
Slave#4 | stopped | exec time=0.013177445
Master | interrupted | counter=7001 | exec time=0.021135505
```

## Результат из ЛР №2

```text
➜  2-lab git:(2-lab) ✗ sh run.sh 4
^[[ASlave#3 | stopped | exec time=0.009501152
Slave#1 | stopped | exec time=0.010445047
Slave#2 | stopped | exec time=0.01045
Master | interrupted | counter=3006 | exec time=0.010371365
➜  2-lab git:(2-lab) ✗ sh run.sh 8
Slave#7 | stopped | exec time=0.01052071
Slave#5 | stopped | exec time=0.014540694
Slave#1 | stopped | exec time=0.016356052
Slave#3 | stopped | exec time=0.015746818
Slave#6 | stopped | exec time=0.016523636
Master | interrupted | counter=7014 | exec time=0.016587512
Slave#2 | stopped | exec time=0.016543884
```

# Вывод

> В среднем мы получаем двухкратное уменьшение latency на отправку сообщений slave'ами
>
> но так как получение сообщений master'ом блокирующая операция - мы несмотря на ускорение slave'ов получаем аналогичное время работы
>
> для обобщенной передачи сообщения относительно коллективной
