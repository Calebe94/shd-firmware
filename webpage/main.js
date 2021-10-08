function get_result_from_route(route, callback)
{
    var request = new XMLHttpRequest();
    request.open('GET', route, true);

    request.onload = function() 
    {
        if (request.status >= 200 && request.status < 400) 
        {
            // Success!
            callback(request.responseText);
        }
        else 
        {
            // We reached our target server, but it returned an error
        }
    };

    request.onerror = function() {
    // There was a connection error of some sort
    };

    request.send();
    return false;

}

function check_id_input_field()
{
    get_result_from_route("/get/id", function(result){
        const obj = JSON.parse(result);
        var input_id = document.getElementsByName("id")[0]; 
        input_id.placeholder = obj.id;
    });
}

function check_mode_input_field()
{
    get_result_from_route("/get/mode", function(result){
        const obj = JSON.parse(result);
        if(obj.mode === "controller")
        {
            var input_id = document.getElementById("controller");
            input_id.checked = true;
            var devices_fieldset = document.getElementById("devices-fieldset");
            devices_fieldset.style.display = 'inline';
            var peripheral_id_fieldset = document.getElementById("peripheral_id_fieldset");
            peripheral_id_fieldset.style.display = 'none';
            var device_mode_filedset = document.getElementById("device_mode_filedset");
            device_mode_filedset.style.display = 'none';
            var phone_number_fieldset = document.getElementById("phone_number_fieldset");
            phone_number_fieldset.style.display = 'inline';
            var local_fieldset = document.getElementById("local_fieldset");
            local_fieldset.style.display = 'inline';
            var interval_fieldset = document.getElementById("interval_fieldset");
            interval_fieldset.style.display = 'inline';
        }
        else
        {
            var input_id = document.getElementById("peripheral");
            input_id.checked = true;
            var devices_fieldset = document.getElementById("devices-fieldset");
            devices_fieldset.style.display = 'none';
            var peripheral_id_fieldset = document.getElementById("peripheral_id_fieldset");
            peripheral_id_fieldset.style.display = 'inline';
            var device_mode_filedset = document.getElementById("device_mode_filedset");
            device_mode_filedset.style.display = 'inline';
            var phone_number_fieldset = document.getElementById("phone_number_fieldset");
            phone_number_fieldset.style.display = 'none';
            var local_fieldset = document.getElementById("local_fieldset");
            local_fieldset.style.display = 'none';
            var interval_fieldset = document.getElementById("interval_fieldset");
            interval_fieldset.style.display = 'none';
        }
    });
}

function check_devices_table_list()
{
    get_result_from_route("/get/devices", function(result){
        const obj = JSON.parse(result);
        devices = obj.devices;
        console.log(devices);
        devices.forEach(function(device){
            console.log(device);
            var devices_table = document.getElementById("devices-table");
            console.log(devices_table);

            var row = document.createElement("tr");

            var td1 = document.createElement("td");
            var td2 = document.createElement("td");

            td1.innerHTML = device.toString();
            td2.innerHTML = "<form action='/delete/device/"+device.toString()+"' method='post'><input type='submit' value='Excluir'></form>";

            row.appendChild(td1);
            row.appendChild(td2);

            devices_table.children[0].appendChild(row);
        });
    });
}

function check_phones_table_list()
{
    get_result_from_route("/get/phones", function(result){
        const obj = JSON.parse(result);
        phones = obj.phones;
        console.log(phones);
        var index = 0;
        phones.forEach(function(phone){
            console.log(phone);
            var phones_table = document.getElementById("phones-table");
            console.log(phones_table);

            var row = document.createElement("tr");

            var td1 = document.createElement("td");
            var td2 = document.createElement("td");

            td1.innerHTML = phone.toString();
            td2.innerHTML = "<form action='/delete/phone/"+phone.toString()+"' method='post'><input type='submit' value='Excluir'></form>";

            row.appendChild(td1);
            row.appendChild(td2);

            phones_table.children[0].appendChild(row);
        });
    });
}

function check_phone_input_field()
{
    get_result_from_route("/get/phones", function(result){
        const obj = JSON.parse(result);
        if (obj.phone)
        {
            var input_phone = document.getElementsByName("phone")[0]; 
            input_phone.placeholder = obj.phone;
        }
    });
}

function check_local_input_field()
{
    get_result_from_route("/get/local", function(result){
        const obj = JSON.parse(result);
        if (obj.local)
        {
            var input_local = document.getElementsByName("local")[0]; 
            input_local.placeholder = obj.local;
        }
    });
}

function check_interval_input_field()
{
    get_result_from_route("/get/interval", function(result){
        const obj = JSON.parse(result);
        if (obj.interval)
        {
            var input_interval = document.getElementsByName("interval")[0]; 
            input_interval.placeholder = obj.interval;
        }
    });
}

function redirect()
{
    window.location.href="/settings.html";
    return true;
}

function alert_restart()
{
    alert("O hardware será reinicializado, você terá que se conectar manualmente mais tarde...");
    redirect();
}
