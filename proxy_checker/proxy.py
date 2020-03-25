from itertools import islice
from logging import getLogger
from multiprocessing import Pool, cpu_count
from requests import get
from requests.exceptions import ConnectionError
import os


log = getLogger(__name__)
workers = cpu_count() * 2 + 1
source = os.path.join(os.path.dirname(__file__), 'static', 'proxies.txt')

with open(source) as f:
    proxies = iter(f.readlines())


def ping(address):
    """Tries to get a response from remote address through proxies"""
    addr = 'http://' + address.strip()
    pr = {'http': addr, 'https': addr}
    try:
        response = get('https://httpbin.org/get', proxies=pr, timeout=5)
        if response.status_code == 200:
            return addr
    except (OSError, ConnectionError) as e:
        log.error(e)


def get_proxy():
    """Checks proxy list and returns working one"""
    working = []
    while not working:
        addresses = list(islice(proxies, workers*10))
        with Pool(workers) as p:
            working.extend([i for i in p.map(ping, addresses) if i])
    return working[0]


if __name__ == '__main__':
    print(get_proxy())
