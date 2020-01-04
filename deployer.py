"""
A simple Flask app that pulls changes from master branch of your Github repository when changes have been comitted

1. Put it into your app directory and do the following:
sudo chown root:user deployer.py
sudo chmod o-rwx deployer.py && sudo chmod g+rwx deployer.py
sudo visudo
add to the end:
"username ALL = NOPASSWD: /path/to/deployer.py
 username ALL = NOPASSWD: /usr/bin/supervisorctl"

2. Then configure supervisor and nginx files (examples in /configs)
"""
import os
from flask import Flask, request, redirect

# your config module
from config import APP_NAME, APP_PATH, GH_REPO_ID, GH_SENDER_ID, GH_SECRET # and other


depl = Flask('Deployer')
depl.logger.filename = APP_PATH + '/log/deployer.log'
depl.logger.level = 20


def update_app():
    commands = (f'cd {APP_PATH}', 'git pull origin master', f'supervisorctl restart {APP_NAME}')
    for command in commands:
        depl.logger.info(f'Executing "{command}"')
        try:
            os.system(command)
        except Exception as e:
            depl.logger.error(f'Could not execute "{command}": {e}')
            break


# https://stackoverflow.com/questions/59580376/how-can-i-validate-payloads-from-github-webhooks-in-python-3
def verify_signature(received_signature, request_body) -> bool:
    # from hmac import HMAC, compare_digest
    # from hashlib import sha1
    # secret = CONFIG.GITHUB_SECRET.encode()
    # expected_sign = HMAC(key=secret, msg=request_body, digestmod=sha1).hexdigest()
    # return compare_digest(received_signature, expected_sign)
    return bool(received_signature)


def check_request(req) -> bool:
    received_sign = req.headers.get('X-Hub-Signature').split('sha1=')[-1].strip()
    conditions = (verify_signature(received_sign, req.data),
                  req.json.get('repository').get('id') == GH_REPO_ID,
                  req.json.get('sender').get('id') == GH_SENDER_ID,)
    return all(conditions)


@depl.route('/deploy', methods=['POST', 'GET'])
def deploy():
    if request.method == 'POST':
        if check_request(request):
            update_app()
            return 'Successfully', 200
        else:
            return 'Forbidden', 403
    return redirect('', code=301)


if __name__ == '__main__':
    depl.run()

