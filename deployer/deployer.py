"""
A simple Flask app that pulls changes from master branch of your Github repository when changes have been comitted

1. Do the following:
    >>>sudo chown root:username deployer.py
    >>>sudo chmod o-rwx deployer.py && sudo chmod g+rwx deployer.py
    >>>sudo visudo

and add it to the end of the file:
    username ALL = NOPASSWD: /path/to/deployer.py
    username ALL = NOPASSWD: /usr/bin/supervisorctl

2. Then configure supervisor and nginx files
"""
import os
from subprocess import check_output
from flask import Flask, request


APP_NAME = 'your program name in supervisor config'
APP_PATH = '/path/to/your/app'
GH_REPO_ID = os.environ.get('GH_REPO_ID')
GH_SENDER_ID = os.environ.get('GH_SENDER_ID')
GH_SECRET = os.environ.get('GH_SECRET')

depl = Flask(__name__)
depl.logger.filename = os.path.abspath(os.curdir) + '/log/deployer.log'
depl.logger.level = 20


def update_app():
    commands = (f'cd {APP_PATH}', 'git stash', 'git pull origin master', f'sudo supervisorctl restart {APP_NAME}')
    for command in commands:
        depl.logger.info(f'Executing "{command}"')
        try:
            output = check_output(command.split(' ')).decode()
            depl.logger.info(output)
        except Exception as e:
            depl.logger.error(f'Could not execute "{command}": {e}')
            break


def verify_signature(req) -> bool:
    from hmac import HMAC, compare_digest
    from hashlib import sha1
    received_sign = req.headers.get('X-Hub-Signature').split('sha1=')[-1].strip()
    secret = GH_SECRET.encode()
    expected_sign = HMAC(key=secret, msg=req.data, digestmod=sha1).hexdigest()
    return compare_digest(received_sign, expected_sign)


def check_request(req) -> bool:
    conditions = (verify_signature(req),
                  req.json.get('ref') == 'refs/heads/master',
                  str(req.json.get('repository').get('id')) == GH_REPO_ID,
                  str(req.json.get('sender').get('id')) == GH_SENDER_ID,)

    if not all(conditions):
        depl.logger.warning(conditions)
        return False
    return True


@depl.route('/deploy', methods=['POST', 'GET'])
def hook():
    if request.method == 'POST':
        if check_request(request):
            update_app()
            return 'Successfully', 200
        return 'Forbidden', 403
    return 'Not allowed', 405


if __name__ == '__main__':
    depl.run()
