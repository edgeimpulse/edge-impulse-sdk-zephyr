'''
Edge Impulse west command extension.
see:
https://docs.zephyrproject.org/latest/develop/west/extensions.html
https://docs.zephyrproject.org/latest/develop/west/zephyr-cmds.html#west-zephyr-ext-cmds
'''

import requests
from textwrap import dedent            # just for nicer code indentation

from west.commands import WestCommand  # your extension must subclass this

EI_URL="https://studio.edgeimpulse.com/v1/api/"

def get_project_name(prj_id, xapikeys):
    info_url =  f"{EI_URL}{prj_id}"
    headers_info = {
        "x-api-key": xapikeys,
    }

    response_info = requests.get(info_url, headers=headers_info)
    if (response_info.status_code != 200):
            print("Error: Can't get project info.")
            return None
            
    response_info_json = response_info.json()
    return (response_info.json())['project']['name']

def build_model(prj_id, xapikeys, impulse_id, engine, model_type):
    url = f"{EI_URL}{prj_id}/jobs/build-ondevice-model"
    querystring = {
        "type":"zephyr",
        "impulseId":str(impulse_id)
    }
    payload = {
        "engine": engine,
        "modelType": model_type
    }
    headers = {
        "x-api-key": xapikeys,
        "Content-Type": "application/json"
    }
    response = requests.post(url, json=payload, headers=headers, params=querystring)
    return response.text, response.status_code

def get_deployment(prj_id, xapikeys, impulse_id, engine, model_type):
    url = f"{EI_URL}{prj_id}/deployment/download"
    querystring = {
        "type":"zephyr",
        "modelType": model_type,
        "engine": engine,
        "impulseId": str(impulse_id)
    }
    headers = {
        "x-api-key": xapikeys,
    }

    response = requests.get(url, headers=headers, params=querystring)

    if response.status_code != 200:
        return False
    
    with open("ei_model.zip", "wb") as f:
        f.write(response.content)
    return True
    

class EdgeImpulseDeploy(WestCommand):

    def __init__(self):
        super().__init__(
            'ei-deploy',  # gets stored as self.name
            'deploy project from Edge Impulse Studio',  # self.help
            # self.description:
            dedent('''
            This command deploys a project from Edge Impulse Studio.'''),
            accepts_unknown_args=False)

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(self.name,
                                help=self.help,
                                    description=self.description)

        parser.add_argument('-p', '--project', help='Project ID', type=int, required=True)      # project d
        parser.add_argument('-k', '--api-key', help='API Key', type=str, required=True)          # x-api-key
        parser.add_argument('-i', '--impulseid', help='Impulse ID. If this is unset then the default impulse is used.', type=int, default=1)   # impulse id

        parser.add_argument('-e', '--engine', help='Optional engine for the build (if not, it uses the default engine for the deployment target)', choices=['tflite', 'tflite-eon'], default = 'tflite-eon')      # engine type
        parser.add_argument('-t', '--modeltype', help='Optional model type of the build (if not, it uses the settings in the Keras block)', choices=['int8', 'float32'], default = 'int8')   # model type

        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        self.args = args        # Avoid having to pass them around

        if not get_deployment(args.project, args.api_key, args.impulseid, args.engine, args.modeltype):
            print("Error: Can't download the model. Make sure the build is completed.")
        else:
            print("Model downloaded as ei_model.zip")

class EdgeImpulseBuild(WestCommand):

    def __init__(self):
        super().__init__(
            'ei-build',  # gets stored as self.name
            'build project with Edge Impulse',  # self.help
            # self.description:
            dedent('''
            This command builds a project with Edge Impulse Studio.'''),
            accepts_unknown_args=False)

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        parser.add_argument('-p', '--project', help='Project ID', type=int, required=True)     # project d
        parser.add_argument('-k', '--api-key', help='API Key', type=str, required=True)   # x-api-key
        parser.add_argument('-i', '--impulseid', help='Impulse ID. If this is unset then the default impulse is used.', type=int, default=1)   # impulse id

        parser.add_argument('-e', '--engine', help='Available options: tflite, tflite-eon.', choices=['tflite', 'tflite-eon'], default = 'tflite-eon')      # engine type
        parser.add_argument('-t', '--modeltype', help='Optional model type of the build (if not, it uses the settings in the Keras block)', choices=['int8', 'float32'], default = 'int8')   # model type
    
        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        self.args = args        # Avoid having to pass them around

        response_text, ret_code = build_model(args.project, args.api_key, args.impulseid, args.engine, args.modeltype)
        if ret_code != 200:
            print("Error: Can't start the build.")
            return ret_code
        else:
            print("Build started successfully.")
            print("Response text = ", response_text)
        
