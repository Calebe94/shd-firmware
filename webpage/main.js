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
        if(obj.mode === "master")
        {
            var input_id = document.getElementById("master");
            input_id.checked = true;
            var devices_fieldset = document.getElementById("devices-fieldset");
            devices_fieldset.style.visibility = 'visible';
        }
        else
        {
            var input_id = document.getElementById("slave");
            input_id.checked = true;
            var devices_fieldset = document.getElementById("devices-fieldset");
            devices_fieldset.style.visibility = 'hidden';
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

function redirect()
{
    window.location.href="/settings.html";
    return true;
}2
