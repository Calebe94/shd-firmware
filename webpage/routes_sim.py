from flask import Flask, request, render_template, send_from_directory
import os

file_path = os.path.realpath(__file__)
app = Flask(__name__, 
        static_url_path=os.path.dirname(file_path), 
        static_folder=os.path.dirname(file_path), 
        template_folder=os.path.dirname(file_path))

@app.route('/<path:path>')
def send_static_file(path):
    return send_from_directory(os.path.dirname(file_path), path)

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
    return '{"id": 1}'

@app.route("/set/id", methods=['POST'])
def set_id():
    return 'OK'

@app.route("/get/mode", methods=['GET'])
def get_mode():
    return '{ "mode": "controller" }'

@app.route("/set/mode", methods=['POST'])
def set_mode():
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

if __name__ == '__main__':
    app.run(debug=True)

