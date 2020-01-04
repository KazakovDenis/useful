import os


USER = 'username'
DB_PASS = 'password_to_db'
PATH = '/path/to/application'

# Github
GH_SECRET = 'some_secret_string_3'
GH_SENDER_ID = 123456
GH_REPO_ID = 1234567


class Configuration:
    DEBUG = False 
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    SQLALCHEMY_DATABASE_URI = f'postgresql+psycopg2://{USER}:{DB_PASS}@localhost/blog'
    SECRET_KEY = 'some_secret_string_1'
    UPLOAD_FOLDER = os.path.abspath('../static/uploads/')
    ALLOWED_EXTENSIONS = ('txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif')
    MAX_CONTENT_LENGTH = 16 * 1024 * 1024

    # flask security
    SECURITY_PASSWORD_SALT = 'some_secret_string_2'
    SECURITY_PASSWORD_HASH = 'sha512_crypt'

