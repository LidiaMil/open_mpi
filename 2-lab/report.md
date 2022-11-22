# Контрольные вопросы

---

1. **Как происходит передача данных от одного процесса всем?**

> *Ответ:*
> MPI_Bcast

2. **Как происходит передача данных от всех процессов одному?**

> *Ответ:*
> MPI_Reduce

3. **Какие используются в MPI для синхронизации вычислений?**

> *Ответ:*
> MPI_Barrier

4. **Как организуется неблокирующий обмен данными между процессами?**

> *Ответ:*
> К названию функций добавляется приставка I (MPI_Isend, MPI_Ibcast)

5. **Как организуется одновременное выполнение прием и передачи данных?**

> *Ответ:*
> MPI_Sendrecv

# Выполнение:

---

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

## Результат из ЛР №1

```text
➜  1-lab git:(1-lab) ✗ sh run.sh 4
Slave#1 stopped (exec: 0.002401894)
Slave#2 stopped (exec: 0.003447657)
Slave#3 stopped (exec: 0.003781841)
Master handled 1914 messages and terminated from Slave#1 (exec : 0.004337455)
➜  1-lab git:(1-lab) ✗ sh run.sh 8
Slave#5 stopped (exec: 0.004100501)
Slave#4 stopped (exec: 0.003983008)
Slave#6 stopped (exec: 0.004220908)
Slave#1 stopped (exec: 0.004286093)
Slave#2 stopped (exec: 0.004295918)
Slave#7 stopped (exec: 0.004307119)
Master handled 4920 messages and terminated from Slave#6 (exec : 0.011401314)
Slave#3 stopped (exec: 0.002512518)
```
