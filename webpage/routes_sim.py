from flask import Flask, request, render_template, send_from_directory, redirect
import os, json, random

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

def update_settings():
    print(global_settings)
    with open(os.path.join(data_path, "settings.json"), "w") as settings_file:
        settings_file.write(json.dumps(global_settings, indent = 4))

def update_devices():
    print(global_devices)
    with open(os.path.join(data_path, "devices.json"), "w") as devices_file:
        devices_file.write(json.dumps(global_devices, indent = 4))

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
    update_settings()
    return redirect('/settings.html')

@app.route("/get/mode", methods=['GET'])
def get_mode():
    return '{"mode": "'+str(global_settings["mode"])+'"}\n'

@app.route("/set/mode", methods=['POST'])
def set_mode():
    req = request.form
    global_settings['mode'] = req["mode"]
    update_settings()
    return redirect('/settings.html')

@app.route("/get/readings", methods=['GET'])
def get_readings():
    return '{"litros": '+str(random.randint(0, 1000))+'}\n'

@app.route("/add/device", methods=['POST'])
def add_device():
    req = request.form
    global_devices["devices"].append(int(req["device"]))
    update_devices()
    return redirect('/settings.html')

@app.route("/delete/device/<device>", methods=['POST'])
def delete_device(device):
    global_devices["devices"].remove(int(device))
    update_devices()
    return redirect('/settings.html')

@app.route("/get/devices", methods=['GET'])
def get_device():
    return str(json.dumps(global_devices, indent=4))

@app.route("/set/phone", methods=['POST'])
def set_device():
    req = request.form
    global_settings["phone"] = req["phone"]
    print(global_settings)
    return redirect('/settings.html')

@app.route("/get/phone", methods=['GET'])
def get_phone():
    return '{"phone": "'+str(global_settings["phone"])+'"}'

@app.route("/set/local", methods=['POST'])
def set_local():
    req = request.form
    global_settings["local"] = req["local"]
    update_settings()
    return redirect('/settings.html')

@app.route("/get/local", methods=['GET'])
def get_local():
    return '{"local": "'+str(global_settings["local"])+'"}'

@app.route("/set/interval", methods=['POST'])
def set_interval():
    req = request.form
    global_settings["interval"] = req["interval"]
    update_settings()
    return redirect('/settings.html')

@app.route("/get/interval", methods=['GET'])
def get_interval():
    return '{"interval":'+str(global_settings["interval"])+'}'

###################################################################################################
# Main
###################################################################################################
if __name__ == '__main__':
    global_settings = load_settings()
    global_devices = load_devices()
    print(global_settings)
    print(global_devices)
    app.run(debug=True)

