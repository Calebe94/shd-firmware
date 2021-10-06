from flask import Flask, request, render_template, send_from_directory
import os, json

file_path = os.path.realpath(__file__)
data_path = os.path.dirname(file_path)

###################################################################################################
# Global data
###################################################################################################
global_settings = dict()
global_devices  = dict()

###################################################################################################
# Miscellaneous
###################################################################################################
def load_settings():
    settings_json = dict()
    with open(os.path.join(data_path, "settings.json")) as settings_file:
        settings_json = json.load(settings_file)
    return settings_json

def load_devices():
    devices_json = dict()
    with open(os.path.join(data_path, "devices.json")) as devices_file:
        devices_json = json.load(devices_file)
    return devices_json

###################################################################################################
# Flask Routes
###################################################################################################
app = Flask(__name__, 
        static_url_path=data_path,
        static_folder=data_path,
        template_folder=data_path)

@app.route('/<path:path>')
def send_static_file(path):
    return send_from_directory(data_path, path)

@app.route('/readings.html', methods=['GET'])
def readings_route():
    return render_template('readings.html')

@app.route('/settings.html', methods=['GET'])
def settings_route():
    return render_template('settings.html')

@app.route('/')
def index():
    return render_template('index.html')

@app.route("/get/id", methods=['GET'])
def get_id():
    return '{"id": '+str(global_settings["id"])+'}'

@app.route("/set/id", methods=['POST'])
def set_id():
    req = request.form
    global_settings['id'] = req["id"]
    print(global_settings)
    return 'OK'

@app.route("/get/mode", methods=['GET'])
def get_mode():
    return '{"mode": '+str(global_settings["mode"])+'}'

@app.route("/set/mode", methods=['POST'])
def set_mode():
    req = request.form
    global_settings['mode'] = req["mode"]
    print(global_settings)
    return 'OK'

@app.route("/get/readings", methods=['GET'])
def get_readings():
    return '{"litros": 700}'

@app.route("/add/device", methods=['POST'])
def add_device():
    return 'OK'

@app.route("/delete/device", methods=['POST'])
def delete_device():
    return 'OK'

@app.route("/get/devices", methods=['GET'])
def get_device():
    return 'OK'

@app.route("/set/phone", methods=['POST'])
def set_device():
    return 'OK'

@app.route("/get/phone", methods=['GET'])
def get_phone():
    return '{"phone": "teste"}'

@app.route("/set/local", methods=['POST'])
def set_local():
    return 'OK'

@app.route("/get/local", methods=['GET'])
def get_local():
    return '{"local": "teste"}'

@app.route("/set/interval", methods=['POST'])
def set_interval():
    return 'OK'

@app.route("/get/interval", methods=['GET'])
def get_interval():
    return '{"interval": "1"}'

###################################################################################################
# Main
###################################################################################################
if __name__ == '__main__':
    global_settings = load_settings()
    global_devices = load_devices()
    print(global_settings)
    print(global_devices)
    app.run(debug=True)

