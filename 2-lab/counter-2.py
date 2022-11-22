from enum import Enum, auto
from functools import wraps

from mpi4py import MPI


class Status(Enum):
    stopped = auto()
    interrupted = auto()


def time(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        begin_time = MPI.Wtime()
        result = func(*args, **kwargs)
        return f'{result} | exec time={MPI.Wtime() - begin_time}'

    return wrapper


def init_state(comm: MPI.Comm, root: int = 0) -> dict:
    if comm.Get_rank() == root:
        return {'counter': 0}
    return {'countdown': 1000}


def body(comm: MPI.Comm, state: dict, root: int = 0) -> Status:
    rank = comm.Get_rank()
    while True:
        countdown = state.get('countdown', 0)
        msg = comm.reduce(sendobj=countdown, op=MPI.MIN, root=root)

        if rank != root:
            state['countdown'] -= 1
            if countdown == -1:
                return Status.stopped
        else:
            state['counter'] += comm.Get_size() - 1
            if msg == -1:
                return Status.interrupted


@time
def main(root: int = 0):
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    state = init_state(comm, root)
    status = body(comm, state, root)
    if rank == root:
        counter = state['counter']
        return f"Master | {status.name} | {counter=}"
    return f'Slave#{rank} | {status.name}'


if __name__ == '__main__':
    print(main(root=0))
